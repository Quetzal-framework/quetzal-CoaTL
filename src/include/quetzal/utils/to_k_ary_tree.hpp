// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef TO_TREE_H_INCLUDED
#define TO_TREE_H_INCLUDED

#include "quetzal/io/newick/ast.hpp"
#include "quetzal/io/newick/parser.hpp"
#include "quetzal/coalescence/graph/k_ary_tree.hpp"

#include <concepts>

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

  namespace format::newick
  {
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
      newick::ast::node ast;
      // We parse the input string into the AST
      auto ok = quetzal::parse(begin(newick), end(newick), newick::parser::tree, ast);
      // todo: handling exceptions in boost::spirit
      assert(ok);
      return quetzal::to_k_ary_tree<VertexProperties,EdgeProperties>(ast);
    }
  }


} // end namespace quetzal

#endif
