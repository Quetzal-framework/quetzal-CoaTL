// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ***************************************************************************/

#pragma once

#include "coordinates.hpp"
#include "resolution.hpp"
#include "extent.hpp"
#include "gdalcpp.hpp"

#include <range/v3/all.hpp>

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <stdexcept> // std::invalid_argument
#include <cmath>
#include <optional>

namespace quetzal::geography
{

  /// @brief Discrete spatio-temporal variations of an environmental variable.
  /// @details Read and write georeferenced raster datasets with multiple bands (layers). 
  ///          Multiple rasters can be assembled into a multivariate discrete quetzal::geography::landscape.
  /// @ingroup geography
  ///
  template<typename Time=int>
  class raster
  {

  public:

    /// @typedef Location unique identifier
    using location_descriptor = int;

    /// @typedef Time unique identifier
    using time_descriptor = int;

    /// @typedef A Latitude/Longitude pair of value.
    using latlon = quetzal::geography::latlon;

    /// @typedef A Longitude/Latitude pair of value.
    using lonlat = quetzal::geography::lonlat;

    /// @typedef A column/row pair of indices.
    using colrow = quetzal::geography::colrow;

    /// @typedef A row/column pair of indices.
    using rowcol = quetzal::geography::rowcol;

    /// @typedef Time period for every band.
    using time_type = Time;

    /// @typedef decimal degree type
    using decimal_degree = double;

    /// @typedef value type
    using value_type = double;

  private:

    gdalcpp::Dataset _dataset;
    std::vector<time_type> _times;
    std::vector<double> _gT;
    latlon _origin;
    int _width;
    int _height;
    int _depth;
    resolution<decimal_degree> _resolution;
    extent<double> _extent;

    value_type _no_data;
    
    // Pixel size in x and y
    auto compute_resolution(const std::vector<double> &gT) const
    {
      return quetzal::geography::resolution<decimal_degree>(gT[5], gT[1]);
    }

    // Top left corner and bottom right corner geographic coordinates
    auto compute_extent(const latlon &origin, const quetzal::geography::resolution<decimal_degree> &r) const
     {
      const auto [lat_max, lon_min] = origin;
      const auto lon_max = lon_min + width() * r.lon();
      const auto lat_min = lat_max + height() * r.lat();
      return quetzal::geography::extent<decimal_degree>(lat_min, lat_max, lon_min, lon_max);
    }

  public:

    /// @brief Constructor
    /// @param file the raster dataset file to read.
    /// @param times the time period of each layer
    /// @invariant The size of the times argument equals the depth of the given dataset
    explicit raster(const std::filesystem::path &file, const std::vector<time_type> &times) : _dataset(file.string()),
                                                                                                           _times(times),
                                                                                                           _gT(_dataset.affine_transformation_coefficients()),
                                                                                                           _origin(compute_origin(_gT)),
                                                                                                           _width( _dataset.width()),
                                                                                                           _height( _dataset.height()),
                                                                                                           _depth( _dataset.depth()),
                                                                                                           _resolution(compute_resolution(_gT)),
                                                                                                           _extent(compute_extent(_origin, _resolution)),
                                                                                                           _no_data(nodata_first_band())
    {
      check_times_equals_depth();
    }

    /// @name Input/Output
    /// @{

    /// @brief Read the raster from an input file in the geotiff format
    /// @param file The file to read from
    /// @param start The first time period to map to the first band
    /// @param end The last time period to map to the last band
    /// @param step The time increment between each band
    /// @return A raster object
    static raster from_file(const std::filesystem::path &file, const std::vector<time_type> &times)
    {
      return raster(file, times);
    }

    /// @brief Export a spatio-temporal raster to a Geotiff file
    /// @param f functor defining the variable: its signature should be equivalent to `double f(location_descriptor, time_descriptor)`
    /// @param start the first time_descriptor value where f should be called.
    /// @param end the last time_descriptor value where f should be called.
    /// @param file the file name where to save
    template <typename Callable>
      requires std::invocable<Callable, location_descriptor, time_descriptor, std::optional<double>>
    void to_geotiff(Callable f, time_descriptor start, time_descriptor end, const std::filesystem::path &file) const
    {
      export_to_geotiff(f, start, end, file);
    }

