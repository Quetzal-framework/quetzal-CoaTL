// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "demography/dispersal_kernel.hpp"
#include "demography/memory_policy.hpp"
#include "demography/History.hpp"


namespace quetzal
{
  ///
  /// @brief Simulation, storage and access of spatial demographic quantities.
  ///
  namespace demography
  {
    ///
    /// @brief Dispersal location kernels as defined in Nathan et al 2012, table 15.1
    /// @details Dispersal kernel refer to the statistical distribution of dispersal distances 
    ///          in a population. In this implementation it is essentially the probability density function (pdf) 
    ///          describing the distribution of the post-dispersal locations relatively to
    ///          the source location. Expressions have been re-parametrized to include a scale parameter \f$ a \f$ 
    ///          homogeneous to a distance, and a shape parameter \f$ b \f$ determining the shape of the curve 
    ///          (weight of long-distance events).
    ///
    namespace dispersal_kernel{}
    ///
    /// @brief Policy classes to customize the dispersal behavior of the simulation algorithm.
    ///
    namespace demographic_policy {}
    ///
    /// @brief Policy classes to customize the memory storage behavior of the demographic history.
    ///
    namespace memory_policy {}
  }
}
