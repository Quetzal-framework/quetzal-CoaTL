// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/


#include "../../SymmetricDistanceMatrix.h"

//! [Example]

int main()
{

	std::vector<int> points {1,2,3,4};
	auto distance = [](int a, int b){return std::abs(a - b);};
	auto D = quetzal::demography::dispersal::make_symmetric_distance_matrix(points, distance);
	std::cout << D << std::endl;

	auto M = D.apply([](int a){return a * 0.5;});
	std::cout << M << std::endl;

	auto X = D.apply([](int a){return a <= 2;});
	std::cout << X << std::endl;

	return 0;
}

//! [Example]
