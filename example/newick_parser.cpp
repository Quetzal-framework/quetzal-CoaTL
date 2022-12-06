#include "quetzal/io.hpp"          // parsers and generators
#include <iostream>                // std::cout
#include <functional>              // std::function
#include <utility>                 // std::pair

// BGL
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/breadth_first_search.hpp>

namespace quetzal::coalescence
{
  ///
  /// @brief Defines the desired properties and constraints for a coalescent graph
  ///
  struct tree_traits
  {
    ///
    /// @brief Trees are sparse graph in nature, adjacency_matrix would not be justified here
    ///
    template<class... Types>
    using implementation = boost::adjacency_list<Types...>;
    ///
    /// @brief We want to enforce avoiding multi-graphs (edges with same end nodes)
    ///
    using out_edge_list_type = boost::setS;
    ///
    /// @brief We are prioritizing speed for space
    ///
    using vertex_list_type = boost::listS;
    ///
    /// @brief Since the graph is not a multigraph then \f$(u,v)\f$ and \f$(v,u)\f$ are the same edge.
    /// @see https://www.boost.org/doc/libs/1_80_0/libs/graph/doc/graph_concepts.html#sec:undirected-graphs
    ///
    using directed_type = boost::undirectedS ;
  }; // end tree_traits

  ///
  /// @brief A graph with properties suited to represent a coalescent tree
  ///
  template<class VertexProperties=boost::no_property, class EdgeProperties=boost::no_property>
  class Tree :
  tree_traits::implementation
  <
    tree_traits::out_edge_list_type,
    tree_traits::vertex_list_type,
    tree_traits::directed_type,
    VertexProperties,
    EdgeProperties
  >
  {
    ///
    /// @brief Properties of an edge, like the demes visited
    /// @see https://www.boost.org/doc/libs/1_80_0/libs/graph/doc/bundles.html
    ///
    using edge_properties = EdgeProperties;
    ///
    /// @brief Properties of a vertex, like the mutational state
    /// @see https://www.boost.org/doc/libs/1_80_0/libs/graph/doc/bundles.html
    ///
    using vertex_properties = VertexProperties;
  };
}

// Define a namespace alias to shorten notation
namespace newick = quetzal::format::newick;

int main()
{
  // A bunch of Newick strings inputs
  std::vector<std::string> cases =
  {
    "(,,(,));",
    "(A,B,(C,D)E)F;",
    "(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);",
    "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;",
  };

  // Loop over all Newick strings cases
  for(const auto& input : cases)
  {
    // We define a shorter type alias
    using ast_type = newick::ast::node;

    // We initialize the root of Abstract Syntax Tree (AST)
    ast_type ast_root;

    // We parse the input string into the AST
    auto ok = quetzal::parse(begin(input), end(input), newick::parser::tree, ast_root);

    // We check by printing the AST if the string was successfully parsed
    std::cout << quoted(input) << "\t " << (ok ? "Parsed" : "Failed");
    if (ok) std::cout << "\n" << ast_root << std::endl;

    // You can now iterate over the AST to populate a data structure of your choice

    // Here we illustrate the case with a Quetzal Tree
    using quetzal::coalescence::container::Tree;
    // each Tree node is defined to contain a cell with 2 fields (name and branch length)
    using cell_type = std::pair<std::string, double>;
    // We define another short type alias
    using tree_type = Tree<cell_type>;

    // We initialize the root of the Quetzal Tree
    tree_type tree_root({ast_root.name, ast_root.distance_to_parent});

    // A hacky recursive lambda (C++23 will make this much simpler)
    static const auto populate = [](tree_type& tree, const ast_type& ast){
      static auto recursive = [](auto& tree, const auto& ast, auto& self) mutable -> void {
        // the idea is simple: while traversing the ast ...
        for(const auto& ast_child : ast.children){
          // ... add nodes to the quetzal tree container
          auto subtree = tree.add_child({ast_child.name, ast_child.distance_to_parent});
          // and propagate to the next level
          self(subtree, ast_child, self);}};
      recursive(tree, ast, recursive);
    };

    // Now we simply call the recursive function to populate the tree from the ast
    populate(tree_root, ast_root);

    // Post treatment, for example visiting the leaves
    auto printer = [](const cell_type& p){ std::cout << std::quoted(p.first) << " " ; };
    tree_root.visit_cells_by_pre_order_DFS(printer);
    std::cout << std::endl;



    //typedef property_map< Graph, vertex_name_t >::type actor_name_map_t;
    // actor_name_map_t actor_name = get(vertex_name, g);
    // typedef property_map< Graph, edge_name_t >::type movie_name_map_t;
    // movie_name_map_t connecting_movie = get(edge_name, g);
    //
    // typedef graph_traits< Graph >::vertex_descriptor Vertex;
    // typedef std::map< std::string, Vertex > NameVertexMap;
    // NameVertexMap actors;

  }

  return 0;
}
