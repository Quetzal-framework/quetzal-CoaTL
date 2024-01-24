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

#include <concepts>
#include <ranges>

namespace quetzal::geography
{

template <typename T>
concept two_dimensional = requires(T a) {
                              requires std::convertible_to<decltype(a.width()), int>;
                              requires std::convertible_to<decltype(a.height()), int>;
                          };

namespace detail
{
template <typename T, typename... U>
concept is_any_of = (std::same_as<T, U> || ...);
}

template <typename T>
concept directional = detail::is_any_of<T, isotropy, anisotropy>;

template <typename T, class G, class S>
concept bounding = two_dimensional<S> and requires(T t, typename G::vertex_descriptor s, G &graph, S const &grid) {
                                              {
                                                  t(s, graph, grid)
                                                  } -> std::same_as<void>;
                                          };
namespace detail
{
    
template<typename T>
struct edge_construction
{
    static inline constexpr void delegate(auto s, auto t, auto& graph)
    {
        graph.add_edge(s, t, T(s,t));
    }
};

template<>
struct edge_construction<boost::no_property>
{
    static inline constexpr void delegate(auto s, auto t, auto& graph)
    {
        graph.add_edge(s, t);
    }
};

}
} // namespace quetzal::geography