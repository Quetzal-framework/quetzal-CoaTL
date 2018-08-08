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

// Here we simulate a population expansion through a 2 demes landscape.

struct transition_matrix {

	using coord_type = std::string;
	using time_type = unsigned int;

	std::vector<coord_type> arrival_space(coord_type x, time_type t)
	{
		return {"Paris", "Ann Arbor"};
	}

	double operator()(coord_type x, coord_type y, time_type t)
	{
		return 0.5; // 1/2 probability to change of location
	}

};

int main(){

	using coord_type = std::string;
	using time_type = unsigned int;
	using generator_type = std::mt19937;

	// Initialize history: 10 individuals introduced at x=1, t=2018
	using quetzal::demography::strategy::mass_based;
	quetzal::demography::History<coord_type, time_type, mass_based> history("Paris", 2018, 10);

	// Growth function
	auto N = std::cref(history.pop_sizes());
	auto growth = [N](auto& gen, coord_type x, time_type t){ return 2*N(x,t) ; };

	// Number of non-overlapping generations for the demographic simulation
	unsigned int nb_generations = 3;

	// Random number generation
	generator_type gen;

	transition_matrix M;

	history.expand(nb_generations, growth, M, gen);

	std::cout << "Population flows from x to y at time t:\n\n" << history.flows() << std::endl;

	std::cout << "\nKnowing an indiviual was in Paris in 2021, where could it have been just before dispersal ?\n";
	std::cout << "Answer: it could have been in " << history.backward_kernel("Paris", 2021, gen) << std::endl;

	return 0;
}

//! [Example]
