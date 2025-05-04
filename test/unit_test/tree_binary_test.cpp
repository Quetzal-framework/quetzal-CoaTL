#include <gtest/gtest.h>

#include <random>

#include <quetzal/coalescence/graph/binary_tree.hpp>

namespace quetzal::coalescence
{
TEST(binary_tree, no_property_binary_interface)
{
    // default tree with no edge/vertex properties attached
    using tree_type = quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>;
    using vertex_descriptor = tree_type::vertex_descriptor;

    tree_type tree;

    vertex_descriptor a = tree.add_vertex();
    vertex_descriptor b = tree.add_vertex();
    vertex_descriptor c = tree.add_vertex();
    vertex_descriptor d = tree.add_vertex();
    vertex_descriptor e = tree.add_vertex();

    auto [ab_edge, ac_edge] = tree.add_edges(a, b, c);
    auto [cd_edge, ce_edge] = tree.add_edges(c, d, e);

    auto root = tree.find_root_from(e);
    ASSERT_EQ(root, a);
    ASSERT_EQ(tree.degree(c), 3);
    ASSERT_EQ(tree.in_degree(c), 1);
    ASSERT_EQ(tree.out_degree(c), 2);
    ASSERT_EQ(tree.has_predecessor(root), false);
    ASSERT_EQ(tree.predecessor(c), root);
    ASSERT_EQ(tree.has_successors(root), true);
    auto have_children = [&tree = std::as_const(tree)](auto v) { return tree.has_successors(v); };
    ASSERT_FALSE(ranges::all_of(tree.successors(c), have_children));

    struct tree_visitor
    {
        void operator()(boost::visit order, vertex_descriptor v)
        {
            switch (order)
            {
            case boost::visit::pre:
                std::cout << "Pre " << v << std::endl;
                break;
            case boost::visit::in:
                std::cout << "In " << v << std::endl;
                break;
            case boost::visit::post:
                std::cout << "Post " << v << std::endl;
                break;
            default:
                std::cout << "Oops" << std::endl;
            }
        }
    } visitor;
    tree.depth_first_search(a, visitor);
}

TEST(binary_tree, simple_vertex_properties)
{
    // tree with string vertices but no edge properties
    using vertex_properties = std::string;
    using tree_type = quetzal::coalescence::binary_tree<vertex_properties, boost::no_property>;
    using vertex_descriptor = tree_type::vertex_descriptor;

    tree_type tree;

    vertex_descriptor a = tree.add_vertex("a");
    vertex_descriptor b = tree.add_vertex("b");
    vertex_descriptor c = tree.add_vertex("c");
    vertex_descriptor d = tree.add_vertex("d");
    vertex_descriptor e = tree.add_vertex("e");

    // Read
    ASSERT_EQ(tree[a], "a");

    // Write
    tree[a] = tree[b];
    ASSERT_EQ(tree[a], "b");
}

TEST(binary_tree, structure_vertex_properties)
{
    // tree with string & int vertices but no edge properties
    struct vertex_info
    {
        std::string whatever;
        int othervalue;
    };
    using tree_type = quetzal::coalescence::binary_tree<vertex_info, boost::no_property>;
    using vertex_descriptor = tree_type::vertex_descriptor;

    tree_type tree;

    vertex_descriptor a = tree.add_vertex(vertex_info{"a", 0});
    vertex_descriptor b = tree.add_vertex(vertex_info{"b", 1});
    vertex_descriptor c = tree.add_vertex(vertex_info{"c", 2});
    vertex_descriptor d = tree.add_vertex(vertex_info{"d", 3});
    vertex_descriptor e = tree.add_vertex(vertex_info{"e", 4});

    // Read
    ASSERT_EQ(tree[e].whatever, "e");
    ASSERT_EQ(tree[e].othervalue, 4);

    // Write
    tree[e] = {"new", 5};

    ASSERT_EQ(tree[e].whatever, "new");
    ASSERT_EQ(tree[e].othervalue, 5);
}

TEST(binary_tree, simple_edge_properties)
{
    // tree with string edges but no vertex properties attached
    using vertex_properties = boost::no_property;
    using edge_properties = std::string;
    using tree_type = quetzal::coalescence::binary_tree<vertex_properties, edge_properties>;
    using vertex_descriptor = tree_type::vertex_descriptor;

    tree_type tree;

    vertex_descriptor a = tree.add_vertex();
    vertex_descriptor b = tree.add_vertex();
    vertex_descriptor c = tree.add_vertex();
    vertex_descriptor d = tree.add_vertex();
    vertex_descriptor e = tree.add_vertex();

    // Add two edges with two different edge properties
    auto [ab_edge, ac_edge] = tree.add_edges(a, {b, edge_properties{"a->b"}}, {c, edge_properties{"a->c"}});
    auto [cd_edge, ce_edge] =
        tree.add_edges(ac_edge.second, {d, edge_properties{"c->d"}}, {e, edge_properties{"c->e"}});

    tree[ab_edge] = "a...b";
    ASSERT_EQ(tree[ab_edge], "a...b");
}

TEST(binary_tree, struct_edge_properties)
{
    // default tree with no edge/vertex properties attached
    using vertex_properties = boost::no_property;
    struct edge_info
    {
        std::string whatever;
        int othervalue;
    };
    using tree_type = quetzal::coalescence::binary_tree<vertex_properties, edge_info>;
    using vertex_descriptor = tree_type::vertex_descriptor;

    tree_type tree;

    // No property on vertices
    vertex_descriptor a = tree.add_vertex();
    vertex_descriptor b = tree.add_vertex();
    vertex_descriptor c = tree.add_vertex();
    vertex_descriptor d = tree.add_vertex();
    vertex_descriptor e = tree.add_vertex();

    // Pass info to build new edges
    auto [ab_edge, ac_edge] = tree.add_edges(a, {b, edge_info{"a->b", 10}}, {c, edge_info{"a->c", 11}});
    auto [cd_edge, ce_edge] =
        tree.add_edges(ac_edge.second, {d, edge_info{"c->d", 12}}, {e, edge_info{"c->e", 13}});

    // Read edge info
    ASSERT_EQ(tree[ab_edge].whatever, "a->b");
    ASSERT_EQ(tree[ab_edge].othervalue, 10);

    // Write edge info
    tree[ab_edge] = {"yolo", 99};
    ASSERT_EQ(tree[ab_edge].whatever, "yolo");
    ASSERT_EQ(tree[ab_edge].othervalue, 99);
}

TEST(binary_tree, struct_both_properties)
{
    // default tree with no edge/vertex properties attached
    struct vertex_info
    {
        std::string whatever;
        int othervalue;
    };
    struct edge_info
    {
        std::string whatever;
        int othervalue;
    };
    using tree_type = quetzal::coalescence::binary_tree<vertex_info, edge_info>;
    using vertex_descriptor = tree_type::vertex_descriptor;

    tree_type tree;

    vertex_descriptor a = tree.add_vertex(vertex_info{"a", 0});
    vertex_descriptor b = tree.add_vertex(vertex_info{"b", 1});
    vertex_descriptor c = tree.add_vertex(vertex_info{"c", 2});
    vertex_descriptor d = tree.add_vertex(vertex_info{"d", 3});
    vertex_descriptor e = tree.add_vertex(vertex_info{"e", 4});

    // Pass info to build new edges
    auto [ab_edge, ac_edge] = tree.add_edges(a, {b, edge_info{"a->b", 10}}, {c, edge_info{"a->c", 11}});
    auto [cd_edge, ce_edge] = tree.add_edges(ac_edge.second, {d, edge_info{"c->d", 12}}, {e, edge_info{"c->e", 13}});

    // Read vertices
    ASSERT_EQ(tree[e].whatever, "e");
    ASSERT_EQ(tree[e].othervalue, 4);

    // Write vertices
    tree[e] = {"new", 5};
    ASSERT_EQ(tree[e].whatever, "new");
    ASSERT_EQ(tree[e].othervalue, 5);

    // Read edge info
    ASSERT_EQ(tree[ab_edge].whatever, "a->b");
    ASSERT_EQ(tree[ab_edge].othervalue, 10);

    // Write edge info
    tree[ab_edge] = {"yolo", 99};
    ASSERT_EQ(tree[ab_edge].whatever, "yolo");
    ASSERT_EQ(tree[ab_edge].othervalue, 99);
}

TEST(binary_tree, random_binary_tree)
{
    std::random_device rd;

    std::mt19937 gen(rd());

    int nb_leaves = 5;

    auto [tree1, root1] = quetzal::coalescence::get_random_binary_tree(nb_leaves, gen);
    auto [tree2, root2] = quetzal::coalescence::get_random_binary_tree(nb_leaves, gen);

    ASSERT_EQ(tree1.has_predecessor(root1), false);
    ASSERT_EQ(tree1.degree(root1), 2);

    struct tree_visitor
    {
        void operator()(boost::visit order, vertex_descriptor v)
        {
            switch (order)
            {
            case boost::visit::pre:
                std::cout << "Pre " << v << std::endl;
                break;
            case boost::visit::in:
                std::cout << "In " << v << std::endl;
                break;
            case boost::visit::post:

