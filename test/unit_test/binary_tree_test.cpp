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
#include <string>
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/coalescence/graph/binary_tree.hpp>
#include <random>

template <typename Order, typename Vertex>
struct tree_visitor
{
  void operator()(Order stage, Vertex v)
  {
    switch(stage) {
      case boost::visit::pre :
        std::cout << "Pre " << v << std::endl;
        break;
      case boost::visit::in:
        std::cout << "In " << v << std::endl;
        break;
      case boost::visit::post:
        std::cout << "Post " << v << std::endl;
        break;
      default:
        std::cout << "Oops" << std::endl;
    }
  }
};


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
  using tree_type = boost::bidirectional_binary_tree<>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree);
  vertex_descriptor b = add_vertex(tree);
  vertex_descriptor c = add_vertex(tree);
  vertex_descriptor d = add_vertex(tree);
  vertex_descriptor e = add_vertex(tree);

  add_edge(a, b, tree);
  add_edge(a, c, tree);
  add_edge(c, d, tree);
  add_edge(c, e, tree);

  tree_visitor<boost::visit, vertex_descriptor> visitor;
  depth_first_search(tree, a, visitor);
}

BOOST_AUTO_TEST_CASE(no_property_binary_interface)
{
  // default tree with no edge/vertex properties attached
  using tree_type = quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree);
  vertex_descriptor b = add_vertex(tree);
  vertex_descriptor c = add_vertex(tree);
  vertex_descriptor d = add_vertex(tree);
  vertex_descriptor e = add_vertex(tree);

  auto [ab_edge, ac_edge] = add_edges(tree, a, b, c);
  auto [cd_edge, ce_edge] = add_edges(tree, c, d, e);

  using vertex_descriptor = boost::graph_traits<quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>>::vertex_descriptor;
  tree_visitor<boost::visit, vertex_descriptor> visitor;
  depth_first_search(tree, a, visitor);
}

BOOST_AUTO_TEST_CASE(simple_vertex_properties)
{
  // tree with string vertices but no edge properties
  using vertex_properties = std::string;
  using tree_type = quetzal::coalescence::binary_tree<vertex_properties, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;

  vertex_descriptor a = add_vertex(tree, "a");
  vertex_descriptor b = add_vertex(tree, "b");
  vertex_descriptor c = add_vertex(tree, "c");
  vertex_descriptor d = add_vertex(tree, "d");
  vertex_descriptor e = add_vertex(tree, "e");

  // Read
  BOOST_CHECK(tree[a] == "a");

  // Write
  tree[a] = tree[b];
  BOOST_CHECK(tree[a] == "b"); 

}

struct vertex_info 
{ 
    std::string whatever; 
    int othervalue;
};

BOOST_AUTO_TEST_CASE(structure_vertex_properties)
{
  // tree with string & int vertices but no edge properties
  using vertex_properties = vertex_info;
  using tree_type = quetzal::coalescence::binary_tree<vertex_properties, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree, "a", 0);
  vertex_descriptor b = add_vertex(tree, "b", 1);
  vertex_descriptor c = add_vertex(tree, "c", 2);
  vertex_descriptor d = add_vertex(tree, "d", 3);
  vertex_descriptor e = add_vertex(tree, "e", 4);

  // Read
  BOOST_CHECK(tree[e].whatever == "e");
  BOOST_CHECK(tree[e].othervalue == 4);

  // Write 
  tree[e] = { "new", 5};

  BOOST_TEST(tree[e].whatever == "new");
  BOOST_CHECK(tree[e].othervalue == 5);

}

BOOST_AUTO_TEST_CASE(simple_edge_properties)
{
  // tree with string edges but no vertex properties attached
  using vertex_properties = boost::no_property;
  using edge_properties = std::string;
  using tree_type = quetzal::coalescence::binary_tree<vertex_properties, edge_properties>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree);
  vertex_descriptor b = add_vertex(tree);
  vertex_descriptor c = add_vertex(tree);
  vertex_descriptor d = add_vertex(tree);
  vertex_descriptor e = add_vertex(tree);

  // Add two edges with two different edge properties
  auto [ab_edge, ac_edge] = add_edges(tree, a, std::make_tuple(b, "a->b"), std::make_tuple(c, "a->c"));
  auto [cd_edge, ce_edge] = add_edges(tree, ac_edge.second, std::make_tuple(d, "c->d"), std::make_tuple(e, "c->e"));

  tree[ab_edge] = "a...b";
  BOOST_CHECK(tree[ab_edge] == "a...b");
}

