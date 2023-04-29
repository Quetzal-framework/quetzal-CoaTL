#include "quetzal/quetzal.hpp"
#include "assert.h"

struct VertexInfo
{
  std::string field1;
  int field2;
};

int main()
{
  using edge_info = boost::no_property;
  using tree_type = quetzal::coalescence::binary_tree<VertexInfo, edge_info>;

  /*
  *             a
  *           /   \
  *          /     c
  *         /     / \
  *        b     d   e
  */

  tree_type tree;
  auto a = tree.add_vertex("Im Root", 47);
  auto b = tree.add_vertex();
  auto c = tree.add_vertex();
  auto d = tree.add_vertex();
  auto e = tree.add_vertex();

  auto [ab_edge, ac_edge] = tree.add_edges(a, b, c);
  auto [cd_edge, ce_edge] = tree.add_edges(c, d, e);

  auto root = tree.find_root_from(e);
  assert(root == a && !tree.has_predecessor(root));

  std::cout << "Degree of inner vertex c is " << tree.degree(c) << std::endl;

  // Root vertex field values were assigned
  std::cout << "Root vertex first field is:\t" << tree[root].field1 << std::endl;
  std::cout << "Root other field is:\t"<< tree[a].field2 << std::endl;

  // Other vertices fields values were left default initialized
  assert(tree[b].field1.size() == tree[b].field2);
}
