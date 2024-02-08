#include "quetzal/geography.hpp"

namespace geo = quetzal::geography;

// User-defined data structure to store arbitrary vertex information
struct MyVertexInfo {
    std::string population_label = "NA";
    std::vector<double> population_data_chunk;
    // ... anything else required by the user context
};

// User-defined data structure to store arbitrary edge information
struct MyEdgeInfo {
    mp_units::quantity<mp_units::si::metre> distance;
    // needs to be default-constructible
    MyEdgeInfo() = default;
    // constructor required by from_grid method: takes a source and a target vertex descriptor, and a spatial grid
    MyEdgeInfo(auto s, auto t, auto const& raster){ 
        distance = raster.to_lonlat(s).great_circle_distance_to( raster.to_lonlat(t) );
    }
};

int main()
{
    auto file = std::filesystem::current_path() / "data/bio1.tif";

    // The raster have 10 bands that we will assign to 2001 ... 2010.
    std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

    // Landscape construction
    using landscape_type = geo::raster<>;
    auto land = geo::raster<>::from_file(file, times);

    // Graph construction with vertex and edge info
    auto graph = geo::from_grid(land, MyVertexInfo(), MyEdgeInfo(), geo::connect_fully(), geo::isotropy(), geo::mirror());

    std::cout << "Graph has " << graph.num_vertices() << " vertices, " << graph.num_edges() << " edges." << std::endl;

    for( auto const& e : graph.edges() ){
        std::cout << graph.source(e) << " <-> " << graph.target(e) << " : " << graph[e].distance << std::endl;
    }

}
