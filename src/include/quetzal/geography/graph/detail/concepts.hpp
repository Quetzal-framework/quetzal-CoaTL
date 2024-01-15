// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
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
  ///
  /// @brief Concept of a landscape for the from_grid graph construction method
  ///
  template <typename T>
  concept SpatialGrid = requires(T a, typename T::location_type loc)
  {
    requires std::is_convertible_v<decltype(a.width()), int>;
    requires std::is_convertible_v<decltype(a.height()), int>;
    {  a.locations()  } -> std::ranges::range;
    loc = std::ranges::begin(a.locations());
  };

  ///
  /// @brief Concept of a connector
  ///
  template <typename T>
  concept Vicinity = requires(T t, typename T::graph_type const& graph)
  {
    {  t.connect(graph) } -> std::same_as<void>;
  };

  namespace detail
  {
  template <typename T, typename... U>
  concept IsAnyOf = (std::same_as<T, U> || ...);
  }

  template <typename T>
  concept Directionality = detail::IsAnyOf<T, isotropy, anisotropy>;

  template<typename T, class U, class Graph>
  concept BoundPolicy = SpatialGrid<U> and requires(T policy, typename Graph::vertex_descriptor s, Graph& graph, U const& grid)
  {
    { policy(s, graph, grid) } -> std::same_as<void>;
  };

}