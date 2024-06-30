#include "quetzal/geography.hpp"

namespace geo = quetzal::geography;

int main()
{
    auto file = std::filesystem::current_path() / "data/bio1.tif";

    // The raster have 10 bands that we will assign to 2001 ... 2010.
    std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

    // Landscape construction injecting times information
    auto land = geo::raster<>::from_file(file, times);

    // Graph construction with vertex and edge info
    using vertex_info = std::vector<double>;
    vertex_info N(times.size(), 0);

    using edge_info = geo::no_property;
    auto graph = geo::from_grid(land, N, edge_info(), geo::connect_fully(), geo::isotropy(), geo::mirror());
    graph[0][0] = 1;

    // Part of what happens in a demographic simulator:
    for( size_t t = 1; t < times.size(); ++t){
        for( auto x : graph.vertices() ){
            graph[x][t] = 2 * graph[x][t-1];
        }
    }

    // Post treatment
    std::cout <<  "Time series:\n"; 
    for(auto x : graph.vertices()){
         std::cout << "at vertex " << x << " : ";
        for(auto value : graph[x] ){
            std::cout << value << " ";
        }
        std::cout << "\n";
    }
}
