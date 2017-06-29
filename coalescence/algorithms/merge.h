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

namespace quetzal{
namespace coalescence {

  /**
   * \brief merge 2 randomly selected elements in a range (function template).
   * \param first iterator at the begin of the range
   * \param last iterator to the past-the-end element.
   * \param init the value at which parent is initialized
   * \param op binary operation function object that will be applied for branching a
               child to its parent. The binary operator takes the parent value a
               (initialized to init) and the value of the child b. The signature
               of the function should be equivalent to the following:
               Ret fun(const Type1 &parent, const Type2 &child);
   * \return Past-the-end iterator for the new range of values
   */
  template<class BidirectionalIterator, class T, class BinaryOperation, class Generator>
  auto binary_merge(BidirectionalIterator first, BidirectionalIterator last, T init, BinaryOperation op, Generator& g) {
      std::shuffle(first, last, g);
      *first = op(init, *first);
      *first = op(*first, *(--last));
    return last;
  }

  /**
   * \brief merge randomly selected elements in a range according to a given configuration (function template).
   * \param first iterator at the begin of the range
   * \param last iterator to the past-the-end element.
   * \param sp occupancy spectrum giving the merging configuration
   * \param init the value at which parent is initialized
   * \param op binary operation function object that will be applied for branching a
               child to its parent. The binary operator takes the parent value a
               (initialized to init) and the value of the child b. The signature
               of the function should be equivalent to the following:
               Ret fun(const Type1 &parent, const Type2 &child);
   * \return Past-the-end iterator for the new range of values
   */
  template<class BidirectionalIterator, class T, class BinaryOperation, class OccupancySpectrum, class Generator>
  auto simultaneous_multiple_merge(BidirectionalIterator first, BidirectionalIterator last,
                                   T init, OccupancySpectrum const& sp, BinaryOperation op, Generator& g) {
      std::shuffle(first, last, g);

       // directly go to binary merge.
      auto m_it = sp.cbegin();
      std::advance(m_it, 2);
      int j = 2;

      while(m_it != sp.cend()){
        for(unsigned int i = 1; i <= *m_it; ++i){ // loop on the m_j parents
          *first = op(init, *first);
          for(int k = 1; k < j; ++k){ // loop on the others children
            *first = op(*first, *(--last));
          }
          ++first;
        }
        ++j;
        ++m_it;
      }
      return last;
  }

} // namespace coalescence
} // namespace quetzal

#endif
