// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __GEOGRAPHIC_COORDINATES_H_INCLUDED__
#define __GEOGRAPHIC_COORDINATES_H_INCLUDED__

#include <assert.h>
#include <cmath> // trigonometry
#include <string> // error
#include <ostream>

namespace quetzal {

namespace geography {

class GeographicCoordinates{

public:
	using decimal_degree = double;
	using km = double;

	GeographicCoordinates() = default;

	// construct with decimal degrees
	GeographicCoordinates(decimal_degree lat, decimal_degree lon) : m_lat(lat), m_lon(lon){
		if( (m_lat > 90.) || (m_lat < -90.) || (m_lon < -180.) || (m_lon > 180.))
			throw std::string("invalid coordinates" );
	};

	km great_circle_distance_to(GeographicCoordinates const& other) const {
		km d = distanceEarth(this->m_lat, this->m_lon, other.m_lat, other.m_lon);
		assert(d >= 0.);
		return d;
	}

	decimal_degree lat() const {return m_lat;}

	decimal_degree lon() const {return m_lon;}

	// for using in map
	bool operator<(const GeographicCoordinates& other) const {

    	if(m_lat < other.m_lat) return true;
  		if(m_lat > other.m_lat) return false;

    	//lat == coord.lat

    	if(m_lon < other.m_lon) return true;
    	if(m_lon > other.m_lon) return false;

    	//lat == coord.lat && lon == coord.lon

    	return false;
	}

	bool operator==(const GeographicCoordinates& other) const {
    	if(m_lat == other.m_lat && m_lon == other.m_lon) return true;
    	return false;
    }


private:

	decimal_degree m_lat;
	decimal_degree m_lon;

	// Converts decimal degrees to radians
	double deg2rad(double deg) const {
		double pi = 3.14159265358979323846;
 		return (deg * pi / 180);
	}


	// Converts radians to decimal degrees
	double rad2deg(double rad) const {

		double pi = 3.14159265358979323846;
		return (rad * 180 / pi);
	}

	/**
 	* Returns the distance between two points on the Earth.
	* Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
 	* @param lat1d Latitude of the first point in degrees
	* @param lon1d Longitude of the first point in degrees
 	* @param lat2d Latitude of the second point in degrees
 	* @param lon2d Longitude of the second point in degrees
 	* @return The distance between the two points in kilometers
 	*/
	km distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d) const {

	double earthRadiusKm = 6371.0;
  	double lat1r, lon1r, lat2r, lon2r, u, v;

  	lat1r = deg2rad(lat1d);
  	lon1r = deg2rad(lon1d);
  	lat2r = deg2rad(lat2d);
  	lon2r = deg2rad(lon2d);

  	u = sin((lat2r - lat1r)/2);
  	v = sin((lon2r - lon1r)/2);

	km d = 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));

	assert(d >= 0);

  	return d;
	}

};


std::ostream& operator <<(std::ostream& Stream, GeographicCoordinates const& coord)
{
    Stream << "(" << coord.lat() << " " << coord.lon() << ")";
    return Stream; // N'oubliez pas de renvoyer le flux, afin de pouvoir chaîner les appels
}

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

} // namespace geography

} // namespace quetzal

namespace std
{
  template<> struct hash<quetzal::geography::GeographicCoordinates>
  {
    inline size_t operator()(const quetzal::geography::GeographicCoordinates & v) const
    {
      size_t seed = 0;
      quetzal::geography::hash_combine(seed, v.lon());
      quetzal::geography::hash_combine(seed, v.lat());
      return seed;
    }
  };
}

#endif
