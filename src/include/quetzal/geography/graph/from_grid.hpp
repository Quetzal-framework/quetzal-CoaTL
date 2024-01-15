// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <concepts>
#include <ranges>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>

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
  template <typename C, typename G>
  concept Vecinity = requires(C c, const G &graph)
  {
    typename C::graph_type;
    {  c.connect(graph) } -> std::same_as<void>;
  };

  /// @brief Property of a process independent of the direction of movement
  using isotropy = boost::undirectedS;

  /// @brief Property of a process dependent of the direction of movement
  using anisotropy = boost::directedS;

  namespace detail
  {
  template <typename T, typename... U>
  concept IsAnyOf = (std::same_as<T, U> || ...);
  }

  template <typename T>
  concept Directionality = detail::IsAnyOf<T, isotropy, anisotropy>;

  template<typename T, class Graph>
  concept BoundPolicy = requires(T policy, typename Graph::vertex_descriptor s, Graph& graph, const SpatialGrid auto& grid)
  {
    { policy(s, graph, grid) } -> std::same_as<void>;
  };

  /// @brief Individuals can not escape the landscape's borders.
  class mirror
  {
    /// @brief Connect edges of source vertex s in a graph given a spatial grid
    /// @tparam EdgeProperty The edge information
    /// @tparam Graph The graph type 
    /// @param s The source vertex
    /// @param graph The graph to update
    /// @param grid The spatial grid 
    template<typename EdgeProperty, class Graph>
    void operator()(typename Graph::vertex_descriptor s, Graph & graph, SpatialGrid const& grid) const
    {      
      return void;
    }
  };

  /// @brief Individuals can migrate out of the landscape to a sink vertex, but can not come back.
  class sink
  {
    /// @brief Connect edges of source vertex s in a graph given a spatial grid
    /// @tparam EdgeProperty 
    /// @tparam Graph The graph type 
    /// @param s The source vertex
    /// @param graph The graph to update
    /// @param grid The spatial grid 
    template<typename EdgeProperty, class Graph>
    void operator()(typename Graph::vertex_descriptor s, Graph & graph, SpatialGrid const& grid) const
    {
      int sink = grid.width() * grid.height() ;

      if ( graph.num_vertices() == sink ) // sink vertex is missing
        sink = graph.add_vertex();
      
      graph.add_edge(s, sink, EdgeProperty(s, sink)); // one-way ticket :(
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
    template<typename EdgeProperty, class Graph>
    void operator()(typename Graph::vertex_descriptor s, Graph & graph, SpatialGrid const& grid) const
    {
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
      graph.add_edge(s, symmetricIndex, EdgeProperty(s, symmetricIndex) );
    }
  };

  /// @brief Spatial graph construction method
  /// @param grid A grid fulfilling the DiscreteSpace concept, e.g. raster or landscape.
  /// @param connectivity A policy class giving the connectivity method to be applied
  /// @param directionality A policy class giving the directionality (isotropy or anistropy)
  /// @return A graph with the desired level of connectivity
  template <class VertexProperty, class EdgeProperty>
  auto from_grid(SpatialGrid const &grid, Vecinity const& vecinity, Directionality const &directionality, BoundPolicy bound)
  {
    using graph_type = typename Vecinity::graph_type<Directionality>;
    const int x = grid.width();
    const int y = grid.height();
    const int num_vertices = x * y;
    graph_type graph(num_vertices);
    vecinity.connect(graph, bound);
    return graph;
  }

  /// @brief Policy class to connect vertices of a complete graph
  /// @tparam VertexProperty
  /// @tparam EdgeProperty
  template <class VertexProperty, class EdgeProperty>
  struct connect_fully
  {
    template <Directionality T>
    using graph_type = boost::adjacency_matrix< // dense
        boost::setS,                            // avoid parallel edges
        boost::vecS,                            // no reason to prefer listS
        T,
        VertexProperty,
        EdgeProperty>;

    constexpr void connect(graph_type &graph)
    {
      using directed_type = typename graph_type::directed_type;
      for (auto s = 0; s < graph.num_vertices(); ++s)
      {
        for (auto t = s + 1; t < graph.num_vertices(); ++t)
        {
          graph.add_edge(s, t, EdgeProperty(s, t)); // (s -> v) == (s <- v) if undirected
          if constexpr (std::is_same_as<directed_type, anisotropy>) graph.add_edge(t, s, EdgeProperty(t, s)); // (s -> v) != (s <- v) because directed
        }
      }
    }
  };

  /// @brief Policy class to connect vertices of a spatial graph to their 4 cardinal neighbors.
  /// @tparam VertexProperty Vertex information
  /// @tparam EdgeProperty Edge information
  template <class VertexProperty, class EdgeProperty>
  class connect_4_neighbors
  {
    public:

    /// @brief Type of graph generated by the policy
    /// @tparam T 
    template <Directionality T>
    using graph_type = boost::adjacency_list< // sparse
      boost::setS,                          // avoid parallel edges
      boost::vecS,                          // no reason to prefer listS
      T,
      VertexProperty,
      EdgeProperty>;

    using vertex_descriptor = graph_type::vertex_descriptor;
    private:

    using directed_type = typename graph_type::directed_type;

    void connect(vertex_descriptor s, vertex_descriptor t, graph_type & graph, SpatialGrid const& grid)
    {
      graph.add_edge(s, t, EdgeProperty(s, t));
      if constexpr (std::is_same_as<directed_type, anisotropy>)
        graph.add_edge(t, s, EdgeProperty(t, s)); // (s -> v) != (s <- v) because directed
    }

    void connect_top_left_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s+ 1, graph, grid);
      connect(s, s + grid.width(), graph, grid);
    }
    
    void connect_top_right_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);
      connect(s, s + grid.width(), graph, grid);
    }
    
    void connect_top_border_no_corners(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s -1, graph, grid);   
      connect(s, s + 1, graph, grid);   
      connect(s, s + grid.width(), graph, grid);  
    }
    
    void connect_bottom_left_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s + 1, graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
    }
    
    void connect_bottom_right_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
    }
    
    void connect_bottom_border_no_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);   
      connect(s, s + 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
    }
    
    void connect_left_border_no_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s + 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s + grid.width(), graph, grid);  
    }
    
    void connect_right_border_no_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s - 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s + grid.width(), graph, grid);  
    }
    
    void connect_interior_vertices(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s + 1 , graph, grid);   
      connect(s, s - 1 , graph, grid);   
      connect(s, s + grid.width(), graph, grid);  
      connect(s, s - grid.width(), graph, grid);  
    }

    public:

    /// @brief Connect vertices of a spatial graph to its neighbors
    /// @param graph The graph with \f$n\f$ vertices to be connected
    /// @param grid The spatial grid with \f$n\f$ cells
    /// @param border_policy Policy implementing the handling of border vertices. 
    constexpr void connect(graph_type &graph, SpatialGrid grid, BorderPolicy border_policy)
    {
      int width = grid.width();
      int height = grid.height();

      for (auto s = 0; s < graph.num_vertices(); ++s)
      {
        if (row == 0)
        {
          if (col == 0)
          {
            connect_top_left_corner(s, graph, grid, border_policy);
          }
          else if (col == width - 1)
          {
            connect_top_right_corner(s, graph, grid, border_policy);
          }
          else
          {
            connect_top_border_no_corners(s, graph, grid, border_policy);
          }
        }
        else if (row == height - 1)
        {
          if (col == 0)
          {
            connect_bottom_left_corner(s, graph, grid, border_policy);
          }
          else if (col == width - 1)
          {
            connect_bottom_right_corner(s, graph, grid, border_policy);
          }
          else
          {
            connect_bottom_border_no_corner(s, graph, grid, border_policy);
          }
        }
        else if (col == 0)
        {
          connect_left_border_no_corner(s, graph, grid, border_policy);
        }
        else if (col == width - 1)
        {
          connect_right_border_no_corner(s, graph, grid, border_policy);
        }
        else
        {
          connect_interior_vertices(s, graph);
        }
      }
    }
  };

 /// @brief Policy class to connect vertices of a spatial graph to their 9 cardinal and inter-cardinal neighbors.
  /// @tparam VertexProperty Vertex information
  /// @tparam EdgeProperty Edge information
  template <class VertexProperty, class EdgeProperty>
  class connect_8_neighbors
  {
    public:

    /// @brief Type of graph generated by the policy
    /// @tparam T 
    template <Directionality T>
    using graph_type = boost::adjacency_list< // sparse
      boost::setS,                          // avoid parallel edges
      boost::vecS,                          // no reason to prefer listS
      T,
      VertexProperty,
      EdgeProperty>;

    using vertex_descriptor = graph_type::vertex_descriptor;

    private:

    using directed_type = typename graph_type::directed_type;

    void connect(vertex_descriptor s, vertex_descriptor t, graph_type & graph, SpatialGrid const& grid)
    {
      graph.add_edge(s, t, EdgeProperty(s, t));
      if constexpr (std::is_same_as<directed_type, anisotropy>)
        graph.add_edge(t, s, EdgeProperty(t, s)); // (s -> v) != (s <- v) because directed
    }

    void connect_top_left_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s+ 1, graph, grid);
      connect(s, s + grid.width(), graph, grid);
      connect(s, s + grid.width() + 1, graph, grid);
    }
    
    void connect_top_right_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);
      connect(s, s + grid.width(), graph, grid);
      connect(s, s + grid.width() - 1 , graph, grid);
    }
    
    void connect_top_border_no_corners(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s -1, graph, grid);   
      connect(s, s + 1, graph, grid);   
      connect(s, s + grid.width(), graph, grid);
      connect(s, s + grid.width() - 1, graph, grid);  
      connect(s, s + grid.width() + 1, graph, grid);  
    }
    
    void connect_bottom_left_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s + 1, graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s - grid.width() + 1, graph, grid);  
    }
    
    void connect_bottom_right_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);   
      connect(s, s - grid.width(), graph, grid);
      connect(s, s - grid.width() - 1, graph, grid);  
    }
    
    void connect_bottom_border_no_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);   
      connect(s, s + 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);
      connect(s, s + grid.width() - 1, graph, grid);  
      connect(s, s + grid.width() + 1, graph, grid);  
    }
    
    void connect_left_border_no_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s + 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s + grid.width(), graph, grid);  
      connect(s, s - grid.width() + 1, graph, grid);  
      connect(s, s + grid.width() + 1, graph, grid);  
    }
    
    void connect_right_border_no_corner(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s - 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s + grid.width(), graph, grid);
      connect(s, s - grid.width() - 1, graph, grid);  
      connect(s, s + grid.width() - 1, graph, grid);    
    }
    
    void connect_interior_vertices(vertex_descriptor s, graph_type & graph, SpatialGrid const& grid, BorderPolicy border_policy)
    {
      border_policy(s, graph, grid);
      connect(s, s + 1 , graph, grid);   
      connect(s, s - 1 , graph, grid);   
      connect(s, s + grid.width(), graph, grid);  
      connect(s, s + grid.width() + 1, graph, grid);  
      connect(s, s + grid.width() - 1, graph, grid);  
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s - grid.width() + 1, graph, grid);  
      connect(s, s - grid.width() - 1, graph, grid);  
    }

    public:

    /// @brief Connect vertices of a spatial graph to its neighbors
    /// @param graph The graph with \f$n\f$ vertices to be connected
    /// @param grid The spatial grid with \f$n\f$ cells
    /// @param border_policy Policy implementing the handling of border vertices. 
    constexpr void connect(graph_type &graph, SpatialGrid grid, BorderPolicy border_policy)
    {
      int width = grid.width();
      int height = grid.height();

      for (auto s = 0; s < graph.num_vertices(); ++s)
      {
        if (row == 0)
        {
          if (col == 0)
          {
            connect_top_left_corner(s, graph, grid, border_policy);
          }
          else if (col == width - 1)
          {
            connect_top_right_corner(s, graph, grid, border_policy);
          }
          else
          {
            connect_top_border_no_corners(s, graph, grid, border_policy);
          }
        }
        else if (row == height - 1)
        {
          if (col == 0)
          {
            connect_bottom_left_corner(s, graph, grid, border_policy);
          }
          else if (col == width - 1)
          {
            connect_bottom_right_corner(s, graph, grid, border_policy);
          }
          else
          {
            connect_bottom_border_no_corner(s, graph, grid, border_policy);
          }
        }
        else if (col == 0)
        {
          connect_left_border_no_corner(s, graph, grid, border_policy);
        }
        else if (col == width - 1)
        {
          connect_right_border_no_corner(s, graph, grid, border_policy);
        }
        else
        {
          connect_interior_vertices(s, graph);
        }
      }
    }
  };

  namespace
  {
    template <Vecinity T>
    using Vecinity_checked = T;

    template <class V, class E>
    using Vecinity_t = Vecinity<connect_fully<V, E>>;

    template <class V, class E>
    using Vecinity_t = Vecinity<connect_4_neighbors<V, E>>;

    template <class V, class E>
    using Vecinity_t = Vecinity<connect_8_neighbors<V, E>>;
  }
}