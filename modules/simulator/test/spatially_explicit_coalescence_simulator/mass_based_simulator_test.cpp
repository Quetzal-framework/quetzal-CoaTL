// Copyright 2018 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "../../simulators.h"

#include <string>

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
  using quetzal::demography::strategy::mass_based;

  // Demographic simulation setting
  coord_type x_0 = "Paris";
  time_type t_0 = 1912;
  mass_based::value_type N_0 = 1000;

  using simulator_type = quetzal::simulators::SpatiallyExplicitCoalescenceSimulator<coord_type, time_type, mass_based>;
  simulator_type simulator(x_0, t_0, N_0);

  time_type sampling_time = 5000;

  // Growth function: a lambda expression capturing the population size history
  auto N = simulator.pop_size_history();
	unsigned int K = 20;
  auto growth = [K, N](auto& gen, coord_type x, time_type t){ return std::min(100*N(x,t), K) ; };

  transition_matrix M;

  // Random number generation
  using generator_type = std::mt19937;
  generator_type gen;

  simulator.expand_demography(sampling_time, growth, M, gen);

  // Coalescence setting
  using tree_type = std::string;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;
  forest_type forest;
  forest.insert("Paris", std::vector<tree_type>{"a", "b", "c"});
  forest.insert("Ann Arbor", std::vector<tree_type>{"d", "e", "f"});

  // Newick formater
  auto branching_operator = []( auto parent , auto child ){
    if ( parent.size () == 0)
    return "(" + child ;
    else
    return parent + "," + child + ")" ;
  };

  auto new_forest = simulator.coalesce_along_history(forest, branching_operator, gen);

  for(auto const& it : new_forest ){
    std::cout << it.first << "\t" << it.second << std::endl;
  }

}
