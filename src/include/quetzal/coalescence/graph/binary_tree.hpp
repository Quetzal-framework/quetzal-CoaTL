// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_H_INCLUDED
#define BINARY_TREE_H_INCLUDED

#include "detail/tree_traits.hpp"
#include "detail/cardinal_k_ary_tree.hpp"

namespace quetzal
{
    /// @brief A binary tree class
    /// @remarks Inherits from boost::bidirectional_binary_tree, intends to hide the chaos
    template<class VertexProperty = boost::no_property, class EdgeProperty = boost::no_property>
    class binary_tree : 
        public boost::bidirectional_binary_tree<>
    {
        using base = boost::bidirectional_binary_tree<>;

        using vertex_hashmap_type = std::map<vertex_descriptor, VertexProperty>;
        using edge_hashmap_type   = std::map<edge_descriptor, EdgeProperty>;
        
        vertex_hashmap_type _vertex_property_hashmap;
        edge_hashmap_type _edge_property_hashmap;

        boost::associative_property_map< vertex_hashmap_type > _vertex_property_map { _vertex_property_hashmap };
        boost::associative_property_map< edge_hashmap_type > _edge_property_map { _edge_property_hashmap };

    public:

        /// @brief Inheriting constructors
        using base::base;

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        inline std::pair<edge_descriptor,edge_descriptor>
        add_children(vertex_descriptor parent, vertex_descriptor left, vertex_descriptor right)
        {
            assert(parent != left);
            assert(parent != right);
            assert(left != right);

            std::pair<vertex_descriptor,vertex_descriptor> left_edge = add_left_edge(parent, left);
            std::pair<vertex_descriptor,vertex_descriptor> right_edge = add_right_edge(parent, right);
            return {left_edge, right_edge};
        }

        // /// @brief Add a left edge to the parent
        // inline edge_descriptor add_left_edge(vertex_descriptor parent, vertex_descriptor child)
        // {
        //     assert(parent != child);
        //     return add_left_edge(parent, child, *this);
        // }

        // /// @brief Add a right edge to the parent
        // inline edge_descriptor add_right_edge(vertex_descriptor parent, vertex_descriptor child)
        // {
        //     assert(parent != child);
        //     return add_right_edge(parent, child, *this);
        // }

        /// @brief Find the root given a starting vertex
        /// @note  This function will be an infinite loop if called on a recurrent tree (which is not a tree any more).
        inline vertex_descriptor root(vertex_descriptor start) const
        {
            return root(start, *this);
        }

        /// @brief Return true if vertex u is the left successor of its parent
        inline bool is_left_successor(vertex_descriptor u) const
        {
            return is_left_successor(u, *this);
        }

        /// @brief Return true if vertex u is the right successor of its parent
        inline bool is_right_successor(vertex_descriptor u) const
        {
            return is_right_successor(u, *this);
        }

        /// @brief Return true if vertex u has a parent
        inline bool has_predecessor(vertex_descriptor u) const
        {
            return has_predecessor(u, *this);
        }

        /// @brief Get the parent of u
        inline vertex_descriptor predecessor(vertex_descriptor u)
        {
            return predecessor(u, *this);
        }

    };
}

#endif