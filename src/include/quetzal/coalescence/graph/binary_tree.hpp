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

#include <type_traits>

#include "detail/tree_traits.hpp"
#include "detail/cardinal_k_ary_tree.hpp"
#include <utility>

namespace quetzal::coalescence
{
    /// @brief A binary tree class
    /// @remarks Primary template for partial specialization
    template<class VertexProperty, class EdgeProperty>
    class binary_tree {};
}

namespace boost
{

    namespace detail
    {
        template <class V>
        struct has_vertex_property_type< quetzal::coalescence::binary_tree<V,boost::no_property> >
        {
            static constexpr bool value = !std::is_same_v<V, boost::no_property>;
        };

        template <class E>
        struct has_edge_property_type< quetzal::coalescence::binary_tree<boost::no_property, E> >
        {
            static constexpr bool value = !std::is_same_v<E, boost::no_property>;
        };
    }
 

    template <class V, class E> 
        requires (!std::is_same_v<V, boost::no_property>)
    struct property_map<quetzal::coalescence::binary_tree<V,E>, boost::vertex_index_t>
    {
        using type = typename quetzal::coalescence::binary_tree<V,E>::vertex_property_map_type;
    };

    template <class V, class E> 
        requires (!std::is_same_v<E, boost::no_property>)
    struct property_map<quetzal::coalescence::binary_tree<V,E>, boost::edge_index_t>
    {
        using type = typename quetzal::coalescence::binary_tree<V,E>::edge_property_map_type;
    };
}

namespace quetzal::coalescence 
{
    template<class V, class E>
    auto get(boost::vertex_index_t, binary_tree<V,E> const& g)
    {
        return g.get_vertex_property_map();
    }

    template<class V, class E>
    auto get(boost::vertex_index_t, binary_tree<V,E> const& g, V const* v) 
    {
        return g.get_vertex_property_map()[v];
    }

    template<class V, class E>
    auto get(boost::edge_index_t, binary_tree<V,E> const& g)
    {
        return g.get_edge_property_map();
    }

    template<class V, class E>
    auto get(boost::edge_index_t, binary_tree<V,E> const& g, E const* e) 
    {
        return g.get_edge_property_map()[e];
    }
}


namespace quetzal::coalescence
{
    namespace detail
    {        
        template<class... Types>
        struct count {
            static const std::size_t value = sizeof...(Types);
        };

        template <typename T1, typename... Ts>
        std::tuple<Ts...> unshift_tuple(const std::tuple<T1, Ts...>& tuple)
        {
            return std::apply([](auto&&, const auto&... args) {return std::tie(args...);}, tuple);
        }

        template<typename VertexDescriptor, typename VertexProperty>
        struct VertexManager
        {
            using vertex_descriptor = VertexDescriptor;
            using vertex_hashmap_type = std::map<vertex_descriptor, VertexProperty>;
            using map_type = boost::associative_property_map< vertex_hashmap_type >;

            vertex_hashmap_type _vertex_property_hashmap;
            map_type _vertex_property_map { _vertex_property_hashmap };

            template<class Graph, typename... Args>
            vertex_descriptor add_vertex_to_manager(Graph &g, Args&&... args)
            {
                vertex_descriptor key = add_vertex(g);
                VertexProperty value = { std::forward<Args>(args)...};
                put(_vertex_property_map, key, value);
                return key;
            }

            const VertexProperty& operator [](vertex_descriptor v) const
            {
                return get(_vertex_property_map, v);
            }

            VertexProperty & operator [](vertex_descriptor v)
            {
                return _vertex_property_map[v];
            }
        };

        template<typename EdgeDescriptor, typename EdgeProperty>
        struct EdgeManager
        {
            using edge_descriptor = EdgeDescriptor;
            using edge_hashmap_type = std::map<edge_descriptor, EdgeProperty>;
            using map_type = boost::associative_property_map< edge_hashmap_type >;

            edge_hashmap_type _edge_property_hashmap;
            map_type _edge_property_map { _edge_property_hashmap };

