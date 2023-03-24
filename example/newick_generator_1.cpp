#include "quetzal/utils/random_tree.hpp"
#include "quetzal/io.hpp"

int main()
{
  // Declare a random number generator
  std::mt19937 rng{std::random_device{}()};

  // Generate a random tree from a graph V(10,30)
  auto [tree,root] = quetzal::get_random_binary_tree<>(5, rng);
  assert( degree(root, tree) == 2);

  // boost::bidirectional_binary_tree<> tree2;
  // auto a = add_vertex(tree2);
  // auto b = add_vertex(tree2);
  // auto c = add_vertex(tree2);

  // add_edge(a, b, tree2);
  // add_edge(a, c, tree2);

  // // Generate the newick string
  // using Flavor = quetzal::format::newick::TreeAlign;
  // auto s = quetzal::format::newick::generate_from(tree2, Flavor());

  // std::cout << s << std::endl;

  return 0;
}
