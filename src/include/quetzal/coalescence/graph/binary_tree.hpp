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
#include <utility>

namespace quetzal
{
    namespace detail
    {
        template <typename T1, typename... Ts>
        std::tuple<Ts...> unshift_tuple(const std::tuple<T1, Ts...>& tuple)
        {
            return std::apply([](auto&&, const auto&... args) {return std::tie(args...);}, tuple);
        }

        template<typename Graph, typename VertexProperty>
        struct VertexManager
        {
            using vertex_descriptor = typename Graph::vertex_descriptor;
            using vertex_hashmap_type = std::map<vertex_descriptor, VertexProperty>;
            vertex_hashmap_type _vertex_property_hashmap;
            boost::associative_property_map< vertex_hashmap_type > _vertex_property_map { _vertex_property_hashmap };

            template<typename... Args>
            vertex_descriptor add_vertex(Graph &g, Args&&... args)
            {
                vertex_descriptor key = g.add_vertex();
                VertexProperty value = { std::forward<Args>(args)...};
                put(_vertex_property_map, key, value);
                return key;
            }
        };

        template<typename Graph, typename EdgeProperty>
        struct EdgeManager
        {
            using vertex_descriptor = typename Graph::vertex_descriptor;
            using edge_descriptor = typename Graph::edge_descriptor;
            using edge_hashmap_type   = std::map<edge_descriptor, EdgeProperty>;
            edge_hashmap_type _edge_property_hashmap;
            boost::associative_property_map< edge_hashmap_type > _edge_property_map { _edge_property_hashmap };

            template<typename... Args>
            std::pair<edge_descriptor,edge_descriptor>
            add_children(Graph &g,
                        vertex_descriptor parent, 
                        std::tuple<vertex_descriptor, Args...> left, 
                        std::tuple<vertex_descriptor, Args...> right)
            {
                assert(parent != get<0>(left));
                assert(parent != get<0>(right));
                assert(get<0>(left) != get<0>(right));

                std::pair<vertex_descriptor,vertex_descriptor> left_edge = g.add_left_edge(parent, get<0>(left));
                std::pair<vertex_descriptor,vertex_descriptor> right_edge = g.add_right_edge(parent, get<0>(right));

                auto p1 = std::apply([](Args&&... ts){ return EdgeProperty(std::forward<Args>(ts)...);}, detail::unshift_tuple(left));
                auto p2 = std::apply([](Args&&... ts){ return EdgeProperty(std::forward<Args>(ts)...);}, detail::unshift_tuple(right));

                put(_edge_property_map, left_edge, p1);
                put(_edge_property_map, right_edge, p2);

                return {left_edge, right_edge};
            }
        };

    }

    /// @brief A binary tree class
    /// @remarks Primary template for partial specialization
    template<class VertexProperty, class EdgeProperty>
    class binary_tree {};

    /// @brief A binary tree class 
    /// @remarks Explicit (full) specialization where there is no property attached to either vertices nor edges.
    template<>
    class binary_tree<boost::no_property, boost::no_property> : 
    public boost::bidirectional_binary_tree<>
    {
        private:

        using self_type = binary_tree<boost::no_property, boost::no_property>;

        using base = boost::bidirectional_binary_tree<>;
        
        public:

        /// @brief Inheriting constructors
        using base::base;

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        /// @brief Add a vertex to the graph
        friend 
        vertex_descriptor add_vertex(self_type &g)
        {
            return g.add_vertex();
        }

        /// @brief Add edges between the parent vertex and the two children.
        friend 
        std::pair<edge_descriptor,edge_descriptor> 
        add_children(
            self_type &g,
            vertex_descriptor parent, 
            vertex_descriptor left, 
            vertex_descriptor right)
        {
            assert(parent != left);
            assert(parent != right);
            assert(left != right);

            std::pair<vertex_descriptor,vertex_descriptor> left_edge = g.add_left_edge(parent, left);
            std::pair<vertex_descriptor,vertex_descriptor> right_edge = g.add_right_edge(parent, right);

            return {left_edge, right_edge};
        }

    };

