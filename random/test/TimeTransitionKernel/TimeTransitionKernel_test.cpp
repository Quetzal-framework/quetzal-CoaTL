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

#include <algorithm> // std::generate
#include <random>    // std::mt19937
#include <iostream>
#include <iterator>   // std::ostream_iterator
#include <string>

int main () {

	using deme_ID_type = int;
	using time_type = int;
	using law_type = std::discrete_distribution<deme_ID_type>;
	using kernel_type = quetzal::TransitionKernel<time_type, law_type>;

	std::mt19937 gen;
	law_type d({0.5,0.5});
	kernel_type kernel;

	// Random walk
	deme_ID_type x0 = {0};
	std::cout << x0;
	for(time_type t = 0; t < 10; ++t){
		std::cout << " -> " << x0 ;
		if( ! kernel.has_distribution(x0, t)){
			kernel.set(x0, t, d);
		}
		x0 = kernel(gen, x0, t); // sample new state
	}

	return 0;
}

//! [Example]
