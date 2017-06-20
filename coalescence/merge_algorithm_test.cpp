// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o test merge_algorithm_test.cpp -std=c++14 -Wall

#include "merge_algorithm.h"

#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <assert.h>

template<typename Container>
void coalesce(Container nodes){
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
  auto last = coalescence::binary_merge(nodes.begin(), nodes.end(), init, binary_operator, g);

  std::cout << "\nAfter one binary merge generation:\n";
  for(auto it = nodes.begin(); it != last; ++it){
    std::cout << *it << std::endl;
  }

  // Simultaneous multiple merger
  assert(std::distance(nodes.begin(), last) == 3);
  std::vector<unsigned int> M_j = {99,0,0,1};
  last = coalescence::simultaneous_multiple_merge(nodes.begin(), last, init, M_j, binary_operator, g);

  std::cout << "\nAfter one simultaneous multiple merge generation:\n";
  for(auto it = nodes.begin(); it != last; ++it){
    std::cout << *it << std::endl;
  }
}

int main () {

  std::vector<std::string> str_nodes = {"a", "b", "c", "d"};
  std::vector<int> int_nodes = {1,1,1,1};

  coalesce(str_nodes);
  coalesce(int_nodes);

  return 0;
}
