// Copyright 2017 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o test memoize.cpp -std=c++14 -Wall

#include "memoize.h"
#include <random>

int main(){
  std::mt19937 g;

  // build and copy sample
  auto spectrum_a = quetzal::coalescence::occupancy_spectrum::utils::memoize_OSD<>(10,10)(g);

  // no need to build, sample directly in the memoized distribution, no copy
  auto const& spectrum_b = quetzal::coalescence::occupancy_spectrum::utils::memoize_OSD<>(10,10)(g);
  (void)spectrum_b;

  return 0;
}
