// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/ 

// compiles with g++ -o GeoData_test GeoData_test.cpp -std=c++1y -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include <assert.h>
#include <iostream>

#include "GeoData.h"

int main()
{


	/**********************
	/ Construction
	**********************/
	GeoData data("test_data/test.tif");


	/**********************
	/ Environmental times
	**********************/
	std::vector<GeoData::time_type> times = data.times();
	std::vector<GeoData::time_type> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	assert(times == expected);


	/*************************************
	/ Coordinates where data are avalaible
	*************************************/

	/*************************************
	/// Full data-defined dataset test.tif
	*************************************/

	/* Expected dataSet structure
	
	=> Origin at Lon -5, Lat 52.
	=> 9 cells
	=> 10 layers
	=> Pixel size (-5, 5)
	=> East and South limits ARE NOT in spatial extent



			origin	    -5        0       5      10
				   \   /         /  	 /		/
					\ / 		/		/	   /
				52	 * ------ * ----- * ---- *
					 |    .   |   .   |   .  
					 |   c0   |  c1   |  c2  
				47	 * ------ * ----- * ---- *
					 |    .   |   .   |   .  
					 |   c3   |  c4   |  c5  
				42	 * ------ * ----- * ---- *
					 |   .    |   .   |   .  
					 |   c6   |  c7   |  c8  
				37	 *        *       *      *

				0	-  -----> + 180    Xsize positive in decimal degre (east direction positive)

				90
				+	
					 |
					 |
				-	 \/ Y size negative in decimal degree (south direction negative)
				0
	*/

	// Testing origin
	GeoData::coord_type c0_expected(52., -5.);

	assert( data.origin().get_lon() == -5.);
	assert( data.origin().get_lat() == 52.);
	assert( data.origin() == c0_expected);

	// Testing resolution of pixels
	double expected_west_east_size = 5.;
	double expected_north_south_size = -5.; // negative !
	GeoData::coord_type resolution = data.pixel_size();

	assert(resolution.get_lat() == expected_north_south_size);
	assert(resolution.get_lon() == expected_west_east_size);

	// Testing coordinates of value-defined cells
	double lat_0 = data.origin().get_lat();
	double lon_0 = data.origin().get_lon(); 
	double lat_size = resolution.get_lat();
	double lon_size = resolution.get_lon();

	GeoData::coord_type c0( (lat_0 + 1*lat_size - lat_size/2) , (lon_0 + 1*lon_size - lon_size/2));
	GeoData::coord_type c1( (lat_0 + 1*lat_size - lat_size/2) , (lon_0 + 2*lon_size - lon_size/2));
	GeoData::coord_type c2( (lat_0 + 1*lat_size - lat_size/2) , (lon_0 + 3*lon_size - lon_size/2));

	GeoData::coord_type c3( (lat_0 + 2*lat_size - lat_size/2) , (lon_0 + 1*lon_size - lon_size/2));
	GeoData::coord_type c4( (lat_0 + 2*lat_size - lat_size/2) , (lon_0 + 2*lon_size - lon_size/2));
	GeoData::coord_type c5( (lat_0 + 2*lat_size - lat_size/2) , (lon_0 + 3*lon_size - lon_size/2));

	GeoData::coord_type c6( (lat_0 + 3*lat_size - lat_size/2) , (lon_0 + 1*lon_size - lon_size/2));
	GeoData::coord_type c7( (lat_0 + 3*lat_size - lat_size/2) , (lon_0 + 2*lon_size - lon_size/2));
	GeoData::coord_type c8( (lat_0 + 3*lat_size - lat_size/2) , (lon_0 + 3*lon_size - lon_size/2));

	std::vector<GeoData::coord_type> valid_coords = data.valid_coords(1);
	std::vector<GeoData::coord_type> expected_valid_coords = {c0, c3, c6, c1, c4, c7, c2, c5, c8};
	assert(valid_coords == expected_valid_coords );


	/*******************************************************************
	********************************************************************
	                          Spatial positionning
	********************************************************************
	********************************************************************/

	// Corners
	GeoData::coord_type expected_top_left(52., -5.);
	GeoData::coord_type expected_bottom_right(37., 10.);


	// In boxes
	GeoData::coord_type in_box_0(51., -4.5);
	GeoData::coord_type in_box_1(48.32, 1.5);
	GeoData::coord_type in_box_6(37.21, -3.2);
	GeoData::coord_type in_box_8(38.21, 7.125);

	// Out of all boxes
	GeoData::coord_type out_north(55. , 0.);
	GeoData::coord_type out_south(32. , 0.);
	GeoData::coord_type out_west(47. , -30);
	GeoData::coord_type out_east(47. , 10.001);

	GeoData::coord_type out_NW(52.0001 , -5.00001);
	GeoData::coord_type out_NE(52.0001 , 11);
	GeoData::coord_type out_SW(36.9999 , -5.0001);
	GeoData::coord_type out_SE(30. , 10.00001);

	// At boxes limits
	GeoData::coord_type W_c0_limit(48. , -5.);
	GeoData::coord_type N_c0_limit(52. , -2.);
	GeoData::coord_type c0_c3_limit(47. , -2.);
	GeoData::coord_type W_c0_c3_limit(47. , -5);
	GeoData::coord_type c0_c1_c3_c4_limit(47. , 0.);
	GeoData::coord_type E_c2_c5_limit(47. , 10.);
	GeoData::coord_type S_c6_c7_limit(37. , 0.);
	GeoData::coord_type S_c6_limit(37. , -2.);
	GeoData::coord_type S_E_c8_limit(37. , 10.);



	/*************************
	// Testing spatial extent
	*************************/

	std::pair<GeoData::coord_type, GeoData::coord_type> extremes = data.get_spatial_extent();
	assert(extremes.first == expected_top_left && extremes.second == expected_bottom_right);

	assert( data.is_in_spatial_extent(in_box_0));
	assert(data.is_in_spatial_extent(in_box_1));
	assert(data.is_in_spatial_extent(in_box_6));
	assert(data.is_in_spatial_extent(in_box_8));

	assert( !data.is_in_spatial_extent(out_north));
	assert( !data.is_in_spatial_extent(out_south));
	assert( !data.is_in_spatial_extent(out_west));
	assert( !data.is_in_spatial_extent(out_east));
	assert( !data.is_in_spatial_extent(out_NW));
	assert( !data.is_in_spatial_extent(out_NE));
	assert( !data.is_in_spatial_extent(out_SW));
	assert( !data.is_in_spatial_extent(out_SE));

	assert( data.is_in_spatial_extent(W_c0_limit));
	assert( data.is_in_spatial_extent(N_c0_limit));
	assert( data.is_in_spatial_extent(c0_c3_limit));
	assert( data.is_in_spatial_extent(W_c0_c3_limit));
	assert( data.is_in_spatial_extent(c0_c1_c3_c4_limit));

	// East and South limits ARE NOT in spatial extent
	assert( !data.is_in_spatial_extent(E_c2_c5_limit));
	assert( !data.is_in_spatial_extent(S_c6_c7_limit));
	assert( !data.is_in_spatial_extent(S_c6_limit));
	assert( !data.is_in_spatial_extent(S_E_c8_limit));

	/************************************
	// Testing coordinates reprojection
	************************************/

	// In boxes
	assert(data.reproject_to_centroid(in_box_0) == c0);
	assert(data.reproject_to_centroid(in_box_1) == c1);
	assert(data.reproject_to_centroid(in_box_6) == c6);
	assert(data.reproject_to_centroid(in_box_8) == c8);

	// Out of all boxes
	bool is_exception_thrown = false;
	try
	{
		data.reproject_to_centroid(out_north);
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
	assert(data.reproject_to_centroid(W_c0_limit) == c0);
	assert(data.reproject_to_centroid(N_c0_limit) == c0);
	assert(data.reproject_to_centroid(c0_c3_limit) == c3);
	assert(data.reproject_to_centroid(W_c0_c3_limit) == c3);
	assert(data.reproject_to_centroid(c0_c1_c3_c4_limit) == c4);

	is_exception_thrown = false;
	try
	{
		data.reproject_to_centroid(E_c2_c5_limit);
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


	/*******************************************************************
	********************************************************************
	                          Data access
	********************************************************************
	********************************************************************/

	double expected_min = 82.51064 ;
	double expected_max = 148.2 ;
	GeoData::value_type min = data.min();
	GeoData::value_type max = data.max();
	double epsilon = 0.1; // more than acceptable precision for environmental data
	assert( ((expected_min - epsilon) <= min) && (min <= (expected_min + epsilon)));
	assert( ((expected_max - epsilon) <= max) && (max <= (expected_max + epsilon)));
	return 0;
}