// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/


#include "../../PopulationFlux.h"

//! [Example]

#include <iostream>   // std::cout
#include <algorithm>  // std::copy
#include <string>

#include "assert.h"

int main(){

	using coord_type = std::string;
	using time_type = unsigned int;
	using value_type = int;

	time_type t = 2017;
	coord_type i = "Paris";
	coord_type j = "Bordeaux";

	quetzal::demography::PopulationFlux<coord_type, time_type, value_type> Phi;

	assert( ! Phi.flux_to_is_defined(i, t) );
	Phi(i,j,t) = 12;
	Phi.flux_from_to(j,j,t) += 1;
	assert(Phi.flux_to_is_defined(j, t));

	assert(Phi.flux_from_to(i,j,t) == 12);
	assert(Phi(j,j,t) == 1);

	for(auto const& it : Phi.flux_to(j,t)){
		std::cout << it.first << " \t->\t " << j << "\t=\t" << it.second << std::endl;
	}

	return 0;
}

//! [Example]
