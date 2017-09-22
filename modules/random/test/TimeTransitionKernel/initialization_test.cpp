// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

/**
 * Program for unit testing
 *
 * Test program for interface of DiscreteTransitionKernel class.
 * Compiles with g++ -o test DiscreteTransitionKernel_test.cpp -std=c++14 -Wall
 */

#include "../../TransitionKernel.h"

//! [Example]

#include <random>    // std::mt19937
#include <iostream>

int main () {

	using State = int;
	using Time = int;
	using Distribution = std::discrete_distribution<State>;
	using Kernel = quetzal::random::TransitionKernel<Time, Distribution>;

	// initialization by default constructor
	Kernel first;

	// initialization by copy constructor
	Kernel second(first);

	// Also initialization by copy constructor
	Kernel third = first;

	// assignment by copy assignment operator
	second = third;

	// assignment by move assignment operator
	second = std::move(third);

	// initialization copying distribution
	Distribution d({1, 10, 20, 30});
	State x = 1;
	Time t = 0;
	Kernel A(x, t, d);

	// initialization moving distribution
	Kernel B(x, t, std::move(d));
	std::mt19937 gen;
	std::cout << B(gen, x, t) << std::endl;

	return 0;

}

//! [Example]
