#include "quetzal/quetzal.hpp"

#include <cassert>
#include <filesystem>
#include <ranges>

namespace geo = quetzal::geography;
using namespace mp_units::si::unit_symbols; // SI system: km, m, s

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

    // We convert the grid to a graph, passing our assumptions
    auto graph = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::isotropy(),
                                 geo::mirror());

    // Define a helper function to compute distance on Earth between two location_descriptors
    auto sphere_distance = [&](auto point1, auto point2)
    {
        return land.to_latlon(point1).great_circle_distance_to(land.to_latlon(point2));
    };

    // Define the type of distance-based kernel to use
    using kernel = quetzal::demography::dispersal_kernel::exponential_power<>;

    // Transform the edges of the graph to a vector of probability to disperse from source to target
    auto probabilities = graph.edges()
    	| std::views::transform( [&](auto const& e){ return sphere_distance( graph.source( e ), graph.target( e ) );} )
    	| std::views::transform( [&](auto distance){ return kernel::pdf( distance, { .a=200.*km , .b=5.5 } ) ;} );

    // Print the result
    for (auto const& i : probabilities)
        std::cout << i << '\n';
}
