// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ***************************************************************************/

#pragma once

#include "gdalcpp.hpp"
#include "GeographicCoordinates.hpp"
#include "Resolution.hpp"
#include "Extent.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <numeric>   // std::iota
#include <stdexcept> // std::invalid_argument
#include <cmath>

namespace quetzal::geography::detail
{
  /// @brief Generate a sequence from start to end with increasing step
  static auto stepped_iota(int start, int end, int step)
  {
    return std::ranges::views::iota(0, (end - start + step - 1) / step) |
           std::ranges::views::transform([=](int x)
                                         { return x * step + start; });
  }
}

namespace quetzal::geography
{

  /// @brief Spatio-temporal variations of an environmental variable.
  /// @details Read and write georeferenced raster datasets. Can be be assembled into multivariate discrete landscapes.
  /// @ingroup geography
  ///
  class variable : private gdalcpp::Dataset
  {

  public:
    /// @typedef Location unique identifier
    using location_descriptor = int;

    /// @typedef Time unique identifier
    using time_descriptor = int;

    /// @typedef A Latitude/Longitude pair of value.
    template <typename T>
    using latlon = std::pair<T, T>;

    /// @typedef A column/row pair of indices.
    using colrow = std::pair<int, int>;

    /// @typedef A row/column pair of indices.
    using colrow = std::pair<int, int>;

    /// @typedef coordinate type
    using time_type = int;

    /// @typedef decimal degree type
    using decimal_degree = typename GeographicCoordinates::decimal_degree;

    /// @typedef value type
    using value_type = double;

  private:
    std::vector<time_type> _times;
    std::vector<double> _gT;

    // Spatial grid properties
    latlon<double> _origin;
    latlon<decimal_degree> _resolution;
    latlon<decimal_degree> _extent;

    /// @brief Constructor
    /// @param file the raster dataset file to read.
    /// @param start the time at which the first layer starts
    /// @param step the time step
    /// @invariant The size of the times argument equals the depth of the given dataset
    variable(const std::filesystem::path &file, time_type start, time_type end, time_type step) : gdalcpp::Dataset(file.string()),
                                                                                                  _times(detail::stepped_iota(start, end, step)),
                                                                                                  _gT(affine_transformation_coefficients()),
                                                                                                  _origin(compute_origin(_gT)),
                                                                                                  _resolution(compute_resolution(_gT)),
                                                                                                  _extent(compute_extent(_origin, _origin)),
                                                                                                  _width(gdalcpp::Dataset::width()),
                                                                                                  _height(gdalcpp::Dataset::height()),
                                                                                                  _depth(gdalcpp::Dataset::width())
    {
      check_times_equals_depth();
    }

  public:
  
    /// @name Input/Output
    /// @{

    static variable from_file(const std::filesystem::path &file, time_type start, time_type end, time_type step)
    {
      return variable(file, start, end, step);
    }

