// Copyright 2017 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o test occupancy_spectrum_sampler_test.cpp -std=c++14 -Wall

#include "spectrum_creation_policy.h"
#include "../../../presentation/printers.h"
#include <random>
#include <iostream>

int main(){

  std::mt19937 g;

  auto a = coalescence::occupancy_spectrum::on_the_fly::sample(10,10, g);
  std::cout << a << std::endl;
  auto b = coalescence::occupancy_spectrum::in_memoized_distribution<>::sample(10,10,g);
  std::cout << b << std::endl;

  return 0;
}
