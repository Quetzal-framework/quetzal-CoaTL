// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE binary_tree_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/coalescence/graph/k_ary_tree.hpp>
#include <random>

BOOST_AUTO_TEST_SUITE( k_ary_tree )

    /* Topology :
    *             a
    *           /   \
    *          /     c
    *         /    / | \
    *        b    d  e  f
     */

BOOST_AUTO_TEST_CASE(no_property)
{

  // tree with named verticesand no edge properties attached  
  using tree_type = quetzal::coalescence::k_ary_tree<boost::no_property, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;

  vertex_descriptor a = tree.add_vertex();
  vertex_descriptor b = tree.add_vertex();
  vertex_descriptor c = tree.add_vertex();
  vertex_descriptor d = tree.add_vertex();
  vertex_descriptor e = tree.add_vertex();
  vertex_descriptor f = tree.add_vertex();

  auto edges1 = tree.add_edges(a, {b, c} );
  auto edges2 = tree.add_edges(c, {d, e, f} );

  auto root = tree.find_root_from(f);
  BOOST_CHECK(root == a);
  BOOST_CHECK(tree.degree(c) == 4);
  BOOST_CHECK(tree.in_degree(c) == 1);
  BOOST_CHECK(tree.out_degree(c) == 2);
  BOOST_CHECK(tree.has_predecessor(root) == false);

  std::mt19937 rng{std::random_device{}()};
  auto [tree2, root2] = tree_type::make_random_tree(5, 10, rng);
  BOOST_CHECK(tree.is_isomorphic(tree2) == false);


}

BOOST_AUTO_TEST_SUITE_END()
