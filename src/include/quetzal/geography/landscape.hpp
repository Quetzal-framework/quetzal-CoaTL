// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#pragma once

#include "raster.hpp"

#include <algorithm>
#include <assert.h>
#include <functional> // std::cref
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple> // std::forward_as_tuple

namespace quetzal::geography
{
/// @brief Discrete spatio-temporal variations of a set of environmental variables.
/// @details  Read and write multiple geo-referenced raster datasets with multiple bands (layers).
///           Multiple quetzal::geography::raster objects are assembled into a multivariate discrete
///           quetzal::geography::landscape. This geo-spatial collection is constructed from raster files and gives
///           strong guarantees on grids consistency.
/// @remark Working with multiple variables can lead to simulation problems if the rasters have even slightly different
///         grid properties,
///         like different resolutions, origins or extent. To prevent these problems from happening, a
///         `quetzal::geography::landscape` wraps multiple quetzal::geography::raster objects into one single coherent
///         object and checks that all rasters are aligned.
/// @remark A quetzal::geography::landscape shares a large part of its spatial semantics with a
/// quetzal::geography::raster.
/// @tparam Key A key used to uniquely identify a variable, e.g. std::string.
/// @tparam Time Type used as time period for every band, e.g. std::string with `4.2-0.3 ka`
/// @ingroup geography
template <typename Key = std::string, typename Time = int> class landscape
{

  public:
    //! \typedef ecological quantities identifier type
    using key_type = Key;

    //! \typedef Time period for every band.
    using time_type = Time;

    /// @typedef Location unique identifier
    using location_descriptor = typename raster<time_type>::location_descriptor;

    /// @typedef Time unique identifier
    using time_descriptor = typename raster<time_type>::time_descriptor;

    /// @typedef A Latitude/Longitude pair of value.
    using latlon = typename raster<time_type>::latlon;

    /// @typedef A Longitude/Latitude pair of value.
    using lonlat = typename raster<time_type>::lonlat;

    /// @typedef A column/row pair of indices.
    using colrow = typename raster<time_type>::colrow;

    /// @typedef A row/column pair of indices.
    using rowcol = typename raster<time_type>::rowcol;

    /// @typedef decimal degree type
    using decimal_degree = typename raster<time_type>::decimal_degree;

  private:
    std::map<key_type, raster<Time>> _variables;

    /// @brief Constructor
    landscape(const std::map<key_type, std::filesystem::path> &files, const std::vector<time_type> &times)
        : _variables()
    {

        for (auto const &it : files)
        {
            _variables.emplace(std::piecewise_construct, std::forward_as_tuple(it.first),
                               std::forward_as_tuple(it.second, times));
        }

        auto have_same_origin = [this](auto const &it) {
            return it.second.origin() == _variables.cbegin()->second.origin();
        };

        if (!all_of(_variables.cbegin(), _variables.cend(), have_same_origin))
        {
            throw std::runtime_error("all ecological quantities dataset must have same origin");
        }

        auto have_same_extent = [this](auto const &it) {
            return it.second.get_extent() == _variables.cbegin()->second.get_extent();
        };

        if (!all_of(_variables.cbegin(), _variables.cend(), have_same_extent))
        {
            throw std::runtime_error("all ecological quantities dataset must have same extent");
        }

        auto have_same_resolution = [this](auto const &it) {
            return it.second.get_resolution() == _variables.cbegin()->second.get_resolution();
        };

        if (!all_of(_variables.cbegin(), _variables.cend(), have_same_resolution))
        {
            throw std::runtime_error("all ecological quantities dataset must have same resolution");
        }

        auto have_same_depth = [this](auto const &it) {
            return it.second.depth() == _variables.cbegin()->second.depth();
        };

        if (!all_of(_variables.cbegin(), _variables.cend(), have_same_depth))
        {
            throw std::runtime_error("all ecological quantities dataset must have same depth");
        }
    }

  public:
    /// @name Input/Output
    /// @{

    /// @brief Read the rasters from a set of input geotiff files
    /// @param files A map giving as keys the name of a variable, and as value a path to read it from.
    /// @param start The first time period to map to the first band
    /// @param end The last time period to map to the last band
    /// @param step The time increment between each band
    /// @return A raster object
    /// @remark error will be thrown if the datasets do not have the exactly same geographical properties (origin,
    /// extent, resolution, number of layers).
    inline static landscape from_file(const std::map<key_type, std::filesystem::path> &files,
                                const std::vector<time_type> &times)
    {
        return landscape(files, times);
    }

    /// @brief Export a spatio-temporal raster to a Geotiff file
    /// @param f functor defining the variable: its signature should be equivalent to `double f(location_descriptor,
    /// time_descriptor)`
    /// @param start the first time_descriptor value where f should be called.
    /// @param end the last time_descriptor value where f should be called.
    /// @param file the file name where to save
    template <typename Callable>
        requires std::invocable<Callable, location_descriptor, time_descriptor, std::optional<double>>
    inline void to_geotiff(Callable f, time_descriptor start, time_descriptor end, const std::filesystem::path &file) const
    {
        _variables.cbegin()->second.to_geotiff(f, start, end, file);
    }

    /// @brief Export spatial points to a shapefile
    /// @param points the points to export
    /// @param file the path to write to
    inline void to_shapefile(std::vector<latlon> points, const std::filesystem::path &file) const
    {
        _variables.cbegin()->second.to_shapefile(points, file);
    }

    /// @brief Export geolocalized counts as spatial-points to a shapefile
    /// @brief counts the number of points at every point
    /// @brief file the path to write to.
    inline void to_shapefile(std::map<latlon, int> counts, const std::filesystem::path &file) const
    {
        _variables.cbegin()->second.to_shapefile(counts, file);
    }

    /// @brief Landscape is streamable
    std::ostream &write(std::ostream &stream) const
    {
        stream << "Landscape of " << _variables.size() << " aligned rasters:\n";

        for (const auto &it : _variables)
            stream << it.first << " ";

        stream << "\nOrigin: " << origin() << "\nWidth: " << width() << "\nHeight: " << height()
               << "\nDepth: " << depth() << "\nResolution: "
               << "\n\tLat: " << get_resolution().lat() << "\n\tLon: " << get_resolution().lon() << "\nExtent:"
               << "\n\tLat min: " << get_extent().lat_min() << "\n\tLat max: " << get_extent().lat_max()
               << "\n\tLon min: " << get_extent().lon_min() << "\n\tLon max: " << get_extent().lon_max();
        return stream;
    }

    /// @}

    /// @name Capacity
    /// @{

    /// @brief Retrieves the number of variables (rasters)
    /// @return the number of variables
    inline auto num_variables() const noexcept
    {
        return _variables.size();
    }

    /// @brief Number of cells in the raster
    /// @return the number of variables
    inline auto num_locations() const noexcept
    {
        return this->width() * this->height();
    }

    /// @}

    /// @name Element access
    /// @{

    /// @brief Returns a const reference to the mapped raster with key equivalent to `key`.
    ///        If no such raster exists, an exception of type std::out_of_range is thrown.
    /// @param key the identifier of the variable to be retrieved
    /// @return the raster object of the specified variable.
    inline const raster<time_type> &operator[](const key_type &key) const
    {
        return _variables.at(key);
    }

    /// @brief Returns a reference to the mapped raster with key equivalent to `key`.
    ///        If no such raster exists, an exception of type std::out_of_range is thrown.
    /// @param key the identifier of the variable to be retrieved
    /// @return the raster object of the specified variable.
    inline raster<time_type> &operator[](const key_type &key)
    {
        return _variables.at(key);
    }

    /// @}

    /// @name Spatial Grid properties
    /// @{

    /// @brief Origin of the spatial grid
    inline latlon origin() const noexcept
    {
        return _variables.cbegin()->second.origin();
    }

    /// @brief Resolution of the spatial grid
    inline resolution<decimal_degree> get_resolution() const noexcept
    {
        return _variables.cbegin()->second.get_resolution();
    }

    /// @brief Extent of the spatial grid
    inline extent<decimal_degree> get_extent() const noexcept
    {
        return _variables.cbegin()->second.get_extent();
    }

    /// @brief Width of the spatial grid
    inline int width() const noexcept
    {
        return _variables.cbegin()->second.width();
    }

    /// @brief Height of the spatial grid
    inline int height() const noexcept
    {
        return _variables.cbegin()->second.height();
    }

    /// @brief Depth of the spatial grid
    inline int depth() const noexcept
    {
        return _variables.cbegin()->second.depth();
    }

    /// @}

    /// @name Look-up
    /// @{

    /// @brief Location descriptors (unique identifiers) of the grid cells
    inline auto locations() const noexcept
    {
        return _variables.cbegin()->second.locations();
    }

    /// @brief Time descriptors (unique identifiers) of the dataset bands
    inline auto times() const noexcept
    {
        return _variables.cbegin()->second.times();
    }

    ///@brief checks if the raster contains a coordinate
    inline bool is_valid(location_descriptor x) const noexcept
    {
        return _variables.cbegin()->second.is_valid(x);
    }

    ///@brief checks if the raster contains a coordinate
    inline bool contains(const latlon &x) const noexcept
    {
        return _variables.cbegin()->second.contains(x);
    }

    ///@brief checks if the raster contains a coordinate
    inline bool contains(const lonlat &x) const noexcept
    {
        return _variables.cbegin()->second.contains(x);
    }

    ///@brief checks if the raster contains a coordinate
    inline bool is_valid(const colrow &x) const noexcept
    {
        return _variables.cbegin()->second.is_valid(x);
    }

    ///@brief checks if the raster contains a coordinate
    inline bool is_valid(const rowcol &x) const noexcept
    {
        return _variables.cbegin()->second.is_valid(x);
    }

    ///@brief checks if the raster contains a coordinate
    inline bool is_valid(time_descriptor t) const noexcept
    {
        return _variables.cbegin()->second.is_valid(t);
    }

    ///@brief checks if the raster temporal extent contains a time point
    inline bool is_recorded(const time_type &t) const noexcept
    {
        return _variables.cbegin()->second.is_recorded(t);
    }

    ///@brief checks if the raster temporal extent contains a time point
    inline bool is_within_span(const time_type &t) const noexcept
    {
        return _variables.cbegin()->second.is_within_span(t);
    }

    /// @}

    /// @name Coordinate systems
    /// @{

    /// @brief Location descriptor of the cell identified by its column/row.
    /// @param x the coordinate to evaluate.
    /// @return The descriptor value.
    /// @pre x is in spatial extent.
    inline location_descriptor to_descriptor(const colrow &x) const noexcept
    {
        assert( is_valid(x) );
        return _variables.cbegin()->second.to_descriptor(x);
    }

    /// @brief Location descriptor of the cell to which the given coordinate belongs.
    /// @param x the coordinate to evaluate.
    /// @return The descriptor value.
    /// @pre x is in spatial extent.
    inline location_descriptor to_descriptor(const latlon &x) const noexcept
    {
        assert( contains(x) );
        return _variables.cbegin()->second.to_descriptor(x);
    }

    /// @brief Column and row of the cell to which the given coordinate belongs.
    /// @param x the coordinate to evaluate.
    /// @return A pair (column, row).
    /// @pre x is in spatial extent.
    colrow to_colrow(const latlon &x) const noexcept
    {
        assert( contains(x) );
        return _variables.cbegin()->second.to_colrow(x);
    }

    /// @brief Row and column of the cell to which the given coordinate belongs.
    /// @param x the coordinate to evaluate.
    /// @return A pair (row, column).
    /// @pre x is in spatial extent.
    inline rowcol to_rowcol(const latlon &x) const noexcept
    {
        assert( contains(x) );
        return _variables.cbegin()->second.to_rowcol(x);
    }

    /// @brief Column and row of the cell to which the given descriptor belongs.
    /// @param x the location to evaluate.
    /// @return A pair (column, row).
    /// @pre x is in spatial extent.
    inline colrow to_colrow(location_descriptor x) const noexcept
    {
        assert( is_valid(x) );
        return _variables.cbegin()->second.to_colrow(x);
    }

    /// @brief Latitude and longitude of the cell to which the given coordinate belongs.
    /// @param x the location to evaluate.
    /// @return A pair (lat, lon).
    /// @pre x is in spatial extent.
    inline latlon to_latlon(location_descriptor x) const noexcept
    {
        assert( is_valid(x) );
        return _variables.cbegin()->second.to_latlon(x);
    }

    /// @brief Longitude and latitude of the cell to which the given coordinate belongs.
    /// @param x the location to evaluate.
    /// @return A pair (lon, lat).
    /// @pre x is in spatial extent
    inline lonlat to_lonlat(location_descriptor x) const noexcept
    {
        assert( is_valid(x) );
        return _variables.cbegin()->second.to_lonlat(x);
    }
    
    /// @brief Latitude and longitude of the cell identified by its column/row.
    /// @param x the location to evaluate.
    /// @return A pair (latitude, longitude).
    /// @pre x is in spatial extent.
    inline latlon to_latlon(const colrow &x) const noexcept
    {
        assert( is_valid(x) );
        return _variables.cbegin()->second.to_latlon(x);
    }

    /// @brief Longitude and latitude of the deme identified by its column/row.
    /// @param x the location to evaluate.
    /// @return A pair (longitude, latitude)
    /// @pre x is in spatial extent
    inline lonlat to_lonlat(const colrow &x) const noexcept
    {
        assert( is_valid(x) );
        return _variables.cbegin()->second.to_lonlat(x);
    }

    /// @brief Reprojects a coordinate to the centroid of the cell it belongs.
    /// @pre The landscape must contain the coordinate in its spatial extent.
    /// @param x The location to reproject
    /// @return The coordinate of the centroid of the cell it belongs.
    /// @pre x is in spatial extent
    inline latlon to_centroid(const latlon &x) const
    {
        assert(this->contains(x));
        return _variables.cbegin()->second.to_centroid(x);
    }

    /// @}
};

template <typename Key, typename Time> std::ostream &operator<<(std::ostream &os, const landscape<Key, Time> &l)
{
    return l.write(os);
}

} // namespace quetzal::geography
