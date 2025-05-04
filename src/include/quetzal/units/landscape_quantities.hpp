// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __LANDSCAPE_QUANTITIES_H_INCLUDED__
#define __LANDSCAPE_QUANTITIES_H_INCLUDED__

#include <iostream>
#include <type_traits>
#include <units/base_dimension.h>
#include <units/isq/si/prefixes.h>
#include <units/quantity.h>
#include <units/quantity_io.h>
#include <units/unit.h>

using namespace units;
using namespace units::isq;

namespace quetzal::units
{
///
/// @brief The extent to which a local habitat is suitable to a species
///
template <typename T = double> using habitability = units::dimensionless<units::one, ranged_representation<T, 0, 1>>;

struct generation : named_unit<generation, "generation">
{
};

///
/// @brief Percentage change of a specific variable within a specific time period.
///
template <typename T = double> using growth_rate = units::dimensionless<percent> / units::dim_time;

} // namespace quetzal::units
#endif
