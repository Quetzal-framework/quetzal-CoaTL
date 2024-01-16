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

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>

namespace quetzal::geography
{
  struct connect_fully
  {
    template <directional T, class VertexProperty, class EdgeProperty>
    using graph_type = boost::adjacency_matrix< boost::setS, boost::vecS,T, VertexProperty, EdgeProperty>;

    template<class G, two_dimensional S, directional D, bounding<G,S> B>
    void connect(G & graph, [[maybe_unused]] S const& grid, [[maybe_unused]] B policy)
    {
      using directed_type = typename G::directed_type;
      using vertex_property_type = typename G::vertex_property_type;
      using edge_property_type = typename G::edge_property_type;

      static_assert( std::same_as<G, graph_type<directed_type, vertex_property_type, edge_property_type>>);

      for (auto s = 0; s < graph.num_vertices(); ++s)
      {
        for (auto t = s + 1; t < graph.num_vertices(); ++t)
        {
          graph.add_edge(s, t, edge_property_type(s, t)); // (s -> v) == (s <- v) if undirected
          if constexpr (std::same_as<directed_type, anisotropy>) 
            graph.add_edge(t, s, edge_property_type(t, s)); // (s -> v) != (s <- v) because directed
        }
      }
    }

  };

  class connect_4_neighbors
  {
    public:

    template <two_dimensional T, class VertexProperty, class EdgeProperty>
    using graph_type = boost::adjacency_list<boost::setS, boost::vecS, T, VertexProperty, EdgeProperty>;

    private:

    template<typename G>
    void connect(auto s, auto t, G & graph, auto const& grid)
    {
      using edge_property_type = typename G::edge_property_type;
      graph.add_edge(s, t, edge_property_type(s, t));
      if constexpr (std::same_as< typename G::directed_type, anisotropy >)
      {
        graph.add_edge(t, s, edge_property_type(t, s));
      }
    }
    
