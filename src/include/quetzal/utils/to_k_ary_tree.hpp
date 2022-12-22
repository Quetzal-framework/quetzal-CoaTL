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
#include "quetzal/coalescence/graph/k_ary_tree.hpp"

#include <concepts>

namespace quetzal::utils
{
  namespace
  {
    using ast_type = quetzal::format::newick::ast::node;
    using vertex_properties = ast_type::vertex_properties;
    using edge_properties = ast_type::edge_properties;
    using synthetized_tree_type = quetzal::coalescence::k_ary_tree<vertex_properties, edge_properties>;
  }

  ///
  /// @brief Converts a standard Newick AST to a quetzal Tree graph
  ///
  template<class VertexProperties = vertex_properties, class EdgeProperties= edge_properties>
      requires std::constructible_from<VertexProperties, vertex_properties> &&
               std::constructible_from<EdgeProperties, edge_properties>
  auto to_k_ary_tree(quetzal::format::newick::ast::node ast_root)
  {
    synthetized_tree_type tree;
    auto root = add_vertex(ast_root.name, tree);

    // A hacky recursive lambda (C++23 will make this much simpler)
    static const auto populate = [](auto& graph, auto parent, const auto& ast){
      static auto recursive = [](auto& graph, auto parent, const auto& ast, auto& self) mutable -> void {
        // the idea is simple: while traversing the ast ...
        for(const auto& ast_child : ast.children){
          // add node to the quetzal tree graph
          auto child = add_vertex(ast_child.name, graph);
          // add directed edge from child to parent
          add_edge(child, parent, ast_child.distance_to_parent, graph);
          // and propagate to the next level
          self(graph, child, ast_child, self);}};
      recursive(graph, parent, ast, recursive);
    };

    populate(tree, root, ast_root);
    return tree;
  }

} // end namespace quetzal

#endif
