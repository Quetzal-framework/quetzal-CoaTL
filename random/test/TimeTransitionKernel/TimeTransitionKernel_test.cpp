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
#include <iostream>
#include <string>

int main () {

	std::mt19937 gen;
	using state_space = int;
	using time_type = std::string;
	using D = std::poisson_distribution<state_space>;
	using kernel_type = quetzal::TransitionKernel<time_type, D>;

	kernel_type kernel;
	kernel.add(0, "2014", D(5));
	kernel.add(1, "2015", D(10));

	if(kernel.has_distribution(2, "2014")){
		std::cout <<"kernel defined for x=2, t=2014." << std::endl;
	}

	kernel.add(2, "2014", D(3));
	for(int i = 0; i < 10; ++i){
		std::cout << kernel(gen,2, "2014") << std::endl;
	}

	return 0;
}

//! [Example]
