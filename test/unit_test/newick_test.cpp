// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE coalescence_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/io.h>
#include <random>

// Simplistic tree for testing
struct Node
{
  Node *parent;
  Node *left;
  Node *right;
  int data;
} ;

BOOST_AUTO_TEST_SUITE( format )

BOOST_AUTO_TEST_CASE(newick_filters)
{
  namespace fmt = quetzal::format;

  std::string s1 = "(A:5,B:5)r;";
  std::string s2 = "(A:5,B[Comment]:5)r;";
  std::string s3 = "(A:5,B[Nested[Comment]]:5)r;";

  // Testing policies
  BOOST_CHECK(fmt::is_balanced<fmt::parenthesis>::check(s2));
  BOOST_CHECK(fmt::is_balanced<fmt::square_bracket>::check(s3));

  BOOST_CHECK(fmt::is_balanced<fmt::square_bracket>::check("(A:5,B[Comment:5r;") == false );
  BOOST_CHECK(fmt::is_balanced<fmt::parenthesis>::check("(A:5,B[Comment:5))r;") == false );

  BOOST_CHECK_EQUAL(fmt::newick::remove_comments_of_depth<0>::edit(s2) , s2);
  BOOST_CHECK_EQUAL(fmt::newick::remove_comments_of_depth<1>::edit(s3) , s1);
  BOOST_CHECK_EQUAL(fmt::newick::remove_comments_of_depth<2>::edit(s3) , s2);
}

BOOST_AUTO_TEST_CASE(newick_formatting)
{
  namespace newick = quetzal::format::newick;

  std::random_device rd;  // Used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(0.0, 2.0); // Arbitrary banch length distribution

  int id = 0; // node id

  Node root;
  root.data = id;

  // Interface Quetzal formatter with non-quetzal tree types
  constexpr std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};
  constexpr std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};

  newick::Formattable<Node> auto label = [&id](const Node& n ){return std::to_string(++id) + "[my[comment]]";};
  
  newick::Formattable<Node> auto branch_length = [&gen,&dis](const Node& n){return std::to_string(dis(gen));};

  auto formatter = newick::make_formatter<Node>(has_children, has_parent, label, branch_length);

  // Enables the use of nested comments
  auto s1 = formatter.format<newick::PAUP>(root);

  // Writes a root node branch length (with a value of 0.0).
  auto s2 = formatter.format<newick::TreeAlign>(root);

  // Requires that an unrooted tree begin with a trifurcation
  auto s3 = formatter.format<newick::PHYLIP>(root);
}

BOOST_AUTO_TEST_SUITE_END()
