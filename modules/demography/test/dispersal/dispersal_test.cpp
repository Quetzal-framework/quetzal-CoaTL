// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

//! [Example]

#include "../../dispersal.h"
#include <random>     // std::mt19937
#include <iostream>   // std::cout
#include <algorithm>  // std::copy
#include <iterator>  // std::iostream_iterator
#include <string>

int main(){

	using generator_type = std::mt19937;
	using coord_type = double;
	using law_type = quetzal::demography::dispersal::Gaussian;

	std::vector<coord_type> demes {0.0, 12.0, 30.0};
	auto distance = [](coord_type x, coord_type y){return std::abs(x-y);};
	auto kernel = quetzal::demography::dispersal::make_dispersal_kernel<law_type>(demes, distance, 20.0);

	generator_type gen;
	std::vector<coord_type> path;
	path.push_back(demes.front());
	for(unsigned int t = 0; t < 10; ++t)
	{
		path.push_back(kernel(gen, path.back()));
	}

	std::copy(path.begin(), path.end(), std::ostream_iterator<coord_type>(std::cout, " -> "));

	return 0;
}

//! [Example]
