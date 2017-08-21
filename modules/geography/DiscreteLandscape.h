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
#include <algorithm>
#include <tuple>

#include "EnvironmentalQuantity.h"

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

	using geo_data_type = EnvironmentalQuantity<Time>;

public:

	//! \typedef ecological quantities identifier type
	using key_type = Key;

	//! \typedef coordinate type
	using coord_type = typename geo_data_type::coord_type;

	//! \typedef time type
	using time_type = Time;

	//! \typedef decimal degree type
	using decimal_degree = typename GeographicCoordinates::decimal_degree;

private:

	std::map<key_type, geo_data_type> m_quantities;

	void build(std::map<key_type, std::string> const& files, std::set<Time> times){
		for(auto const& it : files){
			m_quantities.emplace( std::piecewise_construct,
				                    std::forward_as_tuple(it.first),
														std::forward_as_tuple(it.second, std::vector<Time>(times.begin(), times.end())));
		}
	}

public:

	//! Constructor reading ecological quantities at path to geotiff files.
	//! \remark error will be throw if data do not have same temporal definition space.
	//! \remark error will be throw if data do not have same geographic definition space
	//!         (coordinates of cells with defined value)
	//! \remark For now all quantities are casted to double.
	DiscreteLandscape(std::map<key_type, std::string> const& files, std::set<Time> const& times){

		build(files, times);

		auto have_same_origin = [this](auto const& it){return it.second.origin() == m_quantities.cbegin()->second.origin();};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_origin)){
			throw std::runtime_error("all ecological quantities dataset must have same origin");
		}

		auto have_same_extent = [this](auto const& it){return it.second.extent() == m_quantities.cbegin()->second.extent();};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_extent)){
			throw std::runtime_error("all ecological quantities dataset must have same extent");
		}

		auto have_same_resolution = [this](auto const& it){return it.second.resolution() == m_quantities.cbegin()->second.resolution();};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_resolution)){
			throw std::runtime_error("all ecological quantities dataset must have same resolution");
		}

		auto have_same_depth = [this](auto const& it){return it.second.depth() == m_quantities.cbegin()->second.depth();};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_depth)){
			throw std::runtime_error("all ecological quantities dataset must have same depth");
		}

		auto have_same_domain = [this](auto const& it){
			auto const& domain = it.second.geographic_definition_space();
			return std::equal(domain.cbegin(),
												domain.cend(),
												m_quantities.cbegin()->second.geographic_definition_space().cbegin());
		};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_domain)){
			throw std::runtime_error("all ecological quantities dataset must have same domain");
		}
	}

	//! Number of ecological quantities represented in the Environment.
	auto quantities_nbr() const {
		return m_quantities.size();
	}

	//! Ecological quantity as a function of space and time
	const auto&  operator [](key_type const& k) const {
		assert(m_quantities.find(k) != m_quantities.end());
		return m_quantities.at(k);
	}

	//! Return the times for which all ecological quantities are defined.
	const auto & temporal_definition_space() const {
		return m_quantities.cbegin()->second.temporal_definition_space();
	}

	//! Return the geographic coordinates for which all ecological quantities are defined at all times
	const auto & geographic_definition_space() const {
		return m_quantities.cbegin()->second.geographic_definition_space();
	}

	// Geographic coordinates of the top left corner.
	const auto & origin() const {
		return m_quantities.cbegin()->second.origin();
	}

	// Pixel size in x and y
	const auto & resolution() const {
		return m_quantities.cbegin()->second.resolution();
	}

	// Top left corner and bottom right corner geographic coordinates
	const auto & extent() const {
		return m_quantities.cbegin()->second.extent();
	}

	// Check if a given geographic coordinate is in the landscpae spatial extent
	bool is_in_spatial_extent(coord_type const& c) const {
		return m_quantities.cbegin()->second.is_in_spatial_extent(c);
	}

};

} // geography
} // quetzal

#endif
