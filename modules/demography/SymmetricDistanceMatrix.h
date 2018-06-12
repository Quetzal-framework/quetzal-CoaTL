// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __SYMMETRIC_DISTANCE_MATRIX_H_INCLUDED__
#define __SYMMETRIC_DISTANCE_MATRIX_H_INCLUDED__

#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <vector>

namespace quetzal{
namespace demography{
  namespace dispersal{

template<typename Space, typename Value>
class SymmetricDistanceMatrix
{

  template<typename T>
  using matrix_type = boost::numeric::ublas::symmetric_matrix<T>;

  std::vector<Space> m_points;
  matrix_type<Value> m_matrix;

public:

  using coord_type = Space;
  using value_type = Value;

template<typename F>
 SymmetricDistanceMatrix(std::vector<coord_type> const& points, F f):
 m_points(points),
 m_matrix(construct_matrix(points, f))
 {}

 SymmetricDistanceMatrix(std::vector<coord_type> const& points, matrix_type<value_type> const& m):
 m_points(points),
 m_matrix(m)
 {}

 friend std::ostream& operator <<(std::ostream& Stream, const SymmetricDistanceMatrix & M)
 {
     Stream << M.m_matrix;
     return Stream;
 }

 template<typename UnaryOperation>
 auto apply(UnaryOperation op)
 {
   using other_type = typename std::result_of_t<UnaryOperation(std::remove_cv_t<std::remove_reference_t<const value_type &>>)>;

   matrix_type<other_type> m (m_points.size(), m_points.size());
   for (unsigned i = 0; i < m.size1 (); ++ i)
   {
     for (unsigned j = 0; j <= i; ++ j)
     {
       m (i, j) = op(m_matrix(i, j));
     }
   }

   return SymmetricDistanceMatrix<coord_type, other_type>(m_points, m);
 }

private:

  template<typename F>
  auto construct_matrix(std::vector<coord_type> const& points, F f) const
  {
    matrix_type<value_type> m (points.size(), points.size());
    for (unsigned i = 0; i < m.size1 (); ++ i)
    {
      for (unsigned j = 0; j <= i; ++ j)
      {
        m (i, j) = f(points.at(i), points.at(j));
      }
    }
    return m;
  }

};

template<typename T, typename F>
auto make_symmetric_distance_matrix(std::vector<T> const& points, F f){
  using value_type = typename std::result_of_t<F(std::remove_cv_t<std::remove_reference_t<const T &>>,
                                                 std::remove_cv_t<std::remove_reference_t<const T &>>)>;
  return SymmetricDistanceMatrix<T,value_type>(points, f);
}

} // namespace dispersal
} // namespace geography
} // namespace quetzal

#endif
