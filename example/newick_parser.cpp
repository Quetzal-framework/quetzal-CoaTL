#include "quetzal/io.hpp"
#include <iostream>

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
    // Initialize the abstract syntax tree
    newick::ast::node root;

    // parse the input into the AST
    auto ok = quetzal::parse(begin(input), end(input), newick::parser::tree, root);

    // Process the AST using its << operator
    std::cout << quoted(input) << "\t " << (ok ? "Parsed" : "Failed");
    if (ok) std::cout << "\n" << root << std::endl;

    // You can also manually explore the recursive AST
    // for (auto& child : root.children)
    //   std::cout << root.name << " <-> " << child.name << ":" << root.length << std::endl;
  }

  return 0;
}
