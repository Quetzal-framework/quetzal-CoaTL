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

#include "quetzal/io/newick/extended/to_network.hpp"

#include <iomanip>

namespace x3 = boost::spirit::x3;

struct FixtureCases
{
  // Set up fixture
  FixtureCases()
  {
    BOOST_TEST_MESSAGE( "setup fixture Newick cases" );

    this->name_cases =
    {
      "a34",
      "A856BC",
      "k56aH"
    };

    this->length_cases =
    {
      ":1",
      ":999",
      ":0.001"
    };

    this->leaf_cases =
    {
      "",
      "A",
      "B897kj"
    };

    this->branch_cases =
    {
      "leaf",
      "(in,ternal)",
      "leaf:10.0",
      "(in,ternal):50.0",
    };

    this->internal_cases =
    {
      "(,)",
      "(A,B)F",
      "(A:10,B:10)F"
    };

    this-> subtree_cases =
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

    this->tree_standard_cases =
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

    this->network_cases =
    {
      "((1, ((2, (3, (4)Y#H1)g)e, (((Y#H1, 5)h, 6)f)X#H2)c)a, ((X#H2, 7)d, 8)b)r;",
      "(A,B,((C,(Y)x#H1)c,(x#H1,D)d)e)f"
    };

  }

  // Tear down fixture
  ~FixtureCases()
  {
    BOOST_TEST_MESSAGE( "teardown fixture Newick cases" );
  }

  // Member data of fixture
  std::vector<std::string> name_cases;
  std::vector<std::string> length_cases;
  std::vector<std::string> leaf_cases;
  std::vector<std::string> branch_cases;
  std::vector<std::string> internal_cases;
  std::vector<std::string> subtree_cases;
  std::vector<std::string> tree_standard_cases;
  std::vector<std::string> network_cases;

}; // end FixtureHomozygoteParents

// Generic test function
auto test(const auto& cases, auto parser)
{
  for(const auto& input : cases)
  {
    auto iter = input.begin();
    auto iter_end = input.end();
    bool r = x3::parse(iter, iter_end, parser);
    BOOST_CHECK(r && iter == iter_end);
  }
}

BOOST_AUTO_TEST_SUITE( newick_parser )

BOOST_FIXTURE_TEST_CASE(name_grammar, FixtureCases)
{
  test(name_cases, quetzal::format::newick::parser::name);
}

BOOST_FIXTURE_TEST_CASE(length_grammar, FixtureCases)
{
  test(length_cases, quetzal::format::newick::parser::length);
}

BOOST_FIXTURE_TEST_CASE(leaf_grammar, FixtureCases)
{
  test(leaf_cases, quetzal::format::newick::parser::leaf);
}

BOOST_FIXTURE_TEST_CASE(tree_standard_grammar, FixtureCases)
{
  test(tree_standard_cases, quetzal::format::newick::parser::tree);
}

// BOOST_FIXTURE_TEST_CASE(tree_exotic_grammar, FixtureCases)
// {
//   test(tree_exotic_cases, quetzal::format::newick::parser::tree);
// }

BOOST_AUTO_TEST_SUITE_END()
