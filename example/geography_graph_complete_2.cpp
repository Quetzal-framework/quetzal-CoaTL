#include "quetzal/geography.hpp"

#include <cassert>
#include <filesystem>
#include <ranges>

namespace geo = quetzal::geography;

int main()
{
    auto file1 = std::filesystem::current_path() / "data/bio1.tif";
    auto file2 = std::filesystem::current_path() / "data/bio12.tif";

    // The raster have 10 bands that we will assign to 2001 ... 2010.
    std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

    // Initialize the landscape: for each var a key and a file, for all a time series.
    using landscape_type = geo::landscape<>;
    auto land = landscape_type::from_files({{"bio1", file1}, {"bio12", file2}}, times);

    // Our graph will not store any useful information
    using vertex_info = geo::no_property;
    using edge_info = geo::no_property;

    // Few of the possible assumptions combinations

    auto graph1 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::isotropy(), geo::mirror());
    auto graph2 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::isotropy(), geo::sink());
    //auto graph3 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::anisotropy(), geo::sink());
    //auto graph4 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_4_neighbors(), geo::isotropy(), geo::torus());
    //auto graph4 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_8_neighbors(), geo::isotropy(), geo::torus());

    // Checking the numbers of edges is consistent with our assumptions

    // Typical undirected complete graph
    int w = land.width();
    int h = land.height();

    int n1 = graph1.num_vertices();
    int e1 = n1 * ( n1 - 1 ) / 2  ;
    assert( n1 == land.num_locations() );
    assert( e1 == graph1.num_edges() );

    int n2 = graph2.num_vertices();
    int e2 = e1 + 2 * ( w + h - 2 ) ; 
    std::cout << e2 << " " <<  graph2.num_edges() << std::endl;
    assert( n2 == n1 + 1 );              // sink vertex added
    assert( e2 == graph2.num_edges() );  // border vertices connected to sink
    
    // Edges become directed: (u->v) != (u<-v) , and border vertices are one-way connected to the outland, a sink vertex
    //n = graph2.num_vertices();
    //assert( graph2.num_edges() == 2 * ( w + h - 2) + 2 * ( w -2) * (h - 2));
    // assert( graph2.num_edges() == 2 * (w + h -2) // number of border vertices connected to sink
    //     + 2 * (n-1) * n / 2 );                   // number of edges are doubled with anisotropy

    // Edges are undirected and border vertices are connected to each other to form a doughnut
    // n = graph3.num_vertices();
    // assert( graph3.num_edges() == 4 * 2                   // 4 corners have 2 neighbors
    //     + 2 * 3 * (land.width() - 2)                      // top and bottom borders have 3
    //     + 2 * 3 * (land.height() - 2)                     // left and right borders have 3
    //     + 4 * (land.width() - 2 ) * (land.height() - 2 )  // internal have 4 
    //     + land.width() + land.height()                    // torus: borders are joined
    //     );

    std::cout << "Assumptions checked!" << std::endl;
}
