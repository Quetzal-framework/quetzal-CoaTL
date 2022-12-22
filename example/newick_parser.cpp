#include "quetzal/io.hpp"          // parsers and generators
#include "quetzal/coalescence.hpp" // coalescence tree
#include "quetzal/utils.hpp"       // to_tree
#include <iostream>                // std::cout

// Define a namespace alias to shorten notation
namespace newick = quetzal::format::newick;

int main()
{
  std::string s = "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;";

  // We define a shorter type alias
  using ast_type = newick::ast::node;

  // We initialize the root of Abstract Syntax Tree (AST)
  ast_type ast;

  // We parse the input string into the AST
  auto ok = quetzal::parse(begin(s), end(s), newick::parser::tree, ast);

  // We check by printing the AST if the string was successfully parsed
  std::cout << quoted(s) << "\t " << (ok ? "Parsed" : "Failed");
  if (ok) std::cout << "\n\nAbstract Tree Syntax:\n\n" << ast << std::endl;

  // For a Quetzal tree graph, helper functions have been defined
  auto tree = quetzal::utils::to_k_ary_tree<>(ast);

  // Post-treatment, for example export the tree to a graphviz format
  std::cout << "\nGraphviz format:\n" << std::endl;
  tree.to_graphviz(std::cout);

  return 0;
}
