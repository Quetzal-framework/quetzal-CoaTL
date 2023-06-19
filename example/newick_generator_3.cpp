#include "quetzal/quetzal.hpp"
#include <random>
#include <assert.h>

int main()
{
  // Declare a random number generator
  std::mt19937 rng{std::random_device{}()};

  // Generate a random tree with 5 leaves
  using quetzal::coalescence::get_random_k_ary_tree;
  auto [tree, root] = get_random_k_ary_tree<>(5, rng);

  // Chose a formatting standard
  using Flavor1 = quetzal::format::newick::TreeAlign;
  using Flavor2 = quetzal::format::newick::PAUP;
  using Flavor3 = quetzal::format::newick::PHYLIP;

  // Generate the newick string
  auto s1 = quetzal::format::newick::generate_from(tree, root, Flavor1());
  auto s2 = quetzal::format::newick::generate_from(tree, root, Flavor2());
  auto s3 = quetzal::format::newick::generate_from(tree, root, Flavor3());

  std::cout << s1 << "\n" << s2 << "\n" << s3 << std::endl;

  return 0;
}
