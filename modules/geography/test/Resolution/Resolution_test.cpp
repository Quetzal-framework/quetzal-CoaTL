// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "../../Resolution.h"

//! [Example]

#include <assert.h>
#include <iostream>

int main()
{

	using decimal_degree = double;
	decimal_degree lat_resolution = 2.0;
	decimal_degree lon_resolution = 1.0;

	quetzal::geography::Resolution<decimal_degree> res(lat_resolution, lon_resolution);

	assert(res.lat() == lat_resolution && res.lon() == lon_resolution);

	auto copy = res;
	copy.lon(2.0);

	bool are_same(copy == res);
	assert(!are_same);
	std::cout << "Are resolutions equals ?\n" << (are_same ? "true" : "false") << std::endl;

  return 0;
}

//! [Example]
