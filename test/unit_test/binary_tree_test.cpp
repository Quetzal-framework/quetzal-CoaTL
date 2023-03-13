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
  using tree_type = quetzal::binary_tree<boost::no_property, boost::no_property>;
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
  using tree_type = quetzal::binary_tree<boost::no_property, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree);

  auto [ab_edge, ac_edge] = add_children(tree, a, 1, 2);
  auto [cd_edge, ce_edge] = add_children(tree, ac_edge.second, 3, 4);

}

BOOST_AUTO_TEST_CASE(simple_vertex_properties)
{
  // default tree with no edge/vertex properties attached
  using vertex_properties = std::string;
  using tree_type = quetzal::binary_tree<vertex_properties, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  std::cout << "yoyo" << std::endl;

  vertex_descriptor a = add_vertex(tree, "a");
  std::cout << "yo" << std::endl;

  vertex_descriptor b = add_vertex(tree, "b");
  vertex_descriptor c = add_vertex(tree, "c");
  vertex_descriptor d = add_vertex(tree, "d");
  vertex_descriptor e = add_vertex(tree, "e");

  std::cout << "yo" << std::endl;
  assert(tree[a] == "a");
}

struct vertex_info { 
    std::string whatever; 
    int othervalue;
};

BOOST_AUTO_TEST_CASE(structure_vertex_properties)
{
  // default tree with no edge/vertex properties attached
  using vertex_properties = vertex_info;
  using tree_type = quetzal::binary_tree<vertex_properties, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree, "a", 0);
  vertex_descriptor b = add_vertex(tree, "b", 1);
  vertex_descriptor c = add_vertex(tree, "c", 2);
  vertex_descriptor d = add_vertex(tree, "d", 3);
  vertex_descriptor e = add_vertex(tree, "e", 4);

  assert(tree[e].whatever == "e");
  assert(tree[e].otherwise == 4);
}

BOOST_AUTO_TEST_CASE(simple_edge_properties)
{
  // default tree with no edge/vertex properties attached
  using vertex_properties = boost::no_property;
  using edge_properties = std::string;
  using tree_type = quetzal::binary_tree<vertex_properties, edge_properties>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree);
  vertex_descriptor b = add_vertex(tree);
  vertex_descriptor c = add_vertex(tree);
  vertex_descriptor d = add_vertex(tree);
  vertex_descriptor e = add_vertex(tree);

  auto [ab_edge, ac_edge] = add_children(tree, a, std::make_tuple(b, "b"), std::make_tuple(c, "c"));
  auto [cd_edge, ce_edge] = add_children(tree, ac_edge.second, std::make_tuple(d, "d"), std::make_tuple(e, "e"));

}

BOOST_AUTO_TEST_SUITE_END()
