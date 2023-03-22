// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef RANDOM_TREE_H_INCLUDED
#define RANDOM_TREE_H_INCLUDED

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/random_spanning_tree.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <iomanip>
#include <random>

#include <quetzal/coalescence/graph/k_ary_tree.hpp>
#include <quetzal/coalescence/graph/binary_tree.hpp>

namespace quetzal
{

  namespace detail
  {
    // Default, no type
    template <typename T> struct make_undirected { using type = void; };

    // For boost adjacency list
    template <typename A, typename B, typename C, typename D, typename E, typename F>
    struct make_undirected<boost::adjacency_list<A, B, C, D, E, F>>
    {
        using type = boost::adjacency_list<A, B, boost::undirectedS, D, E, F>;
    };

    // For quetzal trees
    template <typename V, typename E>
    struct make_undirected<quetzal::coalescence::k_ary_tree<V, E>>
    {
        using type = boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, V, E>;
    };

    template <typename T> using Undirect = typename make_undirected<T>::type;
  } // namespace detail

  ///
  /// @brief Generates a random directed tree
  ///
  template<class Graph=quetzal::coalescence::k_ary_tree<>, class Generator>
  auto get_random_spanning_tree(int n_vertices, int n_edges, Generator& rng)
  {
    using UG = detail::Undirect<Graph>;
    using vertex_t = typename UG::vertex_descriptor;

    // assuming integral vertex index for simplicity
    static_assert(std::is_same_v<vertex_t, size_t>);
    static_assert(std::is_same_v<typename UG::vertex_descriptor,
                                 typename Graph::vertex_descriptor>);

    // Initialize a random undirected graph
    UG ug;
    generate_random_graph(ug, n_vertices, n_edges, rng);
    vertex_t const root = random_vertex(ug, rng);

    // make the graph fully connected
    {
      std::map<vertex_t, int> components;
      auto from = [&](int component) { // just picking the first...
        for (auto& [v, c] : components) if (c == component) return v;
        throw std::range_error("component");
        };

      // records the component number in the component property map
      auto cmap = boost::make_assoc_property_map(components);
      // The algorithm computes how many connected components are in the graph
      if (int n = connected_components(ug, cmap); n > 1)
      {
        for (int c = 1; c < n; ++c)
          add_edge(from(c - 1), from(c), ug);
      }
    }

    std::map<vertex_t, vertex_t> predecessors;
    random_spanning_tree(ug, rng,
        boost::root_vertex(root) //
            .predecessor_map(boost::make_assoc_property_map(predecessors)));

    Graph tree(num_vertices(ug)); // build a tree copy
    for (auto v : boost::make_iterator_range(vertices(ug)))
        if (predecessors.contains(v))
            if (auto pred = predecessors.at(v); ug.null_vertex() != pred)
                add_edge(predecessors.at(v), v, tree);

    return std::tuple(std::move(tree), root);
  }

  namespace detail
  {
    template<class T, class G, class Generator>
    auto update_tree(G& tree, std::vector<T> leaves, Generator& rng)
    {
      // this is a leaf
      if(leaves.size() == 1 ){
        return leaves.front();
      } else {
        std::uniform_int_distribution<> distrib(1, leaves.size() -1 );
        int split = distrib(rng);

        std::vector<T> left(leaves.begin(), leaves.begin() + split);
        std::vector<T> right(leaves.begin() + split, leaves.end());

        auto parent = add_vertex(tree);

        // recursion
        add_children(tree, parent, update_tree(tree, left, rng), update_tree(tree, right, rng));
        return parent;
      }
    }
  }

  template<typename Generator>
  std::tuple<
    quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>, 
    quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>::vertex_descriptor>
  get_random_binary_tree(int n_leaves, Generator& rng)
  {
    using tree_type = quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>;
    using vertex_descriptor = tree_type::vertex_descriptor;

    tree_type tree;
    std::vector<vertex_descriptor> v(n_leaves);
    std::transform(v.cbegin(), v.cend(), v.begin(), [&tree](auto) { return add_vertex(tree); });

    vertex_descriptor root = detail::update_tree(tree, v, rng);
    return std::tuple(std::move(tree), root);

  }
} // end namespace quetzal

#endif
