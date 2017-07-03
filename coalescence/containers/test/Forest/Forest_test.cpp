// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#include "../../Forest.h"

//! [Example]

#include <string>
#include <assert.h>
#include <iostream>

int main(){

	using position_type = int;
	using tree_type = std::string;
	using forest_type = quetzal::coalescence::Forest<position_type, tree_type>;

	forest_type forest;

	auto summarize = [](auto const& forest){
	std::cout << "Forest has "
	  << forest.nb_trees() << " trees at "
		<< forest.positions().size() << " geographic positions." << std::endl;
	};

	auto print = [](auto const& forest){
		for(auto const& it : forest){
			std::cout << "Position: " << it.first << "\t" << "Species: " << it.second << std::endl;
		}
	};

	// Insert some trees at some positions
	std::vector<tree_type> trees = {"populus_tremula","salix_nigra"};
	forest.insert(1,  std::move(trees));
	forest.insert(2, std::string("populus_tremula"));
	forest.insert(3, std::string("quercus_robur"));

	summarize(forest);
	print(forest);

	std::cout << "\nErase coordinate 1:" << std::endl;
	forest.erase(1);

	summarize(forest);
	print(forest);

	return 0;
}

//! [Example]