    /// @brief Export spatial points to a shapefile
    /// @param points the points to export
    /// @param file the path to write to
    void to_shapefile(std::vector<latlon> points, const std::filesystem::path &file) const
    {
      points_to_shapefile(points, file);
    }

    /// @brief Export geolocalized counts as spatial-points to a shapefile
    /// @brief counts the number of points at every point
    /// @brief file the path to write to.
    void to_shapefile(std::map<latlon, int> counts, const std::filesystem::path &file) const
    {
      counts_to_shapefile(counts, file);
    }

    /// @brief Raster is streamable
    std::ostream& write(std::ostream& stream) const
    {
      stream << "Origin: " << origin() << 
                "\nWidth: " << width() <<
                "\nHeight: " << height() <<
                "\nDepth: " << depth() <<
                "\nResolution: " <<
                  "\n\tLat: " << get_resolution().lat() <<
                  "\n\tLon: " << get_resolution().lon() <<
                "\nExtent:" <<
                  "\n\tLat min: " << get_extent().lat_min() <<
                  "\n\tLat max: " << get_extent().lat_max() <<
                  "\n\tLon min: " << get_extent().lon_min() <<
                  "\n\tLon max: " << get_extent().lon_max() <<
                "\nNA value: " << NA() << std::endl;
      return stream;
    }
    
    /// @}

    /// @name Spatial Grid properties
    /// @{

    /// @brief Origin of the spatial grid
    latlon origin() const noexcept
    {
      return _origin;
    }

    /// @brief Resolution of the spatial grid
    /// @note The vertical pixel size will be negative (south pointing)
    geography::resolution<decimal_degree> get_resolution() const noexcept
    {
      return _resolution;
    }

    /// @brief Extent of the spatial grid
    geography::extent<decimal_degree> get_extent() const noexcept
    {
      return _extent;
    }

    /// @brief Width of the spatial grid
    int width() const noexcept
    {
      return _width;
    }

    /// @brief Height of the spatial grid
    int height() const noexcept
    {
      return _height;
    }

    /// @brief Depth of the spatial grid
    int depth() const noexcept
    {
      return _depth;
    }

    /// @brief NA value, as read in the first band of the dataset.
    value_type NA() const noexcept
    {
      return _no_data;
    }

    /// @}

    /// @name Look-up
    /// @{

    /// @brief Location descriptors (unique identifiers) of the grid cells
    auto locations() const noexcept
    {
      return ranges::views::iota(0, width() * height() - 1);
    }

    /// @brief Time descriptors (unique identifiers) of the dataset bands
    auto times() const noexcept
    {
      return ranges::views::iota(0, depth() - 1);
    }

    ///@brief checks if the raster contains a layer with specific time
    bool contains(const latlon &x) const noexcept
    {
      return is_in_spatial_extent(x);
    }

    ///@brief checks if the raster contains a layer with specific time
    bool contains(const lonlat &x) const noexcept
    {
      return is_in_spatial_extent(to_latlon(x));
    }

    ///@brief checks if the raster contains a layer with specific time
    bool contains(const time_type &t) const noexcept
    {
      return is_in_temporal_extent(t);
    }

    /// @brief Read the value at location x and time t
    /// @param x the location
    /// @param t the time
    /// @return An optional that is empty if the value read is equal to NA.
    std::optional<value_type> at(location_descriptor x, time_descriptor t) const noexcept
    {
      assert(x >= 0 && x < locations().size());
      const auto colrow = to_colrow(x);
      return read(colrow.col, colrow.row, t);
    }

