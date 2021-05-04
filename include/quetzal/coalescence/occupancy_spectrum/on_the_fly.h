// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __ON_THE_FLY_H_INCLUDED__
#define __ON_THE_FLY_H_INCLUDED__

#include "utils.h"

#include <vector>
#include <random>

namespace quetzal{
namespace coalescence {
namespace occupancy_spectrum{

struct on_the_fly {
  template<typename Generator>
  static auto sample(unsigned int k, unsigned int N, Generator& g){

    std::vector<unsigned int> M_j(k+1);
    std::vector<unsigned int> parents(N);

    std::uniform_int_distribution<unsigned int> dist(0,N-1);
    for(unsigned int i = 1; i <=k; ++i){
      parents[dist(g)] +=1;
    }

    for(unsigned int it : parents){
        M_j[it] += 1;
    }

    assert(M_j.size() == k+1);
    assert(test_number_of_balls_conservation(k, M_j));
    assert(test_number_of_urns_conservation(N, M_j));

    return M_j;
  }
};

} // namespace occupancy_spectrum
} //namespace coalescence
} //namespace quetzal

#endif
