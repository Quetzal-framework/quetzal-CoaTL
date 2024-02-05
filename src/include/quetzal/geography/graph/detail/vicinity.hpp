// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/////////////////////////////////////////////////////////////////////////// *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "../graph.hpp"
#include "concepts.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>

#include <cassert>

namespace quetzal::geography
{

namespace detail
{
    bool is_on_top_border(auto index, auto width) {
        return index < width;
    }

    bool is_on_bottom_border(auto index, auto width, auto height) {
        return index >= (height - 1) * width;
    }

    bool is_on_left_border(auto index, auto width) {
        return index % width == 0;
    }

    bool is_on_right_border(auto index, auto width) {
        return (index + 1) % width == 0;
    }

    bool is_on_border(auto index, auto width, auto height)
    {
        return ( is_on_top_border(index, width) or
            is_on_bottom_border(index, width, height) or
            is_on_left_border(index, width) or
            is_on_right_border(index, width) );
    }

}

struct connect_fully
{
    using connectedness = dense;

    template <class G, two_dimensional S, bounding<G, S> B>
    void connect(G &graph, S const &grid, B bound_policy) const
    {
        using directed_category = typename G::directed_category;
        using vertex_property = typename G::vertex_property;
        using edge_property = typename G::edge_property;

        int width = grid.width();
        int height = grid.height();
        int num_land_vertices = width * height;

        assert(num_land_vertices > 0 and "trying to initialize a graph from an empty grid.");

        for (auto s = 0; s < num_land_vertices; ++s)
        {
            for (auto t = s + 1; t < num_land_vertices; ++t)
            {
                detail::edge_construction<edge_property>::delegate(s, t, graph, grid); // (s -> v) == (s <- v) if undirected
                if constexpr (std::same_as<directed_category, anisotropy>)
                    detail::edge_construction<edge_property>::delegate(t, s, graph, grid); // (s -> v) != (s <- v) because directed
            }
        
            if( detail::is_on_border(s, width, height ) ) 
                bound_policy(s, graph, grid);
        }
    }
};

class connect_4_neighbors
{
  public:
    using connectedness = sparse;

  private:
    template <typename G> void connect(auto s, auto t, G &graph, auto const &grid) const
    {
        using edge_property = typename G::edge_property;
        detail::edge_construction<edge_property>::delegate(s, t, graph, grid);
        if constexpr (std::same_as<typename G::directed_category, anisotropy>)
            detail::edge_construction<edge_property>::delegate(t, s, graph, grid);
    }

    void connect_top_left_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s + grid.width(), graph, grid);
    }

    void connect_top_right_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s + grid.width(), graph, grid);
    }

    void connect_top_border_no_corners(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s + grid.width(), graph, grid);
    }

    void connect_bottom_left_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
    }

    void connect_bottom_right_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
    }

    void connect_bottom_border_no_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
    }

    void connect_left_border_no_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
        connect(s, s + grid.width(), graph, grid);
    }

    void connect_right_border_no_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
        connect(s, s + grid.width(), graph, grid);
    }

    void connect_interior_vertices(auto s, auto &graph, auto const &grid) const
    {
        connect(s, s + 1, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s + grid.width(), graph, grid);
        connect(s, s - grid.width(), graph, grid);
    }

  public:
    template <class G, two_dimensional S, bounding<G, S> B> 
    void connect(G &graph, S const &grid, B bound_policy) const
    {
        using directed_category = typename G::directed_category;
        using vertex_property = typename G::vertex_property;
        using edge_property = typename G::edge_property;

        int width = grid.width();
        int height = grid.height();
        int num_land_vertices = width * height;

        assert(num_land_vertices > 0 and "trying to initialize a graph from an empty grid.");

        for (auto s = 0; s < num_land_vertices; ++s)
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
                connect_interior_vertices(s, graph, grid);
            }
        }
    }
};

class connect_8_neighbors
{
  public:
    using connectedness = sparse;

  private:
    template <class G> void connect(auto s, auto t, G &graph, auto const &grid) const
    {
        using directed_category = typename G::directed_category;
        using vertex_property = typename G::vertex_property;
        using edge_property = typename G::edge_property;


        int width = grid.width();
        int height = grid.height();
        int num_land_vertices = width * height;
        assert( s >= 0 );
        assert( s < num_land_vertices);
        assert( t >= 0 );
        assert( t < num_land_vertices);

        detail::edge_construction<edge_property>::delegate(s, t, graph, grid);
        if constexpr (std::same_as<directed_category, anisotropy>)
            detail::edge_construction<edge_property>::delegate(t, s, graph, grid);
    }

    void connect_top_left_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s + grid.width(), graph, grid);
        connect(s, s + grid.width() + 1, graph, grid);
    }

    void connect_top_right_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s + grid.width(), graph, grid);
        connect(s, s + grid.width() - 1, graph, grid);
    }

    void connect_top_border_no_corners(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s + grid.width(), graph, grid);
        connect(s, s + grid.width() - 1, graph, grid);
        connect(s, s + grid.width() + 1, graph, grid);
    }

    void connect_bottom_left_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
        connect(s, s - grid.width() + 1, graph, grid);
    }

    void connect_bottom_right_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
        connect(s, s - grid.width() - 1, graph, grid);
    }

    void connect_bottom_border_no_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
        connect(s, s - grid.width() - 1, graph, grid);
        connect(s, s - grid.width() + 1, graph, grid);
    }

    void connect_left_border_no_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s + 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
        connect(s, s + grid.width(), graph, grid);
        connect(s, s - grid.width() + 1, graph, grid);
        connect(s, s + grid.width() + 1, graph, grid);
    }

    void connect_right_border_no_corner(auto s, auto &graph, auto const &grid, auto const &bound_policy) const
    {
        bound_policy(s, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
        connect(s, s + grid.width(), graph, grid);
        connect(s, s - grid.width() - 1, graph, grid);
        connect(s, s + grid.width() - 1, graph, grid);
    }

    void connect_interior_vertices(auto s, auto &graph, auto const &grid) const
    {
        connect(s, s + 1, graph, grid);
        connect(s, s - 1, graph, grid);
        connect(s, s + grid.width(), graph, grid);
        connect(s, s + grid.width() + 1, graph, grid);
        connect(s, s + grid.width() - 1, graph, grid);
        connect(s, s - grid.width(), graph, grid);
        connect(s, s - grid.width() + 1, graph, grid);
        connect(s, s - grid.width() - 1, graph, grid);
    }

  public:
    template <class G, two_dimensional S, bounding<G, S> B> 
    void connect(G &graph, S const &grid, B bound_policy) const
    {
        using directed_category = typename G::directed_category;
        using vertex_property = typename G::vertex_property;
        using edge_property = typename G::edge_property;

        int width = grid.width();
        int height = grid.height();
        int num_land_vertices = width * height;

        assert(num_land_vertices > 0 and "trying to initialize a graph from an empty grid.");

        for (auto s = 0; s < num_land_vertices; ++s)
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
                connect_interior_vertices(s, graph, grid);
            }
        }
    }
};

} // namespace quetzal::geography