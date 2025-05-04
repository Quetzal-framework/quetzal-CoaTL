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
    auto land = landscape_type::from_file({{"bio1", file1}, {"bio12", file2}}, times);

    // Our graph will not store any useful information
    using vertex_info = geo::no_property;
    using edge_info = geo::no_property;

    // Few of the possible assumptions combinations
    auto graph1 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::isotropy(), geo::mirror());
    auto graph2 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_4_neighbors(), geo::isotropy(), geo::sink());
    auto graph3 = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_8_neighbors(), geo::anisotropy(), geo::torus());

    std::cout << "Graph 1 has " << graph1.num_vertices() << " vertices, " << graph1.num_edges() << " edges." << std::endl;
    std::cout << "Graph 2 has " << graph2.num_vertices() << " vertices, " << graph2.num_edges() << " edges." << std::endl;
    std::cout << "Graph 3 has " << graph3.num_vertices() << " vertices, " << graph3.num_edges() << " edges." << std::endl;
}
