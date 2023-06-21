// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ***************************************************************************/

#pragma once

namespace quetzal::geography
{

	/// @brief %extent of a raster grid object
	/// @tparam T the value type used for longitude/latitude.
	/// @ingroup geography
	///
	template <typename T>
	class extent
	{
	
	public:
		/// @typedef type used to represent the values of longitude and latitude
		using value_type = T;

	private:
		value_type _lat_min;
		value_type _lat_max;
		value_type _lon_min;
		value_type _lon_max;

	public:

		/// @name Constructors
		/// @{

		/// @brief Constructor
		/// @param lon_min the minimal longitude of the spatial grid
		/// @param lon_max the mmaximal longitude of the spatial grid
		/// @param lat_min the minimal latitude of the spatial grid
		/// @param lat_max the minimal longitude of the spatial grid
		constexpr explicit extent(value_type lat_min, value_type lat_max, value_type lon_min, value_type lon_max) : _lat_min(lat_min),
																								 _lat_max(lat_max),
																								 _lon_min(lon_min),
																								 _lon_max(lon_max)
		{
		}

		/// @}

		/// @name Comparison
		/// @{

		/// @brief Equality comparison
		/// @param other The other extent object to be compared
		/// @return true if the two objects have same lon_min, lon_max, lat_min, lat_max. Else returns false.
		constexpr bool operator==(const extent &other) const noexcept
		{
			if (lon_min() == other.lon_min() &&
				lon_max() == other.lon_max() &&
				lat_min() == other.lat_min() &&
				lat_max() == other.lat_max())
			{
				return true;
			}
			return false;
		}

		
		/// @brief Unequality comparison
		/// @param other The other extent object to be compared
		/// @return false if the two objects have same lon_min, lon_max, lat_min, lat_max. Else returns true.
		constexpr bool operator!=(const extent &other) const noexcept
		{
			return !(operator==(other));
		}

		/// @}

		/// @name Getters
		/// @{

		/// @brief Minimal latitude of the spatial extent
		constexpr value_type lat_min() const noexcept { return _lat_min; }

		/// @brief Maximal latitude of the spatial extent
		constexpr value_type lat_max() const noexcept { return _lat_max; }

		/// @brief Minimal longitude of the spatial extent
		constexpr value_type lon_min() const noexcept { return _lon_min; }

		/// @brief Get the maximal longitude of the spatial extent
		constexpr value_type lon_max() const noexcept { return _lon_max; }

		/// @}

		/// @name Setters
		/// @{

		/// @brief Minimal latitude of the spatial extent
		constexpr extent &lat_min(value_type val) noexcept
		{
			_lat_min = val;
			return *this;
		}

		/// @brief Maximal longitude of the spatial extent
		constexpr extent &lat_max(value_type val) noexcept
		{
			_lat_max = val;
			return *this;
		}

		/// @brief Minimal longitude of the spatial extent
		constexpr extent &lon_min(value_type val) noexcept
		{
			_lon_min = val;
			return *this;
		}

		/// @brief Maximal longitude of the spatial extent
		constexpr extent &lon_max(value_type val) noexcept
		{
			_lon_max = val;
			return *this;
		}
	}; // end class extent

} // namespace quetzal::geography