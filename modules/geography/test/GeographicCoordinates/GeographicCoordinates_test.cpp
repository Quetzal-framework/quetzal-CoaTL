// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "../../GeographicCoordinates.h"

//! [Example]

#include <assert.h>
#include <iostream>

int main()
{
	using coord_type = quetzal::geography::GeographicCoordinates;

	coord_type paris(48.856614, 2.3522219000000177);
	coord_type moscow(55.7522200, 37.6155600 );

	assert(paris < moscow);

	coord_type::km computed = paris.great_circle_distance_to(moscow);
	coord_type::km expected = 2486.22;

	coord_type::km EPSILON = 1.; // a 1-km error is acceptable.
	coord_type::km diff = expected - computed;
	assert((diff < EPSILON) && (-diff < EPSILON));

	std::cout << "Distance Paris <-> Moscow:\nExpected: " << expected
            << " km\nComputed: " << computed
            << " km\nDifference: " << diff << " km" << std::endl;

  return 0;
}

//! [Example]
