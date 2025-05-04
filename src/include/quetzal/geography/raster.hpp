// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#pragma once

#include "coordinates.hpp"
#include "extent.hpp"
#include "gdalcpp.hpp"
#include "resolution.hpp"

#include <range/v3/all.hpp>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <iterator> // std::distance
#include <optional>
#include <stdexcept> // std::invalid_argument
#include <string>
#include <type_traits> // std::is_invocable_r
#include <vector>

namespace quetzal::geography
{

/// @brief Discrete spatio-temporal variations of an environmental variable.
/// @details Read and write georeferenced raster datasets with multiple bands (layers).
///          Multiple rasters can be assembled into a multivariate discrete quetzal::geography::landscape.
/// @ingroup geography
///
template <typename Time = int> class raster
{

  public:
    /// @typedef Time period for every band.
    using time_type = Time;

    /// @typedef Time unique identifier
    using time_descriptor = std::vector<time_type>::size_type;

    /// @typedef Location unique identifier
    using location_descriptor = int;

    /// @typedef A Latitude/Longitude pair of value.
    using latlon = quetzal::geography::latlon;

    /// @typedef A Longitude/Latitude pair of value.
    using lonlat = quetzal::geography::lonlat;

    /// @typedef A column/row pair of indices.
    using colrow = quetzal::geography::colrow;

    /// @typedef A row/column pair of indices.
    using rowcol = quetzal::geography::rowcol;

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
    inline auto compute_resolution(const std::vector<double> &gT) const
    {
        return quetzal::geography::resolution<decimal_degree>(gT[5], gT[1]);
    }

    // Top left corner and bottom right corner geographic coordinates
    inline  auto compute_extent(const latlon &origin, const quetzal::geography::resolution<decimal_degree> &r) const
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
    explicit raster(const std::filesystem::path &file, const std::vector<time_type> &times)
        : _dataset(file.string()), _times(times), _gT(_dataset.affine_transformation_coefficients()),
          _origin(compute_origin(_gT)), _width(_dataset.width()), _height(_dataset.height()), _depth(_dataset.depth()),
          _resolution(compute_resolution(_gT)), _extent(compute_extent(_origin, _resolution)),
          _no_data(nodata_first_band())
    {
        check_times_equals_depth();
    }

    /// @name Input/Output
    /// @{

    /// @brief Read the raster from an input file in the geotiff format
    /// @param file The file to read from
    /// @param times The time periods relative to each band.
    /// @return A raster object
    inline static raster from_file(const std::filesystem::path &file, const std::vector<time_type> &times)
    {
        return raster(file, times);
    }

    /// @brief Export a spatio-temporal raster to a Geotiff file
    /// @param f functor defining the variable: its signature should be equivalent to `std::optional<double>
    /// f(location_descriptor, time_descriptor)`
    /// @param start the first time_descriptor value where f should be called.
    /// @param end the last time_descriptor value where f should be called.
    /// @param file the file name where to save
    /// @remark If the returned optional is empty, the raster NA value will be used.
    template <class Callable>
        requires(std::is_invocable_r_v<std::optional<value_type>, Callable, location_descriptor, time_type>)
    inline void to_geotiff(Callable f, time_type start, time_type end, const std::filesystem::path &file) const
    {
        export_to_geotiff(f, start, end, file);
    }

    /// @brief Export spatial points to a shapefile
    /// @param points the points to export
    /// @param file the path to write to
    inline void to_shapefile(std::vector<latlon> points, const std::filesystem::path &file) const
    {
        points_to_shapefile(points, file);
    }

    /// @brief Export geolocalized counts as spatial-points to a shapefile
    /// @brief counts the number of points at every point
    /// @brief file the path to write to.
    inline void to_shapefile(std::map<latlon, int> counts, const std::filesystem::path &file) const
    {
        counts_to_shapefile(counts, file);
    }

    /// @brief Raster is streamable
    std::ostream &write(std::ostream &stream) const
    {
        stream << "Origin: " << origin() << "\nWidth: " << width() << "\nHeight: " << height() << "\nDepth: " << depth()
               << "\nResolution: "
               << "\n\tLat: " << get_resolution().lat() << "\n\tLon: " << get_resolution().lon() << "\nExtent:"
               << "\n\tLat min: " << get_extent().lat_min() << "\n\tLat max: " << get_extent().lat_max()
               << "\n\tLon min: " << get_extent().lon_min() << "\n\tLon max: " << get_extent().lon_max()
               << "\nNA value: " << NA() << std::endl;
        return stream;
    }

    /// @}

    /// @name Spatial Grid properties
    /// @{

    /// @brief Origin of the spatial grid
    inline latlon origin() const noexcept
    {
        return _origin;
    }

    /// @brief Resolution of the spatial grid
    /// @note The vertical pixel size will be negative (south pointing)
    inline geography::resolution<decimal_degree> get_resolution() const noexcept
    {
        return _resolution;
    }

    /// @brief Extent of the spatial grid
    inline geography::extent<decimal_degree> get_extent() const noexcept
    {
        return _extent;
    }

    /// @brief Width of the spatial grid
    inline int width() const noexcept
    {
        return _width;
    }

    /// @brief Height of the spatial grid
    inline int height() const noexcept
    {
        return _height;
    }

    /// @brief Depth of the spatial grid
    inline int depth() const noexcept
    {
        return _depth;
    }

    /// @brief NA value, as read in the first band of the dataset.
    inline value_type NA() const noexcept
    {
        return _no_data;
    }

    /// @}

    /// @name Look-up
    /// @{

    /// @brief Location descriptors (unique identifiers) of the grid cells
    /// @return A range of unique identifiers for every cell
    inline auto locations() const noexcept
    {
        return ranges::views::iota(0, width() * height());
    }

    /// @brief Time descriptors (unique identifiers) of the dataset bands
    /// @return A range of unique identifiers for every time point recorded
    inline auto times() const noexcept
    {
        // [0 ... depth -1 [
        return ranges::views::iota(0, depth());
    }

    /// @brief Check if a descriptor describes a valid location of the spatial grid.
    /// @param x the descriptor to check
    /// @return True if x is valid, false otherwise.
    inline bool is_valid(location_descriptor x) const noexcept
    {
        return ( x >= 0 and x < locations().size());
    }

    /// @brief Check if the coordinate describes a valid location of the spatial grid.
    /// @param x the column/row to check.
    /// @return True if x is valid, false otherwise.
    inline bool is_valid(const colrow &x) const noexcept
    {
        return x.col >= 0 and x.col < width() and x.row >= 0 and x.row < height() ;
    }

    /// @brief Check if the coordinate describes a valid location of the spatial grid.
    /// @param x the row/column to check.
    /// @return True if x is valid, false otherwise.
    inline bool is_valid(const rowcol &x) const noexcept
    {
        return x.col >= 0 and x.col < width() and x.row >= 0 and x.row < height() ;
    }

    /// @brief Check if the raster contains a coordinate
    /// @param x the lat/lon to check.
    /// @return True if x is valid, false otherwise.    
    inline bool contains(const latlon &x) const noexcept
    {
        return is_in_spatial_extent(x);
    }

    /// @brief Check if the raster contains a coordinate
    /// @param x the lon/lat to check.
    /// @return True if x is valid, false otherwise.
    inline bool contains(const lonlat &x) const noexcept
    {
        return is_in_spatial_extent(to_latlon(x));
    }

    /// @brief Check if the time descriptor is a valid index.
    /// @param t A time descriptor identifying a band of the raster dataset.
    /// @return True if t is a valid index, false otherwise.
    inline bool is_valid(time_descriptor t) const noexcept
    {
        return t >= 0 and t < _times.size();
    }

    /// @brief Search for the exact time in the list of time points recorded by the raster.
    /// @param t An exact point in time
    /// @return True if the time point is found, false otherwise.
    inline bool is_recorded(const time_type &t) const noexcept
    {
        return std::find(_times.begin(), _times.end(), t) != _times.end();
    }

    /// @brief Check if the exact time point falls between the first and last date of record.
    /// @param t An exact point in time
    /// @return True if the time point falls in the temporal range, false otherwise.
    inline bool is_in_interval(const time_type &t) const noexcept
    {
        return t >= _times.front() and t <= _times.back();
    }

    /// @brief Read the value at location x and time t
    /// @param x the location
    /// @param t the time
    /// @return An optional that is empty if the value read is equal to NA.
    /// @pre x is contained in the spatial extent
    /// @pre t is contained in the temporal extent
    inline std::optional<value_type> at(location_descriptor x, time_descriptor t) const noexcept
    {
        assert( is_valid(x) );
        assert( is_valid(t) );
        const auto colrow = to_colrow(x);
        return read(colrow.col, colrow.row, t);
    }

    /// @brief Makes the raster a callable with signature
    ///        `std::optional<value_type> (location_descriptor x, time_descriptor t)`
    ///        returning the value at location x and time t
    /// @remark This callable is cheap to copy, does not have ownership of the data,
    ///         and works well for mathematical composition using `quetzal::expressive`
    inline auto to_view() const noexcept
    {
        return [this](location_descriptor x, time_descriptor t) { return this->at(x, t); };
    }
    /// @}

    /// @name Coordinate systems
    /// @{

    /// @brief Location descriptor of the cell identified by its column/row.
    /// @param x the 1D coordinate to evaluate.
    /// @return The descriptor value of x.
    /// @pre x is in spatial extent.
    inline location_descriptor to_descriptor(const colrow &x) const noexcept
    {
        assert( is_valid(x) );
        return x.col * width() + x.col;
    }

    ///  @brief Location descriptor of the cell to which the given coordinate belongs.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return The descriptor value of x.
    /// @pre x is in the spatial extent.
    inline location_descriptor to_descriptor(const latlon &x) const noexcept
    {
        assert( contains(x) );
        return to_descriptor( to_colrow(x) );
    }

    /// @brief Column and row of the cell to which the given coordinate belongs.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return A pair (column, row).
    /// @pre x is in the spatial extent.
    inline colrow to_colrow(const latlon &x) const noexcept
    {
        assert(contains(x));
        const auto [lat, lon] = x;
        const int col = (lon - _gT[0]) / _gT[1];
        const int row = (lat - _gT[3]) / _gT[5];
        return colrow(col, row);
    }

    /// @brief Row and column of the cell to which the given coordinate belongs.
    /// @param x the latitude/longitude coordinate to evaluate.
    /// @return A pair (row, column).
    /// @pre x is in the spatial extent.
    inline rowcol to_rowcol(const latlon &x) const noexcept
    {
        assert(contains(x));
        auto colrow = to_colrow(x);
        std::swap(colrow.col, colrow.row);
        return colrow;
    }

    /// @brief Column and row of the cell to which the given descriptor belongs.
    /// @param x the location to evaluate.
    /// @return A pair (column, row).
    /// @pre x is in the spatial extent.
    inline colrow to_colrow(location_descriptor x) const noexcept
    {
        assert(is_valid(x));
        const int col = x % width();
        const int row = x / width();
        assert(row < height());
        return colrow(col, row);
    }

    /// @brief Column and row of the cell to which the given descriptor belongs.
    /// @param x the location to evaluate.
    /// @return A pair (column, row).
    /// @pre x is in the spatial extent.
    inline rowcol to_rowcol(location_descriptor x) const noexcept
    {
        assert(is_valid(x));
        auto c = to_colrow(x);
        return rowcol(c.row, c.col);
    }

    /// @brief Latitude and longitude of the cell to which the given coordinate belongs.
    /// @param x the location to evaluate.
    /// @return A pair (lat, lon).
    /// @pre x is in the spatial extent.
    inline latlon to_latlon(location_descriptor x) const noexcept
    {
        assert(is_valid(x));
        return to_latlon(to_colrow(x));
    }

    /// @brief Latitude and longitude of the cell to which the given coordinate belongs.
    /// @param x the location to evaluate.
    /// @return A pair (lon, lat).
    /// @pre x is in the spatial extent
    inline lonlat to_lonlat(location_descriptor x) const noexcept
    {
        assert(is_valid(x));
        return to_lonlat(to_colrow(x));
    }

    /// @brief Latitude and longitude of the cell identified by its column/row.
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude)
    /// @pre x is in the spatial extent
    inline latlon to_latlon(const colrow &x) const noexcept
    {
        assert(is_valid(x));
        const auto [col, row] = x;
        const auto lon = _gT[1] * col + _gT[2] * row + _gT[1] * 0.5 + _gT[2] * 0.5 + _gT[0];
        const auto lat = _gT[4] * col + _gT[5] * row + _gT[4] * 0.5 + _gT[5] * 0.5 + _gT[3];
        return latlon(lat, lon);
    }

