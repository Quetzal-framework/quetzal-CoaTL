// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#include "../../OccupancySpectrumDistribution.h"

//! [Example]

#include "assert.h"
#include <iostream>

int main(){

	using quetzal::coalescence::occupancy_spectrum::OccupancySpectrumDistribution;

	// initialization by default constructor
	OccupancySpectrumDistribution<> a;

	// constructor
	OccupancySpectrumDistribution<> b(5,10);

	// initialization by move constructor
	OccupancySpectrumDistribution<> c(std::move(b)); // a can not be used again

	// deleted initialization by copy constructor
	// OccupancySpectrumDistribution d(c); // uncomment will cause a compilation error

	// deleted assignment by copy assignment operator
	// a = b; // uncomment will cause a compilation error

	// assignment by move assignment operator
	OccupancySpectrumDistribution<> e = std::move(c); // c should not be used anymore !

	std::cout << e << std::endl;
	return 0;
}

//! [Example]
