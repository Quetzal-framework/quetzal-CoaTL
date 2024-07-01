// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/////////////////////////////////////////////////////////////////////////// *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "detail/bound_policy.hpp"
#include "detail/concepts.hpp"
#include "detail/vicinity.hpp"

namespace quetzal::geography
{

/// @brief Spatial graph construction method
/// @param grid A spatial grid, e.g. raster or landscape.
/// @param v An example of information to be stored on vertices
/// @param e An example of information to be stored on edges
/// @param vicinity A policy class giving the connectivity method to be applied
/// @param directionality A policy class giving the directionality (isotropy or anisotropy)
/// @param bound A BoundPolicy
/// @return A graph with the desired level of connectivity
template <two_dimensional SpatialGrid, class VertexProperty, class EdgeProperty, class Vicinity,
          directional Directionality, class Policy>
auto from_grid(SpatialGrid const &grid, VertexProperty const &v, EdgeProperty const &e, Vicinity const &vicinity,
               Directionality dir, Policy const &bounding_policy)
{
    using graph_type = quetzal::geography::graph<VertexProperty, EdgeProperty, typename Vicinity::connectedness, Directionality>;
    graph_type graph( grid.width() * grid.height() + bounding_policy.num_extra_vertices() ) ;
    vicinity.connect(graph, grid, bounding_policy);

    if constexpr ( ! std::is_same_v<VertexProperty, no_property>) {
        for( auto vertex : graph.vertices() ){
            graph[vertex] = v;
        }
    }

    if constexpr ( ! std::is_same_v<EdgeProperty, no_property>) {
        for(auto edge : graph.edges()){
            graph[edge] = e;
        }
    }

    return graph;
}

} // namespace quetzal::geography