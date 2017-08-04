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

#include "Env.h"
#include "Coords.h"


int main()
{
	using geography::Coords;

	/*********************************************
	   Using GeoTif files as environment
	*********************************************/

	// Real" environment
	Env env( {{"bio1","test_data/bio1.tif"},{"bio12","test_data/bio12.tif"}} );

	assert(env.size() == 2);

	auto times = env.times();
	assert( times.size() == 10 );
	assert( times.front() == 1);
	assert( times.back() == 10);

	std::vector<Coords> coords = env.valid_coords(times.front());
	assert( coords.size() == 9 );

	Coords c0 = coords.at(0); 
	Coords c3 = coords.at(1); 
	Coords c6 = coords.at(2); 
	Coords c1 = coords.at(3); 
	Coords c4 = coords.at(4); 
	Coords c8 = coords.at(8); 


	// Access to one value
	auto bio1 = env[0];
	double some_value = bio1(coords.front(), times.front());
	assert( some_value != 0 );
	 
	/************************************
	// Testing coordinates reprojection
	************************************/

	// Corners
	Coords expected_top_left(52., -5.);
	Coords expected_bottom_right(37., 10.);

	// In boxes
	Coords in_box_0(51., -4.5);
	Coords in_box_1(48.32, 1.5);
	Coords in_box_6(37.21, -3.2);
	Coords in_box_8(38.21, 7.125);

	// Out of all boxes
	Coords out_north(55. , 0.);

	// At boxes limits
	Coords W_c0_limit(48. , -5.);
	Coords N_c0_limit(52. , -2.);
	Coords c0_c3_limit(47. , -2.);
	Coords W_c0_c3_limit(47. , -5);
	Coords c0_c1_c3_c4_limit(47. , 0.);
	Coords E_c2_c5_limit(47. , 10.);

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
	catch(GeoData::exception_type&)
	{
		is_exception_thrown = true;
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
	catch(GeoData::exception_type&)
	{
		is_exception_thrown = true;
	}
	catch(...)
	{
		is_exception_thrown = true;
	}
	assert(is_exception_thrown);	
	return 0;
}

