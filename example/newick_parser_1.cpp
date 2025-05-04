#include "quetzal/quetzal.hpp"
#include <cassert>

// Define a namespace alias to shorten notation
namespace newick = quetzal::format::newick;

int main()
{
    std::string s1 = "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;";
    std::string s2 = "(,,(,));";

    // For default graph properties, leave <> empty
    auto [tree1, root1] = newick::to_k_ary_tree<>(s1);
    auto [tree2, root2] = newick::to_k_ary_tree<>(s2);

    std::cout << "These graphs are " << (tree1.is_isomorphic(tree2) ? "toootally" : "not") << " isomorphic!"
              << std::endl;

    return 0;
}
