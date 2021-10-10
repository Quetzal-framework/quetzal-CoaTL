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
#include "PopulationSizeVectorImplementation.h"
#include "Flow.h"

namespace quetzal
{
  namespace memory
  {
    struct on_demand
    {
      template<typename Space, typename Time, typename Value>
      using pop_sizes_type = quetzal::demography::PopulationSizeHashMapImplementation<Space, Time, Value>;

      template<typename Space, typename Time, typename Value>
      using flow_type = quetzal::demography::Flow<Space, Time, Value>;
    };
    struct pre_allocated
    {
      template<typename Space, typename Time, typename Value>
      using pop_sizes_type = quetzal::demography::PopulationSizeVectorImplementation<Space>;

      template<typename Space, typename Time, typename Value>
      using flow_type = quetzal::demography::Flow<Space, Time, Value>;
    };
  } // end namespace memory
} // end namespace quetzal

#endif
