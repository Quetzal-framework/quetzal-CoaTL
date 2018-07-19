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

struct kernel {

	using coord_type = int;
	using time_type = unsigned int;

	std::vector<coord_type> support(time_type t)
	{
		return {-1, 1};
	}

	double operator()(coord_type x, coord_type y, time_type t)
	{
		return 0.5; // 1/2 probability to change deme
	}

};

int main(){

	using coord_type = int;
	using time_type = unsigned int;
	using generator_type = std::mt19937;

  // Initialize history: 10 individuals introduced at x=1, t=2018
	using quetzal::demography::strategy::mass_based;
	quetzal::demography::History<coord_type, time_type, mass_based> history(1, 2018, 10);

  // Growth function
  auto N = std::cref(history.pop_sizes());
  auto growth = [N](auto& gen, coord_type x, time_type t){ return 2*N(x,t) ; };

  // Number of non-overlapping generations for the demographic simulation
  unsigned int nb_generations = 3;

  // Random number generation
  generator_type gen;

	kernel k;

	history.expand(nb_generations, growth, k, gen);

	std::cout << "Population flows from x to y at time t:\n\n" << history.flows() << std::endl;

/*

template<typename coord_type, typename time_type>
struct big_pop_kernel{

  double operator()(coord_type x, coord_type y, time_type t) const {return 0.5;}

  std::vector<coord_type> support(time_type) const {
    std::vector<coord_type> support;
    support.push_back(-1);
    support.push_back(1);
    return(support);
  }
};

	std::map<coord_type, unsigned int> counts;
	for(unsigned int i = 0; i < 1000; ++i)
	{
		counts[history.backward_kernel(1, 2003, gen)] += 1;
	}

	for(auto const& it : counts)
	{
		std::cout << it.first << "\t" << it.second << std::endl;
	}

  // Big populations
  big_pop_kernel<coord_type, time_type> pdf;
  quetzal::demography::History<coord_type, time_type, quetzal::demography::strategy::mass_based> other_history(1, 2000, 10);
  other_history.expand(3, growth, pdf, gen);

  std::cout << "Big populations\n" << other_history.flows() << std::endl;
*/
	return 0;
}

//! [Example]
