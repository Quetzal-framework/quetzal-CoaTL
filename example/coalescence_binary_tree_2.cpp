#include "quetzal/quetzal.hpp"
#include <cassert>

struct vertex_info
{
  std::string field1;
  int field2;
};

int main()
{
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

  // Let's attach information to the root vertex
  auto a = tree.add_vertex( vertex_info{ "Im Root", 47 });

  // Other vertices have default values
  auto b = tree.add_vertex( vertex_info{} );
  auto c = tree.add_vertex( vertex_info{} );
  auto d = tree.add_vertex( vertex_info{} );
  auto e = tree.add_vertex( vertex_info{} );

  // No information are attached to edges
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

  // Root vertex field values were assigned
  std::cout << "Root first field is:\t" << tree[a].field1 << std::endl;
  std::cout << "Root other field is:\t" << tree[a].field2 << std::endl;

  // Other vertices fields values were left default initialized
  assert(tree[b].field1.size() == tree[b].field2);
}
