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

template<typename Space, typename Time, typename Value>
std::ostream& operator <<(std::ostream& stream, const quetzal::demography::Flow<Space, Time, Value>& flows)
{
  for(auto const& it : flows){
    stream << it.first.time << "\t" << it.first.from <<  "\t" << it.first.to << "\t" << it.second << "\n";
  }
    return stream;
}

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


int main(){

  // Small populations
	using generator_type = std::mt19937;
	using coord_type = int;
	using time_type = unsigned int;

	generator_type gen;

	auto stochastic_dispersal = [](auto& gen, coord_type x, time_type t){
	 std::bernoulli_distribution d(0.5);
	 if(d(gen)){ x = -x; }
	 return x;
  };

 	auto growth = [](auto& gen, coord_type x, time_type t){return static_cast<unsigned int>(10);};

	quetzal::demography::History<coord_type, time_type, quetzal::demography::strategy::individual_based> history(1, 2000, 10);
	history.expand(3, growth, stochastic_dispersal, gen);

	std::cout << "Small populations\n" << history.flows() << std::endl;

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

	return 0;
}

//! [Example]