    /// @brief Makes the raster a callable with signature 
    ///        `std::optional<value_type> (location_descriptor x, time_descriptor t)  
    ///        returning the value at location x and time t
    /// @remark This callable is cheap to copy, does not have ownership of the data,
    ///         and works well for mathematical composition using `quetzal::expressive`
    auto to_view() const noexcept
    {
      return [this](location_descriptor x, time_descriptor t){ return this->at(x,t);};
    }
    /// @}

    /// @name Coordinate systems
    /// @{

    /// @brief Location descriptor of the cell identified by its column/row.
    ///        If no such cell exists, an exception of type std::out_of_range is thrown.
    /// @param x the 1D coordinate to evaluate.
    /// @return The descriptor value of x
    location_descriptor to_descriptor(const colrow &x) const
    {
      if(x.col >= width() || x.col < 0 || x.row < 0 || x.row >= height() )
        throw std::out_of_range("colrow has invalid indices and can not be converted to a location descriptor");
      return x.col * width() + x.col;
    }

    ///  @brief Location descriptor of the cell to which the given coordinate belongs.
    ///        If no such cell exists, an exception of type std::out_of_range is thrown.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return The descriptor value of x
    location_descriptor to_descriptor(const latlon &x) const
    {
      if ( ! contains(x) )
        throw std::out_of_range("latlon does not belong to spatial extent and can not be converted to a location descriptor");
      return to_descriptor( to_colrow(x) );
    }

    /// @brief Column and row of the cell to which the given coordinate belongs.
    ///        If no such cell exists, an exception of type std::out_of_range is thrown.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return A pair (column, row)
    colrow to_colrow(const latlon &x) const
    {
      if ( !contains(x))
        throw std::out_of_range("latlon does not belong to spatial extent and can not be converted to a col/row pair");

      const auto [lat, lon] = x;
      const int col = (lon - _gT[0]) / _gT[1];
      const int row = (lat - _gT[3]) / _gT[5];
      return colrow(col, row);
    }

    /// @brief Row and column of the cell to which the given coordinate belongs.
    ///        If no such cell exists, an exception of type std::out_of_range is thrown.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return A pair (row, column)
    rowcol to_rowcol(const latlon &x) const noexcept
    {
      if ( !contains(x))
        throw std::out_of_range("latlon does not belong to spatial extent and can not be converted to a row/col pair.");

      auto colrow = to_colrow(x);
      std::swap(colrow.col, colrow.row);
      return colrow;
    }

    /// @brief Column and row of the cell to which the given descriptor belongs.
    /// @param x the location to evaluate.
    /// @return A pair (column, row)
    colrow to_colrow(location_descriptor x) const noexcept
    {
      const int col = x % width();
      const int row = x / width();
      assert (row < height());
      return colrow(col, row);
    }

    /// @brief Column and row of the cell to which the given descriptor belongs.
    /// @param x the location to evaluate.
    /// @return A pair (column, row)
    rowcol to_rowcol(location_descriptor x) const noexcept
    {
      auto c = to_colrow(x);
      return rowcol(c.row, c.col);
    }

    /// @brief Latitude and longitude of the cell to which the given coordinate belongs.
    /// @param x the location to evaluate.
    /// @return An optional with a pair (lat, lon) if x is in the spatial extent, empty otherwise.
    latlon to_latlon(location_descriptor x) const noexcept
    {
      return to_latlon( to_colrow(x) );
    }

    /// @brief Latitude and longitude of the cell identified by its column/row.
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude)
    latlon to_latlon(const colrow &x) const noexcept
    {
      const auto [col, row] = x;
      const auto lon = _gT[1] * col + _gT[2] * row + _gT[1] * 0.5 + _gT[2] * 0.5 + _gT[0];
      const auto lat = _gT[4] * col + _gT[5] * row + _gT[4] * 0.5 + _gT[5] * 0.5 + _gT[3];
      return latlon(lat, lon);
    }

