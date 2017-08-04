// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

/**
 * \file binary_merge_test.cpp
 * \brief Program for unit testing
 * \author Arnaud Becheler <Arnaud.Becheler@egce.cnrs-gif.fr>
 *
 * Test program for interface of coalescence merge algorithms.
 * Compiles with g++ -o test merge_test.cpp -std=c++14 -Wall
 *
 */

#include "../merge.h"

//! [Example]

#include <random>     // std::mt19937
#include <iostream>   // std::cout
#include <iterator>   // std::ostream_iterator
#include <algorithm>  // std::copy
#include <functional> // std::plus

int main(){

  using node_type = int;
  std::vector<node_type> nodes = {1,1,1,1,1};

  // 99 parents with no child, 1 parent with 2 children, 1 parent with 3 children
  std::vector<unsigned int> sp = {98,0,1,1};
  std::mt19937 rng;

  using quetzal::coalescence::simultaneous_multiple_merge;
  auto last = simultaneous_multiple_merge(nodes.begin(), nodes.end(), node_type(), sp, std::plus<node_type>(), rng);

  std::copy(nodes.begin(), last, std::ostream_iterator<node_type>(std::cout, " "));
  return 0;
}

 //! [Example]
