#include "quetzal/io.hpp"
#include <random>
#include <assert.h>

// Declare a global random number generator
std::mt19937 rng{ std::random_device{}() };

int main()
{

  struct vertex_info
  { 
    std::string data;
    // constructor to randomly initialize the data
    vertex_info(){ data = std::uniform_int_distribution<int>(0, 25)(rng) + 'A'; }
    // access point to the label by the formatter
    std::string label() const { return data; } 
  };

  struct edge_info
  { 
    double data;
    edge_info(){ data = std::uniform_int_distribution<int>(0, 10)(rng); }
    std::string label() const { return std::to_string(data); }
  };

  // Generate a random tree with 5 leaves
  using quetzal::coalescence::get_random_binary_tree;
  auto [tree, root] = get_random_binary_tree<vertex_info, edge_info>(5, rng);

  // Generate the newick string
  using Flavor = quetzal::format::newick::TreeAlign;
  auto s = quetzal::format::newick::generate_from(tree, root, Flavor());

  std::cout << s << std::endl;

  return 0;
}
