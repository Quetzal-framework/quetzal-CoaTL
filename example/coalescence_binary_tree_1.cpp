#include "quetzal/quetzal.hpp"
#include "assert.h"

int main()
{
  // no edge/vertex properties attached
  using vertex_info = boost::no_property;
  using edge_info = boost::no_property;
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

  auto [ab_edge, ac_edge] = tree.add_edges(a, b, c);
  auto [cd_edge, ce_edge] = tree.add_edges(c, d, e);

  auto root = tree.find_root_from(e);
  assert(root == a && !tree.has_predecessor(root));

  std::cout << "Degree of inner vertex c is " << tree.degree(c) << std::endl;

}
