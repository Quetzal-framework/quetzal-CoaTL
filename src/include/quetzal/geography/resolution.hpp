// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#pragma once

namespace quetzal::geography
{
/// @brief Resolution of a spatial grid
/// @ingroup geography
template <typename T> class resolution
{

  private:
    T _lat;
    T _lon;

  public:
    /// @typedef type used to represent the values of  latitude and longitude
    using value_type = T;

    /// @brief Constructor
    /// @param lat the resolution of the spatial grid latitude
    /// @param lon the resolution of the spatial grid longitude
    constexpr explicit resolution(value_type lat, value_type lon) : _lat(lat), _lon(lon)
    {
    }

    /// @brief Gets latitude resolution
    /// @note Negative in decimal degree if the raster is north-up oriented.
    constexpr value_type lat() const noexcept
    {
        return _lat;
    }

    /// @brief Gets longitude resolution
    constexpr value_type lon() const noexcept
    {
        return _lon;
    }

    /// @brief Sets latitude resolution
    /// @param value the new value for latitude resolution.
    /// @return a reference on the Resolution object.
    constexpr resolution &lat(value_type value) noexcept
    {
        _lat = value;
        return *this;
    }

    /// @brief Sets longitude resolution
    /// @param value the new value for longitude resolution.
    /// @return a reference on the Resolution object.
    constexpr resolution &lon(value_type value) noexcept
    {
        _lon = value;
        return *this;
    }

    /// @brief EqualityComparable
    /// @details Checks if two resolutions objects are equals
    /// @return true if latitude and longitude have same resolution, else returns false.
    constexpr bool operator==(const resolution &other) const noexcept
    {
        if (_lat == other.lat() && _lon == other.lon())
            return true;
        return false;
    }

    /// @brief Unequality comparison operator
    /// @details Checks if two resolutions objects are equals
    /// @return false if latitude and longitude have same resolution, else returns true.
    constexpr bool operator!=(const resolution &other) const noexcept
    {
        return !(operator==(other));
    }
};

} // namespace quetzal::geography