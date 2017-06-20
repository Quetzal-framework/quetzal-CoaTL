// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/ 

/**
 * Program for unit testing
 *
 * Test program for trees creation policies
 * Compiles with g++ -o test tree_creation_policies_test.cpp -std=c++14 -Wall
 */

#include "tree_creation_policies.h"
#include "assert.h"
#include <string>

using coalescence::policies::RecordableTime; 
using coalescence::policies::RecordableSpace;

// Some illustration data classes inheriting from policies base classes

template<typename Time>
class TimeData : 
public RecordableTime<Time>
{};

template<typename Space>
class SpaceData : 
public RecordableSpace<Space>
{};

template<typename Space, typename Time>
class SpaceAndTimeData : 
public RecordableSpace<Space>, 
public RecordableTime<Time>
{};

int main(){

	using deme_type = int;
	using time_type = int;
	time_type t = 12;
	deme_type x = 1;

	// Remember neither space or deme, use basic data type, ie string, copying its value.
	std::string s = "foo";
	auto tree1 = coalescence::policies::copy_data::make_tree(x, t, s);
	// retrieval
	assert(tree1.cell() == "foo");

	// Remember time at construction
	TimeData<time_type> a;
	auto tree2 = coalescence::policies::record_time::make_tree(x, t, a);
	//retrieval
	assert(tree2.cell().apparition_time() == 12);

	// Remember deme at construction
	SpaceData<deme_type> b;
	auto tree3 = coalescence::policies::record_deme::make_tree(x, t, b);
	//retrieval
	assert(tree3.cell().apparition_deme() == 1);

	// Remember both time and deme at construction
	SpaceAndTimeData<deme_type, time_type> c;
	auto tree4 = coalescence::policies::record_time_and_deme::make_tree(x, t, c);
	// retrieval
	assert(tree4.cell().apparition_time() == 12 && tree4.cell().apparition_deme() == 1);
}
