// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include <iostream>
#include <assert.h>

#include "../../EnvironmentalQuantity.h"

int main()
{

	using time_type = unsigned int;
	using quantity_type = quetzal::geography::EnvironmentalQuantity<time_type>;
	using coord_type = typename quantity_type::coord_type;

	quantity_type bio1( "../test_data/bio1.tif", {2001,2002,2003,2004,2005,2006,2007,2008,2009,2010} );

	auto times = bio1.temporal_definition_space();
	auto space = bio1.geographic_definition_space();

	assert( space.size() == 9 );
	assert( times.size() == 10);
	assert( bio1.origin() == coord_type(52. , -5.));

	for(auto const& it : space){
		std::cout << it << std::endl;
	}

	coord_type c6 = space.at(0);
	coord_type c8 = space.at(2);
	coord_type c3 = space.at(3);
	coord_type c4 = space.at(4);
	coord_type c0 = space.at(6);
	coord_type c1 = space.at(7);

	std::cout << c0.lat() << "/" << c0.lon() << std::endl;

	double some_value = bio1.at(space.front(), times.front());
	assert( some_value != 0 );


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

	// Corners
	coord_type expected_top_left(52., -5.);
	coord_type expected_bottom_right(37., 10.);

	assert(bio1.is_in_spatial_extent(expected_top_left));
	assert( ! bio1.is_in_spatial_extent(expected_bottom_right));

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
	assert(bio1.reproject_to_centroid(in_box_0) == c0);
	assert(bio1.reproject_to_centroid(in_box_1) == c1);
	assert(bio1.reproject_to_centroid(in_box_6) == c6);
	assert(bio1.reproject_to_centroid(in_box_8) == c8);

	// Out of all boxes
	bool is_exception_thrown = false;
	try
	{
		bio1.reproject_to_centroid(out_north);
	}
	catch(...)
	{
		is_exception_thrown = true;
	}
	assert(is_exception_thrown);


	// At boxes limits
	assert(bio1.reproject_to_centroid(W_c0_limit) == c0);
	assert(bio1.reproject_to_centroid(N_c0_limit) == c0);
	assert(bio1.reproject_to_centroid(c0_c3_limit) == c3);
	assert(bio1.reproject_to_centroid(W_c0_c3_limit) == c3);
	assert(bio1.reproject_to_centroid(c0_c1_c3_c4_limit) == c4);

	is_exception_thrown = false;
	try
	{
		bio1.reproject_to_centroid(E_c2_c5_limit);
	}
	catch(...)
	{
		is_exception_thrown = true;
	}
	assert(is_exception_thrown);

	return 0;
}
