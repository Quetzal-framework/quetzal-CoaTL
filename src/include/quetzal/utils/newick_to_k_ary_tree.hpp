// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef BRIDGE_NEWICK_KARY_TREE_H_INCLUDED
#define BRIDGE_NEWICK_KARY_TREE_H_INCLUDED

#include "quetzal/io/newick/ast.hpp"
#include "quetzal/io/newick/parser.hpp"
#include "quetzal/io/newick/generator.hpp"
#include "quetzal/coalescence/graph/k_ary_tree.hpp"
#include <concepts>
#include <string>

namespace quetzal
{

  ///
  /// @brief Converts a standard Newick AST to a quetzal Tree graph
  ///
  template
  <
    class VertexProperties = quetzal::format::newick::ast::node::vertex_properties,
    class EdgeProperties= quetzal::format::newick::ast::node::edge_properties
  >
  std::tuple
  <
    quetzal::coalescence::k_ary_tree<VertexProperties,EdgeProperties>,
    typename quetzal::coalescence::k_ary_tree<VertexProperties,EdgeProperties>::vertex_descriptor
  >
  to_k_ary_tree(quetzal::format::newick::ast::node ast_root)
  {
    quetzal::coalescence::k_ary_tree<VertexProperties,EdgeProperties> tree;
    auto root = add_vertex( {ast_root.name}, tree);

    // A hacky recursive lambda (C++23 will make this much simpler)
    static const auto populate = [](auto& graph, auto parent, const auto& ast){
      static auto recursive = [](auto& graph, auto parent, const auto& ast, auto& self) mutable -> void {
        // the idea is simple: while traversing the ast ...
        for(const auto& ast_child : ast.children){
          // add node to the quetzal tree graph
          auto child = add_vertex( {ast_child.name}, graph);
          // add directed edge from child to parent
          add_edge(child, parent, {ast_child.distance_to_parent}, graph);
          // and propagate to the next level
          self(graph, child, ast_child, self);}};
      recursive(graph, parent, ast, recursive);
    };

    populate(tree, root, ast_root);
    return {std::move(tree), root};
  }


  ///
  /// @brief Converts a standard Newick string to a quetzal Tree graph
  ///
  template
  <
    class VertexProperties = quetzal::format::newick::ast::node::vertex_properties,
    class EdgeProperties= quetzal::format::newick::ast::node::edge_properties
  >
  std::tuple
  <
    quetzal::coalescence::k_ary_tree<VertexProperties,EdgeProperties>,
    typename quetzal::coalescence::k_ary_tree<VertexProperties,EdgeProperties>::vertex_descriptor
  >
  to_k_ary_tree(const std::string& newick)
  {
    // We initialize the root of Abstract Syntax Tree (AST)
    quetzal::format::newick::ast::node ast;
    // We parse the input string into the AST
    auto ok = quetzal::parse(begin(newick), end(newick), quetzal::format::newick::parser::tree, ast);
    // todo: handling exceptions in boost::spirit
    assert(ok);
    return quetzal::to_k_ary_tree<VertexProperties,EdgeProperties>(ast);
  }


  //
  //   namespace details
  //   {
  //     ///
  //     /// @brief Adaptator for passing a Newick generator to a Boost DFS algorithm
  //     ///
  //     template<class... Args>
  //     struct empty_newick_visitor : boost::default_dfs_visitor
  //     {
  //       empty_newick_visitor(Args&&... args):
  //         _gen(newick::make_generator(std::forward<Args>(args)...)){}
  //
  //       auto discover_vertex(auto u, auto const& g) { return ""; }
  //       auto examine_edge(auto u, auto const& g) { return ""; }
  //       auto finish_vertex(auto u, auto const& g) { return ""; }
  //     };
  //
  //     struct NewickVisitor : boost::default_dfs_visitor {
  //         auto discover_vertex(auto u, auto const& g) {
  //             return g[u];
  //         }
  //         auto discover_edge(auto u, auto const& g) {
  //             return g[u];
  //         }
  //     };
  //   }
  //
  //   ///
  //   /// @brief Generate a Newick string from a k-ary tree with no properties
  //   ///
  //   std::string generate(quetzal::coalescence::k_ary_tree<> tree)
  //   {
  //     namespace newick = quetzal::format::newick;
  //     using vertex_t = typename quetzal::coalescence::k_ary_tree<>::vertex_descriptor;
  //
  //     // Data access
  //     std::predicate<vertex_t> auto has_parent = [&tree](vertex_t v){return tree.has_parent(v);};
  //     std::predicate<vertex_t> auto has_children = [&tree](vertex_t v){return tree.has_children(v);};
  //     newick::Formattable<vertex_t> auto label = [&tree](auto){return "";};
  //     newick::Formattable<vertex_t> auto branch_length = [&tree](auto){return ""};
  //
  //     // We declare a generator passing it the data interfaces
  //     auto generator = newick::make_generator(has_parent, has_children, label, branch_length);
  //
  //     // We expose its interface to the boost DFS algorithm
  //     //depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());
  //     details::EmptyNewickVisitor v();
  //     depth_first_search(tree, boost::visitor(vis));
  //   }
  //
  //
  //   ///
  //   /// @brief Generate a Newick string from a k-ary tree with no properties
  //   ///
  //   template<class VertexProperties, class EdgeProperties>
  //   std::string generate(quetzal::coalescence::k_ary_tree<VertexProperties, EdgeProperties> tree)
  //   {
  //     namespace newick = quetzal::format::newick;
  //     using vertex_t = typename quetzal::coalescence::k_ary_tree<>::vertex_descriptor;
  //     std::predicate<vertex_t> auto has_parent = [&tree](vertex_t v){return tree.has_parent(v);};
  //     std::predicate<vertex_t> auto has_children = [&tree](vertex_t v){return tree.has_children(v);};
  //
  //     // We are not interested into formatting label or branch_length information
  //     newick::Formattable<vertex_t> auto label = [&tree](vertex_t v ){return tree[v];};
  //
  //     newick::Formattable<vertex_t> auto branch_length = [&tree](vertex_t v)
  //     {
  //       std::string result;
  //       auto [it1, it2] = out_edges(v, tree);
  //       assert(std::distance(it1,it2) <= 1);
  //       if(std::distance(it1,it2) == 1) result = std::to_string(tree[*it1]);
  //       return result;
  //     };
  //
  //     // We declare a generator passing it the interfaces
  //     auto generator = newick::make_generator(has_parent, has_children, label, branch_length);
  //
  //     // We expose its interface to the user-defined class DFS algorithm
  //     a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());
  //
  //   }
  // } // end format::newick


} // end namespace quetzal

#endif
