// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

/**
 * \file merger_test.cpp
 * \brief Program for unit testing
 * \author Arnaud Becheler <Arnaud.Becheler@egce.cnrs-gif.fr>
 *
 * Test program for merger policies.
 * Compiles with g++ -o test merger_test.cpp -std=c++14 -Wall
 *
 */

#include "merger.h"

#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <assert.h>

template<typename Container>
void coalesce(Container nodes, unsigned int N){

  using quetzal::coalescence::SimultaneousMultipleMerger;
  using quetzal::coalescence::BinaryMerger;
  using T = typename Container::value_type;

  std::mt19937 g;

  std::cout << "\nNodes at sampling time :\n";
  std::copy(nodes.begin(), nodes.end(), std::ostream_iterator<T>(std::cout, "\n"));
  std::cout << "\n";

  auto init = T();

  // Define what coalescence means in term of data representation
  // In this example, operator is suitable for both types string and int
  auto binary_operator = [](T parent, T child){ return parent + child;};

  // Binary merger
  auto last = BinaryMerger::merge(nodes.begin(), nodes.end(), N, init, binary_operator, g);

  std::cout << "\nAfter one binary merge generation:\n";
  for(auto it = nodes.begin(); it != last; ++it){
    std::cout << *it << std::endl;
  }

  // Simultaneous multiple merger
  assert(std::distance(nodes.begin(), last) == 3);
  using spectrum_creation_policy = quetzal::coalescence::occupancy_spectrum::on_the_fly;
  last = SimultaneousMultipleMerger<spectrum_creation_policy>::merge(nodes.begin(), last, N, init, binary_operator, g);

  std::cout << "\nAfter one simultaneous multiple merge generation:\n";
  for(auto it = nodes.begin(); it != last; ++it){
    std::cout << *it << std::endl;
  }
}

int main () {

  std::vector<std::string> str_nodes = {"a", "b", "c", "d"};
  std::vector<int> int_nodes = {1,1,1,1};
  unsigned int N = 3;
  coalesce(str_nodes, N);
  coalesce(int_nodes, N);

  return 0;
}
