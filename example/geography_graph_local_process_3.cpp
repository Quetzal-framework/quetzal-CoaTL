#include "quetzal/geography.hpp"
#include <random>
#include <cmath>

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

    // Let's compare some time series
    graph[0][0] = 1;
    graph[1][0] = 1;
    graph[2][0] = 1;

    // Declare a random number generator
    std::mt19937 rng{std::random_device{}()};

    // Part of what could happen in a demographic simulator:
    for( size_t t = 1; t < times.size(); ++t){
        for( auto x : graph.vertices() ){
            auto r = std::uniform_real_distribution<double>(1, 5)(rng);
            auto K = 10. * land.at(x, t-1).value_or(10.);
            auto P = graph[x][t-1];
            graph[x][t] = std::max(P + r * P * (1 - P/K), 0.);
        }
    }

    // Post treatment
    std::cout <<  "Time series:\n"; 
    for(auto x : graph.vertices()){
         std::cout << "at vertex " << x << " : ";
        for(auto value : graph[x] ){
            std::cout << static_cast<int>(value) << " ";
        }
        std::cout << "\n";
    }
}
