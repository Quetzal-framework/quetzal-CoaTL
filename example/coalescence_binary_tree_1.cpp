#include "quetzal/quetzal.hpp"
#include <cassert>

int main()
{
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
  assert(tree.degree(c) == 3);
  assert(tree.in_degree(c) == 1);
  assert(tree.out_degree(c) == 2);
  assert(tree.has_predecessor(root) == false);
  assert(tree.predecessor(c) == root);
  assert(tree.has_successors(root) == true);
  assert( std::ranges::none_of( tree.successors(c), [&](const auto& v){ return tree.has_successors(v);} ));

  std::cout << "Degree of inner vertex c is " << tree.degree(c) << std::endl;

}
