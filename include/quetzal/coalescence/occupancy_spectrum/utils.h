// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __OCCUPANCY_SPECTRUM_SAMPLER_H_INCLUDED__
#define __OCCUPANCY_SPECTRUM_SAMPLER_H_INCLUDED__

#include <vector>
#include <random>
#include <utility> // std::forward, std::move

namespace quetzal{
namespace coalescence {
namespace occupancy_spectrum{

template<class Spectrum>
bool test_number_of_balls_conservation(unsigned int k, Spectrum const& M_j){
  unsigned int sum = 0;
  for(unsigned int i = 0; i < M_j.size(); ++i)  {
    sum += i*M_j.at(i);
  }
  return(sum == k);
}

template<class Spectrum>
bool test_number_of_urns_conservation(unsigned int m, Spectrum const& M_j){
  unsigned int sum = 0;
  for(unsigned int i = 0; i < M_j.size(); ++i)  {
    sum += M_j.at(i);
  }
  return(sum == m);
}

struct return_always_true {
    bool operator()(double /*param*/) const
    {
      return true;
    }
};

template<class Spectrum>
struct identity {
    static Spectrum handle(Spectrum&& M_j)
    {
        return std::move(M_j);
    }
};

template<class Spectrum>
struct truncate_tail {
  static Spectrum handle(Spectrum&& M_j)
  {
    auto first = --(M_j.end());
    while (first != M_j.begin() && *first == 0)
    {
      --first;
    }
    M_j.erase(++first, M_j.end());
    return std::move(M_j);
  }
};

} // namespace occupancy_spectrum
} // namespace coalescence
} // namespace quetzal

#endif