struct edge_info { 
    std::string whatever; 
    int othervalue;
};

BOOST_AUTO_TEST_CASE(struct_edge_properties)
{
  // default tree with no edge/vertex properties attached
  using vertex_properties = boost::no_property;
  using edge_properties = edge_info;
  using tree_type = quetzal::coalescence::binary_tree<vertex_properties, edge_properties>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  // No property on vertices
  vertex_descriptor a = add_vertex(tree);
  vertex_descriptor b = add_vertex(tree);
  vertex_descriptor c = add_vertex(tree);
  vertex_descriptor d = add_vertex(tree);
  vertex_descriptor e = add_vertex(tree);

  // Pass info to build new edges
  auto [ab_edge, ac_edge] = add_edges(tree, a, std::tuple(b, "a->b", 10), std::tuple(c, "a->c", 11));
  auto [cd_edge, ce_edge] = add_edges(tree, ac_edge.second, std::tuple(d, "c->d", 12), std::tuple(e, "c->e", 13));

  // Read edge info
  BOOST_CHECK(tree[ab_edge].whatever == "a->b");
  BOOST_CHECK(tree[ab_edge].othervalue == 10);

  // Write edge info
  tree[ab_edge] =  { "yolo", 99 };
  BOOST_CHECK(tree[ab_edge].whatever == "yolo");
  BOOST_CHECK(tree[ab_edge].othervalue == 99);


}

BOOST_AUTO_TEST_CASE(struct_both_properties)
{
  // default tree with no edge/vertex properties attached
  using vertex_properties = vertex_info;
  using edge_properties = edge_info;
  using tree_type = quetzal::coalescence::binary_tree<vertex_properties, edge_properties>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = add_vertex(tree, "a", 0);
  vertex_descriptor b = add_vertex(tree, "b", 1);
  vertex_descriptor c = add_vertex(tree, "c", 2);
  vertex_descriptor d = add_vertex(tree, "d", 3);
  vertex_descriptor e = add_vertex(tree, "e", 4);

  // Pass info to build new edges
  auto [ab_edge, ac_edge] = add_edges(tree, a, std::make_tuple(b, "a->b", 10), std::make_tuple(c, "a->c", 11));
  auto [cd_edge, ce_edge] = add_edges(tree, ac_edge.second, std::make_tuple(d, "c->d", 12), std::make_tuple(e, "c->e", 13));

  // Read vertices
  assert(tree[e].whatever == "e");
  assert(tree[e].othervalue == 4);

  // Write vertices
  tree[e] = { "new", 5};
  assert(tree[e].whatever == "new");
  assert(tree[e].othervalue == 5);

  // Read edge info
  assert(tree[ab_edge].whatever == "a->b");
  assert(tree[ab_edge].othervalue == 10);

  // Write edge info
  tree[ab_edge] =  { "yolo", 99 };
  assert(tree[ab_edge].whatever == "yolo");
  assert(tree[ab_edge].othervalue == 99);

}


BOOST_AUTO_TEST_CASE(random_binary_tree)
{
  std::random_device rd;
  
  std::mt19937 gen(rd());

  int nb_leaves = 5;

  auto [tree1, root1] = quetzal::coalescence::get_random_binary_tree(nb_leaves, gen);
  auto [tree2, root2] = quetzal::coalescence::get_random_binary_tree(nb_leaves, gen);

  assert( !isomorphism(tree1,tree2) );

  using vertex_descriptor = boost::graph_traits<quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>>::vertex_descriptor;
  tree_visitor<boost::visit, vertex_descriptor> visitor;
  depth_first_search(tree1, root1, visitor);

}
BOOST_AUTO_TEST_SUITE_END()
