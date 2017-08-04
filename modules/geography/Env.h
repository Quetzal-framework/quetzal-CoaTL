// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/ 

#ifndef __ENV_H_INCLUDED
#define __ENV_H_INCLUDED

#include <string>
#include <map>
#include <assert.h>
#include "../../utils/utils_map_access.h"
#include "GeoData.h"

using namespace utils::map_access;

class Env
{
	using id_type = unsigned int;
	using data_type = GeoData;
	using path_type = std::string;
	using names_type = std::vector<std::string>;

public:

	using coord_type = typename GeoData::coord_type;
	using value_type = typename GeoData::value_type;
	using time_type = typename GeoData::time_type;

	// Constructor using path to geotiff files
	Env(std::map<std::string, path_type> data){
		for(auto const& it : data){
			data_stub.insert(std::make_pair(m_id, GeoData(it.second)));
			m_names.push_back(it.first);
			++m_id;
		}
	}

	auto size() const { return data_stub.size(); }

	std::vector<time_type> times() const { return data_stub.begin()->second.times(); }

	time_type begin_time() const { return times().front() ;}

	time_type end_time() const { return times().back() ;}

	std::vector<coord_type> valid_coords(time_type const& t) const { return data_stub.begin()->second.valid_coords(t); }

	auto const& operator[](id_type id) const { return data_stub.at(id); }

	coord_type reproject_to_centroid(coord_type const& coord) const { return data_stub.begin()->second.reproject_to_centroid(coord);}

	value_type operator()(id_type const& id, coord_type const& x, time_type t) const { return (data_stub/id)->operator()(x, t);	}

	std::string name(id_type id) const{
		assert( ! m_names.empty());
		return m_names.at(id);
	}

private:

	std::map<id_type, data_type> data_stub;
	id_type m_id = 0;
	names_type m_names;
};

#endif
