// Copyright 2017 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "../../OccupancySpectrumDistribution.h"

//! [Example]

#include <random>    // std::mt19937
#include <algorithm> // std::accumulate
int main(){

  using quetzal::coalescence::occupancy_spectrum::OccupancySpectrumDistribution;
  unsigned int n = 5;
  unsigned int m = 10;

  OccupancySpectrumDistribution<> dist1(n, m);
  std::cout << "Throwing " << dist1.n() << " balls in " << dist1.m() << " urns.\n";
  std::cout << "Full distribution: "
            << dist1.support().size() << " configurations and weights sum to "
            << std::accumulate(dist1.weights().begin(), dist1.weights().end(), 0.0)
            << std::endl;

   std::cout << dist1 << "\n" << std::endl;

  // Sample from it :
  std::mt19937 g;
  auto spectrum = dist1(g);

  // Truncate the empty spectrum for memory optimization
  using quetzal::coalescence::occupancy_spectrum::RetrieveLastEmptyUrns;
  using quetzal::coalescence::occupancy_spectrum::ReturnAlwaysTrue;
  OccupancySpectrumDistribution<ReturnAlwaysTrue, RetrieveLastEmptyUrns> dist2(n, m);
  std::cout << "Shorten spectra:\n" << dist2 << "\n" << std::endl;

  // Filter the occupancy spectrum with small probability.
  auto pred = [](double p){
    bool keep = false;
    if(p > 0.01){ keep = true; }
    return keep;
  };
  OccupancySpectrumDistribution<decltype(pred)> dist3(n, m, pred);
  std::cout << "Truncated distribution:\n" << dist3 << "\n" << std::endl;

  // Combine strategies
  OccupancySpectrumDistribution<decltype(pred), RetrieveLastEmptyUrns> dist4(n, m, pred);
  std::cout << "Combined strategies:\n" << dist4 << "\n" << std::endl;

  return 0;
}

//! [Example]
