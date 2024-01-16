// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "concepts.hpp"

namespace quetzal::geography
{
  /// @brief Individuals can not escape the landscape's borders.
  class mirror
  {
    /// @brief Does nothing as by default the bounding box is reflective
    /// @tparam EdgeProperty The edge information
    /// @tparam Graph The graph type 
    /// @param s The source vertex
    /// @param graph The graph to update
    /// @param grid The spatial grid 
    template<class Graph>
    void operator()([[maybe_unused]] typename Graph::vertex_descriptor s, [[maybe_unused]] Graph & graph, [[maybe_unused]] const two_dimensional auto & grid) const
    {
      // do nothing as by default bounding box is reflective
    }
  };

  /// @brief Individuals can migrate out of the landscape to a sink vertex, but can not come back.
  class sink
  {
    /// @brief Connect source vertex s to a sink vertex if on the border
    /// @tparam EdgeProperty 
    /// @tparam Graph The graph type 
    /// @param s The source vertex
    /// @param graph The graph to update
    /// @param grid The spatial grid
    template<class Graph>
    void operator()(typename Graph::vertex_descriptor s, Graph & graph, const two_dimensional auto & grid) const
    {
      using edge_property_type = typename Graph::edge_property_type;
      int sink = grid.width() * grid.height() ;

      if ( graph.num_vertices() == sink ) // sink vertex is missing
        sink = graph.add_vertex();
      
      graph.add_edge(s, sink, edge_property_type(s, sink)); // one-way ticket :(
    }
  };

  /// @brief The 2D landscape becomes a 3D torus connecting opposed borders
  class torus
  {
    /// @brief Connect edges of source vertex s in a graph given a spatial grid
    /// @tparam EdgeProperty
    /// @tparam Graph The graph type
    /// @param s The source vertex
    /// @param graph The graph to update
    /// @param grid The spatial grid 
    template<class Graph>
    void operator()(typename Graph::vertex_descriptor s, Graph & graph, const two_dimensional auto & grid) const
    {
      using edge_property_type = typename Graph::edge_property_type;
      auto width = grid.width();
      auto height = grid.height();
      int symmetricIndex = 0;

      // top border
      if (s < width)
      {
        symmetricIndex = s + width * (height - 1 ) ;
      }
      // bottom border
      else if (s >=  (height - 1) * width )
      {
        symmetricIndex = s - (height - 1) * width;
      }
      // left border
      else if (s % width == 0)
      {
        symmetricIndex = s + width - 1 ;
      }
      // right border
      else if ( (s + 1) % width == 0)
      {
        symmetricIndex = s - width + 1;
      }
      graph.add_edge(s, symmetricIndex, edge_property_type(s, symmetricIndex) );
    }
  };

}