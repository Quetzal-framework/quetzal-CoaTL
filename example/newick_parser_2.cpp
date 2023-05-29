#include "quetzal/quetzal.hpp"
#include <iostream>               

// Your custom little vertex class
struct vertex_info{ std::string name; };

// Your custom little edge class
struct edge_info{ double length; };

// Make your custom vertex streamable for the visitor
static inline auto& operator<<(std::ostream& os, vertex_info const& v) { return os << quoted(v.name); }

// we want to trigger a custom event when a vertex is discovered by the DFS
struct MyVisitor : boost::default_dfs_visitor {
    auto discover_vertex(auto u, auto const& graph) {
      // vertex_info g[u] is streamable!
      std::cout << "Discover vertex " << u << " (" << graph[u] << ")\n";
    }
};

// Define a namespace alias to shorten notation
namespace newick = quetzal::format::newick;

int main()
{
  std::string s = "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;";

  // Inject your custom littles classes here: the parser will generate these
  // structures and embed them into the graph.
  auto [tree, root] = newick::to_k_ary_tree<vertex_info, edge_info>(s);

  // depth-first traversal of the vertices in the directed graph
  MyVisitor visitor;
  tree.depth_first_search(root, visitor);

  return 0;
}
