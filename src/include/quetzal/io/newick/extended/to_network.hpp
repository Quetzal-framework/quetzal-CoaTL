// Copyright 2020 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#pragma once

#include "quetzal/io/newick/ast.hpp"
#include "quetzal/io/newick/parser.hpp"
#include "quetzal/io/newick/generator.hpp"
#include "quetzal/coalescence/graph/network.hpp"

namespace quetzal::format::newick::extended
{
  namespace detail
  {
    template<class Graph>
    void regraft_cycles(Graph& graph, auto root)
    {
      using graph_type = Graph;
      using vertex_descriptor = typename graph_type::vertex_descriptor;
      using vertex_property = typename Graph::vertex_property;
      using dict_type = std::map<std::string, std::map<std::string, std::vector<vertex_descriptor>>>;

      dict_type records;
      std::regex rgx( 
          "[a-zA-Z0-9_]*"       // Label: only upper and lowercase letters, numbers, underscores, or empty
          "#"                   // Cycle detected
          "([H|LGT|R])"         // Hybrid, Lateral Gene Transfer or Recombination
          "(0|[1-9][0-9]*)"     // Any positive integer, including 0
        );

      // iterate through vertices/labels
      for (auto vertex : graph.vertices() )
      {
        std::smatch match;
        std::string label;
        if constexpr (std::is_same_v<vertex_property, std::string>)
          label = graph[vertex];
        else
          label = graph[vertex].label();

        // find if cycle detected
        if(std::regex_search(label, match, rgx))
        {
          // save vertices id to their rightful groups
          const auto event_type = match.str(1);
          const auto event_id = match.str(2);
          records[event_type][event_id].push_back(vertex);
        }
      }

      using namespace ranges;

      // iterate through results
      for( auto const& [event_type, val] : records )
      {
        for(auto const& [event_id, duplicated_vertices] : val)
        {
          auto new_vertex = graph.add_vertex("#" + event_type + event_id);

          auto new_sources = duplicated_vertices 
            | views::transform([&graph](auto v){return graph.in_edges(v);})
            | views::take(1)
            | views::transform([&graph](auto&& range_of_e){return graph.source(*range_of_e.begin());}); 

          auto new_targets = duplicated_vertices 
            | views::transform([&graph](auto v){return graph.out_edges(v);})
            | views::join
            | views::transform([&graph](auto e){return graph.target(e);});

          ranges::for_each(duplicated_vertices, [&graph](auto v){graph.clear_vertex(v); graph.remove_vertex(v);});

          auto regraft = [&graph, new_vertex](auto s, auto t){
            graph.add_edge(s, new_vertex);
            graph.add_edge(new_vertex, t);
          };

          auto todo = ranges::views::zip(new_sources, new_targets);
          ranges::for_each(todo, [regraft](auto&& z){ std::apply(regraft, z);});
        }
      }
    }
  } // end namespace detail

  /// @brief Converts an Extended Newick AST to a quetzal network graph
  template
  <
    class VertexProperty = quetzal::format::newick::ast::node::vertex_property,
    class EdgeProperty= quetzal::format::newick::ast::node::edge_property
  >
  std::tuple
  <
    quetzal::coalescence::network<VertexProperty,EdgeProperty>,
    typename quetzal::coalescence::network<VertexProperty,EdgeProperty>::vertex_descriptor
  >
  to_network(quetzal::format::newick::ast::node ast_root)
  {
    using graph_type = quetzal::coalescence::network<VertexProperty,EdgeProperty>;
    using vertex_descriptor = typename graph_type::vertex_descriptor;

    graph_type graph;
    auto root = graph.add_vertex( VertexProperty(ast_root.name) );

    // A hacky recursive lambda (C++23 will make this much simpler)
    static const auto populate = [](graph_type& graph, vertex_descriptor parent, const auto& ast)
    {
      static auto recursive = [](graph_type& graph, vertex_descriptor parent, const auto& ast, auto& self) mutable -> void 
      {
        std::vector<std::tuple< vertex_descriptor, EdgeProperty>> children;

        children.reserve(ast.children.size());

        for(const auto& ast_child : ast.children){
          children.push_back(std::make_tuple( graph.add_vertex( VertexProperty(ast_child.name) ), EdgeProperty(ast_child.distance_to_parent) ));
        }

        graph.add_edges(parent, children);

        for ( int i = 0; i < children.size(); i++){
          self(graph, std::get<0>(children[i]), ast.children[i], self);
        }
        
      }; // end recursive
      recursive(graph, parent, ast, recursive);
    }; // end populate

    populate(graph, root, ast_root);
    detail::regraft_cycles(graph, root);
    return {std::move(graph), root};
  }

  ///
  /// @brief Converts a standard Newick string to a quetzal network graph
  ///
  template
  <
    class VertexProperty = quetzal::format::newick::ast::node::vertex_property,
    class EdgeProperty= quetzal::format::newick::ast::node::edge_property
  >
  std::tuple
  <
    quetzal::coalescence::network<VertexProperty,EdgeProperty>,
    typename quetzal::coalescence::network<VertexProperty,EdgeProperty>::vertex_descriptor
  >
  to_network(const std::string& extended_newick)
  {
    // We initialize the root of Abstract Syntax Tree (AST)
    quetzal::format::newick::ast::node ast;
    // We parse the input string into the AST
    auto ok = quetzal::parse(begin(extended_newick), end(extended_newick), quetzal::format::newick::parser::tree, ast);
    // todo: handling exceptions in boost::spirit
    assert(ok);
    return to_network<VertexProperty, EdgeProperty>(ast);
  }

}