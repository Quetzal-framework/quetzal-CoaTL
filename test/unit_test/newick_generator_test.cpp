// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#include <gtest/gtest.h>

#include <concepts>
#include <quetzal/io/newick/from_tree_graph.hpp>
#include <quetzal/io/newick/generator.hpp>
#include <quetzal/io/newick/to_k_ary_tree.hpp>
#include <quetzal/utils/random_tree.hpp>
#include <random>

TEST(NewickGeneratorTest, ExternalTreeNoLabel)
{
    namespace newick = quetzal::format::newick;

    // Interfacing Quetzal generator with non-quetzal tree types
    std::predicate<Node> auto has_parent = [](const Node &n) { return n.parent != nullptr; };
    std::predicate<Node> auto has_children = [](const Node &n) { return n.left != nullptr && n.right != nullptr; };

    // We are not interested into formatting label or branch_length information
    newick::Formattable<Node> auto no_label = [](const Node &n) { return ""; };
    newick::Formattable<Node> auto no_branch_length = [](const Node &n) { return ""; };

    // We declare a generator passing it the interfaces
    auto generator = newick::generator(has_parent, has_children, no_label, no_branch_length);

    // We expose its interface to the user-defined class DFS algorithm
    a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

    // We retrieve the formatted string
    auto result = generator.take_result();
    EXPECT_EQ(result, "(,(,));");
}

TEST(NewickGeneratorTest, ExternalTreePrintLabel)
{
    namespace newick = quetzal::format::newick;

    // Interfacing Quetzal generator with non-quetzal tree types
    std::predicate<Node> auto has_parent = [](const Node &n) { return n.parent != nullptr; };
    std::predicate<Node> auto has_children = [](const Node &n) { return n.left != nullptr && n.right != nullptr; };

    // We are not interested into formatting the branch_length information
    newick::Formattable<Node> auto no_branch_length = [](const Node &n) { return ""; };

    // This time we want to record the data field
    newick::Formattable<Node> auto print_label = [](const Node &n) { return std::string(1, n.data); };

    // We configure a generator passing it the interfaces
    auto generator = newick::generator(has_parent, has_children, print_label, no_branch_length);

    // We expose its interface to the user-defined class DFS algorithm
    a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

    // We retrieve the formatted string
    auto result = generator.take_result();
    EXPECT_EQ(result, "(b,(d,e)c)a;");
}

TEST(NewickGeneratorTest, ExternalRandomizedLabels)
{
    namespace newick = quetzal::format::newick;

    // Interfacing Quetzal generator with non-quetzal tree types
    std::predicate<Node> auto has_parent = [](const Node &n) { return n.parent != nullptr; };
    std::predicate<Node> auto has_children = [](const Node &n) { return n.left != nullptr && n.right != nullptr; };

    // Get a seed for the random number engine
    std::random_device rd;
    // Standard mersenne_twister_engine seeded with rd()
    std::mt19937 rng(rd());
    // Arbitrary banch length distribution
    std::uniform_real_distribution<> dis(0.0, 2.0);

    // Random data is generated for the branch length
    newick::Formattable<Node> auto branch_length = [&rng, &dis](const Node &n) { return std::to_string(dis(rng)); };
    // More sophisticated label formatting
    newick::Formattable<Node> auto label = [](const Node &n) { return std::string(1, n.data) + "[my[comment]]"; };

    // Create the generator passing it the interfaces
    auto generator = newick::generator(has_parent, has_children, label, branch_length);

    // We expose its interface to the user-defined class DFS algorithm
    a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

    // We don't have an expectation because of the randomness, but you can always print it
    std::cout << generator.take_result() << std::endl;
}

TEST(NewickGeneratorTest, ExternalTreeAlignFormat)
{
    namespace newick = quetzal::format::newick;

    // Interfacing Quetzal generator with non-quetzal tree types
    std::predicate<Node> auto has_parent = [](const Node &n) { return n.parent != nullptr; };
    std::predicate<Node> auto has_children = [](const Node &n) { return n.left != nullptr && n.right != nullptr; };

    // Random data is generated for the branch length
    newick::Formattable<Node> auto branch_length = [](const Node &n) { return "0.1"; };
    // More sophisticated label formatting
    newick::Formattable<Node> auto label = [](const Node &n) { return std::string(1, n.data) + "[my[comment]]"; };

    // Writes a root node branch length with a value of 0.0 and disable/remove nested comments
    using flavor = quetzal::format::newick::TreeAlign;

    auto generator = newick::generator(has_parent, has_children, label, branch_length, flavor());
    a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

    // We retrieve the formatted string
    auto result = generator.take_result();
    EXPECT_EQ(result, "(b[my]:0.1,(d[my]:0.1,e[my]:0.1)c[my]:0.1)a[my]:0.0;");
    // Enables the use of nested comments
    // using flavor = quetzal::format::newick::PAUP;

    // TODO: Requires that an unrooted tree begin with a trifurcation & no nested comments
    // TODO: using flavor = quetzal::format::newick::PHYLIP;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