    /// @brief Latitude and longitude of the cell identified by its row/column.
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude).
    /// @pre x is in the spatial extent.
    inline latlon to_latlon(const rowcol &x) const noexcept
    {
        assert(is_valid(x));
        return to_latlon(colrow(x.col, x.row));
    }

    /// @brief Longitude and latitude of the deme identified by its column/row.
    /// @param x the location to evaluate.
    /// @return A pair (longitude, latitude).
    /// @pre x is in the spatial extent.
    inline lonlat to_lonlat(const colrow &x) const noexcept
    {
        assert(is_valid(x));
        const auto [lat, lon] = to_latlon(x);
        return lonlat(lon, lat);
    }

    /// @brief Reprojects a coordinate to the centroid of the cell it belongs.
    /// @pre The spatial extent of the raster must contain the coordinate.
    /// @param x The location to reproject
    /// @return The coordinate of the centroid of the cell it belongs.
    /// @pre x is in the spatial extent
    inline latlon to_centroid(const latlon &x) const
    {
        assert(this->contains(x) and 
                "latlon does not belong to spatial extent and can not be reprojected to a cell centroid.");

        return to_latlon(to_colrow(x));
    }

    /// @}

  private:

    /// @brief Latlon to lonlat conversation
    /// @param x the location to evaluate.
    /// @return A pair (longitude, latitude)
    inline lonlat to_lonlat(const latlon &x) const noexcept
    {
        return lonlat(x.lon, x.lat);
    }

    /// @brief Latlon to lonlat conversation
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude)
    inline latlon to_latlon(const lonlat &x) const noexcept
    {
        return latlon(x.lat, x.lon);
    }
    
    /// @brief Read the NA in the first band
    inline value_type nodata_first_band() const
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
        const auto err = _dataset.band(band + 1).RasterIO(GF_Read, col, row, 1, 1, line, nXSize, 1, GDT_Float32, 0, 0);

        if (err != CE_None)
            throw std::invalid_argument("received negative or invalid col row or band index");

        const double val = static_cast<double>(*line);
        CPLFree(line);
        if (val != this->NA())
            return {val};
        return {};
    }

    /// @brief Export a callable to a raster
    template <typename Callable>
    void export_to_geotiff(Callable f, time_type start, time_type end, const std::filesystem::path &file) const
    {
        // Number of bands to create
        const auto times = ranges::views::iota(start, end);
        const int depth = times.size();
        assert(depth >= 1);
        GDALDataset *sink =
            _dataset.get().GetDriver()->Create(file.string().c_str(), width(), height(), depth, GDT_Float32, NULL);

        // Bands begin at 1 in GDAL
        int band = 1;
        for (time_descriptor t : times)
        {
            auto fun = [f, t, this](location_descriptor x) {
                const auto o = f(x, t);
                return o.has_value() ? static_cast<float>(o.value()) : this->NA();
            };

            auto buffer = locations() | ranges::views::transform(fun) | ranges::to<std::vector>();
            GDALRasterBand *sink_band = sink->GetRasterBand(band);
            [[maybe_unused]] const auto err2 = sink_band->RasterIO(GF_Write, 0, 0, width(), height(), buffer.data(),
                                                                   width(), height(), GDT_Float32, 0, 0);
            sink_band->SetNoDataValue(NA());
            sink->FlushCache();
            band++;
        }

        // write metadata
        std::vector<double> geo_transform(6);
        _dataset.get().GetGeoTransform(geo_transform.data());
        sink->SetGeoTransform(geo_transform.data());
        sink->SetProjection(_dataset.get().GetProjectionRef());

        GDALClose(static_cast<GDALDatasetH>(sink));
    }

    /// @brief Export counts to a shapefile
    void counts_to_shapefile(std::map<latlon, int> counts, const std::filesystem::path &file) const
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

    /// @brief Check if there is a 1 to 1 mapping from times to layer
    void check_times_equals_depth()
    {
        if (_times.size() != depth())
        {
            std::string message(
                "the size of times argument should be equal to the depth of the given dataset: size is ");
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

template <typename Time> std::ostream &operator<<(std::ostream &os, const raster<Time> &r)
{
    return r.write(os);
}
} // namespace quetzal::geography
