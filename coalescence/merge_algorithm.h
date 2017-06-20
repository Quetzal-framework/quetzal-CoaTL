// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __COALESCENCE_MERGE_ALGORITHM_H_INCLUDED__
#define __COALESCENCE_MERGE_ALGORITHM_H_INCLUDED__

#include <algorithm> // std::shuffle
#include <iterator> // std::advance

namespace coalescence {

  //! \remark Return value : Output iterator to the element past the last element transformed.
  //! \remark It pointed value should be swappable
  template<class BidirectionalIterator, class T, class BinaryOperation, class Generator>
  auto binary_merge(BidirectionalIterator first, BidirectionalIterator last, T init, BinaryOperation const& binary_op, Generator& g) {
      std::shuffle(first, last, g);
      *first = binary_op(init, *first);
      *first = binary_op(*first, *(--last));
    return last;
  }

  //! \remark Return value : Output iterator to the element past the last element transformed.
  //! \remark It pointed value should be swappable
  template<class BidirectionalIterator, class T, class BinaryOperation, class Generator>
  auto simultaneous_multiple_merge(BidirectionalIterator first, BidirectionalIterator last, T init, std::vector<unsigned int> const& M_j, BinaryOperation const& binary_op, Generator& g) {
      std::shuffle(first, last, g);

       // directly go to binary merge.
      auto m_it = M_j.cbegin();
      std::advance(m_it, 2);
      int j = 2;

      while(m_it != M_j.cend()){
        for(unsigned int i = 1; i <= *m_it; ++i){ // loop on the m_j parents
          *first = binary_op(init, *first);
          for(int k = 1; k < j; ++k){ // loop on the others children
            *first = binary_op(*first, *(--last));
          }
          ++first;
        }
        ++j;
        ++m_it;
      }
      return last;
  }

} // namespace coalescence

#endif
