// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __EDITOR_POLICY_H_INCLUDED__
#define __EDITOR_POLICY_H_INCLUDED__

#include "OccupancySpectrum.h"

#include <utility> // std::forward, std::move

namespace quetzal
{
  namespace coalescence
  {
    namespace occupancy_spectrum
    {
      namespace editor_policy
      {
        ///
        /// @brief No editions are operated on the spectrum (default behavior)
        ///
        struct identity
        {
          static OccupancySpectrum edit(OccupancySpectrum&& M_j)
          {
            return std::move(M_j);
          }
        }; // end struct identity
        ///
        /// @brief Edition consists in truncating the last nul elements of the spectrum to avoid useless iterations
        ///
        struct truncate_tail
        {
          static OccupancySpectrum edit(OccupancySpectrum&& M_j)
          {
            auto first = --(M_j.end());
            while (first != M_j.begin() && *first == 0)
            {
              --first;
            }
            M_j.erase(++first, M_j.end());
            return std::move(M_j);
          }
        }; // end struct truncate_tail
      } // end namespace editor_policy
    } // end namespace editor_policy
  } // end namespace coalescence
} // end namespace quetzal

#endif
