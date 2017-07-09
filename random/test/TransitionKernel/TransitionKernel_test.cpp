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

#include <random>    // std::mt19937, std::poisson_distribution
#include <iostream>

int main () {

	std::mt19937 gen;
	using state_space = int;
	using D = std::poisson_distribution<state_space>;
	using kernel_type = quetzal::TransitionKernel<D>;

	kernel_type kernel;
	kernel.add(0, D(5));
	kernel.add(1, D(10));

	if(kernel.has_distribution(2)){
		std::cout <<"kernel defined for x=2." << std::endl;
	}

	kernel.add(2, D(3));
	for(int i = 0; i < 10; ++i){
		std::cout << kernel(gen,2) << std::endl;
	}
	return 0;

}

//! [Example]
