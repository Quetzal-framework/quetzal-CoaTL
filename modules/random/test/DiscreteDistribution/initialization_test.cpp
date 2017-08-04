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

#include <random>
#include <iostream>

int main () {
  std::mt19937 gen;
  using quetzal::DiscreteDistribution;

	// initialization by default constructor
	DiscreteDistribution<int> first;

	// initialization by copy constructor
	DiscreteDistribution<int> second(first);

  // initialization by move constructor
	DiscreteDistribution<int> third(std::move(first)); // first should not be used again

	// Also initialization by copy constructor
	DiscreteDistribution<int> fourth = second;

	// assignment by copy assignment operator
	fourth = third;

	// assignment by move assignment operator
	fourth = std::move(second); // second should not be used again

  std::discrete_distribution<int> e;

  // Default construction is a Dirac on the default constructible value.
  std::cout << fourth(gen) << std::endl;

  return 0;

  //! [Example]

}
