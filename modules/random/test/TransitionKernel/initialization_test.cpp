// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#include "../../TransitionKernel.h"

//! [Example]

#include <random>
#include <assert.h>
#include <iostream>

int main () {

	using state_type = int;
	using distribution_type = std::discrete_distribution<state_type>;
	using kernel_type = quetzal::TransitionKernel<distribution_type>;

	// initialization by default constructor
	kernel_type first;

	// initialization by copy constructor
	kernel_type second(first);

	// Also initialization by copy constructor
	kernel_type third = first;

	// assignment by copy assignment operator
	second = third;

	// assignment by move assignment operator
	second = std::move(third);

	// initialization copying distribution
	distribution_type d({1, 10, 20, 30});
	kernel_type A(1, d);

	// initialization moving cell
	kernel_type B(1, std::move(d));

	std::cout << "done" << std::endl;
	return 0;

}

//! [Example]
