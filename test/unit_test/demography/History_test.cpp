// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE History_test

#include <modules/demography/History.h>

#include <boost/test/included/unit_test.hpp>
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE( history_test )

BOOST_AUTO_TEST_CASE( boost_features )
{
  using strategy_type = quetzal::demography::strategy::mass_based;
  using matrix_type = typename strategy_type::Interface::matrix_type;
  matrix_type m(3,3);
  for(unsigned int i = 0; i < m.size1()){
    for(unsigned int j = 0; i < m.size2()){
      m(i,j) = 1;
    }
  }
  std::cout << m << std::endl;
  auto B = strategy_type::divide_terms_by_row_sum(A);
  std::cout << B << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
