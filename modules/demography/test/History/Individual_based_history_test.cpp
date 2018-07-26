// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "../../History.h"

//! [Example]

#include <iostream>
#include <random>
#include <map>

int main(){

	// Here we simulate a population expansion through a 2 demes landscape.
	
	using coord_type = int;
	using time_type = unsigned int;
	using generator_type = std::mt19937;

	// Initialize an individual-based history: 10 individuals introduced at x=1, t=2018
	using quetzal::demography::strategy::individual_based;
	quetzal::demography::History<coord_type, time_type, individual_based> history(1, 2018, 10);

	// Growth function
	auto N = std::cref(history.pop_sizes());
	auto growth = [N](auto& gen, coord_type x, time_type t){ return 2*N(x,t) ; };

	// Number of non-overlapping generations for the demographic simulation
	unsigned int nb_generations = 3;

	// Random number generation
	generator_type gen;

	// Stochastic dispersal kernel, which geographic sampling space is {-1 , 1}
	auto sample_location = [](auto& gen, coord_type x, time_type t){
	 std::bernoulli_distribution d(0.5);
	 if(d(gen)){ x = -x; }
	 return x;
	};

	history.expand(nb_generations, growth, sample_location, gen);

	std::cout << "Population flows from x to y at time t:\n\n" << history.flows() << std::endl;

	std::cout << "\nKnowing an indiviual was in deme 1 in 2021, where could it have been just before dispersal ?\n";
	std::cout << "Answer: it could have been in deme " << history.backward_kernel(1, 2021, gen) << std::endl;
	return 0;
}

//! [Example]
