#include "quetzal/geography.hpp"
#include <random>
#include <cmath>

namespace geo = quetzal::geography;

// Time series of the 3 species population sizes to embed along the graph
struct vertex_info {
    using time_series_type = std::vector<double>;
    time_series_type P1, P2, P3;
    // Constructor to initialize the vectors with a specified length and fill them with zeros
    vertex_info(int length): P1(length, 0), P2(length, 0), P3(length, 0){}
    // Needs to be default constructible
    vertex_info() = default;
};

// Overload the << operator for the vertex_info struct
std::ostream& operator<<(std::ostream& os, const vertex_info& vertex) {
    auto format = [&](const auto & v, const auto& name){
        os << "\t" << name << " : ";
        for(const auto& val : v)
            os << val << " ";
        os << "\n";
    };
    format(vertex.P1, "P1");
    format(vertex.P2, "P2");
    format(vertex.P3, "P3");
    return os;
}

int main()
{
    auto file = std::filesystem::current_path() / "data/bio1.tif";

    // The raster have 10 bands that we will assign to 2001 ... 2010.
    std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

    // Landscape construction injecting times information
    auto land = geo::raster<>::from_file(file, times);

    // Graph construction with vertex and edge info
    using edge_info = geo::no_property;
    auto graph = geo::from_grid(land, vertex_info(times.size()), edge_info(), geo::connect_fully(), geo::isotropy(), geo::mirror());

    // Let's initialize population sizes for a single vertex for demonstration
    graph[0].P1[0] = 200;
    graph[0].P2[0] = 1;
    graph[0].P3[0] = 1;

    // Declare a random number generator
    std::mt19937 rng{std::random_device{}()};

    auto random_double = [&](double min, double max){ return std::uniform_real_distribution<double>(min, max)(rng);};

    double r1 = random_double(0.0, 1.0);  // Intrinsic growth rate for species 1
    double r2 = random_double(0.0, 1.0);  // Intrinsic growth rate for species 2
    double r3 = random_double(0.0, 1.0);  // Intrinsic growth rate for species 3

    double K1 = random_double(50.0, 100.0);  // Carrying capacity for species 1
    double K2 = random_double(50.0, 100.0);  // Carrying capacity for species 2
    double K3 = random_double(50.0, 100.0);  // Carrying capacity for species 3

    double a12 = random_double(0.0, 0.1);  // Predation rate of species 2 on species 1
    double a13 = random_double(0.0, 0.1);  // Predation rate of species 3 on species 1

    double b21 = random_double(0.0, 0.1);  // Benefit species 2 gets from preying on species 1
    double c23 = random_double(0.0, 0.1);  // Competition rate between species 2 and 3

    double b31 = random_double(0.0, 0.1);  // Benefit species 3 gets from preying on species 1
    double c32 = random_double(0.0, 0.1);  // Competition rate between species 3 and 2

    // Part of what could happen in a demographic simulator:
    for( size_t t = 1; t < times.size(); ++t){
        for( auto x : graph.vertices() ){
            auto P1 = graph[x].P1[t-1];
            auto P2 = graph[x].P2[t-1];
            auto P3 = graph[x].P3[t-1];
            graph[x].P1[t] = std::max(P1 + r1 * P1 * (1 - P1 / K1) - a12 * P1 * P2 - a13 * P1 * P3, 0.);
            graph[x].P2[t] = std::max(P2 + r2 * P2 * (1 - P2 / K2) + b21 * P1 * P2 - c23 * P2 * P3, 0.);
            graph[x].P3[t] = std::max(P3 + r3 * P3 * (1 - P3 / K3) + b31 * P1 * P3 - c32 * P2 * P3, 0.);
        }
    }

    // Post treatment
    std::cout <<  "Time series:\n"; 
    for(auto x : graph.vertices()){
         std::cout << "at vertex " << x << " : \n";
         std::cout << graph[x] << std::endl;
    }
}
