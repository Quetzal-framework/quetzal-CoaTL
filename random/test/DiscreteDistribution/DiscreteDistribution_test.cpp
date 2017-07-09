// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#include "../../DiscreteDistribution.h"

//! [Example]

#include <random>    // std::mt19937
#include <iostream>
#include <string>
#include <map>

int main () {
  using quetzal::DiscreteDistribution;
  std::mt19937 gen;
	DiscreteDistribution<std::string> d({"Bordeaux", "Paris"}, {0.2,0.8});
  std::map<std::string, int> m;
  for(int n=0; n<10000; ++n) {
      ++m[d(gen)];
  }
  for(auto p : m) {
      std::cout << p.first << " generated " << p.second << " times\n";
  }

	return 0;

  //! [Example]

}
