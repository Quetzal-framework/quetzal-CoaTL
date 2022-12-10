#include "quetzal/io.hpp"          // parsers and generators
#include "quetzal/coalescence.hpp" // coalescence tree

#include <iostream>                // std::cout

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
    if (ok) std::cout << "\n\nAbstract Tree Syntax:\n\n" << ast_root << std::endl;

    // You can now iterate over the AST to populate a data structure of your choice
    // Here we illustrate the case with a Quetzal Tree

    // Vertices are simple strings, edges are simple double
    using tree_type = quetzal::coalescence::Tree<std::string, double>;

    // We initialize the Quetzal tree Graph
    tree_type tree;

    // We insert the root of tree
    auto root = tree.add_vertex(ast_root.name);

    // A hacky recursive lambda (C++23 will make this much simpler)
    static const auto populate = [](auto& graph, auto parent, const auto& ast){
      static auto recursive = [](auto& graph, auto parent, const auto& ast, auto& self) mutable -> void {
        // the idea is simple: while traversing the ast ...
        for(const auto& ast_child : ast.children){
          // add node to the quetzal tree graph
          auto child = graph.add_vertex(ast_child.name);
          // add directed edge from child to parent
          graph.add_edge(child, parent, ast_child.distance_to_parent);
          // and propagate to the next level
          self(graph, child, ast_child, self);}};
      recursive(graph, parent, ast, recursive);
    };

    // Now we simply call the recursive function to populate the tree from the ast
    populate(tree, root, ast_root);

    // Post-treatment, for example we can export the tree to a graphviz format
    std::cout << "\nGraphviz format:\n" << std::endl;
    tree.to_graphviz(std::cout);

  } // end loop over cases

  return 0;
}
