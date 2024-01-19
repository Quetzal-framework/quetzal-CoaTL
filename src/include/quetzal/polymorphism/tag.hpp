// Copyright 2022 Arnaud Becheler    <arnaud.becheler@gmail.com>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __QUETZAL_ACCUMULATOR_H_INCLUDED__
#define __QUETZAL_ACCUMULATOR_H_INCLUDED__

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>

namespace boost::accumulators::tag
{
struct segregating_sites;
struct pairwise_differences;

///
/// @brief Tajima's statistics accumulator tag
///
struct tajima : depends_on<count, pairwise_differences, segregating_sites>
{
    // Define a nested typedef called 'impl' that specifies which accumulator implements this feature.
    typedef accumulators::impl::tajima_accumulator<mpl::_1> impl;
};
} // namespace boost::accumulators::tag

#endif
