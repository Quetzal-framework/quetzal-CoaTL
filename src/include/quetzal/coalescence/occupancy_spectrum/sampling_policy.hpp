// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __SAMPLING_POLICY_H_INCLUDED__
#define __SAMPLING_POLICY_H_INCLUDED__

#include "editor_policy.hpp"
#include "filter_policy.hpp"
#include "memoize.hpp"

#include <vector>
#include <random>
#include <utility> // std::forward, std::move

namespace quetzal
{
  namespace coalescence
  {
    namespace occupancy_spectrum
    {
      namespace sampling_policy
      {
        ///
        /// @brief Sample occupancy spectrums from a pre-computed ProbabilityDistribution object
        ///
        /// @tparam FilterPolicy A filter_policy class, by default filter_policy::return_always_true
        /// @tparam EditorPolicy A editor_policy class, by default editor_policy::identity
        ///
        template <class FilterPolicy = filter_policy::return_always_true, class EditorPolicy = editor_policy::identity>
        struct in_memoized_distribution
        {
          /// @brief Sample an occupancy spectrum
          ///
          /// @tparam Generator A random number generator type
          ///
          /// @param k the number of balls (lineages) to throw (coalesce)
          /// @param N the nummber of urns (parents)
          /// @param g random number generator
          ///
          /// @return a random OccupancySpectrum object
          template<typename Generator>
          static auto const& sample(int k, int N, Generator& g)
          {
            assert(k > 0);
            assert(N > 0);
            return memoize::memoize<FilterPolicy, EditorPolicy>(k, N)(g);
          }
        }; // end class in_memoized_distribution
        ///
        /// @brief Sample occupancy spectrums by simulating a ball to urn assignment random experiment.
        ///
        struct on_the_fly
        {
          /// @brief Sample an occupancy spectrum
          ///
          /// @tparam Generator A random number generator type
          ///
          /// @param k the number of balls (lineages) to throw (coalesce)
          /// @param N the nummber of urns (parents)
          /// @param g random number generator
          ///
          /// @return a random OccupancySpectrum object
          template<typename Generator>
          static auto sample(int k, int N, Generator& g)
          {
            assert(k > 0);
            assert(N > 0);
            std::vector<int> M_j(k+1);
            std::vector<int> parents(N);
            // Populate the urns (parents) by throwing balls (lineages) at them
            std::uniform_int_distribution<int> distribution(0, N-1);
            for(int i = 1; i <=k; ++i)
            {
              parents[distribution(g)] +=1;
            }
            // Build the occupancy spectrum by computing the occupancy numbers
            for(int it : parents)
            {
                M_j[it] += 1;
            }
            assert(M_j.size() == k+1);
            return OccupancySpectrum(M_j, k, N);
          }
        };
      } // end namespace sampling_policy
    } // namespace occupancy_spectrum
  } // namespace coalescence
} // namespace quetzal

#endif
