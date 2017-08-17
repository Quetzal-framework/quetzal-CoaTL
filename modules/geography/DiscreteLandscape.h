// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __DISCRETE_LANDSCAPE_H_INCLUDED
#define __DISCRETE_LANDSCAPE_H_INCLUDED

#include <map>
#include <set>
#include <assert.h>
#include <stdexcept>

#include "gdalcpp.h"
#include "GeographicCoordinates.h"

namespace quetzal {
	namespace geography {

	/*!
	 * \brief Discretized representation of a landscape and its ecological
	 *        quantities through time and space
	 *
	 * \tparam Key      Ecological quantities identifier
	 * \tparam Time     Time type
	 * \ingroup geography
	 */
template<typename Key, typename Time>
class DiscreteLandscape
{

	using geo_data_type = gdalcpp::Dataset;

public:

	//! \typedef ecological quantities identifier type
	using key_type = Key;

	//! \typedef coordinate type
	using coord_type = GeographicCoordinates;

	//! \typedef time type
	using time_type = Time;

	//! \typedef decimal degree type
	using decimal_degree = GeographicCoordinates::decimal_degree;

private:

	struct Resolution{
			Resolution(decimal_degree x_res, decimal_degree y_res) : x(x_res), y(y_res){}
			decimal_degree x;
			decimal_degree y;
	};

	struct Extent{
			Extent(decimal_degree xmin, decimal_degree xmax, decimal_degree ymin, decimal_degree ymax):
			x_min(xmin),
			x_max(xmax),
			y_min(ymin),
			y_max(ymax){}

			decimal_degree x_min;
			decimal_degree x_max;
			decimal_degree y_min;
			decimal_degree y_max;
	};

	struct XY{
		XY(unsigned int col, unsigned int row) : x(col), y(row){}
		unsigned int x;
		unsigned int y;
	};

	std::map<key_type, geo_data_type> m_quantities;
	std::set<coord_type> m_geographic_definition_space;
	std::set<time_type> m_times;
	std::vector<double> m_gT;

	auto read(std::map<key_type, std::string> const& files){
		for(auto const& it : files){
			m_quantities.emplace(it.first, geo_data_type(it.second));
		}
	}

	template<typename Quantity>
	auto get_geographic_definition_space(Quantity const& q){

	}
	
	auto retrieve_coords() const {
		auto first = m_quantities.begin().geographic_definition_space();
		auto pred = [&first](auto const& q){return std::equal(first.begin(), first.end(), get_geographic_definition_space(q));};
		bool are_same = std::all_of( ++m_quantities.begin(), m_quantities.end(), pred);
		if( ! are_same){
			throw std::runtime_error("All ecological quantities do not have same set of defined-value coordinates");
		}
		return first;
	}

public:
	//! Constructor reading ecological quantities at path to geotiff files.
	//! \remark error will be throw if data do not have same temporal definition space.
	//! \remark error will be throw if data do not have same geographic definition space
	//!         (coordinates of cells with defined value)
	//! \remark For now all quantities are casted to double.
	DiscreteLandscape(std::map<key_type, std::string> const& files, std::set<Time> const& times):
	m_quantities(read(files)),
	m_times(times),
	m_geographic_definition_space(retrieve_coords()),
	m_gT(m_quantities.begin().affine_transformation_coefficients())
	{}

	//! Number of ecological quantities represented in the Environment.
	auto quantities_nbr() const {
		return m_quantities.size();
	}

	//! Ecological quantity as a function of space and time
	const auto&  operator [](key_type const& k) const {
		assert(m_quantities.find(k) != m_quantities.end());
		if(m_quantities[k].depth() == 1){
			return [this,k](coord_type const& x, time_type const& t){
				assert(is_in_spatial_extent(x));
				auto xy = lat_lon_to_xy(x);
				return m_quantities.at(k).read(xy.x, xy.y, 0);
		 };
	 }else if(m_quantities[k].depth() == m_times.size()){
			return [this,k](coord_type const& x, time_type const& t){
				assert(is_in_spatial_extent(x));
				auto xy = lat_lon_to_xy(x);
				return m_quantities.at(k).read(xy.x, xy.y, m_times.at(t));
		 };
		}else{
			throw std::runtime_error("GDAL dataset have inconsistent depth");
		}
	}

	//! Return the times for which all ecological quantities are defined.
	const std::set<time_type>& temporal_definition_space() const {
		return m_times;
	}

	//! Return the geographic coordinates for which all ecological quantities are defined at all times
	const std::set<coord_type>& geographic_definition_space() const {
		return m_geographic_definition_space;
	}

	// Geographic coordinates of the top left corner.
	coord_type origin() const {
		return coord_type(m_gT[3], m_gT[0]) ;
	}

	// Pixel size in x and y
	Resolution resolution() const {
			return Resolution(m_gT[1], m_gT[5]);
	}

	// Top left corner and bottom right corner geographic coordinates
	Extent extent() const {
		auto xmin = this->origin().lon();
		auto ymin = this->origin().lat();
		auto xmax = xmin + this->width() * this->resolution().x;
		auto ymax = ymin + this->height() * this->resolution().y;
		return Extent(xmin, xmax, ymin, ymax);
	}

	// Check if a given geographic coordinate is in the landscpae spatial extent
	bool is_in_spatial_extent(coord_type const& c) const {
		bool is_in_spatial_extent = false;
		auto extent = this->extent();
		if( c.lon() >= extent.x_min && c.lon() < extent.x_max &&
				c.lat() <= extent.y_max && c.lat() > extent.y_min      )
		{
			is_in_spatial_extent = true;
		}
		return(is_in_spatial_extent);
	}

	// Coordinate of the centroid of the landscape cell to which the given coordinate belongs
	coord_type reproject_to_centroid(coord_type const& c) const {
		if( !is_in_spatial_extent(c)){
			throw std::runtime_error(std::string("coordinate to be reprojected (" + std::to_string(c.lat()) + ";" + std::to_string(c.lon()) + ")" +
			                    "does not belong to landscape spatial extent"));
		}
		auto xy = lat_lon_to_xy(c);
		return xy_to_lat_lon(xy.first, xy.second);
	}

	// Convert row x and col y to geographic coordinate
	coord_type xy_to_lat_lon(unsigned int x, unsigned int y) const {
		auto lon = m_gT[1] * x + m_gT[2] * y + m_gT[1] * 0.5 + m_gT[2] * 0.5 + m_gT[0];
		auto lat = m_gT[4] * x + m_gT[5] * y + m_gT[4] * 0.5 + m_gT[5] * 0.5 + m_gT[3];
		return coord_type(lat, lon);
	}

	// Convert latitude/longitude to col/row indices
	XY lat_lon_to_xy(coord_type const& c) const {
		int col = (c.lon() - m_gT[0]) / m_gT[1];
		int row = (c.lat() - m_gT[3]) / m_gT[5];
		return XY(col, row);
	}

};

} // geography
} // quetzal

#endif
