// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE newick_parser_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <boost/spirit/home/x3.hpp>
#include "quetzal/io/newick_parser.h"

namespace x3 = boost::spirit::x3;

BOOST_AUTO_TEST_SUITE( newick_parser )

BOOST_AUTO_TEST_CASE(full_grammar)
{
  std::vector<std::string> inputs =
  {
    "(,,(,));",
    "(A,B,(C,D));",
    "(A,B,(C,D)E)F;",
    "(:0.1,:0.2,(:0.3,:0.4):0.5);",
    "(:0.1,:0.2,(:0.3,:0.4):0.5):0.0;",
    "(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);",
    "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;",
    "((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;"
  };

  for(const auto& input : inputs)
  {
    auto iter = input.begin();
    auto iter_end = input.end();
    bool r = phrase_parse(iter, iter_end, quetzal::newick::parser::tree, x3::space );
    BOOST_CHECK(r);
  }
}

BOOST_AUTO_TEST_SUITE_END()
