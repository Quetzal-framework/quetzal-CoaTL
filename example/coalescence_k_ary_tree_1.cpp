#include "quetzal/quetzal.hpp"
#include "assert.h"

int main()
{
  using vertex_info = boost::no_property;
  using edge_info = boost::no_property;
  using tree_type = quetzal::coalescence::k_ary_tree<vertex_info, edge_info>;

  /*
  *              a
  *            /   \
  *           /     c
  *          /    / | \
  *         b    d  e  f
  */

  tree_type tree;
  auto a = tree.add_vertex();
  auto b = tree.add_vertex();
  auto c = tree.add_vertex();
  auto d = tree.add_vertex();
  auto e = tree.add_vertex();
  auto f = tree.add_vertex();

  auto first_edges = tree.add_edges(a, {b, c} );
  auto other_edges = tree.add_edges(c, {d, e, f} );

  auto root = tree.find_root_from(e);
  assert(root == a && !tree.has_predecessor(root));

  std::cout << "Degree of inner vertex c is " << tree.degree(c) << std::endl;

}
