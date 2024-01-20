#include "quetzal/quetzal.hpp"

#include <cassert>
#include <filesystem>

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
    std::cout << land << std::endl;

    using vertex_info = boost::no_property;

    struct edge_info
    {
        int arbitrary_data = 0;
        edge_info(){};
        edge_info(int source, int target)
        {
            arbitrary_data = source + target;
        }
    };

    // We convert the grid to a graph, passing our assumptions
    auto graph = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::isotropy(),
                                 geo::mirror());

    // // Compute distance on Earth between two points
    // auto sphere_distance = [&](auto const &point1, auto const &point2) {
    //     return land.to_latlon(point1).great_circle_distance(land.to_latlon(point2));
    // };

    // auto v = graph.edges()
    // 	| std::transform( [&](auto const& e){ sphere_distance( graph.source( e ), graph.target( e ) );} )
    // 	| std::transform( [&](auto const& d){ return quetzal::demography::dispersal_kernel::exponential_power( d,
    // {.a=1., .b=5.5} ) ;} ) 	| std::ranges::to<std::vector>();

}
