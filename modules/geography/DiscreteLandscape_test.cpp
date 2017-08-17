// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o Env_test Env_test.cpp -std=c++1y -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include <string>
#include <iostream>
#include <assert.h>

#include "DiscreteLandscape.h"

int main()
{

	/*********************************************
	   Using GeoTif files as environment
	*********************************************/

	// Real" environment
	using time_type = unsigned int;
	using landscape_type = quetzal::geography::DiscreteLandscape<std::string, time_type>;
	landscape_type env( {{"bio1","test_data/bio1.tif"},{"bio12","test_data/bio12.tif"}},
	                    {2001,2002,2003,2004,2005,2006,2007,2008,2009,2010} );

	assert(env.quantities_nbr() == 2);

	auto times = env.temporal_definition_space();
	auto space = env.geographic_definition_space();

	using coord_type = landscape_type::coord_type;

	assert( space.size() == 9 );

	std::vector<coord_type> v_space(space.begin(), space.end());
	coord_type c0 = v_space.at(0);
	coord_type c3 = v_space.at(1);
	coord_type c6 = v_space.at(2);
	coord_type c1 = v_space.at(3);
	coord_type c4 = v_space.at(4);
	coord_type c8 = v_space.at(8);


	// Access to one value
	//auto bio1 = env[0];
	//double some_value = bio1(coord_type.front(), times.front());
	//assert( some_value != 0 );

	/************************************
	// Testing coordinates reprojection
	************************************/

	// Corners
	coord_type expected_top_left(52., -5.);
	coord_type expected_bottom_right(37., 10.);

	// In boxes
	coord_type in_box_0(51., -4.5);
	coord_type in_box_1(48.32, 1.5);
	coord_type in_box_6(37.21, -3.2);
	coord_type in_box_8(38.21, 7.125);

	// Out of all boxes
	coord_type out_north(55. , 0.);

	// At boxes limits
	coord_type W_c0_limit(48. , -5.);
	coord_type N_c0_limit(52. , -2.);
	coord_type c0_c3_limit(47. , -2.);
	coord_type W_c0_c3_limit(47. , -5);
	coord_type c0_c1_c3_c4_limit(47. , 0.);
	coord_type E_c2_c5_limit(47. , 10.);

	// In boxes
	assert(env.reproject_to_centroid(in_box_0) == c0);
	assert(env.reproject_to_centroid(in_box_1) == c1);
	assert(env.reproject_to_centroid(in_box_6) == c6);
	assert(env.reproject_to_centroid(in_box_8) == c8);

	// Out of all boxes
	bool is_exception_thrown = false;
	try
	{
		env.reproject_to_centroid(out_north);
	}
	catch(...)
	{
		is_exception_thrown = true;
	}
	assert(is_exception_thrown);


	// At boxes limits
	assert(env.reproject_to_centroid(W_c0_limit) == c0);
	assert(env.reproject_to_centroid(N_c0_limit) == c0);
	assert(env.reproject_to_centroid(c0_c3_limit) == c3);
	assert(env.reproject_to_centroid(W_c0_c3_limit) == c3);
	assert(env.reproject_to_centroid(c0_c1_c3_c4_limit) == c4);

	is_exception_thrown = false;
	try
	{
		env.reproject_to_centroid(E_c2_c5_limit);
	}
	catch(...)
	{
		is_exception_thrown = true;
	}
	assert(is_exception_thrown);
	return 0;
}
