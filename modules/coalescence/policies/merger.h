// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __COALESCENCE_MERGERS_H_INCLUDED__
#define __COALESCENCE_MERGERS_H_INCLUDED__

#include "../algorithms/merge.h"
#include "../occupancy_spectrum/spectrum_creation_policy.h" // default policies

#include <iterator> // std::distance

namespace quetzal{
namespace coalescence {

  struct BinaryMerger{
    template<class BidirectionalIterator, class T, class BinaryOperation, class Generator>
    static auto
    merge(BidirectionalIterator first, BidirectionalIterator last, unsigned int N, T const& init, BinaryOperation const& binop, Generator& g)
    {
        double coal_proba = 1/static_cast<double>(N);
        std::bernoulli_distribution d(coal_proba);
        if( d(g) ){
          last = binary_merge(first, last, init, binop, g);
        }
        return last;
      }
  };

  template<class SpectrumCreationPolicy>
  struct SimultaneousMultipleMerger {
    template<class BidirectionalIterator, class T, class BinaryOperation, class Generator>
    static auto
    merge(BidirectionalIterator first, BidirectionalIterator last, unsigned int N, T const& init, BinaryOperation const& binop, Generator& g)
    {
      unsigned int k = std::distance(first, last);
      return simultaneous_multiple_merge(first, last, init, SpectrumCreationPolicy::sample(k, N, g), binop, g);
    }
  };

} // namespace coalescence
} // namespace quetzal

#endif
