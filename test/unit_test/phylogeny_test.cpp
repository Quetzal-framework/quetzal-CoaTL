// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#define BOOST_TEST_MODULE coalescence_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/coalescence/container/Network.hpp>

BOOST_AUTO_TEST_SUITE(phylogenetics)

BOOST_AUTO_TEST_CASE(network)
{
    using quetzal::coalescence::container::Network;
    using std::string;
    /* Topology : Standardization of the Phylogenetic Network concept: https://doi.org/10.1186/1471-2105-9-532
     *           ... r ....
     *         /           b
     *        a           / \
     *       / \         d   \
     *      /  c        / \   \
     *     /  / \_____ X   \   \
     *    /  e          \   \   \
     *   /  / \          f   \  |
     *  /  /   g        / |  |  |
     * |  |   / \_ Y _ h  |  |  |
     * |  |  |     |   |  |  |  |
     * 1  2  3     4   5  6  7  8
     */
    Network<string> root("r");
    auto &a = root.add_child("a");
    a.add_child("1");
    auto &c = a.add_child("c");
    auto &e = c.add_child("e");
    e.add_child("2");
    auto &g = e.add_child("g");
    g.add_child("3");
    auto &y = g.add_child("Y");
    y.add_child("4");

    auto &b = root.add_child("b");
    b.add_child("8");
    auto &d = b.add_child("d");
    auto &x = d.add_child("X");
    c.add_child(x);
    auto &f = x.add_child("f");
    auto &h = f.add_child("h");
    h.add_child("5");
    h.add_parent(y);
    f.add_child("6");
    d.add_child("7");

    // Depth First Search allowing nodes to be revisited
    std::vector<string> expected = {"r", "a", "1", "c", "e", "2", "g", "3", "Y", "4",
                                    "X", "b", "8", "d", "X", "f", "h", "5", "6", "7"};
    std::vector<string> v;
    auto functor = [&v](string s) { v.push_back(s); };
    // root.visit_cells_by_pre_order_DFS(functor);
    // BOOST_TEST(v == expected);

    // root.to_newick();
}

BOOST_AUTO_TEST_SUITE_END()
