// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __IN_MEMOIZED_H_INCLUDED__
#define __IN_MEMOIZED_H_INCLUDED__

#include "OccupancySpectrumDistribution.h"
#include "memoize.h"
#include "utils.h"

#include <vector>
#include <random>

namespace quetzal{
namespace coalescence {
namespace occupancy_spectrum{

template
<
class UnaryPredicate = return_always_true,
class SpectrumHandler = identity<Generator::occupancy_spectrum_type>,
class Int = cpp_int,
class Float = cpp_dec_float_50
>
struct in_memoized_distribution{
  template<typename Generator>
  static auto const& sample(unsigned int k, unsigned int N, Generator& g){
    return utils::memoize_OSD<UnaryPredicate, SpectrumHandler, Int, Float>(k,N)(g);
  }
};

} // namespace occupancy_spectrum
} //namespace coalescence
} //namespace quetzal

#endif
