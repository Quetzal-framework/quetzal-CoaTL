#include "quetzal/quetzal.hpp"
#include "assert.h"
#include <vector>
#include <iterator> // for std::ostream_iterator
#include <algorithm> // for std::copy

int main()
{
  using vertex_info = boost::no_property;
  using edge_info = std::vector<int>;
  using tree_type = quetzal::coalescence::binary_tree<vertex_info, edge_info>;

  /*
  *             a
  *           /   \
  *          /     c
  *         /     / \
  *        b     d   e
  */

  tree_type tree;
  auto a = tree.add_vertex();
  auto b = tree.add_vertex();
  auto c = tree.add_vertex();
  auto d = tree.add_vertex();
  auto e = tree.add_vertex();

  auto [ab_edge, ac_edge] = tree.add_edges(a, std::make_tuple(b, 1,2,3 ), std::make_tuple(c, 4,5,6));
  auto [cd_edge, ce_edge] = tree.add_edges(c, {d}, {e});

  auto root = tree.find_root_from(e);
  assert(root == a && !tree.has_predecessor(root));

  std::cout << "Degree of inner vertex c is " << tree.degree(c) << std::endl;

  // Edges from the root were assigned at construction
  std::cout << "Edge (a-b) values are:\t";
  std::copy(tree[ab_edge].cbegin(),tree[ab_edge].cend(), std::ostream_iterator<int>(std::cout, " "));

  std::cout << "\nEdge (a-c) values are:\t";
  std::copy(tree[ac_edge].cbegin(),tree[ac_edge].cend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  
  // Other edges values were left default initialized
  assert(tree[cd_edge].values.size() == 0);
}
