// Copyright 2017 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o test occupancy_spectrum_distribution_test.cpp -std=c++14 -Wall

#include "OccupancySpectrumDistribution.h"
#include <random>    // std::mt19937

int main(){

  using quetzal::coalescence::occupancy_spectrum::OccupancySpectrumDistribution;
  unsigned int k = 5;
  unsigned int N = 10;

  // Default construction
  OccupancySpectrumDistribution<> dist1(k, N);
  std::cout << dist1 << std::endl;

  // Sample from it :
  std::mt19937 g;
  auto spectrum = dist1(g);

  // Truncate the empty spectrum for memory optimization
  using quetzal::coalescence::occupancy_spectrum::RetrieveLastEmptyUrns;
  using quetzal::coalescence::occupancy_spectrum::ReturnAlwaysTrue;
  OccupancySpectrumDistribution<ReturnAlwaysTrue, RetrieveLastEmptyUrns> dist4(k, N);
  std::cout << dist4 << std::endl;

  // Filter the occupancy spectrum with small probability.
  auto pred = [](double p){
    bool keep = false;
    if(p > 0.01){ keep = true; }
    return keep;
  };
  OccupancySpectrumDistribution<decltype(pred)> dist2(k, N, pred);
  std::cout << dist2 << std::endl;

  // Add a filter for truncating the empty spectrum extremity.
  OccupancySpectrumDistribution<decltype(pred), RetrieveLastEmptyUrns> dist3(k, N, pred);
  std::cout << dist3 << std::endl;

  return 0;
}
