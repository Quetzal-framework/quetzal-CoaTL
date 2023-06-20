#include "quetzal/quetzal.hpp"
#include "assert.h"

struct vertex_info
{
  std::string field1;
  int field2;
};

int main()
{
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

  // Let's attach information to the root vertex
  auto a = tree.add_vertex( vertex_info{"Im Root", 47} );

  // Other vertices have default values
  auto b = tree.add_vertex( vertex_info{} );
  auto c = tree.add_vertex( vertex_info{} );
  auto d = tree.add_vertex( vertex_info{} );
  auto e = tree.add_vertex( vertex_info{} );
  auto f = tree.add_vertex( vertex_info{} );

  // Attaches information to the edges from the root
  auto first_edges  = tree.add_edges( a, { {b, edge_info{1,2,3} }, {c, edge_info{4,5,6} } } );

  // Other edges have default values
  auto other_edges  = tree.add_edges( c, { {d, edge_info{}}, {e, edge_info{}} } );

  auto root = tree.find_root_from(e);

  assert(root == a && !tree.has_predecessor(root));
  assert(tree.degree(c) == 4);
  assert(tree.in_degree(c) == 1);
  assert(tree.out_degree(c) == 3);
  assert(tree.has_predecessor(root) == false);
  assert(tree.predecessor(c) == root);
  assert(tree.has_successors(root) == true);
  assert( ! std::ranges::none_of( tree.successors(c), [&t = std::as_const(tree)](auto v){ t.has_successors(v);} ));
  
  std::cout << "Degree of inner vertex c is " << tree.degree(c) << std::endl;

  // Retrieve root vertex information
  std::cout << "Root first field is:\t" << tree[a].field1 << std::endl;
  std::cout << "Root other field is:\t" << tree[a].field2 << std::endl;

  // Retrieve edges information
  std::cout << "Edge (a-b) values are:\t";
  std::copy(tree[first_edges[0]].cbegin(), tree[first_edges[0]].cend(), std::ostream_iterator<int>(std::cout, " "));
  
  std::cout << "\nEdge (a-c) values are:\t";
  std::copy(tree[first_edges[1]].cbegin(),tree[first_edges[1]].cend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  
  // Other edges values were left default initialized
  assert(tree[other_edges[0]].size() == 0);

}
