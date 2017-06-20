// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/ 

/**
 * \file Forest_test.cpp
 * \brief Program for unit testing
 * \author Arnaud Becheler <Arnaud.Becheler@egce.cnrs-gif.fr>
 *
 * Test program for interface of Forest class using basic parameters type for illustration.
 * Compiles with g++ -o test Forest_test.cpp -std=c++14 -Wall
 *
 */

#include "Forest.h"
#include <string>
#include <assert.h>
#include <set>

int main(){

	// \remark These are very basic types sufficient for testing, but have little to see with coalescence.
	using position_type = int;
	using tree_type = std::string;
	using forest_type = coalescence::utils::Forest<position_type, tree_type>;

	/**
	 * Initialization
	 */

	// initialization by default constructor
	forest_type first;

	// initialization by copy constructor
	forest_type second(first);

	// Also initialization by copy constructor
	forest_type third = first;

	// assignment by copy assignment operator
	second = third;           

	// assignment by move assignment operator
	second = std::move(third);


	forest_type forest;
	// Insert some trees at some positions
	forest.insert(1, std::string("populus_tremula"));
	forest.insert(2, std::string("salix_nigra"));
	forest.insert(1, std::string("populus_tremula"));

	// Retrieve an iterator pointing at the inserted value
	auto it = forest.insert(5, std::string("quercus_robur"));
	assert(it->first == 5);

	// Number of trees in the forest
	assert(forest.nb_trees() == 4);

	// Number of trees in the forest at place 1
	assert(forest.nb_trees(1) == 2);

	
	std::set<position_type> space;
	std::set<tree_type> species;
	for(auto const& it : forest){
		space.insert(it.first);
		species.insert(it.second);
	}

	// Testing spatial structure retrieval works well
	std::set<position_type> expected_space = {1,2,5};
	assert(expected_space == space);

	// Testing composition is ok.
	std::set<tree_type> expected_species = {"populus_tremula","quercus_robur","salix_nigra"};
	assert(expected_species == species);

	return 0;
}