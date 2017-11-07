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


#include "../../merger.h"

//! [Example]

#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <cassert>

int main () {

  using node_type = std::string;
  using quetzal::coalescence::BinaryMerger;

  std::vector<node_type> nodes = {"a", "b", "c", "d"};

  unsigned int N = 3;

  std::mt19937 gen;

  std::cout << "\nNodes at sampling time :\n";
  std::copy(nodes.begin(), nodes.end(), std::ostream_iterator<node_type>(std::cout, "\n"));
  std::cout << "\n";

  auto last = BinaryMerger::merge(nodes.begin(), nodes.end(), N, gen);

  std::cout << "\nAfter one binary merge generation:\n";
  for(auto it = nodes.begin(); it != last; ++it){
    std::cout << *it << std::endl;
  }

  return 0;
}

//! [Example]
