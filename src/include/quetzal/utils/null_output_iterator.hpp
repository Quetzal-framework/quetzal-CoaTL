
// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
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
template <typename T> class null_output_iterator
{
  private:
    T _p;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = T;
    using pointer = T *;
    using reference = T &;
    ///
    /// @brief Constructor
    null_output_iterator(T p) : _p(p){};
    ///
    /// @brief Assignment
    ///
    void operator=(T p)
    {
        _p = p;
    }
    ///
    /// @brief Can be pre-incremented
    ///
    null_output_iterator &operator++()
    {
        _p++;
        return *this;
    }
    ///
    /// @brief Can be post-incremented
    ///
    null_output_iterator operator++(int)
    {
        _p++;
        return *this;
    }
    ///
    /// @brief Can be dereferenced
    ///
    null_output_iterator &operator*()
    {
        return *this;
    }
    ///
    /// @brief We want comparison it != end;
    friend bool operator==(const null_output_iterator &a, const null_output_iterator &b)
    {
        return a._p == b._p;
    };
    friend bool operator!=(const null_output_iterator &a, const null_output_iterator &b)
    {
        return a._p != b._p;
    };
};
} // namespace quetzal::utils

#endif
