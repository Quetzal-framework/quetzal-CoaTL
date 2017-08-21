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
	using deme_ID_type = int;
	using law_type = std::discrete_distribution<deme_ID_type>;
	using kernel_type = quetzal::random::TransitionKernel<law_type>;

	law_type distribution({0.5,0.5}); // return deme 0 or deme 1 with same probability
	kernel_type kernel;
	std::mt19937 gen;

	// Random walk
	deme_ID_type x0 = 0;
	std::cout << x0;
	for(int i = 0; i < 10; ++i){
		std::cout << " -> " << x0 ;
		if( ! kernel.has_distribution(x0)){
			kernel.set(x0, distribution);
		}
		x0 = kernel(gen, x0); // sample new state
	}
	return 0;
}

//! [Example]
