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
  // default tree with no edge/vertex properties attached  
  using tree_type = quetzal::coalescence::k_ary_tree<boost::no_property,boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  vertex_descriptor parent = 0;
  
  for (vertex_descriptor child = 1; child < 5; child++)
  {
    if (child % 2 == 1)
      add_edge(parent, child, tree);
    else
      add_edge(parent++, child, tree);
  } 
}

BOOST_AUTO_TEST_CASE(name_property)
{
  // tree with named verticesand no edge properties attached  
  using tree_type = quetzal::coalescence::k_ary_tree<std::string,boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;

  vertex_descriptor a = add_vertex("a", tree);
  vertex_descriptor b = add_vertex("b", tree);
  vertex_descriptor c = add_vertex("c", tree);
  vertex_descriptor d = add_vertex("d", tree);
  vertex_descriptor e = add_vertex("e", tree);
  vertex_descriptor f = add_vertex("f", tree);

  add_edge(a, b, tree);
  add_edge(a, c, tree);
  add_edge(c, d, tree);
  add_edge(c, e, tree);
  add_edge(c, f, tree);
}
BOOST_AUTO_TEST_SUITE_END()
