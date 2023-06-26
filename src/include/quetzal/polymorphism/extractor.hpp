// Copyright 2022 Arnaud Becheler    <arnaud.becheler@gmail.com>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __QUETZAL_EXTRACTORS_H_INCLUDED__
#define __QUETZAL_EXTRACTORS_H_INCLUDED__

namespace boost::accumulators
{
  namespace extract
  {
    ///
    /// @brief Define the tajima extractor
    ///
    extractor< tag::tajima > const tajima = {};
  }

///
/// @brief Pull the extractor into the enclosing namespace.
///
using extract::tajima;

}

#endif
