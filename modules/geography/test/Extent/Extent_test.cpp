// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "../../Extent.h"

//! [Example]

#include <assert.h>
#include <iostream>

int main()
{

	using decimal_degree = double;
	decimal_degree paris_lat = 48.8566;
	decimal_degree paris_lon = 2.3522;
	decimal_degree moscow_lat = 55.7522;
	decimal_degree moscow_lon = 37.6155;

	quetzal::geography::Extent<decimal_degree> extent(paris_lat, moscow_lat, paris_lon, moscow_lon);

	assert(extent.lat_min() == paris_lat && extent.lat_max() == moscow_lat &&
			   extent.lon_min() == paris_lon && extent.lon_max() == moscow_lon );

	auto copy = extent;
	copy.lat_min(0.0);

	bool are_same(copy == extent);
	assert(!are_same);
	std::cout << "Are extent equals ?\n" << (are_same ? "true" : "false") << std::endl;

  return 0;
}

//! [Example]
