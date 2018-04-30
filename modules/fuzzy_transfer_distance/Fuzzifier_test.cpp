// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/ 

// g++ -o Fuzzifier_test Fuzzifier_test.cpp -std=c++14 -Wall

#include "Fuzzifier.h"
#include <iostream>

int main(){
	using Element = int;
	using Cluster = std::string;
	std::unordered_map<Element, std::vector<Cluster>> data = {  {0, {"a", "b", "a", "a"}}, 
																{1, {"b", "b", "b"}},
																{2, {"a", "b", "c"}} };

	auto M = fuzzy_transfer_distance::Fuzzifier::fuzzifie(data);
	std::cout << M << std::endl;
 	return 0;
}