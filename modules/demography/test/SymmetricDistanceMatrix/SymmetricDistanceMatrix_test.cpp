// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/


#include "../../dispersal.h"
#include <random>

//! [Example]

int main()
{
	using coord_type = int;
	std::vector<coord_type> points {1,2,3,4};
	auto distance = [](coord_type a, coord_type b){return std::abs(a - b);};
	auto D = quetzal::demography::dispersal::make_symmetric_distance_matrix(points, distance);
	std::cout << D << std::endl;

	auto M = D.apply([](coord_type a){return a * 0.5;});
	std::cout << M << std::endl;

	auto X = D.apply([](coord_type a){return a <= 2;});
	std::cout << X << std::endl;

	using quetzal::demography::dispersal::Gaussian;
	Gaussian::param_type p;
	p.a(1.);
	using quetzal::demography::dispersal::make_discrete_location_sampler;
	auto k = make_discrete_location_sampler<Gaussian>(D, p);

	std::mt19937 rng;
	int position = 1;
	for(unsigned int i = 0; i < 10 ; ++i){
		position = k(position, rng);
		std::cout << position << " -> ";
	}

	return 0;
}

//! [Example]
