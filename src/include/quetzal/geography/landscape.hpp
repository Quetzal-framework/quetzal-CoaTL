// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ***************************************************************************/

#pragma once

#include "raster.hpp"

#include <map>
#include <set>
#include <assert.h>
#include <stdexcept>
#include <algorithm>
#include <tuple>	  // std::forward_as_tuple
#include <functional> // std::cref
#include <optional>

namespace quetzal::geography
{
	/// @brief Discrete spatio-temporal variations of a set of environmental variables.
	/// @details  Read and write multiple georeferenced raster datasets with multiple bands (layers). 
  	///           Multiple quetzal::geography::raster objects are assembled into a multivariate discrete quetzal::geography::landscape. 
	///           This geospatial collection is constructed from raster files and gives strong guarantees on grids consistency.
	/// @remark Working with multiple variables can lead to simulation problems if the rasters have even slightly different grid properties,
	///         like different resolutions, origins or extent. To prevent these problems from happening, a `quetzal::geography::landscape` 
	///         wraps multiple quetzal::geography::raster objects into one single coherent object and checks that all rasters are aligned.
	/// @remark A quetzal::geography::landscape shares a large part of its spatial semantics with a quetzal::geography::raster.
	/// @tparam Key A key used to uniquely identifie a variable, e.g. std::string.
	/// @tparam Time Type used as time period for every band, e.g. std::string with `4.2-0.3 ka`
	/// @ingroup geography
	template <typename Key, typename Time>
	class landscape
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
		landscape(const std::map<key_type, std::filesystem::path> &files, const std::vector<time_type> & times) :
		_variables()
		{

			for (auto const &it : files)
			{
				_variables.emplace(std::piecewise_construct, std::forward_as_tuple(it.first), std::forward_as_tuple(it.second, times));
			}

			auto have_same_origin = [this](auto const &it)
			{ return it.second.origin() == _variables.cbegin()->second.origin(); };

			if (!all_of(_variables.cbegin(), _variables.cend(), have_same_origin))
			{
				throw std::runtime_error("all ecological quantities dataset must have same origin");
			}

			auto have_same_extent = [this](auto const &it)
			{ return it.second.get_extent() == _variables.cbegin()->second.get_extent(); };

			if (!all_of(_variables.cbegin(), _variables.cend(), have_same_extent))
			{
				throw std::runtime_error("all ecological quantities dataset must have same extent");
			}

			auto have_same_resolution = [this](auto const &it)
			{ return it.second.get_resolution() == _variables.cbegin()->second.get_resolution(); };

			if (!all_of(_variables.cbegin(), _variables.cend(), have_same_resolution))
			{
				throw std::runtime_error("all ecological quantities dataset must have same resolution");
			}

			auto have_same_depth = [this](auto const &it)
			{ return it.second.depth() == _variables.cbegin()->second.depth(); };

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
		/// @remark error will be thrown if the datasets do not have the exactly same geographical properties (origin, extent, resolution, number of layers).
		static landscape from_files(const std::map<key_type, std::filesystem::path> &files, const std::vector<time_type> & times)
		{
			return landscape(files, times);
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
			_variables.cbegin()->second.to_geotiff(f, start, end, file);
		}

		/// @brief Export spatial points to a shapefile
		/// @param points the points to export
		/// @param file the path to write to
		void to_shapefile(std::vector<latlon> points, const std::filesystem::path &file) const
		{
			_variables.cbegin()->second.to_shapefile(points, file);
		}

		/// @brief Export geolocalized counts as spatial-points to a shapefile
		/// @brief counts the number of points at every point
		/// @brief file the path to write to.
		void to_shapefile(std::map<latlon, int> counts, const std::filesystem::path &file) const
		{
			_variables.cbegin()->second.to_shapefile(counts, file);
		}

		/// @}

		/// @name Capacity
		/// @{

		/// @brief Retrieves the number of variables (rasters)
		/// @return the number of variables
		auto size() const noexcept
		{
			return _variables.size();
		}

		/// @}

		/// @name Element access
		/// @{

		/// @brief Returns a const reference to the mapped raster with key equivalent to `key`.
		///        If no such raster exists, an exception of type std::out_of_range is thrown.
		/// @param key the identifier of the variable to be retrieved
		/// @return the raster object of the specified variable.
		const raster<time_type> & operator[](const key_type &key) const
		{
			return _variables.at(key);
		}

		/// @brief Returns a reference to the mapped raster with key equivalent to `key`.
		///        If no such raster exists, an exception of type std::out_of_range is thrown.
		/// @param key the identifier of the variable to be retrieved
		/// @return the raster object of the specified variable.
		raster<time_type> & operator[](const key_type &key)
		{
			return _variables.at(key);
		}

		/// @}

		/// @name Spatial Grid properties
		/// @{

		/// @brief Origin of the spatial grid
		latlon origin() const noexcept
		{
			return _variables.cbegin()->second.origin();
		}

		/// @brief Resolution of the spatial grid
		resolution<decimal_degree> get_resolution() const noexcept
		{
			return _variables.cbegin()->second.get_resolution();
		}

		/// @brief Extent of the spatial grid
		extent<decimal_degree> get_extent() const noexcept
		{
			return _variables.cbegin()->second.get_extent();
		}

		/// @brief Width of the spatial grid
		int width() const noexcept
		{
			return _variables.cbegin()->second.width();
		}

		/// @brief Height of the spatial grid
		int height() const noexcept
		{
			return _variables.cbegin()->second.height();
		}

		/// @brief Depth of the spatial grid
		int depth() const noexcept
		{
			return _variables.cbegin()->second.depth();
		}

		/// @}

		/// @name Look-up
		/// @{

		/// @brief Location descriptors (unique identifiers) of the grid cells
		auto locations() const noexcept
		{
			return _variables.cbegin()->second.locations();
		}

		/// @brief Time descriptors (unique identifiers) of the dataset bands
		auto times() const noexcept
		{
			return _variables.cbegin()->second.times();
		}

		///@brief checks if the raster contains a layer with specific time
		bool contains(const latlon &x) const noexcept
		{
			return _variables.cbegin()->second.contains(x);
		}

		///@brief checks if the raster contains a layer with specific time
		bool contains(const lonlat &x) const noexcept
		{
			return _variables.cbegin()->second.contains( latlon(x.lat, x.lon) );
		}

		///@brief checks if the raster contains a layer with specific time
		bool contains(const time_type &t) const noexcept
		{
			return _variables.cbegin()->second.contains(t);
		}

		/// @}

		/// @name Coordinate systems
		/// @{

		/// @brief Location descriptor of the cell identified by its column/row.
		/// @param x the latitude/longitude coordinate to evaluate.
		/// @return An optional with the descriptor value if x is in the spatial extent, empty otherwise.
		std::optional<location_descriptor> to_descriptor(const colrow &x) const noexcept
		{
			return _variables.cbegin()->second.to_descriptor(x);
		}

		/// @brief Location descriptor of the cell to which the given coordinate belongs.
		/// @param x the latitude/longitude coordinate to evaluate.
		/// @return An optional with the descriptor value if x is in the spatial extent, empty otherwise.
		std::optional<location_descriptor> to_descriptor(const latlon &x) const noexcept
		{
			return _variables.cbegin()->second.to_descriptor(x);
		}

		/// @brief Column and row of the cell to which the given coordinate belongs.
		/// @param x the latitude/longitude coordinate to evaluate.
		/// @return An optional with a pair (column, row) if x is in the spatial extent, empty otherwise.
		std::optional<colrow> to_colrow(const latlon &x) const noexcept
		{
			return _variables.cbegin()->second.to_colrow(x);
		}

		/// @brief Row and column of the cell to which the given coordinate belongs.
		/// @param x the latitude/longitude coordinate to evaluate.
		/// @return An optional with a pair (row, column) if x is in the spatial extent, empty otherwise.
		std::optional<rowcol> to_rowcol(const latlon &x) const noexcept
		{
			return _variables.cbegin()->second.to_rowcol(x);
		}

		/// @brief Column and row of the cell to which the given descriptor belongs.
		/// @param x the location to evaluate.
		/// @return An optional with a pair (column, row) if x is in the spatial extent, empty otherwise.
		std::optional<colrow> to_colrow(location_descriptor x) const noexcept
		{
			return _variables.cbegin()->second.to_colrow(x);
		}

		/// @brief Latitude and longitude of the cell to which the given coordinate belongs.
		/// @param x the location to evaluate.
		/// @return An optional with a pair (lat, lon) if x is in the spatial extent, empty otherwise.
		std::optional<latlon> to_latlon(location_descriptor x) const noexcept
		{
			return _variables.cbegin()->second.to_latlon(x);
		}

		/// @brief Latitude and longitude of the cell identified by its column/row.
		/// @param x the location to evaluate.
		/// @return A pair (latitude, longitude)
		latlon to_latlon(const colrow &x) const noexcept
		{
			return _variables.cbegin()->second.to_latlon(x);
		}

		/// @brief Longitude and latitude of the deme identified by its column/row.
		/// @param x the location to evaluate.
		/// @return A pair (latitude, longitude)
		lonlat to_lonlat(const colrow &x) const noexcept
		{
			return _variables.cbegin()->second.to_lonlat(x);
		}

		/// @brief Reprojects a coordinate to the centroid of the cell it belongs.
		///        If no such cell exists, an exception of type std::out_of_range is thrown.
		/// @param x The location to reproject
		/// @return The coordinate of the centroid of the cell it belongs.
		latlon to_centroid(const latlon &x) const 
		{
			return _variables.cbegin()->second.to_centroid(x);
		}

		/// @}
	};

} // geography
