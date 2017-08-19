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

	assert( space.size() == 9 );

	return 0;
}
