#include "quetzal/coalescence.hpp"
#include <algorithm> // for std::ranges::copy
#include <cassert>
#include <iterator> // for std::ostream_iterator
#include <string>
#include <vector>

struct vertex_info
{
    std::string field1;
    int field2;
};

int main()
{
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

    // Let's attach information to the root vertex
    auto a = tree.add_vertex(vertex_info{"Im Root", 47});

    // Other vertices have default values
    auto b = tree.add_vertex(vertex_info{});
    auto c = tree.add_vertex(vertex_info{});
    auto d = tree.add_vertex(vertex_info{});
    auto e = tree.add_vertex(vertex_info{});

    auto [ab_edge, ac_edge] = tree.add_edges(a, {b, edge_info{1, 2, 3}}, {c, edge_info{4, 5, 6}});
    auto [cd_edge, ce_edge] = tree.add_edges(c, {d, edge_info{}}, {e, edge_info{}});

    auto root = tree.find_root_from(e);

    assert(root == a && !tree.has_predecessor(root));
    assert(root == a && !tree.has_predecessor(root));
    assert(tree.degree(c) == 3);
    assert(tree.in_degree(c) == 1);
    assert(tree.out_degree(c) == 2);
    assert(tree.has_predecessor(root) == false);
    assert(tree.predecessor(c) == root);
    assert(tree.has_successors(root) == true);
    for (auto v : tree.successors(c)) {
        assert(! tree.has_successors(v));
    }
    std::cout << "Degree of inner vertex c is " << tree.degree(c) << std::endl;

    // Root vertex field values were assigned at construction
    std::cout << "Root first field is:\t" << tree[a].field1 << std::endl;
    std::cout << "Root other field is:\t" << tree[a].field2 << std::endl;

    // Other vertices fields values were left default initialized
    assert(tree[b].field1.size() == tree[b].field2);

    // Edges from the root were assigned at construction
    std::cout << "Edge (a-b) values are:\t";
    std::copy(tree[ab_edge].cbegin(), tree[ab_edge].cend(), std::ostream_iterator<int>(std::cout, " "));

    std::cout << "\nEdge (a-c) values are:\t";
    std::copy(tree[ac_edge].cbegin(), tree[ac_edge].cend(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    // Other edges values were left default initialized
    assert(tree[cd_edge].size() == 0);
}
