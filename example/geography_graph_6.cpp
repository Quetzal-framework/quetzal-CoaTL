#include "quetzal/geography.hpp"

namespace geo = quetzal::geography;

int main()
{
    using vertex_info = geo::no_property;
    using edge_info = geo::no_property;
    using graph_type = geo::graph<vertex_info, edge_info, geo::dense, geo::anisotropy>;

    int num_vertices = 100;
    graph_type graph( num_vertices );

    graph.add_edge( 0, 1 );
    graph.add_edge( 0, 2 );
    graph.add_edge( 0, 3 );
    
    std::cout << "Graph has " << graph.num_vertices() << " vertices, " << graph.num_edges() << " edges." << std::endl;

}
