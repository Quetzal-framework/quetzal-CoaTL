// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#pragma once

#include <assert.h>
#include <cmath> // trigonometry
#include <ostream>
#include <tuple>
#include <numbers>

#include <mp-units/systems/si.h>
#include <mp-units/ostream.h>

namespace quetzal::geography
{

using std::numbers::pi;

class latlon;
class lonlat;
class rowcol;
class colrow;

/// @brief Requires a type to define a row and a col member.
template <typename T>
concept GridCoordinate = requires(T t) {
                             {
                                 t.col <=> t.row
                             };
                         };

/// @brief GridCoordinates are equality comparable
template <GridCoordinate C1, GridCoordinate C2> auto operator==(const C1 &c1, const C2 &c2)
{
    return std::tie(c1.col, c1.row) == std::tie(c2.col, c2.row);
}

/// @brief GridCoordinates are three-way comparable
template <GridCoordinate C1, GridCoordinate C2> auto operator<=>(const C1 &c1, const C2 &c2)
{
    return std::tie(c1.col, c1.row) <=> std::tie(c2.col, c2.row);
}

/// @brief GridCoordinates are streamable
template <GridCoordinate C> std::ostream &operator<<(std::ostream &stream, const C &c)
{
    stream << "(Column: " << c.col << ", Row: " << c.row << ")";
    return stream;
}

/// @brief Grid coordinates
/// @details Row/column indices
/// @ingroup geography
struct rowcol
{
    /// @typedef Latitude and Longitude value type
    using value_type = int;

    /// @brief Row descriptor
    value_type row;

    /// @brief Column descriptor
    value_type col;

    /// @brief Constructor
    constexpr explicit rowcol(value_type row, value_type column) noexcept : row(row), col(column)
    {
    }
};
static_assert(GridCoordinate<rowcol>);

/// @brief Grid coordinates
/// @details Column/row indices
/// @ingroup geography
struct colrow
{
    /// @typedef Latitude and Longitude value type
    using value_type = int;

    /// @brief Column descriptor
    value_type col;

    /// @brief Row descriptor
    value_type row;

    /// @brief Constructor
    constexpr explicit colrow(value_type column, value_type row) noexcept : col(column), row(row)
    {
    }
};
static_assert(GridCoordinate<colrow>);

/// @brief Requires a type to define a lat and a lon member.
template <typename T>
concept Coordinate = requires(T t) {
                         {
                             t.lon <=> t.lat
                         };
                     };

/// @brief Coordinates are equality comparable
template <Coordinate C1, Coordinate C2> auto operator==(const C1 &c1, const C2 &c2)
{
    return std::tie(c1.lat, c1.lon) == std::tie(c2.lat, c2.lon);
}

/// @brief Coordinates are three-way comparable
template <Coordinate C1, Coordinate C2> auto operator<=>(const C1 &c1, const C2 &c2)
{
    return std::tie(c1.lat, c1.lon) <=> std::tie(c2.lat, c2.lon);
}

/// @brief Coordinates are streamable
template <Coordinate C> std::ostream &operator<<(std::ostream &stream, const C &c)
{
    stream << "(Lat: " << c.lat << ", Lon: " << c.lon << ")";
    return stream;
}

namespace details
{

/// @brief Check lat lon consistency
void check(double lat, double lon)
{
    if ((lat > 90.) || (lat < -90.) || (lon < -180.) || (lon > 180.))
        throw std::invalid_argument("invalid coordinates");
}

/// @brief Converts decimal degrees to radians
constexpr double deg2rad(double deg) noexcept
{
    return (deg * pi / 180.0);
}

/// @brief Converts radians to decimal degrees
constexpr double rad2deg(double rad) noexcept
{
    return (rad * 180.0 / pi);
}

/// @brief Returns the distance between two points on the Earth in kilometers
/// @details Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
/// @param lat1d Latitude of the first point in degrees
/// @param lon1d Longitude of the first point in degrees
/// @param lat2d Latitude of the second point in degrees
/// @param lon2d Longitude of the second point in degrees
/// @return The distance between the two points in kilometers
constexpr double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d)
{

    const double earthRadiusKm = 6371.0;

    const double lat1r = deg2rad(lat1d);
    const double lon1r = deg2rad(lon1d);
    const double lat2r = deg2rad(lat2d);
    const double lon2r = deg2rad(lon2d);

    const double u = sin((lat2r - lat1r) / 2);
    const double v = sin((lon2r - lon1r) / 2);

    const double d = 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
    assert(d >= 0);
    return d;
}

} // namespace details

/// @brief Geographic coordinates.
/// @details Geographic coordinates (longitude and latitude) in decimal degree.
/// @ingroup geography
struct latlon
{
    /// @typedef Latitude and Longitude value type
    using decimal_degree = double;

    /// @brief Latitude
    decimal_degree lat;

    /// @brief Longitude
    decimal_degree lon;

    /// @brief Constructor
    constexpr explicit latlon(decimal_degree latitude, decimal_degree longitude) noexcept
        : lat(latitude), lon(longitude)
    {
    }

    /// @brief Compute great-circle distance between two points on the Earth.
    /// @details Compute the great circle distance (in kilometers) from this point to an other coordinate.
    /// @remark Implementation follows a direct translation of <a
    /// href="http://en.wikipedia.org/wiki/Haversine_formula">Haversine formula</a>.
    /// @tparam T Either quetzal::geography::latlon or quetzal::geography::lonlat
    /// @param other the other coordinate
    /// @return the distance in kilometers
    /// @invariant The returned distance is not negative.
    template <class T> constexpr 
    mp_units::quantity<mp_units::si::metre> great_circle_distance_to(const T &other) const noexcept
    {
        const double d = details::distanceEarth(this->lat, this->lon, other.lat, other.lon);
        assert(d >= 0.);
        return d * 1000.0 * mp_units::si::metre;
    }
};

static_assert(Coordinate<latlon>);

/// @brief Geographic coordinates.
/// @details Geographic coordinates (longitude and latitude) in decimal degree.
/// @ingroup geography
struct lonlat
{
    /// @typedef Latitude and Longitude value type
    using decimal_degree = double;

    /// @brief Longitude
    decimal_degree lon;

    /// @brief Latitude
    decimal_degree lat;

    /// @brief Constructor
    constexpr explicit lonlat(decimal_degree longitude, decimal_degree latitude) noexcept
        : lon(longitude), lat(latitude)
    {
    }

    /// @brief Compute great-circle distance between two points on the Earth.
    /// @details Compute the great circle distance (in kilometers) from this point to an other coordinate.
    /// @remark Implementation follows a direct translation of <a
    /// href="http://en.wikipedia.org/wiki/Haversine_formula">Haversine formula</a>.
    /// @tparam T Either quetzal::geography::latlon or quetzal::geography::lonlat
    /// @param other the other coordinate
    /// @return the distance in kilometers
    /// @invariant The returned distance is not negative.
    template <class T> 
    constexpr 
    mp_units::quantity<mp_units::si::metre> great_circle_distance_to(const T &other) const noexcept
    {
        const double d = details::distanceEarth(this->lat, this->lon, other.lat, other.lon);
        assert(d >= 0.0);
        return d * 1000.0 * mp_units::si::metre;
    }

}; // end struct latlon
static_assert(Coordinate<lonlat>);

} // namespace quetzal::geography
