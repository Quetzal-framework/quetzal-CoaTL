#include "quetzal/quetzal.hpp"
#include <iostream> // std::cout
#include <memory>   // std::unique_ptr

// Assume this binary tree class pervades your code
struct MyNode
{
    std::string id;                     // the vertex name
    double length;                      // the edge length
    std::unique_ptr<MyNode> left_child; // never use bare pointers in interfaces
    std::unique_ptr<MyNode> right_child;
};

// Define a namespace alias to shorten notation
namespace newick = quetzal::format::newick;

int main()
{
    std::string s = "(A:0.1,(C:0.3,D:0.4)B:0.5)E;";

    // We define a shorter type alias
    using ast_type = newick::ast::node;

    // We initialize the root of Abstract Syntax Tree (AST)
    ast_type ast;

    // We parse the input string into the AST
    auto ok = quetzal::parse(begin(s), end(s), newick::parser::tree, ast);

    // We check by printing the AST if the string was successfully parsed
    std::cout << quoted(s) << "\t " << (ok ? "Parsed" : "Failed");
    if (ok)
        std::cout << "\n\nAbstract Tree Syntax:\n\n" << ast << std::endl;

    // Let's use a recursive lambda until C++23 makes this much simpler
    static const auto populate = [](auto &tree_root, const auto &ast_root) {
        static auto recurse = [](auto &subtree, const auto &ast, auto &self) mutable -> void {
            // the idea is simple: traverse the AST ...
            if (ast.children.size() == 2)
            {
                // ... by instantiating the children and propagating to the next subtree
                subtree.left_child = std::unique_ptr<MyNode>(new MyNode{ast.name, ast.distance_to_parent});
                self(*subtree.left_child, ast.children.front(), self);

                subtree.right_child = std::unique_ptr<MyNode>(new MyNode{ast.name, ast.distance_to_parent});
                self(*subtree.right_child, ast.children.back(), self);
            }
        };
        recurse(tree_root, ast_root, recurse);
    };

    // Initialize your root using the AST data members
    MyNode my_root{.id = ast.name, .length = ast.distance_to_parent};

    // And then call the recursion to populate the children
    populate(my_root, ast);

    std::cout << (my_root.left_child == nullptr ? "Failed" : "Populated") << std::endl;
    std::cout << (my_root.right_child == nullptr ? "Failed" : "Populated") << std::endl;

    return 0;
}
