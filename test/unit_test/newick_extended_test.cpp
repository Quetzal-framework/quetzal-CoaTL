// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE extended_newick_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/io.h>

BOOST_AUTO_TEST_SUITE( extended_newick_formatting )


BOOST_AUTO_TEST_CASE(extended_newick_filters)
{
  std::string s = "((1, ((2, (3, (4)Y#H1)g)e, (((Y#H1, 5)h, 6)f)X#H2)c)a, ((X#H2, 7)d, 8)b)r;";

}


BOOST_AUTO_TEST_SUITE_END()
