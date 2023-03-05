// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE binary_tree_test
#include <assert.h>
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/coalescence/graph/binary_tree.hpp>

BOOST_AUTO_TEST_SUITE( binary_tree )

    /* Topology :
    *             a
    *           /   \
    *          /     c
    *         /     / \
    *        b     d   e
     */

BOOST_AUTO_TEST_CASE(no_property_base_interface)
{
  // default tree with no edge/vertex properties attached
  using tree_type = quetzal::binary_tree<>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree);

  vertex_descriptor b = add_vertex(tree);
  vertex_descriptor c = add_vertex(tree);
  vertex_descriptor d = add_vertex(tree);
  vertex_descriptor e = add_vertex(tree);

  // Can not compile or would break binary tree invariant
  // add_edge(a, b, tree);
  // add_edge(a, c, tree);
  // add_edge(c, d, tree);
  // add_edge(c, e, tree);
}

BOOST_AUTO_TEST_CASE(no_property_binary_interface)
{
  // default tree with no edge/vertex properties attached
  using tree_type = quetzal::binary_tree<>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree);

  auto [ab_edge, ac_edge] = tree.add_children(a, 1, 2);
  auto [cd_edge, ce_edge] = tree.add_children(ac_edge.second, 3, 4);

}

BOOST_AUTO_TEST_CASE(binary_interface_bundled_properties_vertices)
{
  // default tree with no edge/vertex properties attached
  using vertex_properties = std::string;
  using tree_type = quetzal::binary_tree<vertex_properties>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree, "a");
  vertex_descriptor b = add_vertex(tree, "b");
  vertex_descriptor c = add_vertex(tree, "c");
  vertex_descriptor d = add_vertex(tree, "d");
  vertex_descriptor e = add_vertex(tree, "e");

  assert(tree[a] == "a");
}

BOOST_AUTO_TEST_SUITE_END()
