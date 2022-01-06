// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __MEMORY_TRAIT_H_INCLUDED__
#define __MEMORY_TRAIT_H_INCLUDED__

#include "PopulationSizeHashMapImplementation.h"
#include "PopulationSizeOnDiskImplementation.h"

#include "FlowHashMapImplementation.h"
#include "FlowOnDiskImplementation.h"

namespace quetzal
{
  namespace demography
  {
    namespace memory_policy
    {
      ///
      /// @brief Keep the demographic data on RAM.
      ///
      struct on_RAM
      {
        template<typename Space, typename Time, typename Value>
        using pop_sizes_type = quetzal::demography::PopulationSizeHashMapImplementation<Space, Time, Value>;

        template<typename Space, typename Time, typename Value>
        using flow_type = quetzal::demography::FlowHashMapImplementation<Space, Time, Value>;
      };
      ///
      /// @brief Serialize the unused layers of demographic data on disk, and deserialize them when access is needed.
      ///
      struct on_disk
      {
        template<typename Space, typename Time, typename Value>
        using pop_sizes_type = quetzal::demography::PopulationSizeOnDiskImplementation<Space, Time, Value>;

        template<typename Space, typename Time, typename Value>
        using flow_type = quetzal::demography::FlowOnDiskImplementation<Space, Time, Value>;
      };
    } // end namespace memory
  } // end namespace demography
} // end namespace quetzal

#endif
