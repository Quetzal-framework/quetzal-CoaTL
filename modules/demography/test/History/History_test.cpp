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
std::ostream& operator <<(std::ostream& stream, const quetzal::demography::PopulationFlux<Space, Time, Value>& flows)
{
  for(auto const& it : flows){
    stream << it.first.time << "\t" << it.first.from <<  "\t" << it.first.to << "\t" << it.second << "\n";
  }
    return stream;
}

int main(){

	using generator_type = std::mt19937;
	using coord_type = int;
	using time_type = unsigned int;

	generator_type gen;

	auto kernel = [](auto& gen, coord_type x, time_type t){
	 std::bernoulli_distribution d(0.5);
	 if(d(gen)){ x = -x; }
	 return x;
  };

 	auto growth = [](auto& gen, coord_type x, time_type t){return static_cast<unsigned int>(10);};

	quetzal::demography::History<coord_type, time_type, unsigned int> history(1, 2000, 10);
	history.expand(3, growth, kernel, gen);

	std::cout << history.flows() << std::endl;

	std::map<coord_type, unsigned int> counts;
	for(unsigned int i = 0; i < 1000; ++i)
	{
		counts[history.backward_kernel(1, 2003, gen)] += 1;
	}

	for(auto const& it : counts)
	{
		std::cout << it.first << "\t" << it.second << std::endl;
	}

	return 0;
}

//! [Example]
