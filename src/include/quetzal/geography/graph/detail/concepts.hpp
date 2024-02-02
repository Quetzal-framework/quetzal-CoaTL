// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/////////////////////////////////////////////////////////////////////////// *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "directionality.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>

#include <concepts>
#include <ranges>

namespace quetzal::geography
{

namespace detail
{
template <typename T, typename... U>
concept is_any_of = (std::same_as<T, U> || ...);

template<typename T>
struct edge_construction
{
    static inline constexpr void delegate(auto s, auto t, auto& graph, auto const& grid)
    {
        graph.add_edge(s, t, T(s,t, grid));
    }
};

template<>
struct edge_construction<boost::no_property>
{
    static inline constexpr void delegate(auto s, auto t, auto& graph, [[maybe_unused]] auto const& grid)
    {
        graph.add_edge(s, t);
    }
};

}

/// @brief Concept to represent the directionality of edges in a graph
template <typename T>
concept directional = detail::is_any_of<T, isotropy, anisotropy>;

static_assert ( directional<isotropy> );
static_assert ( directional<anisotropy> );

/// @brief Represents no information carried by vertices or edges of a graph.
using no_property = boost::no_property;

/// @brief Tag for sparse graph representation
struct sparse 
{
  template<directional Directed, class VertexProperty, class EdgeProperty>
  using rebind = boost::adjacency_list<boost::setS, boost::vecS, Directed, VertexProperty, EdgeProperty>;
};

/// @brief Tag for sparse graph representation
struct dense 
{
  template<class Directed, class VertexProperty, class EdgeProperty>
  using rebind = boost::adjacency_matrix<Directed, VertexProperty, EdgeProperty, no_property, std::allocator<bool>>;
};

/// @brief Concept to represent the connectedness of a graph
template<typename T>
concept connectedness = detail::is_any_of<T, sparse, dense>;

static_assert ( connectedness<dense> );
static_assert ( connectedness<sparse> );

/// @brief Concept to represent a 2D spatial grid
template <typename T>
concept two_dimensional = requires(T a) {
                              requires std::convertible_to<decltype(a.width()), int>;
                              requires std::convertible_to<decltype(a.height()), int>;
                          };

/// @brief Concept to represent the bounding policy of a spatial graph
template <typename T, class G, class S>
concept bounding = two_dimensional<S> and requires(T t, typename G::vertex_descriptor s, G &graph, S const &grid) {
                                              {
                                                  t(s, graph, grid)
                                                  } -> std::same_as<void>;
                                          };
} // namespace quetzal::geography