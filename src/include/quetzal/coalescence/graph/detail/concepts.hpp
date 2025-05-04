//=======================================================================
// Copyright 2018 Jeremy William Murphy
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/////////////////////////////////////////////////////////////////////////// *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef BOOST_GRAPH_DETAIL_CONCEPTS_TREE
#define BOOST_GRAPH_DETAIL_CONCEPTS_TREE

#include <boost/concept/assert.hpp>
#include <boost/concept/detail/concept_def.hpp>
#include <boost/concept_check.hpp>
#include <boost/graph/graph_concepts.hpp>

namespace boost::concepts
{

BOOST_concept(ForwardBinaryTree, (G)) : Graph<G>
{
    BOOST_CONCEPT_USAGE(ForwardBinaryTree)
    {
        t = has_left_successor(u, g);
        t = has_right_successor(u, g);
        v = left_successor(u, g);
        v = right_successor(u, g);
        t = empty(u, g);
        const_constraints(g);
    }
    void const_constraints(G const &g)
    {
        t = has_left_successor(u, g);
        t = has_right_successor(u, g);
        v = left_successor(u, g);
        v = right_successor(u, g);
        t = empty(u, g);
    }
    bool t;
    G g;
    typename graph_traits<G>::vertex_descriptor u, v;
};

BOOST_concept(BidirectionalBinaryTree, (G)) : ForwardBinaryTree<G>
{
    BOOST_CONCEPT_USAGE(BidirectionalBinaryTree)
    {
        t = has_predecessor(u, g);
        t = predecessor(u, g);
        u = root(u, g);
        const_constraints(g);
    }

    void const_constraints(G const &g)
    {
        t = has_predecessor(u, g);
        t = predecessor(u, g);
        u = root(u, g);
    }

    bool t;
    G g;
    typename graph_traits<G>::vertex_descriptor u;
};

BOOST_concept(MutableForwardBinaryTree, (G)) : ForwardBinaryTree<G>
{
    BOOST_CONCEPT_USAGE(MutableForwardBinaryTree)
    {
        e = add_left_edge(u, v, g);
        e = add_right_edge(u, v, g);
        // TODO: remove_left_edge, remove_right_edge
    }
    G g;
    typename graph_traits<G>::vertex_descriptor u, v;
    typename graph_traits<G>::edge_descriptor e;
};

BOOST_concept(MutableBidirectionalBinaryTree, (G)) : MutableForwardBinaryTree<G>
{
    BOOST_CONCEPT_USAGE(MutableBidirectionalBinaryTree)
    {
        e = add_predecessor(u, v, g);
        remove_predecessor(u, g);
    }
    G g;
    typename graph_traits<G>::vertex_descriptor u, v;
    typename graph_traits<G>::edge_descriptor e;
};

} // namespace boost::concepts

#endif