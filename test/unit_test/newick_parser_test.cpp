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

#define BOOST_SPIRIT_X3_DEBUG

#include <boost/spirit/home/x3.hpp>
#include "quetzal/io/newick_parser.h"
#include <iomanip>

namespace x3 = boost::spirit::x3;


BOOST_AUTO_TEST_SUITE( newick_parser )

BOOST_AUTO_TEST_CASE(name_grammar)
{
  std::vector<std::string> inputs =
  {
    "a34",
    "A856BC",
    "k56aH"
  };

  for(const auto& input : inputs)
  {
    auto iter = input.begin();
    auto iter_end = input.end();
    bool r = phrase_parse(iter, iter_end, quetzal::newick::parser::name, x3::space );
    BOOST_CHECK(r && iter == iter_end);
  }
}

BOOST_AUTO_TEST_CASE(length_grammar)
{
  std::vector<std::string> inputs =
  {
    ":1",
    ":999",
    ":0.001"
  };

  for(const auto& input : inputs)
  {
    auto iter = input.begin();
    auto iter_end = input.end();
    bool r = phrase_parse(iter, iter_end, quetzal::newick::parser::length, x3::space );
    BOOST_CHECK(r && iter == iter_end);
  }
}

BOOST_AUTO_TEST_CASE(leaf_grammar)
{
  std::vector<std::string> inputs =
  {
    "",
    "A",
    "B897kj"
  };

  for(const auto& input : inputs)
  {
    auto iter = input.begin();
    auto iter_end = input.end();
    bool r = phrase_parse(iter, iter_end, quetzal::newick::parser::leaf, x3::space );
    BOOST_CHECK(r && iter == iter_end);
  }
}

BOOST_AUTO_TEST_CASE(branch_grammar)
{
  std::vector<std::string> inputs =
  {
    "",
    "A",
    "A38",
    "A38:10.0"
  };

  for(const auto& input : inputs)
  {
    auto iter = input.begin();
    auto iter_end = input.end();
    bool r = phrase_parse(iter, iter_end, quetzal::newick::parser::branch, x3::space );
    BOOST_CHECK(r && iter == iter_end);
  }
}

BOOST_AUTO_TEST_CASE(internal_grammar)
{
  std::vector<std::string> inputs =
  {
    "(,)",
    "(A,B)F",
    "(A:10,B:10)F"
  };

  for(const auto& input : inputs)
  {
    auto iter = input.begin();
    auto iter_end = input.end();
    bool r = phrase_parse(iter, iter_end, quetzal::newick::parser::internal, x3::space );
    BOOST_CHECK(r && iter == iter_end);
  }
}

BOOST_AUTO_TEST_CASE(subtree_grammar)
{
  std::vector<std::string> inputs =
  {
    "leaf",
    "(in,ternal)",
    "(,,(,))",
    "(A,B,(C,D))",
    "(A,B,(C,D)E)F",
    "(:0.1,:0.2,(:0.3,:0.4):0.5)",
    "(:0.1,:0.2,(:0.3,:0.4):0.5)",
    "(A:0.1,B:0.2,(C:0.3,D:0.4):0.5)",
    "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F",
    "((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A"
  };

  for(const auto& input : inputs)
  {
    auto iter = input.begin();
    auto iter_end = input.end();
    bool r = phrase_parse(iter, iter_end, quetzal::newick::parser::subtree, x3::space );
    BOOST_CHECK(r && iter == iter_end);
  }
}

BOOST_AUTO_TEST_CASE(full_grammar)
{
  std::vector<std::string> inputs =
  {
    ";",
    "(,);",
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
    std::cout << quoted(input) << " \t-> " << std::boolalpha
              << parse(begin(input), end(input), quetzal::newick::parser::tree) << std::endl;
    BOOST_CHECK(r && iter == iter_end);
  }
}

BOOST_AUTO_TEST_SUITE_END()
