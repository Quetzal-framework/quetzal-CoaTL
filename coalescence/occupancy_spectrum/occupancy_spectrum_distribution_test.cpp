// Copyright 2017 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o test occupancy_spectrum_distribution_test.cpp -std=c++14 -Wall

#include "occupancy_spectrum_distribution.h"
#include <random>
#include <algorithm> // std::accumulate
#include "../../../presentation/printers.h"

template<typename OSD>
void print(OSD const& dist){
  std::cout << "\nk = " << dist.k() << ", N = " << dist.N() << " : " << "\n";
  std::cout << dist;
  double sum = std::accumulate(dist.weights().cbegin(), dist.weights().cend(), 0.);
  std::cout << "\nProbabilities sum to : " << sum << std::endl;
}

int main(){

  // M_j configuration sampling
  std::vector<unsigned int> Ns = {10,50,100};
  std::vector<unsigned int> ks = {3,4,5,10,20};

  using coalescence::occupancy_spectrum::OccupancySpectrumDistribution;
  using coalescence::occupancy_spectrum::HandleRetrieveLastEmptyUrns;
  using coalescence::occupancy_spectrum::HandleIdentity;

  auto pred = [](double p){
    bool memoize_spectrum = false;
    if(p > 0.0001){ memoize_spectrum = true; }
    return memoize_spectrum;
  };


  for(auto k : ks){
    for(auto N : Ns){
      OccupancySpectrumDistribution<> dist1(k,N);
      OccupancySpectrumDistribution<decltype(pred), HandleRetrieveLastEmptyUrns> dist2(k,N, pred);
      print(dist1);
      print(dist2);
    }
  }



  OccupancySpectrumDistribution<decltype(pred), HandleRetrieveLastEmptyUrns> dist(5,10, pred);
  std::cout << dist << std::endl;
  std::mt19937 g;
  std::cout << "Sampling occuapncy spectrum :\n";
  for(int i = 0; i < 10; ++i)
    std::cout << dist(g) << std::endl;

  return 0;
}
