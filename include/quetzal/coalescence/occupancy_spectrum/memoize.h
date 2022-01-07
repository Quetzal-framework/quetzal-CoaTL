
// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __OCCUPANCY_SPECTRUM_MEMOIZE_H_INCLUDED__
#define __OCCUPANCY_SPECTRUM_MEMOIZE_H_INCLUDED__

#include "editor_policy.h"
#include "filter_policy.h"
#include "ProbabilityDistribution.h"

#include <unordered_map>
#include <functional> // std::hash

namespace quetzal
{
  namespace coalescence
  {
    namespace occupancy_spectrum
    {
      namespace memoize
      {
        ///
        /// @brief To store pairs of distribution parameters \f$k\f$ and \f$N\f$ in a hashmap
        ///
        struct hash_parameters
        {
          std::size_t operator()(const std::pair<int, int>& param) const
          {
            static const auto hacher = std::hash<int>();
            const std::size_t h1 = hacher(param.first);
            const std::size_t h2 = hacher(param.second);
            return h1 ^ (h2 << 1);
          }
        };
        ///
        /// @brief Store on RAM the entire distributions of occupancy spectrums for every pair \f$k\f$ and \f$N\f$.
        ///
        /// @param k the number of balls (lineages) to throw (coalesce)
        /// @param N the nummber of urns (parents)
        ///
        /// @return A reference on a pre-computed OccupancySpectrumDistribution
        ///
        /// @remark Builds the distribution the first time that the pair \f$\{k,N\}\f$ is called.
        ///         Next time the pair is encountered, it returns a reference to the already-computed result.
        ///
        template <class FilterPolicy = filter_policy::return_always_true, class EditorPolicy = editor_policy::identity>
        const ProbabilityDistribution<FilterPolicy, EditorPolicy>&
        memoize(int k, int N)
        {
          static std::unordered_map<std::pair<int, int>, ProbabilityDistribution<FilterPolicy, EditorPolicy>, hash_parameters> cache;
          const auto pair_k_N = std::make_pair(k, N);
          const auto it = cache.find(pair_k_N);
          if(it != cache.end())
          {
            return it->second;
          }
          const auto pair_itNewResult_true = cache.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(pair_k_N),
            std::forward_as_tuple(k,N)
          );
          return pair_itNewResult_true.first->second;
        }
      } // end namespace memoize
    } // end namespace occupancy_spectrum
  } // end namespace coalescence
} // end namespace quetzal


#endif