    /// @brief Latitude and longitude of the cell identified by its row/column.
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude)
    latlon to_latlon(const rowcol &x) const noexcept
    {
      return to_latlon(colrow(x.col, x.row));
    }

    /// @brief Longitude and latitude of the deme identified by its column/row.
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude)
    lonlat to_lonlat(const colrow &x) const noexcept
    {
      const auto [lat, lon] = to_latlon(x);
      return lonlat(lon, lat);
    }

    /// @brief Latlon to lonlat conversation
    /// @param x the location to evaluate.
    /// @return A pair (longitude, latitude)
    lonlat to_lonlat(const latlon &x) const noexcept
    {
      return lonlat(x.lon, x.lat);
    }

    /// @brief Latlon to lonlat conversation
    /// @param x the location to evaluate.
    /// @return A pair (longitude, latitude)
    latlon to_latlon(const lonlat &x) const noexcept
    {
      return latlon(x.lat, x.lon);
    }

    /// @brief Longitude and latitude of the deme identified by its column/row.
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude)
    lonlat to_lonlat(location_descriptor x) const noexcept
    {
      return to_lonlat(to_latlon(x));
    }

    /// @brief Reprojects a coordinate to the centroid of the cell it belongs.
    ///        If no such cell exists, an exception of type std::out_of_range is thrown.
    /// @param x The location to reproject
    /// @return The coordinate of the centroid of the cell it belongs.
    latlon to_centroid(const latlon &x) const 
    {
      if ( !contains(x))
        throw std::out_of_range("latlon does not belong to spatial extent and can not be reprojected to a cell centroid.");

      return to_latlon( to_colrow(x));
    }

    /// @}

  private:
    /// @brief Is time in the temporal extent
    bool is_in_temporal_extent(time_type t) const noexcept
    {
      return t >= _times.front() && t <= _times.back();
    }

    /// @brief Read the NA in the first band
    value_type nodata_first_band() const
    {
      return _dataset.get().GetRasterBand(1)->GetNoDataValue();
    }

    /// @brief Read 1 value using GDAL API.
    /// @note To change if access is too slow
    /// @return An optional that is empty if the value read is equal to NA.
    std::optional<double> read(int col, int row, int band) const
    {
      const int nXSize = 1;
      auto line = (float *)CPLMalloc(sizeof(float) * nXSize);

      // band begins at 1
      const auto err =_dataset.band(band + 1).RasterIO(GF_Read, col, row, 1, 1, line, nXSize, 1, GDT_Float32, 0, 0);

      if (err != CE_None)
        throw std::invalid_argument("received negative or invalid col row or band index");

      const double val = static_cast<double>(*line);
      CPLFree(line);
      if (val != this->NA() )
        return {val};
      return {};
    }

    /// @brief Export a callable to a raster
    template <typename Callable>
      requires std::invocable<Callable, location_descriptor, time_descriptor, std::optional<double>>
    void export_to_geotiff(Callable f, time_descriptor start, time_descriptor end, const std::filesystem::path &file) const
    {
      // Number of bands to create
      const auto times = ranges::views::iota(start, end);
      const int depth = times.size();
      assert(depth >= 1);

      // Create a model dataset with default values
      GDALDataset *sink = _dataset.get().GetDriver()->Create(file.string().c_str(), width(), height(), depth, GDT_Float32, NULL);

      // RasterBands begins at 1
      GDALRasterBand *source_band = _dataset.get().GetRasterBand(1);

      auto fun = [f, this](location_descriptor x, time_descriptor t)
      {
        const auto o = f(x, t);
        return o.has_value() ? static_cast<float>(o) : this->NA();
      };

      for (time_descriptor t : times)
      {
        auto buffer = locations() | ranges::views::transform(fun) | ranges::to<std::vector>();
        GDALRasterBand *sink_band = sink->GetRasterBand(t + 1);
        [[maybe_unused]] const auto err2 = sink_band->RasterIO(GF_Write, 0, 0, width(), height(), buffer.data(), width(), height(), GDT_Float32, 0, 0);
        sink_band->SetNoDataValue(NA());
        sink->FlushCache();
      }

      // write metadata
      std::vector<double> geo_transform(6);
      _dataset.get().GetGeoTransform(geo_transform.data());
      sink->SetGeoTransform(geo_transform.data());
      sink->SetProjection(_dataset.get().GetProjectionRef());

      GDALClose(static_cast<GDALDatasetH>(sink));
    }

    /// @brief Export counts to a shapefile
    void counts_to_shapefile(std::map< latlon, int> counts, const std::filesystem::path &file) const
    {
      std::string driver_name = "ESRI Shapefile";
      GDALDriver *poDriver;
      GDALAllRegister();
      poDriver = GetGDALDriverManager()->GetDriverByName(driver_name.c_str());
      assert(poDriver != NULL);

      GDALDataset *poDS;
      poDS = poDriver->Create(file.string().c_str(), 0, 0, 0, GDT_Unknown, NULL);
      assert(poDS != NULL);

      OGRLayer *poLayer;
      poLayer = poDS->CreateLayer("sample", NULL, wkbPoint, NULL);
      assert(poLayer != NULL);

      OGRFieldDefn oField("count", OFTInteger);
      if (poLayer->CreateField(&oField) != OGRERR_NONE)
      {
        throw(std::string("Creating sample size field failed."));
      }

      for (auto const &it : counts)
      {
        const auto [y, x] = it.first;
        int sample_size = it.second;

        OGRFeature *poFeature;
        poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

        poFeature->SetField("count", sample_size);

        OGRPoint pt;
        pt.setX(x);
        pt.setY(y);

        poFeature->SetGeometry(&pt);

        if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
        {
          throw(std::string("Failed to create feature in shapefile."));
        }
        OGRFeature::DestroyFeature(poFeature);
      }
      GDALClose(poDS);
    }

    /// @brief Export points to a shapefile
    void points_to_shapefile(std::vector<latlon> coords, const std::filesystem::path &file) const
    {
      std::string driver_name = "ESRI Shapefile";
      GDALDriver *poDriver;
      GDALAllRegister();
      poDriver = GetGDALDriverManager()->GetDriverByName(driver_name.c_str());
      assert(poDriver != NULL);

      GDALDataset *poDS;
      poDS = poDriver->Create(file.string().c_str(), 0, 0, 0, GDT_Unknown, NULL);
      assert(poDS != NULL);

      OGRLayer *poLayer;
      poLayer = poDS->CreateLayer("sample", NULL, wkbPoint, NULL);
      assert(poLayer != NULL);

      for (auto const [y, x] : coords)
      {
        OGRFeature *poFeature;
        poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

        OGRPoint pt;
        pt.setX(x);
        pt.setY(y);

        poFeature->SetGeometry(&pt);

        if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
        {
          throw(std::string("Failed to create feature in shapefile."));
        }
        OGRFeature::DestroyFeature(poFeature);
      }
      GDALClose(poDS);
    }

    /// @brief check if a point belongs to the spatial extent of the dataset
    bool is_in_spatial_extent(const latlon &x) const noexcept
    {
      const auto [lat, lon] = x;
      bool answer = false;
      if (lon >= _extent.lon_min() && lon < _extent.lon_max() && lat > _extent.lat_min() && lat <= _extent.lat_max())
        answer = true;
      return answer;
    }

    /// @brief checks if there is a 1 to 1 mapping from times to layer
    void check_times_equals_depth()
    {
      if (_times.size() != depth())
      {
        std::string message("the size of times argument should be equal to the depth of the given dataset: size is ");
        message += std::to_string(_times.size());
        message += ", should be ";
        message += std::to_string(depth());
        message += ".";
        throw std::runtime_error(message.c_str());
      }
    }

    // Geographic coordinates of the top left corner.
    latlon compute_origin(const std::vector<double> &gT) const
    {
      return latlon(gT[3], gT[0]);
    }

  }; // end class raster

template<typename Time>
std::ostream &operator<<(std::ostream &os, const raster<Time> &r) { 
   return r.write(os);
}

} // namespace quetzal::geography
