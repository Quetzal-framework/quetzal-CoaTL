// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef NEWICK_FROM_KARY_TREE_H_INCLUDED
#define NEWICK_FROM_KARY_TREE_H_INCLUDED

#include "quetzal/io/newick/ast.hpp"
#include "quetzal/io/newick/parser.hpp"
#include "quetzal/io/newick/generator.hpp"
#include "quetzal/coalescence/graph/k_ary_tree.hpp"
#include "quetzal/coalescence/graph/binary_tree.hpp"

#include <concepts>
#include <string>

namespace quetzal::format::newick
{

  //
  // @brief Generate a Newick string from a k-ary tree with no properties attached to edges or vertices
  //
  std::string generate_from(quetzal::coalescence::k_ary_tree<boost::no_property, boost::no_property> const& graph, auto flavor) {
      using Graph      = quetzal::coalescence::k_ary_tree<>;
      using Node       = Graph::vertex_descriptor;
      namespace newick = quetzal::format::newick;

      // Data access
      std::predicate<Node> auto      has_parent    = [&graph](Node const& v) { return graph.has_parent(v); };
      std::predicate<Node> auto      has_children  = [&graph](Node v) { return graph.has_children(v); };
      newick::Formattable<Node> auto branch_length = [](Node) { return ""; };
      newick::Formattable<Node> auto label         = [](Node v) { return ""; };

      // We declare a generator passing it the data interfaces
      newick::generator gen{has_parent, has_children, label, branch_length, flavor};
      using Gen = decltype(gen);
      using Stack = std::stack<int>;
      Stack nth_child;

      // We expose its interface to the boost DFS algorithm
      struct VisWrap : boost::default_dfs_visitor {
          Gen& gen_;
          Stack& stack_;
          VisWrap(Gen& gen, Stack& s) : gen_(gen), stack_(s) {}
          void discover_vertex(Node v, Graph const&) const {
              stack_.push(0);
              gen_.pre_order()(v);
          }
          void finish_vertex(Node v, Graph const&) const {
              gen_.post_order()(v);
              stack_.pop();
          }
          void tree_edge(Graph::edge_descriptor e, Graph const& g) const {
              if (stack_.top()++ > 0)
                  gen_.in_order()();
          }
      } vis{gen, nth_child};

      depth_first_search(graph, boost::visitor(vis));
      return gen.take_result();
  }

  //
  // @brief Generate a Newick string from a k-ary tree with no properties attached to edges or vertices
  //
  std::string generate_from(quetzal::coalescence::binary_tree<boost::no_property, boost::no_property> const& graph, auto flavor) {
      using Graph      = quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>;
      using Node       = Graph::vertex_descriptor;
      namespace newick = quetzal::format::newick;

      // Data access
      std::predicate<Node> auto      has_parent    = [&graph](Node const& v) { return has_predecessor(v,graph); };
      std::predicate<Node> auto      has_children  = [&graph](Node v) { return static_cast<bool>(out_degree(v, graph)); };
      newick::Formattable<Node> auto branch_length = [](Node) { return ""; };
      newick::Formattable<Node> auto label         = [](Node v) { return ""; };

      // We declare a generator passing it the data interfaces
      newick::generator gen{has_parent, has_children, label, branch_length, flavor};
      using Gen = decltype(gen);
      using Stack = std::stack<int>;
      Stack nth_child;

      // We expose its interface to the boost DFS algorithm
      struct VisWrap : boost::default_dfs_visitor {
          Gen& gen_;
          Stack& stack_;
          VisWrap(Gen& gen, Stack& s) : gen_(gen), stack_(s) {}
          void discover_vertex(Node v, Graph const&) const {
              stack_.push(0);
              gen_.pre_order()(v);
          }
          void finish_vertex(Node v, Graph const&) const {
              gen_.post_order()(v);
              stack_.pop();
          }
          void tree_edge(Graph::edge_descriptor e, Graph const& g) const {
              if (stack_.top()++ > 0)
                  gen_.in_order()();
          }
      } vis{gen, nth_child};

      depth_first_search(graph, boost::visitor(vis));
      return gen.take_result();
  }

  //
  // @brief Generate a Newick string from a k-ary tree with no properties attached to edges or vertices
  //
  std::string generate_from(boost::bidirectional_binary_tree<> const& graph, auto flavor) {
      using Graph      = boost::bidirectional_binary_tree<>;
      using Node       = Graph::vertex_descriptor;
      namespace newick = quetzal::format::newick;

      // Data access
      std::predicate<Node> auto      has_parent    = [&graph](Node const& v) { return has_predecessor(v,graph); };
      std::predicate<Node> auto      has_children  = [&graph](Node v) { return static_cast<bool>(out_degree(v, graph)); };
      newick::Formattable<Node> auto branch_length = [](Node) { return ""; };
      newick::Formattable<Node> auto label         = [](Node v) { return ""; };

      // We declare a generator passing it the data interfaces
      newick::generator gen{has_parent, has_children, label, branch_length, flavor};
      using Gen = decltype(gen);
      using Stack = std::stack<int>;
      Stack nth_child;

      // We expose its interface to the boost DFS algorithm
      struct VisWrap : boost::default_dfs_visitor {
          Gen& gen_;
          Stack& stack_;
          VisWrap(Gen& gen, Stack& s) : gen_(gen), stack_(s) {}
          void discover_vertex(Node v, Graph const&) const {
              stack_.push(0);
              gen_.pre_order()(v);
          }
          void finish_vertex(Node v, Graph const&) const {
              gen_.post_order()(v);
              stack_.pop();
          }
          void tree_edge(Graph::edge_descriptor e, Graph const& g) const {
              if (stack_.top()++ > 0)
                  gen_.in_order()();
          }
      } vis{gen, nth_child};

      depth_first_search(graph, boost::visitor(vis));
      return gen.take_result();
  }

  // ///
  // /// @brief Generate a Newick string from a k-ary tree with properties
  // ///
  // template<class VertexProperties, class EdgeProperties>
  // std::string generate_from(quetzal::coalescence::k_ary_tree<VertexProperties, EdgeProperties> graph)
  // {
  //   namespace newick = quetzal::format::newick;

  //   using vertex_t = typename quetzal::coalescence::k_ary_tree<>::vertex_descriptor;

  //   std::predicate<vertex_t>      auto has_parent    = [&graph](vertex_t v){return graph.has_parent(v);};
  //   std::predicate<vertex_t>      auto has_children  = [&graph](vertex_t v){return graph.has_children(v);};
  //   newick::Formattable<vertex_t> auto label         = [&graph](vertex_t v ){return graph[v];};
  //   newick::Formattable<vertex_t> auto branch_length = [&graph](vertex_t v)
  //   {
  //     std::string result;
  //     auto [it1, it2] = out_edges(v, graph);
  //     assert(std::distance(it1,it2) <= 1);
  //     if(std::distance(it1,it2) == 1) result = std::to_string(graph[*it1]);
  //     return result;
  //   };

  //   // We declare a generator passing it the interfaces
  //   auto generator = newick::make_generator(has_parent, has_children, label, branch_length);

  //   // We expose its interface to the user-defined class DFS algorithm
  //   graph.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

  // }

} // end namespace quetzal::format::newick

#endif
