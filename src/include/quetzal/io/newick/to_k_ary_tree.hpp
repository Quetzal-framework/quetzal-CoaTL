// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/////////////////////////////////////////////////////////////////////////// *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "quetzal/coalescence/graph/k_ary_tree.hpp"
#include "quetzal/io/newick/ast.hpp"
#include "quetzal/io/newick/generator.hpp"
#include "quetzal/io/newick/parser.hpp"

#include <concepts>
#include <ranges>
#include <string>
#include <tuple>
#include <vector>

namespace quetzal::format::newick
{

///
/// @brief Converts a standard Newick AST to a quetzal Tree graph
///
template <class VertexProperty = quetzal::format::newick::ast::node::vertex_property,
          class EdgeProperty = quetzal::format::newick::ast::node::edge_property>
std::tuple<quetzal::coalescence::k_ary_tree<VertexProperty, EdgeProperty>,
           typename quetzal::coalescence::k_ary_tree<VertexProperty, EdgeProperty>::vertex_descriptor>
to_k_ary_tree(quetzal::format::newick::ast::node ast_root)
{
    using tree_type = quetzal::coalescence::k_ary_tree<VertexProperty, EdgeProperty>;
    using vertex_descriptor = typename tree_type::vertex_descriptor;
    tree_type tree;
    auto root = tree.add_vertex(VertexProperty{ast_root.name});

    // A hacky recursive lambda (C++23 will make this much simpler)
    static const auto populate = [](tree_type &graph, vertex_descriptor parent, const auto &ast) {
        static auto recursive = [](tree_type &graph, vertex_descriptor parent, const auto &ast,
                                   auto &self) mutable -> void {
            if (ast.children.size() > 0)
            {
                assert(ast.children.size() != 0);
                assert(ast.children.size() > 1);

                std::vector<std::tuple<vertex_descriptor, EdgeProperty>> children;
                children.reserve(ast.children.size());

                for (const auto &ast_child : ast.children)
                {
                    children.push_back(std::make_tuple(graph.add_vertex(VertexProperty{ast_child.name}),
                                                       EdgeProperty{ast_child.distance_to_parent}));
                }

                graph.add_edges(parent, children);

                for (int i = 0; i < children.size(); i++)
                {
                    self(graph, std::get<0>(children[i]), ast.children[i], self);
                }
            }
        }; // end recursive
        recursive(graph, parent, ast, recursive);
    }; // end populate

    populate(tree, root, ast_root);
    return {std::move(tree), root};
}

///
/// @brief Converts a standard Newick string to a quetzal Tree graph
///
template <class VertexProperty = quetzal::format::newick::ast::node::vertex_property,
          class EdgeProperty = quetzal::format::newick::ast::node::edge_property>
std::tuple<quetzal::coalescence::k_ary_tree<VertexProperty, EdgeProperty>,
           typename quetzal::coalescence::k_ary_tree<VertexProperty, EdgeProperty>::vertex_descriptor>
to_k_ary_tree(const std::string &newick)
{
    // We initialize the root of Abstract Syntax Tree (AST)
    quetzal::format::newick::ast::node ast;
    // We parse the input string into the AST
    auto ok = quetzal::parse(begin(newick), end(newick), quetzal::format::newick::parser::tree, ast);
    // todo: handling exceptions in boost::spirit
    assert(ok);
    return to_k_ary_tree<VertexProperty, EdgeProperty>(ast);
}

} // end namespace quetzal::format::newick
