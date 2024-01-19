#include "quetzal/quetzal.hpp"
#include <cassert>

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

    auto first_edges = tree.add_edges(a, {b, c});
    auto other_edges = tree.add_edges(c, {d, e, f});

    auto root = tree.find_root_from(e);

    assert(root == a && !tree.has_predecessor(root));
    assert(tree.degree(c) == 4);
    assert(tree.in_degree(c) == 1);
    assert(tree.out_degree(c) == 3);
    assert(tree.has_predecessor(root) == false);
    assert(tree.predecessor(c) == root);
    assert(tree.has_successors(root) == true);
    assert(std::ranges::none_of(tree.successors(c), [&](const auto &v) { return tree.has_successors(v); }));

    std::cout << "Degree of inner vertex c is " << tree.degree(c) << std::endl;
}