    /// @brief Export a spatio-temporal variable to a Geotiff file
    /// @param f functor defining the variable: its signature should be equivalent to `double f(location_descriptor, time_descriptor)`
    /// @param g functor defining the locations where f should be called. Signature equivalent to `std::set<location_descriptor> g(time_descriptor)`
    /// @param times the time_descriptor values where f should be called.
    /// @param file the file name where to save
    template <typename Callable1, typename Callable2>
    requires std::invocable<Callable1, location_descriptor, time_descriptor, double> &&
             std::invocable<Callable2, time_descriptor, std::set<location_descriptor> >
    void to_geotiff(Callable1 f, Callable2 g, time_descriptor start, time_descriptor end, const std::filesystem::path &file) const
    {
      export_to_geotiff(f,g, times, file);
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
    latlon<decimal_degree> resolution() const noexcept
    {
      return _resolution;
    }

    /// @brief Extent of the spatial grid
    extent<decimal_degree> extent() const noexcept
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

    /// @}

    /// @name Look-up
    /// @{

    /// @brief Location descriptors (unique identifiers) of the grid cells
    std::iota_view<location_descriptor> locations() const noexcept
    {
      std::ranges::iota_view(0, width() * height() - 1);
    }

    /// @brief Time descriptors (unique identifiers) of the dataset bands
    std::iota_view<time_descriptor> times() const noexcept
    {
      std::ranges::iota_view(0, depth() - 1);
    }

    ///@brief checks if the variable contains a layer with specific time
    bool contains(const latlon &x) const noexcept
    {
      return is_in_spatial_extent(x);
    }

    ///@brief checks if the variable contains a layer with specific time
    bool contains(const time_type &t) const noexcept
    {
        return is_in_temporal_extent(t)
    }

    /// @brief Read the value at location x and time t
    std::optional<value_type> at(location_descriptor x, time_descriptor t) const noexcept
    {
      const auto colrow = to_colrow(x);
      if(colrow.has_value())
        return read(col, row, t);
      return {};
    }

    /// @}

    /// @name Coordinate systems
    /// @{

    /// @brief Location descriptor of the cell identified by its column/row.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return An optional with the descriptor value if x is in the spatial extent, empty otherwise. 
    std::optional<location_descriptor> to_descriptor(const colrow &x) const noexcept
    {
      if (contains(x))
        return {  row * width() + col };
      return {};
    }

    /// @brief Location descriptor of the cell to which the given coordinate belongs.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return An optional with the descriptor value if x is in the spatial extent, empty otherwise. 
    std::optional<location_descriptor> to_descriptor(const latlon<double> &x) const noexcept
    {
      if (contains(x))
        return to_descriptor(to_colrow(x)) ;
      return {};
    }

    /// @brief Column and row of the cell to which the given coordinate belongs.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return An optional with a pair (column, row) if x is in the spatial extent, empty otherwise. 
    std::optional<colrow> to_colrow(const latlon<double> &x) const noexcept
    {
      if (contains(x))
      {
        const & auto [lat, lon] = x;
        const int col = (lon - _gT[0]) / _gT[1];
        const int row = (lat - _gT[3]) / _gT[5];
        return { col, row };        
      }
      return {};
    }

    /// @brief Row and column of the cell to which the given coordinate belongs.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return An optional with a pair (row, column) if x is in the spatial extent, empty otherwise. 
    std::optional<rowcol> to_rowcol(const latlon<double> &x) const noexcept
    {
        auto colrow = to_colrow(x);
        if(colrow.has_value())
          std::swap(colrow->first, colrow->second);
        return colrow;
    }

    /// @brief Column and row of the cell to which the given descriptor belongs.
    /// @param x the location to evaluate.
    /// @return An optional with a pair (column, row) if x is in the spatial extent, empty otherwise. 
    std::optional<colrow> to_colrow(location_descriptor x) const noexcept
    {
      const int col = x % width();
      const int row = x / width();
      if( row < height() )
        return { col , row};
      return {};
    }

    /// @brief Latitude and longitude of the cell to which the given coordinate belongs.
    /// @param x the location to evaluate.
    /// @return An optional with a pair (lat, lon) if x is in the spatial extent, empty otherwise. 
    std::optional<latlon> to_latlon(location_descriptor x) const noexcept
    {
      return to_latlon(to_colrow(x));
    }

    /// @brief Latitude and longitude of the cell identified by its column/row.
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude) 
    latlon to_latlon(const colrow &x) const noexcept
    {
      const auto [col, row] = x;
      const auto lon = _gT[1] * col + _gT[2] * row + _gT[1] * 0.5 + _gT[2] * 0.5 + _gT[0];
      const auto lat = _gT[4] * col + _gT[5] * row + _gT[4] * 0.5 + _gT[5] * 0.5 + _gT[3];
      return {lat, lon};
    }

    /// @brief Longitude and latitude of the deme identified by its column/row.
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude) 
    lonlat to_lonlat(const colrow &x) const noexcept
    {
      const auto [lat, lon] = to_latlon(x);
      return {lon, lat};
    }

    /// @}

    private:

    // Read 1 value using GDAL API. To change if access is too slow
    std::optional<double> read(int x, int y, int bandID) const
    {
      const int nXSize = 1;
      auto line = (float *)CPLMalloc(sizeof(float) * nXSize);
      const auto err = band(bandID).RasterIO(GF_Read, x, y, 1, 1, line, nXSize, 1, GDT_Float32, 0, 0);

      if (err != CE_None)
        throw std::invalid_argument("received negative x y or band ID");
      
      const double val = static_cast<double>(*line);
      CPLFree(line);
      if( val > std::numeric_limits<double>::min())
        return { val };
      return {};
    }

    template <typename Callable1, typename Callable2>
    requires std::invocable<Callable1, location_descriptor, time_descriptor, double> &&
             std::invocable<Callable2, time_descriptor, std::set<location_descriptor> > 
    void to_geotiff(F f, G g, time_descriptor t1, time_descriptor t2, const std::filesystem::path &file) const
    {
      // Number of bands to create:
      const int nBands = std::abs(t2 - t1) + 1;
      assert(nBands >= 1);

      // Create a model dataset with  default values
      GDALDataset *p_sink = get().GetDriver()->Create(file.string().c_str(), width(), height(), nBands, GDT_Float32, NULL);
      std::vector<float> buffer(width() * height());

      // RasterBands begins at 1
      GDALRasterBand *p_source_band = get().GetRasterBand(1);
      const double na = p_source_band->GetNoDataValue();

      // Modify the default data in every band
      for (int i = 1; i <= nBands; ++i)
      {
        [[maybe_unused]] const auto err = p_source_band->RasterIO(GF_Read, 0, 0, width(), height(), buffer.data(), width(), height(), GDT_Float32, 0, 0);

        // Replace all cells where F is defined by F
        auto t_curr = t1 + i - 1;
        auto f_defined_cells = g(t_curr);
        for (coord_type const &it : f_defined_cells)
        {
          auto xy = to_xy(it);
          buffer.at(xy.x + (xy.y * width())) = static_cast<float>(f(it, t_curr));
        }

        // write the data
        GDALRasterBand *p_sink_band = p_sink->GetRasterBand(i);
        p_sink_band->SetNoDataValue(na);
        auto err2 = p_sink_band->RasterIO(GF_Write, 0, 0, width(), height(), buffer.data(), width(), height(), GDT_Float32, 0, 0);
        (void)err2;
        p_sink->FlushCache();

      } // end modify data

      // write metadata
      std::vector<double> geo_transform(6);
      get().GetGeoTransform(geo_transform.data());
      p_sink->SetGeoTransform(geo_transform.data());
      p_sink->SetProjection(get().GetProjectionRef());
      GDALClose((GDALDatasetH)p_sink);
    }

