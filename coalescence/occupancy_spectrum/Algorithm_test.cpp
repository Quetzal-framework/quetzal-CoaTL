// Copyright 2017 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o test generative_algorithm_test.cpp -std=c++14 -Wall

#include "Algorithm.h"

#include <iostream>
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator

int main(){
  using namespace coalescence::occupancy_spectrum;
  using spectrum_type = Algorithm::occupancy_spectrum_type;

  auto spectrum_handler = [](spectrum_type const& M_j){
    std::copy(M_j.begin(), M_j.end(), std::ostream_iterator<spectrum_type::value_type>(std::cout, " "));
    std::cout << "\n";
  };

  Algorithm::generate(10,10, spectrum_handler);

  return 0;
}