            template<class Graph, typename... Args>
            std::pair<edge_descriptor,edge_descriptor>
            add_children(Graph &g,
                        typename Graph::vertex_descriptor parent, 
                        std::tuple<typename Graph::vertex_descriptor, Args...> left, 
                        std::tuple<typename Graph::vertex_descriptor, Args...> right)
            {
                assert(parent != get<0>(left));
                assert(parent != get<0>(right));
                assert(get<0>(left) != get<0>(right));

                std::pair<typename Graph::vertex_descriptor, typename Graph::vertex_descriptor> left_edge = add_left_edge(parent, get<0>(left), g);
                std::pair<typename Graph::vertex_descriptor, typename Graph::vertex_descriptor> right_edge = add_right_edge(parent, get<0>(right), g);

                auto p1 = std::apply([](Args&&... ts){ return EdgeProperty { std::forward<Args>(ts)... }; }, detail::unshift_tuple(left));
                auto p2 = std::apply([](Args&&... ts){ return EdgeProperty { std::forward<Args>(ts)... }; }, detail::unshift_tuple(right));

                put(_edge_property_map, left_edge, p1);
                put(_edge_property_map, right_edge, p2);

                return {left_edge, right_edge};
            }

            template<typename VertexDescriptor>
            const EdgeProperty& operator [](const std::pair<VertexDescriptor,VertexDescriptor>& edge) const
            {
                return get(_edge_property_map, edge);
            }

            template<typename VertexDescriptor>
            EdgeProperty & operator [](const std::pair<VertexDescriptor,VertexDescriptor>& edge)
            {
                return _edge_property_map[edge];
            }
        };

    }


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
    requires (!std::is_same_v<VertexProperty, boost::no_property>)
    class binary_tree<VertexProperty, boost::no_property> :
    public boost::bidirectional_binary_tree<>
    {
        private:

        using self_type = binary_tree<VertexProperty, boost::no_property>;

        using base = boost::bidirectional_binary_tree<>;

        detail::VertexManager<base::vertex_descriptor, VertexProperty> _vertex_manager;

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
        requires ( detail::count<Args...>::value != 0U  )
        friend
        vertex_descriptor add_vertex(self_type &g, Args&&... args)
        {
            return g._vertex_manager.add_vertex_to_manager(g, std::forward<Args>(args)...);
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

        const VertexProperty& operator [](vertex_descriptor vertex) const
        {
            return _vertex_manager[vertex];
        }

        VertexProperty & operator [](vertex_descriptor vertex)
        {
            return _vertex_manager[vertex];
        }

        auto get_vertex_property_map(){return _vertex_manager;}
    };

    /// @brief A binary tree class 
    /// @remarks Partial specialization where there is no vertex property attached
    template<class EdgeProperty>
    requires (!std::is_same_v<EdgeProperty, boost::no_property>)
    class binary_tree<boost::no_property, EdgeProperty> :
    public boost::bidirectional_binary_tree<>
    {
        private:

        using self_type = binary_tree<boost::no_property, EdgeProperty>;

        using base = boost::bidirectional_binary_tree<>;

        detail::EdgeManager<base::edge_descriptor, EdgeProperty> _edge_manager;

        public:

        auto get_vedge_property_map(){return _edge_manager;}

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

        const EdgeProperty& operator [](const std::pair<vertex_descriptor, vertex_descriptor>& edge) const
        {
            return _edge_manager[edge];
        }

        EdgeProperty & operator [](const std::pair<vertex_descriptor, vertex_descriptor>& edge)
        {
            return _edge_manager[edge];
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

        detail::EdgeManager<base::edge_descriptor, EdgeProperty> _edge_manager;

        detail::VertexManager<base::vertex_descriptor, VertexProperty> _vertex_manager;

        public:
 
        /// @brief Inheriting constructors
        using base::base;

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        auto get_vertex_property_map(){return _vertex_manager;}
        auto get_vedge_property_map(){return _edge_manager;}

        /// @brief Add a vertex to the graph
          /// @brief Add a vertex to the graph
        template<typename... Args>
        requires ( detail::count<Args...>::value != 0U  )
        friend
        vertex_descriptor add_vertex(self_type &g, Args&&... args)
        {
            return g._vertex_manager.add_vertex_to_manager(g, std::forward<Args>(args)...);
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

        const VertexProperty& operator [](vertex_descriptor v) const
        {
            return _vertex_manager[v];
        }

        VertexProperty & operator [](vertex_descriptor v)
        {
            return _vertex_manager[v];
        }

        const EdgeProperty& operator [](const std::pair<vertex_descriptor, vertex_descriptor>& edge) const
        {
            return _edge_manager[edge];
        }

        EdgeProperty & operator [](const std::pair<vertex_descriptor, vertex_descriptor>& edge)
        {
            return _edge_manager[edge];
        }
    };
}

#endif