    void to_shapefile(std::map<coord_type, unsigned int> counts, std::string const &filename) const
    {
      std::string driver_name = "ESRI Shapefile";
      GDALDriver *poDriver;
      GDALAllRegister();
      poDriver = GetGDALDriverManager()->GetDriverByName(driver_name.c_str());
      assert(poDriver != NULL);

      GDALDataset *poDS;
      poDS = poDriver->Create(filename.c_str(), 0, 0, 0, GDT_Unknown, NULL);
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
        double x = it.first.lon();
        double y = it.first.lat();
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

    void export_to_shapefile(std::vector<coord_type> coords, std::string const &filename) const
    {
      std::string driver_name = "ESRI Shapefile";
      GDALDriver *poDriver;
      GDALAllRegister();
      poDriver = GetGDALDriverManager()->GetDriverByName(driver_name.c_str());
      assert(poDriver != NULL);

      GDALDataset *poDS;
      poDS = poDriver->Create(filename.c_str(), 0, 0, 0, GDT_Unknown, NULL);
      assert(poDS != NULL);

      OGRLayer *poLayer;
      poLayer = poDS->CreateLayer("sample", NULL, wkbPoint, NULL);
      assert(poLayer != NULL);

      for (auto const &it : coords)
      {
        double x = it.lon();
        double y = it.lat();

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

  private:

    bool is_in_spatial_extent(GeographicCoordinates const &c) const noexcept
    {
      bool is_in_spatial_extent = false;
      if (c.lon() >= _extent.lon_min() && c.lon() < _extent.lon_max() &&
          c.lat() <= _extent.lat_min() && c.lat() > _extent.lat_max())
      {
        is_in_spatial_extent = true;
      }
      return (is_in_spatial_extent);
    }

    void check_times_equals_depth()
    {
      if (_times.size() != depth())
      {
        std::string message("the size of times argument should be equal to the depth of the given dataset: size is ");
        message += std::to_string(times.size());
        message += ", should be ";
        message += std::to_string(depth());
        message += ".";
        throw std::runtime_error(message.c_str());
      }
    }



    // Convert row x and col y to geographic coordinate
    GeographicCoordinates to_lat_lon(unsigned int x, unsigned int y) const
    {
      auto lon = _gT[1] * x + _gT[2] * y + _gT[1] * 0.5 + _gT[2] * 0.5 + _gT[0];
      auto lat = _gT[4] * x + _gT[5] * y + _gT[4] * 0.5 + _gT[5] * 0.5 + _gT[3];
      return GeographicCoordinates(lat, lon);
    }

    // Geographic coordinates of the top left corner.
    latlon compute_origin(std::vector<double> const &gT) const
    {
      return {gT[3], gT[0]};
    }

    // Pixel size in x and y
    Resolution<decimal_degree> compute_resolution(std::vector<double> const &gT) const
    {
      return Resolution<decimal_degree>(gT[5], gT[1]);
    }

    // Top left corner and bottom right corner geographic coordinates
    Extent<decimal_degree> compute_extent(GeographicCoordinates const &o, Resolution<decimal_degree> const &r) const
    {
      auto lon_max = o.lon() + width() * r.lon();
      auto lat_max = o.lat() + height() * r.lat();
      return Extent<decimal_degree>(o.lat(), lat_max, o.lon(), lon_max);
    }

    std::vector<GeographicCoordinates> to_lat_lon(std::vector<XY> const &xy) const
    {
      std::vector<GeographicCoordinates> out;
      for (auto const &it : xy)
      {
        out.push_back(to_lat_lon(it.x, it.y));
      }
      return out;
    }

    bool has_same_domain_at_all_times()
    {
      bool answer = true;
      if (depth() > 1)
      {
        std::vector<unsigned int> bands(depth() - 1);
        std::iota(bands.begin(), bands.end(), 1);
        auto s_0 = get_domain(0);
        auto have_same_domain = [this, &s_0](unsigned int i)
        {
          auto s_i = this->get_domain(i);
          return std::equal(s_0.begin(), s_0.end(), s_i.begin());
        };
        answer = std::all_of(bands.begin(), bands.end(), have_same_domain);
      }
      return answer;
    }
  };

} // namespace quetzal
