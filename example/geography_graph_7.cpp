#include "quetzal/geography.hpp"

namespace geo = quetzal::geography;

struct MyVertexInfo {
    std::string label = "NA";
    std::vector<double> pop_data_chunk;
};

struct MyEdgeInfo {
    double distance;
    MyEdgeInfo() = default;
    // required by from_grid
    MyEdgeInfo(auto u, auto v, auto const& land){ 
        distance = land.to_lonlat(u).great_circle_distance_to( land.to_lonlat(v) );
    }
};

int main()
{
    auto file = std::filesystem::current_path() / "data/bio1.tif";

    // The raster have 10 bands that we will assign to 2001 ... 2010.
    std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

    using landscape_type = geo::raster<>;
    auto land = geo::raster<>::from_file(file, times);
    auto graph = geo::from_grid(land, MyVertexInfo(), MyEdgeInfo(), geo::connect_fully(), geo::isotropy(), geo::mirror());

    std::cout << "Graph has " << graph.num_vertices() << " vertices, " << graph.num_edges() << " edges." << std::endl;

}
