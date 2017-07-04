// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o test Populations_test.cpp -std=c++14 -Wall

#include "../../PopulationSize.h"

#include <iostream>   // std::cout
#include <algorithm>  // std::copy
#include <iterator>   // std::ostream_iterator
#include <string>

#include "assert.h"

int main(){

	using coord_type = std::string;
	using time_type = unsigned int;
	using size_type = int;

	time_type t0 = 2017;
	coord_type x0 = "Paris";
	size_type N0 = 12;

	demography::PopulationSize<coord_type, time_type, size_type> N;
	assert(N.is_defined(x0, t0) == false );

	N(x0,t0) = N0;
	assert(N.is_defined(x0, t0) );
	assert( N.get(x0,t0) == N0 );

	assert( N.definition_space(t0).size() == 1 );

	N.set("Bordeaux", t0, 2*N0);

	auto X = N.definition_space(t0);
	std::copy(X.begin(), X.end(), std::ostream_iterator<coord_type>(std::cout, " "));

	return 0;
}
