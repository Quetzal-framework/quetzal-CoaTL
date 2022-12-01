// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "demography/dispersal_kernel.h"
#include "demography/memory_policy.h"
#include "demography/History.h"


namespace quetzal
{
  ///
  /// @brief Generic components for simulation, storage and access of spatial demographic quantities.
  ///
  namespace demography
  {
    ///
    /// @brief Dispersal location kernels as defined in Nathan et al 2012, table 15.1
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
