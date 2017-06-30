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
  std::vector<node_type> nodes = {1,1,1,1};
  std::mt19937 rng;

  using quetzal::coalescence::binary_merge;
  auto last = binary_merge(nodes.begin(), nodes.end(), node_type(), std::plus<node_type>(), rng);

  std::copy(nodes.begin(), last, std::ostream_iterator<node_type>(std::cout, " "));
  return 0;
}

 //! [Example]
