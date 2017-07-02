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

	// \remark These are very basic types sufficient for testing, but have little to see with coalescence.
	using position_type = int;
	using std::string;
	using tree_type = string;
	using forest_type = quetzal::coalescence::Forest<position_type, tree_type>;
	forest_type forest;

	// Insert some trees at some positions
	forest.insert(1, string("salix_nigra"));

	std::vector<string> u {"populus_tremula", "salix_nigra"};
	forest.insert(2, u);

	std::vector<string> v {"quercus_robur", "salix_nigra"};
	forest.insert(3, std::move(v));

	auto it = forest.insert(4, string("salix_nigra"));
	assert(it->first == 4);
	
	assert(forest.nb_trees() == 6);
	assert(forest.nb_trees(3) == 2);

	for(auto const& it : forest){
		std::cout << "Position "<< it.first << " : " << it.second << std::endl;
	}

	return 0;
}

//! [Example]