    /// @brief A binary tree class 
    /// @remarks Partial specialization where there is no edge property attached
    template<class VertexProperty>
    class binary_tree<VertexProperty, boost::no_property> :
    public boost::bidirectional_binary_tree<>
    {
        private:

        using self_type = binary_tree<VertexProperty, boost::no_property>;

        using base = boost::bidirectional_binary_tree<>;

        detail::VertexManager<self_type, VertexProperty> _vertex_manager;

        public:

        /// @brief Inheriting constructors
        using base::base;

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        /// @brief Add a vertex to the graph
        template<typename... Args>
        friend
        vertex_descriptor add_vertex(self_type &g, Args&&... args)
        {
            return g._vertex_manager.add_vertex(g, std::forward<Args>(args)...);
        }

        /// @brief Add edges between the parent vertex and the two children.
        friend 
        std::pair<edge_descriptor,edge_descriptor> 
        add_children(
            self_type &g,
            vertex_descriptor parent, 
            vertex_descriptor left, 
            vertex_descriptor right)
        {
            assert(parent != left);
            assert(parent != right);
            assert(left != right);

            std::pair<vertex_descriptor,vertex_descriptor> left_edge = g.add_left_edge(parent, left);
            std::pair<vertex_descriptor,vertex_descriptor> right_edge = g.add_right_edge(parent, right);

            return {left_edge, right_edge};
        }
    };

    /// @brief A binary tree class 
    /// @remarks Partial specialization where there is no vertex property attached
    template<class EdgeProperty>
    class binary_tree<boost::no_property, EdgeProperty> :
    public boost::bidirectional_binary_tree<>
    {
        private:

        using self_type = binary_tree<boost::no_property, EdgeProperty>;

        using base = boost::bidirectional_binary_tree<>;

        detail::EdgeManager<self_type, EdgeProperty> _edge_manager;

        public:

        /// @brief Inheriting constructors
        using base::base;

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        friend
        vertex_descriptor add_vertex(self_type &g)
        {
            return g.add_vertex();
        }

        /// @brief Add edges between the parent vertex and the two children.
        template<typename... Args>
        friend 
        std::pair<edge_descriptor,edge_descriptor>
        add_children(self_type &g,
                    vertex_descriptor parent, 
                    std::tuple<vertex_descriptor, Args...> left, 
                    std::tuple<vertex_descriptor, Args...> right)
        {
            return g._edge_manager.add_children(g, parent, left, right);
        }
    };

    /// @brief A binary tree class 
    /// @remarks Partial specialization where there is no vertex property attached
    template<class VertexProperty, class EdgeProperty>
    requires (!std::is_same_v<VertexProperty, boost::no_property> &&  !std::is_same_v<VertexProperty, boost::no_property>)
    class binary_tree<VertexProperty, EdgeProperty> :
    public boost::bidirectional_binary_tree<>
    {
        private:

        using self_type = binary_tree<VertexProperty, EdgeProperty>;

        using base = boost::bidirectional_binary_tree<>;

        detail::EdgeManager<self_type, EdgeProperty> _edge_manager;

        detail::VertexManager<self_type, VertexProperty> _vertex_manager;

        public:
 
        /// @brief Inheriting constructors
        using base::base;

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        /// @brief Add a vertex to the graph
        template<typename... Args>
        friend
        vertex_descriptor add_vertex(self_type &g, Args&&... args)
        {
            return g._vertex_manager.add_vertex(g, std::forward<Args>(args)...);
        }

        /// @brief Add edges between the parent vertex and the two children.
        template<typename... Args>
        friend 
        std::pair<edge_descriptor,edge_descriptor>
        add_children(self_type &g,
                    vertex_descriptor parent, 
                    std::tuple<vertex_descriptor, Args...> left, 
                    std::tuple<vertex_descriptor, Args...> right)
        {
            return g._edge_manager.add_children(g, parent, left, right);
        }
    };
}

#endif