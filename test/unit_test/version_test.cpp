// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE random_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/version.h>
#include <iostream>

BOOST_AUTO_TEST_SUITE( random_suite )

BOOST_AUTO_TEST_CASE( discrete_distribution )
{
  std::cout << gVERSION << std::endl;
  std::cout << gVERSION_SHORT << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
