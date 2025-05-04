// Copyright 2018 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#include <gtest/gtest.h>

#include <quetzal/geography.hpp>

#include <filesystem>
#include <ranges>

namespace geo = quetzal::geography;

TEST(geography_graph, expected_number_edges)
{
    auto file1 = std::filesystem::current_path() / "data/bio1.tif";
    auto file2 = std::filesystem::current_path() / "data/bio12.tif";

    // The raster have 10 bands that we will assign to 2001 ... 2010.
    std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

    // Initialize the landscape: for each var a key and a file, for all a time series.
    using landscape_type = geo::landscape<>;
    auto land = landscape_type::from_file({{"bio1", file1}, {"bio12", file2}}, times);

    // Our graph will not store any useful information
    using vertex_info = geo::no_property;
    using edge_info = geo::no_property;

    // Few of the possible assumptions combinations

    auto graph1 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::isotropy(), geo::mirror());
    auto graph2 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::isotropy(), geo::sink());
    auto graph3 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::anisotropy(), geo::sink());
    auto graph4 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_4_neighbors(), geo::isotropy(), geo::torus());
    auto graph5 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_8_neighbors(), geo::isotropy(), geo::mirror());
    auto graph6 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_8_neighbors(), geo::anisotropy(), geo::mirror());
    auto graph7 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_8_neighbors(), geo::anisotropy(), geo::torus());

    // Checking the numbers of edges is consistent with our assumptions

    int w = land.width();
    int h = land.height();

    int n1 = graph1.num_vertices();
    int e1 = n1 * ( n1 - 1 ) / 2  ;      // typical undirected complete graph

    EXPECT_EQ( n1 , land.num_locations() );
    EXPECT_EQ( e1 , graph1.num_edges() );

    int n2 = graph2.num_vertices();
    int e2 = e1 + 2 * ( w + h - 2 ) ;    // border vertices connected to the outland - a sink vertex
    EXPECT_EQ( n2 , n1 + 1 );              // sink vertex added
    EXPECT_EQ( e2 , graph2.num_edges() ); 
    
    int n3 = graph3.num_vertices();
    int e3 = 2 * e1 + 2 * ( w + h - 2 ); // number of land edges are doubled with anisotropy
    EXPECT_EQ( n3 , n2 );                  // sink vertex added
    EXPECT_EQ( e3 , graph3.num_edges() );
                
    int n4 = graph4.num_vertices();
    int e4 = h * ( w - 1 )               // horizontal edges
        + w * ( h - 1 )                  // vertical edges
        + h                              // torus joining West to East
        + w - 2 ;                        // torus joining North to South
    EXPECT_EQ( n4 , n1 );                  // no vertex added
    EXPECT_EQ( e4 , graph4.num_edges() );

    int n5 = graph5.num_vertices();
    int e5 = h * ( w - 1 )               // horizontal edges
        + w * ( h - 1 )                  // vertical edges
        + 2 * ( w - 1 ) * ( h - 1 ) ;    // intercardinal for internal vertices only
    EXPECT_EQ( n5 , n1 );                      // no vertex added
    EXPECT_EQ( e5 , graph5.num_edges() );

    int n6 = graph6.num_vertices();
    int e6 = 2* e5;
    EXPECT_EQ( n5 , n1 );                      // no vertex added
    EXPECT_EQ( e6 , graph6.num_edges() );

    int n7 = graph7.num_vertices();
    int e7 = 2 * e5 + 2 * (w + h - 2);
    EXPECT_EQ( n7 , n1 );                      // no vertex added
    EXPECT_EQ( e7 , graph7.num_edges() );

}

