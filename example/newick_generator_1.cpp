#include "quetzal/io.hpp"
#include <random>
#include <assert.h>

int main()
{
  // Declare a random number generator
  std::mt19937 rng{std::random_device{}()};

  // Generate a random tree with 5 leaves
  using quetzal::coalescence::get_random_binary_tree;
  auto [tree, root] = get_random_binary_tree<>(5, rng);
  assert( degree(root, tree) == 2);

  // Generate the newick string
  using Flavor = quetzal::format::newick::TreeAlign;
  auto s = quetzal::format::newick::generate_from(tree, root, Flavor());

  std::cout << s << std::endl;

  return 0;
}
