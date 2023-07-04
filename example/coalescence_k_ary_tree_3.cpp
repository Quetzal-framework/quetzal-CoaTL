#include "quetzal/quetzal.hpp"
#include <cassert>

int main()
{
  using vertex_info = boost::no_property;
  using edge_info = std::vector<int>;
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

  auto first_edges  = tree.add_edges( a, { {b, edge_info{1,2,3} }, {c, edge_info{4,5,6} } } );
  auto other_edges  = tree.add_edges( c, { {d, edge_info{}}, {e, edge_info{}} } );

  auto root = tree.find_root_from(e);

  assert(root == a && !tree.has_predecessor(root));
  assert(root == a && !tree.has_predecessor(root));
  assert(tree.degree(c) == 4);
  assert(tree.in_degree(c) == 1);
  assert(tree.out_degree(c) == 3);
  assert(tree.has_predecessor(root) == false);
  assert(tree.predecessor(c) == root);
  assert(tree.has_successors(root) == true);
  assert( ! std::ranges::none_of( tree.successors(c), [&t = std::as_const(tree)](auto v){ t.has_successors(v);} ));
  
  std::cout << "Degree of inner vertex c is " << tree.degree(c) << std::endl;

  // Edges from the root were assigned at construction
  std::cout << "Edge (a-b) values are:\t";
  std::copy(tree[first_edges[0]].cbegin(), tree[first_edges[0]].cend(), std::ostream_iterator<int>(std::cout, " "));

  std::cout << "\nEdge (a-c) values are:\t";
  std::copy(tree[first_edges[1]].cbegin(),tree[first_edges[1]].cend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  
  // Other edges values were left default initialized
  assert(tree[other_edges[0]].size() == 0);
}
