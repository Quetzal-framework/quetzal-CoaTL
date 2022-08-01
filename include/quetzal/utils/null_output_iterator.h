
// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __NULL_OUTPUT_ITERATOR_H_INCLUDED__
#define __NULL_OUTPUT_ITERATOR_H_INCLUDED__

#include <iterator>

namespace quetzal::utils
{
  ///
  /// @brief Allows to discard the output of functions that requires an output iterator
  ///
  /// @note Use in TajimasD class
  ///
  template<class Base = std::iterator< std::output_iterator_tag, null_output_iterator > >
  struct null_output_iterator : Base
  {
    ///
    /// @brief No-op assignment
    ///
    template<typename T>
    void operator=(T const&) {}
    ///
    /// @brief Can be pre-incremented
    ///
    null_output_iterator & operator++()
    {
      return *this;
    }
    ///
    /// @brief Can be post-incremented
    ///
    null_output_iterator operator++(int)
    {
      return *this;
    }
    ///
    /// @brief Can be dereferenced
    ///
    null_output_iterator & operator*()
    {
      return *this;
    }
  };
}

#endif