    void connect_top_left_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s+ 1, graph, grid);
      connect(s, s + grid.width(), graph, grid);
    }
    
    void connect_top_right_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);
      connect(s, s + grid.width(), graph, grid);
    }
    
    void connect_top_border_no_corners(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s -1, graph, grid);   
      connect(s, s + 1, graph, grid);   
      connect(s, s + grid.width(), graph, grid);  
    }

    void connect_bottom_left_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s + 1, graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
    }

    void connect_bottom_right_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
    }

    void connect_bottom_border_no_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);   
      connect(s, s + 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
    }
    
    void connect_left_border_no_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s + 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s + grid.width(), graph, grid);  
    }
    
    void connect_right_border_no_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s - 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s + grid.width(), graph, grid);  
    }
    
    void connect_interior_vertices(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s + 1 , graph, grid);   
      connect(s, s - 1 , graph, grid);   
      connect(s, s + grid.width(), graph, grid);  
      connect(s, s - grid.width(), graph, grid);  
    }

    public:

    template<class G, two_dimensional S, directional D, bounding<G, S> B>
    void connect(G & graph, S const& grid, B bound_policy)
    {
      using directed_type = typename G::directed_type;
      using vertex_property_type = typename G::vertex_property_type;
      using edge_property_type = typename G::edge_property_type;

      static_assert(std::same_as<G, graph_type<directed_type, vertex_property_type, edge_property_type>> );

      int width = grid.width();
      int height = grid.height();

      for (auto s = 0; s < graph.num_vertices(); ++s)
      {
        int row = s / width;
        int col = s % width;

        if (row == 0)
        {
          if (col == 0)
          {
            connect_top_left_corner(s, graph, grid, bound_policy);
          }
          else if (col == width - 1)
          {
            connect_top_right_corner(s, graph, grid, bound_policy);
          }
          else
          {
            connect_top_border_no_corners(s, graph, grid, bound_policy);
          }
        }
        else if (row == height - 1)
        {
          if (col == 0)
          {
            connect_bottom_left_corner(s, graph, grid, bound_policy);
          }
          else if (col == width - 1)
          {
            connect_bottom_right_corner(s, graph, grid, bound_policy);
          }
          else
          {
            connect_bottom_border_no_corner(s, graph, grid, bound_policy);
          }
        }
        else if (col == 0)
        {
          connect_left_border_no_corner(s, graph, grid, bound_policy);
        }
        else if (col == width - 1)
        {
          connect_right_border_no_corner(s, graph, grid, bound_policy);
        }
        else
        {
          connect_interior_vertices(s, graph);
        }
      }
    }
  };

  class connect_8_neighbors
  {
    public:

    template <two_dimensional T, class VertexProperty, class EdgeProperty>
    using graph_type = boost::adjacency_list< boost::setS, boost::vecS, T, VertexProperty, EdgeProperty>;

    private:

    template<class G>
    void connect(auto s, auto t, G & graph, auto const& grid)
    {
      using directed_type = typename G::directed_type;
      using vertex_property_type = typename G::vertex_property_type;
      using edge_property_type = typename G::edge_property_type;

      static_assert(std::same_as< G, graph_type<directed_type, vertex_property_type, edge_property_type>> );

      graph.add_edge(s, t, edge_property_type(s, t));

      if constexpr (std::same_as<directed_type, anisotropy>)
      {
        graph.add_edge(t, s, edge_property_type(t, s));
      }

    }

    void connect_top_left_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s+ 1, graph, grid);
      connect(s, s + grid.width(), graph, grid);
      connect(s, s + grid.width() + 1, graph, grid);
    }
    
    void connect_top_right_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);
      connect(s, s + grid.width(), graph, grid);
      connect(s, s + grid.width() - 1 , graph, grid);
    }

    void connect_top_border_no_corners(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s -1, graph, grid);   
      connect(s, s + 1, graph, grid);   
      connect(s, s + grid.width(), graph, grid);
      connect(s, s + grid.width() - 1, graph, grid);  
      connect(s, s + grid.width() + 1, graph, grid);  
    }

    void connect_bottom_left_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s + 1, graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s - grid.width() + 1, graph, grid);  
    }

    void connect_bottom_right_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);   
      connect(s, s - grid.width(), graph, grid);
      connect(s, s - grid.width() - 1, graph, grid);  
    }

    void connect_bottom_border_no_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s - 1, graph, grid);   
      connect(s, s + 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);
      connect(s, s + grid.width() - 1, graph, grid);  
      connect(s, s + grid.width() + 1, graph, grid);  
    }

    void connect_left_border_no_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s + 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s + grid.width(), graph, grid);  
      connect(s, s - grid.width() + 1, graph, grid);  
      connect(s, s + grid.width() + 1, graph, grid);  
    }

    void connect_right_border_no_corner(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
      connect(s, s - 1 , graph, grid);   
      connect(s, s - grid.width(), graph, grid);  
      connect(s, s + grid.width(), graph, grid);
      connect(s, s - grid.width() - 1, graph, grid);  
      connect(s, s + grid.width() - 1, graph, grid);    
    }

    void connect_interior_vertices(auto s, auto & graph, auto const& grid, auto const& bound_policy)
    {
      bound_policy(s, graph, grid);
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

    template<class G, two_dimensional S, directional D, bounding<G, S> B>
    void connect(G & graph, S const& grid, B bound_policy)
    {
      using directed_type = typename G::directed_type;
      using vertex_property_type = typename G::vertex_property_type;
      using edge_property_type = typename G::edge_property_type;
      static_assert(std::same_as<G, graph_type<directed_type, vertex_property_type, edge_property_type> > );

      int width = grid.width();
      int height = grid.height();

      for (auto s = 0; s < graph.num_vertices(); ++s)
      {
        int row = s / width;
        int col = s % width;

        if (row == 0)
        {
          if (col == 0)
          {
            connect_top_left_corner(s, graph, grid, bound_policy);
          }
          else if (col == width - 1)
          {
            connect_top_right_corner(s, graph, grid, bound_policy);
          }
          else
          {
            connect_top_border_no_corners(s, graph, grid, bound_policy);
          }
        }
        else if (row == height - 1)
        {
          if (col == 0)
          {
            connect_bottom_left_corner(s, graph, grid, bound_policy);
          }
          else if (col == width - 1)
          {
            connect_bottom_right_corner(s, graph, grid, bound_policy);
          }
          else
          {
            connect_bottom_border_no_corner(s, graph, grid, bound_policy);
          }
        }
        else if (col == 0)
        {
          connect_left_border_no_corner(s, graph, grid, bound_policy);
        }
        else if (col == width - 1)
        {
          connect_right_border_no_corner(s, graph, grid, bound_policy);
        }
        else
        {
          connect_interior_vertices(s, graph);
        }
      }
    }
  };

}