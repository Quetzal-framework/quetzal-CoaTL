#include "quetzal/geography.hpp"

namespace geo = quetzal::geography;

// User-defined concept of spatial grid
struct MySpatialGrid
{
    // It is just required to define these two functions
    constexpr int width() const { return 300; }
    constexpr int height() const { return 100; }
};

int main()
{

    static_assert(geo::two_dimensional<MySpatialGrid>, "MySpatialGrid does not fulfill the two_dimensional requirements");
    MySpatialGrid land;

    // Our graph will not store any useful information
    using vertex_info = geo::no_property;
    using edge_info = geo::no_property;

    auto graph = geo::from_grid(land, vertex_info(), edge_info(), geo::connect_fully(), geo::isotropy(), geo::mirror());

    std::cout << "Graph has " << graph.num_vertices() << " vertices, " << graph.num_edges() << " edges." << std::endl;

}
