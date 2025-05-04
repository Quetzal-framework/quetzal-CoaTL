#include "quetzal/quetzal.hpp"
#include <cassert>

// Define a namespace alias to shorten notation
namespace newick = quetzal::format::newick;

int main()
{
    // std::string s1 = "((1, ((2, (3, (4)Y#H1)g)e, (((Y#H1, 5)h, 6)f)X#H2)c)a, ((X#H2, 7)d, 8)b)r;";
    // std::string s2 = "((,((,(,()#H1)),(((#H1,),))#H2)),((#H2,),));";

    // // For default graph properties, leave <> empty
    // auto [network1, root1] = newick::extended::to_network<>(s1);
    // auto [network2, root2] = newick::extended::to_network<>(s2);

    // std::cout << "These graphs are "
    //           << (network1.is_isomorphic(network2) ? "toootally" : "not")
    //           << " isomorphic!" << std::endl;

    // return 0;
}
