#include "quetzal/io.hpp"          // parsers and generators
#include "quetzal/coalescence.hpp" // Tree data structure
#include <iostream>                // std::cout
#include <functional>              // std::function
#include <utility>                 // std::pair

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

    // We declare a recursive function object to update the Tree reading from the AST
    std::function<void(tree_type&, const ast_type&)> populate =
    [&](auto& tree, const auto& ast){
      for(const auto& ast_child : ast.children){
        auto subtree = tree.add_child({ast_child.name, ast_child.distance_to_parent});
        populate(subtree, ast_child);
      }
    };

    // Now we simply call the recursive function to populate from the root
    populate(tree_root, ast_root);
  }

  return 0;
}
