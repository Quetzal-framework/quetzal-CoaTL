// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "detail/concepts.hpp"
#include "detail/vicinity.hpp"
#include "detail/bound_policy.hpp"

namespace quetzal::geography
{
  /// @brief Spatial graph construction method
  /// @param grid A spatial grid, e.g. raster or landscape.
  /// @param vicinity A policy class giving the connectivity method to be applied
  /// @param directionality A policy class giving the directionality (isotropy or anistropy)
  /// @param bound A BoundPolicy
  /// @return A graph with the desired level of connectivity
  template <class VertexProperty, class EdgeProperty>
  auto from_grid(SpatialGrid auto const& grid, Vicinity auto const& vicinity, Directionality auto const& directionality, auto bound)
  {
    using graph_type = typename decltype(vicinity)::template graph_type<decltype(directionality)>;
    const int x = grid.width();
    const int y = grid.height();
    const int num_vertices = x * y;
    graph_type graph(num_vertices);
    vicinity.connect(graph, bound);
    return graph;
  }

}