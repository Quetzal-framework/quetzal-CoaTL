// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __FILTER_POLICY_H_INCLUDED__
#define __FILTER_POLICY_H_INCLUDED__

namespace quetzal
{
namespace coalescence
{
namespace occupancy_spectrum
{
namespace filter_policy
{
///
/// @brief Default policy: accept all spectrums whatever their sampling probability
///
struct return_always_true
{
    bool operator()(double /*param*/) const
    {
        return true;
    }
}; // end struct return_always_true
} // namespace filter_policy
} // namespace occupancy_spectrum
} // namespace coalescence
} // namespace quetzal

#endif
