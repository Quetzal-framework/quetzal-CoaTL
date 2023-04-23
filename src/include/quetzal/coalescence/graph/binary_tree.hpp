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
#include <algorithm>
#include <iterator>
#include <tuple>

namespace quetzal::coalescence
{
    namespace detail
    {

        template<class... Types>
        struct count { static const std::size_t value = sizeof...(Types); };

        template <typename T1, typename... Ts>
        std::tuple<Ts...> unshift_tuple(const std::tuple<T1, Ts...>& tuple)
        {
            return std::apply([](auto&&, const auto&... args) {return std::tie(args...);}, tuple);
        }

        // Getting std::map::emplace to work with aggregate initialization for POD EdgeProperties 
        template<typename T>
        struct tag { using type = T; };

        template<typename F>
        struct initializer
        {
            F f;
            template<typename T>
            operator T() &&
            {
                return std::forward<F>(f)(tag<T>{});
            }
        };

        template<typename F>
        initializer(F&&) -> initializer<F>;

        template<typename... Args>
        auto initpack(Args&&... args)
        {
            return initializer{[&](auto t) {
                using Ret = typename decltype(t)::type;
                return Ret{std::forward<Args>(args)...};
            }};
        }

        template<typename VertexDescriptor, typename VertexProperty>
        struct VertexManager
        {
            using vertex_descriptor = VertexDescriptor;
            using vertex_hashmap_type = std::map<vertex_descriptor, VertexProperty>;

            vertex_hashmap_type _property_map;

            template<typename... Args>
            void add_vertex_to_manager(vertex_descriptor v, Args&&... args)
            {
                _property_map[v] = { std::forward<Args>(args)... };
            }

            const VertexProperty& operator [](vertex_descriptor v) const
            {
                return _property_map.at(v);
            }

            VertexProperty & operator [](vertex_descriptor v)
            {
                return _property_map.at(v);
            }
        };

        template<typename EdgeDescriptor, typename EdgeProperty>
        struct EdgeManager
        {
            using edge_descriptor = EdgeDescriptor;
            using vertex_descriptor = typename EdgeDescriptor::first_type;
            using edge_hashmap_type = std::map<edge_descriptor, EdgeProperty>;

            edge_hashmap_type _property_map;

            template<typename... Args>
            void
            add_edges(EdgeDescriptor u, std::tuple<Args...> left, EdgeDescriptor v, std::tuple<Args...> right)
            {
                _property_map.emplace(std::piecewise_construct, 
                    std::forward_as_tuple(u), 
                    std::forward_as_tuple( std::apply( [](auto &&... args) { return initpack(args...); }, left) ));
                _property_map.emplace(std::piecewise_construct, 
                    std::forward_as_tuple(v), 
                    std::forward_as_tuple( std::apply( [](auto &&... args) { return initpack(args...); }, right)));
            }

            template<typename VertexDescriptor>
            const EdgeProperty& operator [](const std::pair<VertexDescriptor, VertexDescriptor>& edge) const
            {
                return _property_map.at(edge);
            }

            template<typename VertexDescriptor>
            EdgeProperty & operator [](const std::pair<VertexDescriptor, VertexDescriptor>& edge)
            {
                return _property_map.at(edge);
            }
        };

        namespace adl_resolution
        {
            void add_left_edge() = delete;
            void add_right_edge() = delete;
            void add_vertex() = delete;
        }
    }


    /// @brief A binary tree class
    /// @remarks Primary template for partial specialization
    template<class VertexProperty, class EdgeProperty>
    class binary_tree
    {};
  
    /// @brief A binary tree class 
    /// @remarks Explicit (full) specialization where there is no property attached to either vertices nor edges.
    template<>
    class binary_tree<boost::no_property, boost::no_property>
    {
    private: 
    
        using base = boost::bidirectional_binary_tree<>;
        base _graph; 

    public:
 
        /// @brief Default constructor
        binary_tree(){}

        /// @brief Construct graph with n vertices
        binary_tree(size_t n) : _graph(n) {}

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        /// @brief The ways in which the vertices in the graph can be traversed.
        using traversal_category = typename base::traversal_category;
      
        /// @brief Bidirectional
        using directed_category = typename base::directed_category ;

        using edge_parallel_category = typename base::edge_parallel_category ;

        static constexpr inline vertex_descriptor null_vertex()
        {
          return base::null_vertex();
        }

        /// @brief Add a vertex to the graph
        friend vertex_descriptor add_vertex(binary_tree& tree)
        {
            using detail::adl_resolution::add_vertex;
            return add_vertex(tree._graph);
        }

        /// @brief Add edges between the parent vertex and the two children.
        friend std::pair<edge_descriptor, edge_descriptor> 
        add_edges(binary_tree &tree, vertex_descriptor parent, vertex_descriptor left, vertex_descriptor right)
        {
            assert(parent != left);
            assert(parent != right);
            assert(left != right);
            return { add_left_edge(parent, left, tree._graph),  add_right_edge(parent, right, tree._graph) };
        }

      /// @brief Returns the number of out-edges of vertex v in graph g.
      friend
      degree_size_type
      out_degree(vertex_descriptor v, binary_tree const &tree)
      {
        return out_degree(v, tree._graph);
      }

      /// @brief Finds the root of the tree graph starting from a vertex u.
      /// @param u The vertex to start from.
      /// @param tree The binary tree graph.
      /// @remark This function will be an infinite loop if called on a recurrent
      ///         tree (which is not a tree any more).
      friend
      vertex_descriptor
      root(vertex_descriptor u, binary_tree const &tree)
      {
        return root(u, tree._graph);
      }

      /// @brief Evaluate if a vertex is a left successor (child)
      /// @param u The vertex to be evaluated
      /// @param tree The binary tree graph
      /// @return True if u is a left successoir, false otherwise.
      friend
      bool
      is_left_successor(vertex_descriptor u, binary_tree const &tree)
      {
        return is_left_successor(u, tree._graph);
      }

      /// @brief Evaluate if a vertex is a right successor (child)
      /// @param u The vertex to be evaluated
      /// @param tree The binary tree graph
      /// @return True if u is a right successoir, false otherwise.
      friend
      bool
      is_right_successor(vertex_descriptor u, binary_tree const &tree)
      {
        return is_right_successor(u, tree._graph);
      }

      /// @brief Evaluates if the vertex has a predecessor (parent)
      /// @param u The vertex to evaluate
      /// @param tree The binary tree graph
      /// @return True if u has a predecessor, false otherwise
      friend
      bool
      has_predecessor(vertex_descriptor u, binary_tree const &tree)
      {
        return has_predecessor(u, tree._graph);
      }

      /// @brief The predecessor of a given vertex
      /// @param u The vertex 
      /// @param tree The binary tree graph.
      /// @return The vertex that is the predecessor of u.
      friend
      vertex_descriptor
      predecessor(vertex_descriptor u, binary_tree const &tree)
      {
        return predecessor(u, tree._graph);
      }

        /// @brief Iterate through the in-edges.
      using in_edge_iterator = base::in_edge_iterator;

      /// @brief Provides iterators to iterate over the in-going edges of vertex u
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return A pair of iterators
      /// @relates quetzal::coalescence::binary_tree
      friend
      std::pair<in_edge_iterator, in_edge_iterator>
      in_edges(vertex_descriptor u, binary_tree const &tree)
      {
        return in_edges(u, tree._graph);
      }

      /// @brief Returns the number of in-edges of vertex v in graph g.
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return The number of in-edges.
      friend
      degree_size_type
      in_degree(vertex_descriptor u, binary_tree const &tree)
      {
        return has_predecessor(u, tree);
      }

      /// @brief Returns the number of in-edges plus out-edges.
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return Returns the number of in-edges plus out-edges 
      friend
      degree_size_type
      degree(vertex_descriptor u, binary_tree const &tree)
      {
        return degree(u, tree._graph);
      }
      
        /// @brief performs a depth-first traversal of the vertices in a directed graph
        /// @tparam Vertex 
        /// @tparam DFSTreeVisitor 
        /// @param tree The binary tree graph
        /// @param s The vertex to start from
        /// @param vis The visitor to apply
        template <typename DFSTreeVisitor>
        friend void
        depth_first_search(binary_tree &tree, vertex_descriptor s, DFSTreeVisitor &vis)
        {
            return depth_first_search(tree._graph, s, vis);
        }

        /// @brief performs a depth-first traversal of the vertices in a directed graph
        /// @tparam Vertex 
        /// @tparam DFSTreeVisitor 
        /// @param tree The binary tree graph
        /// @param s The vertex to start from
        /// @param vis The visitor to apply
        template <typename DFSTreeVisitor>
        friend void
        depth_first_search(binary_tree const&tree, vertex_descriptor s, DFSTreeVisitor &vis)
        {
            return depth_first_search(tree._graph, s, vis);
        }

      /// @brief Detects if there is a 1-to-1 mapping of the vertices in one graph to the vertices of another graph such that adjacency is preserved. 
        /// @param g A binary tree graph
        /// @param other A binary tree graph
        /// @return true if there exists an isomorphism between graph g and graph h and false otherwise
        friend bool isomorphism(binary_tree const &tree, binary_tree const &other)
        {
            return isomorphism(tree._graph, other._graph);
        }
    
    }; // end specialization binary_tree<boost::no::property, boost::no_property>






    /// @brief A binary tree class 
    /// @remarks Partial specialization where there is no edge property attached
    template<class VertexProperty>
    requires (!std::is_same_v<VertexProperty, boost::no_property>)
    class binary_tree<VertexProperty, boost::no_property>
    {
        private:

        using base = boost::bidirectional_binary_tree<>;
        base _graph; 
        detail::VertexManager<base::vertex_descriptor, VertexProperty> _vertex_manager;

        public:
 
        /// @brief Default constructor
        binary_tree(){}

        /// @brief Construct graph with n vertices
        binary_tree(size_t n) : _graph(n) {}

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        /// @brief Iterate through the in-edges.
        using in_edge_iterator = base::in_edge_iterator;

        /// @brief The ways in which the vertices in the graph can be traversed.
        using traversal_category = typename base::traversal_category;

        /// @brief Bidirectional
        using directed_category = typename base::directed_category ;

        using edge_parallel_category = typename base::edge_parallel_category ;

        static constexpr inline vertex_descriptor null_vertex()
        {
          return base::null_vertex();
        }

      /// @brief Returns the number of out-edges of vertex v in graph g.
      friend
      degree_size_type
      out_degree(vertex_descriptor v, binary_tree const &tree)
      {
        return out_degree(v, tree._graph);
      }

        /// @brief Read only access to the vertex property
        const VertexProperty& operator [](vertex_descriptor v) const
        {
            return _vertex_manager[v];
        }

        /// @brief Read and write access to the vertex property
        VertexProperty & operator [](vertex_descriptor v)
        {
            return _vertex_manager[v];
        }

        /// @brief Add a vertex and its properties to the graph
        template<typename... Args>
        friend
        vertex_descriptor add_vertex(binary_tree &tree, Args&&... args)
        {
            using detail::adl_resolution::add_vertex;
            vertex_descriptor v = add_vertex(tree._graph);
            tree._vertex_manager.add_vertex_to_manager(v, std::forward<Args>(args)...);
            return v;
        }

        /// @brief Add edges between the parent vertex and the two children.
        friend std::pair<edge_descriptor, edge_descriptor> 
        add_edges(binary_tree &tree, vertex_descriptor parent, vertex_descriptor left, vertex_descriptor right)
        {
            assert(parent != left);
            assert(parent != right);
            assert(left != right);
            using detail::adl_resolution::add_left_edge;
            using detail::adl_resolution::add_right_edge;
            return { add_left_edge(parent, left, tree._graph),  add_right_edge(parent, right, tree._graph) };
        }

      /// @brief Finds the root of the tree graph starting from a vertex u.
      /// @param u The vertex to start from.
      /// @param tree The binary tree graph.
      /// @remark This function will be an infinite loop if called on a recurrent
      ///         tree (which is not a tree any more).
      friend
      vertex_descriptor
      root(vertex_descriptor u, binary_tree const &tree)
      {
        return root(u, tree._graph);
      }

      /// @brief Evaluate if a vertex is a left successor (child)
      /// @param u The vertex to be evaluated
      /// @param tree The binary tree graph
      /// @return True if u is a left successoir, false otherwise.
      friend
      bool
      is_left_successor(vertex_descriptor u, binary_tree const &tree)
      {
        return is_left_successor(u, tree._graph);
      }

      /// @brief Evaluate if a vertex is a right successor (child)
      /// @param u The vertex to be evaluated
      /// @param tree The binary tree graph
      /// @return True if u is a right successoir, false otherwise.
      friend
      bool
      is_right_successor(vertex_descriptor u, binary_tree const &tree)
      {
        return is_right_successor(u, tree._graph);
      }

      /// @brief Evaluates if the vertex has a predecessor (parent)
      /// @param u The vertex to evaluate
      /// @param tree The binary tree graph
      /// @return True if u has a predecessor, false otherwise
      friend
      bool
      has_predecessor(vertex_descriptor u, binary_tree const &tree)
      {
        return has_predecessor(u, tree._graph);
      }

      /// @brief The predecessor of a given vertex
      /// @param u The vertex 
      /// @param tree The binary tree graph.
      /// @return The vertex that is the predecessor of u.
      friend
      vertex_descriptor
      predecessor(vertex_descriptor u, binary_tree const &tree)
      {
        return predecessor(u, tree._graph);
      }

      /// @brief Provides iterators to iterate over the in-going edges of vertex u
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return A pair of iterators
      /// @relates quetzal::coalescence::binary_tree
      friend
      std::pair<in_edge_iterator, in_edge_iterator>
      in_edges(vertex_descriptor u, binary_tree const &tree)
      {
        return in_edges(u, tree._graph);
      }

      /// @brief Returns the number of in-edges of vertex v in graph g.
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return The number of in-edges.
      friend
      degree_size_type
      in_degree(vertex_descriptor u, binary_tree const &tree)
      {
        return has_predecessor(u, tree);
      }

      /// @brief Returns the number of in-edges plus out-edges.
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return Returns the number of in-edges plus out-edges 
      friend
      degree_size_type
      degree(vertex_descriptor u, binary_tree const &tree)
      {
        return degree(u, tree._graph);
      }
      
        /// @brief performs a depth-first traversal of the vertices in a directed graph
        /// @tparam DFSTreeVisitor 
        /// @param tree The binary tree graph
        /// @param s The vertex to start from
        /// @param vis The visitor to apply
        template <typename DFSTreeVisitor>
        friend void
        depth_first_search(binary_tree &tree, vertex_descriptor s, DFSTreeVisitor &vis)
        {
            return depth_first_search(tree._graph, s, vis);
        }

        /// @brief performs a depth-first traversal of the vertices in a directed graph
        /// @tparam DFSTreeVisitor 
        /// @param tree The binary tree graph
        /// @param s The vertex to start from
        /// @param vis The visitor to apply
        template <typename DFSTreeVisitor>
        friend void
        depth_first_search(binary_tree const&tree, vertex_descriptor s, DFSTreeVisitor &vis)
        {
            return depth_first_search(tree._graph, s, vis);
        }

      /// @brief Detects if there is a 1-to-1 mapping of the vertices in one graph to the vertices of another graph such that adjacency is preserved. 
        /// @param g A binary tree graph
        /// @param other A binary tree graph
        /// @return true if there exists an isomorphism between graph g and graph h and false otherwise
        friend bool isomorphism(binary_tree const &tree, binary_tree const &other)
        {
            return isomorphism(tree._graph, other._graph);
        }
    
    }; // end specialization binary_tree<Vertex, boost::no_property>




    /// @brief A binary tree class 
    /// @remarks Partial specialization where there is no vertex property attached
    template<class EdgeProperty>
    requires (!std::is_same_v<EdgeProperty, boost::no_property>)
    class binary_tree<boost::no_property, EdgeProperty>
    {

        private:

        using base = boost::bidirectional_binary_tree<>;
        base _graph; 
        detail::EdgeManager<base::edge_descriptor, EdgeProperty> _edge_manager;

        public:
 
        /// @brief Default constructor
        binary_tree(){}

        /// @brief Construct graph with n vertices
        binary_tree(size_t n) : _graph(n) {}

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        /// @brief Iterate through the in-edges.
        using in_edge_iterator = base::in_edge_iterator;

        /// @brief The ways in which the vertices in the graph can be traversed.
        using traversal_category = typename base::traversal_category;

        /// @brief Bidirectional
        using directed_category = typename base::directed_category ;

        using edge_parallel_category = typename base::edge_parallel_category ;

        static constexpr inline vertex_descriptor null_vertex()
        {
          return base::null_vertex();
        }

      /// @brief Returns the number of out-edges of vertex v in graph g.
      friend
      degree_size_type
      out_degree(vertex_descriptor v, binary_tree const &tree)
      {
        return out_degree(v, tree._graph);
      }

        /// @brief Read only access to the edge property
        const EdgeProperty& operator [](const edge_descriptor& edge) const
        {
            return _edge_manager[edge];
        }

        /// @brief Read and write access to the edge property
        EdgeProperty & operator [](const edge_descriptor& edge)
        {
            return _edge_manager[edge];
        }

        /// @brief Add a vertex to the graph
        friend vertex_descriptor add_vertex(binary_tree& tree)
        {
            using detail::adl_resolution::add_vertex;
            return add_vertex(tree._graph);
        }

        /// @brief Add edges between the parent vertex and the two children.
        template<typename... Args>
        friend 
        std::pair<edge_descriptor,edge_descriptor>
        add_edges(binary_tree &tree,
                    vertex_descriptor parent, 
                    std::tuple<vertex_descriptor, Args...> left, 
                    std::tuple<vertex_descriptor, Args...> right)
        {
            assert(parent != get<0>(left));
            assert(parent != get<0>(right));
            assert(get<0>(left) != get<0>(right));

            using detail::adl_resolution::add_left_edge;
            using detail::adl_resolution::add_right_edge;

            auto left_edge = add_left_edge( parent, get<0>(left),tree._graph );
            auto right_edge = add_right_edge( parent, get<0>(right), tree._graph );

            tree._edge_manager.add_edges( left_edge, detail::unshift_tuple(left), right_edge, detail::unshift_tuple(right) );
            return {left_edge, right_edge};
        }


      /// @brief Finds the root of the tree graph starting from a vertex u.
      /// @param u The vertex to start from.
      /// @param tree The binary tree graph.
      /// @remark This function will be an infinite loop if called on a recurrent
      ///         tree (which is not a tree any more).
      friend
      vertex_descriptor
      root(vertex_descriptor u, binary_tree const &tree)
      {
        return root(u, tree._graph);
      }

      /// @brief Evaluate if a vertex is a left successor (child)
      /// @param u The vertex to be evaluated
      /// @param tree The binary tree graph
      /// @return True if u is a left successoir, false otherwise.
      friend
      bool
      is_left_successor(vertex_descriptor u, binary_tree const &tree)
      {
        return is_left_successor(u, tree._graph);
      }

      /// @brief Evaluate if a vertex is a right successor (child)
      /// @param u The vertex to be evaluated
      /// @param tree The binary tree graph
      /// @return True if u is a right successoir, false otherwise.
      friend
      bool
      is_right_successor(vertex_descriptor u, binary_tree const &tree)
      {
        return is_right_successor(u, tree._graph);
      }

      /// @brief Evaluates if the vertex has a predecessor (parent)
      /// @param u The vertex to evaluate
      /// @param tree The binary tree graph
      /// @return True if u has a predecessor, false otherwise
      friend
      bool
      has_predecessor(vertex_descriptor u, binary_tree const &tree)
      {
        return has_predecessor(u, tree._graph);
      }

      /// @brief The predecessor of a given vertex
      /// @param u The vertex 
      /// @param tree The binary tree graph.
      /// @return The vertex that is the predecessor of u.
      friend
      vertex_descriptor
      predecessor(vertex_descriptor u, binary_tree const &tree)
      {
        return predecessor(u, tree._graph);
      }

      /// @brief Provides iterators to iterate over the in-going edges of vertex u
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return A pair of iterators
      /// @relates quetzal::coalescence::binary_tree
      friend
      std::pair<in_edge_iterator, in_edge_iterator>
      in_edges(vertex_descriptor u, binary_tree const &tree)
      {
        return in_edges(u, tree._graph);
      }

      /// @brief Returns the number of in-edges of vertex v in graph g.
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return The number of in-edges.
      friend
      degree_size_type
      in_degree(vertex_descriptor u, binary_tree const &tree)
      {
        return has_predecessor(u, tree);
      }

      /// @brief Returns the number of in-edges plus out-edges.
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return Returns the number of in-edges plus out-edges 
      friend
      degree_size_type
      degree(vertex_descriptor u, binary_tree const &tree)
      {
        return degree(u, tree._graph);
      }
      
        /// @brief performs a depth-first traversal of the vertices in a directed graph
        /// @tparam DFSTreeVisitor 
        /// @param tree The binary tree graph
        /// @param s The vertex to start from
        /// @param vis The visitor to apply
        template <typename DFSTreeVisitor>
        friend void
        depth_first_search(binary_tree &tree, vertex_descriptor s, DFSTreeVisitor &vis)
        {
            return depth_first_search(tree._graph, s, vis);
        }

        /// @brief performs a depth-first traversal of the vertices in a directed graph
        /// @tparam DFSTreeVisitor 
        /// @param tree The binary tree graph
        /// @param s The vertex to start from
        /// @param vis The visitor to apply
        template <typename DFSTreeVisitor>
        friend void
        depth_first_search(binary_tree const&tree, vertex_descriptor s, DFSTreeVisitor &vis)
        {
            return depth_first_search(tree._graph, s, vis);
        }

      /// @brief Detects if there is a 1-to-1 mapping of the vertices in one graph to the vertices of another graph such that adjacency is preserved. 
        /// @param g A binary tree graph
        /// @param other A binary tree graph
        /// @return true if there exists an isomorphism between graph g and graph h and false otherwise
        friend bool isomorphism(binary_tree const &tree, binary_tree const &other)
        {
            return isomorphism(tree._graph, other._graph);
        }

    }; // end specialization binary_tree<boost::no_property, Edge>

    /// @brief A binary tree class 
    /// @remarks Partial specialization where both edges and vertices have prooperty attached
    template<class VertexProperty, class EdgeProperty>
    requires (!std::is_same_v<VertexProperty, boost::no_property> &&  !std::is_same_v<VertexProperty, boost::no_property>)
    class binary_tree<VertexProperty, EdgeProperty>
    {

        private:

        using base = boost::bidirectional_binary_tree<>;
        base _graph; 
        detail::EdgeManager<base::edge_descriptor, EdgeProperty> _edge_manager;
        detail::VertexManager<base::vertex_descriptor, VertexProperty> _vertex_manager;

        public:
 
        /// @brief Default constructor
        binary_tree(){}

        /// @brief Construct graph with n vertices
        binary_tree(size_t n) : _graph(n) {}

        /// @brief Edge descriptor
        using edge_descriptor = typename base::edge_descriptor;

        /// @brief Vertex descriptor
        using vertex_descriptor = typename base::vertex_descriptor;

        /// @brief degree size type
        using degree_size_type = typename base::degree_size_type;

        /// @brief Iterate through the in-edges.
        using in_edge_iterator = base::in_edge_iterator;

        /// @brief The ways in which the vertices in the graph can be traversed.
        using traversal_category = typename base::traversal_category;
        
        /// @brief Bidirectional
        using directed_category = typename base::directed_category ;

        using edge_parallel_category = typename base::edge_parallel_category ;

        static constexpr inline vertex_descriptor null_vertex()
        {
          return base::null_vertex();
        }

      /// @brief Returns the number of out-edges of vertex v in graph g.
      friend
      degree_size_type
      out_degree(vertex_descriptor v, binary_tree const &tree)
      {
        return out_degree(v, tree._graph);
      }

        /// @brief Read only access to the vertex property
        const VertexProperty& operator [](vertex_descriptor v) const
        {
            return _vertex_manager[v];
        }

        /// @brief Read and write access to the vertex property
        VertexProperty & operator [](vertex_descriptor v)
        {
            return _vertex_manager[v];
        }

        /// @brief Read only access to the edge property
        const EdgeProperty& operator [](const edge_descriptor& edge) const
        {
            return _edge_manager[edge];
        }

        /// @brief Read and write access to the edge property
        EdgeProperty & operator [](const edge_descriptor& edge)
        {
            return _edge_manager[edge];
        }

        /// @brief Add a vertex and its properties to the graph
        template<typename... Args>
        friend
        vertex_descriptor add_vertex(binary_tree &tree, Args&&... args)
        {
            using detail::adl_resolution::add_vertex;
            vertex_descriptor v = add_vertex(tree._graph);
            tree._vertex_manager.add_vertex_to_manager(v, std::forward<Args>(args)...);
            return v;
        }

        /// @brief Add edges between the parent vertex and the two children.
        template<typename... Args>
        friend 
        std::pair<edge_descriptor,edge_descriptor>
        add_edges(binary_tree &tree,
                    vertex_descriptor parent, 
                    std::tuple<vertex_descriptor, Args...> left, 
                    std::tuple<vertex_descriptor, Args...> right)
        {
            assert(parent != get<0>(left));
            assert(parent != get<0>(right));
            assert(get<0>(left) != get<0>(right));
            using detail::adl_resolution::add_left_edge;
            using detail::adl_resolution::add_right_edge;
            auto left_edge = add_left_edge( parent, get<0>(left), tree._graph );
            auto right_edge = add_right_edge( parent, get<0>(right), tree._graph );
            tree._edge_manager.add_edges( left_edge, detail::unshift_tuple(left), right_edge, detail::unshift_tuple(right) );
            return {left_edge, right_edge};
        }

      /// @brief Finds the root of the tree graph starting from a vertex u.
      /// @param u The vertex to start from.
      /// @param tree The binary tree graph.
      /// @remark This function will be an infinite loop if called on a recurrent
      ///         tree (which is not a tree any more).
      friend
      vertex_descriptor
      root(vertex_descriptor u, binary_tree const &tree)
      {
        return root(u, tree._graph);
      }

      /// @brief Evaluate if a vertex is a left successor (child)
      /// @param u The vertex to be evaluated
      /// @param tree The binary tree graph
      /// @return True if u is a left successoir, false otherwise.
      friend
      bool
      is_left_successor(vertex_descriptor u, binary_tree const &tree)
      {
        return is_left_successor(u, tree._graph);
      }

      /// @brief Evaluate if a vertex is a right successor (child)
      /// @param u The vertex to be evaluated
      /// @param tree The binary tree graph
      /// @return True if u is a right successoir, false otherwise.
      friend
      bool
      is_right_successor(vertex_descriptor u, binary_tree const &tree)
      {
        return is_right_successor(u, tree._graph);
      }

      /// @brief Evaluates if the vertex has a predecessor (parent)
      /// @param u The vertex to evaluate
      /// @param tree The binary tree graph
      /// @return True if u has a predecessor, false otherwise
      friend
      bool
      has_predecessor(vertex_descriptor u, binary_tree const &tree)
      {
        return has_predecessor(u, tree._graph);
      }

      /// @brief The predecessor of a given vertex
      /// @param u The vertex 
      /// @param tree The binary tree graph.
      /// @return The vertex that is the predecessor of u.
      friend
      vertex_descriptor
      predecessor(vertex_descriptor u, binary_tree const &tree)
      {
        return predecessor(u, tree._graph);
      }

      /// @brief Provides iterators to iterate over the in-going edges of vertex u
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return A pair of iterators
      /// @relates quetzal::coalescence::binary_tree
      friend
      std::pair<in_edge_iterator, in_edge_iterator>
      in_edges(vertex_descriptor u, binary_tree const &tree)
      {
        return in_edges(u, tree._graph);
      }

      /// @brief Returns the number of in-edges of vertex v in graph g.
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return The number of in-edges.
      friend
      degree_size_type
      in_degree(vertex_descriptor u, binary_tree const &tree)
      {
        return has_predecessor(u, tree);
      }

      /// @brief Returns the number of in-edges plus out-edges.
      /// @param u The vertex u
      /// @param tree The binary tree graph
      /// @return Returns the number of in-edges plus out-edges 
      friend
      degree_size_type
      degree(vertex_descriptor u, binary_tree const &tree)
      {
        return degree(u, tree._graph);
      }
      
        /// @brief performs a depth-first traversal of the vertices in a directed graph
        /// @tparam DFSTreeVisitor 
        /// @param tree The binary tree graph
        /// @param s The vertex to start from
        /// @param vis The visitor to apply
        template <typename DFSTreeVisitor>
        friend void
        depth_first_search(binary_tree &tree, vertex_descriptor s, DFSTreeVisitor &vis)
        {
            return depth_first_search(tree._graph, s, vis);
        }

        /// @brief performs a depth-first traversal of the vertices in a directed graph
        /// @tparam DFSTreeVisitor 
        /// @param tree The binary tree graph
        /// @param s The vertex to start from
        /// @param vis The visitor to apply
        template <typename DFSTreeVisitor>
        friend void
        depth_first_search(binary_tree const&tree, vertex_descriptor s, DFSTreeVisitor &vis)
        {
            return depth_first_search(tree._graph, s, vis);
        }

      /// @brief Detects if there is a 1-to-1 mapping of the vertices in one graph to the vertices of another graph such that adjacency is preserved. 
        /// @param g A binary tree graph
        /// @param other A binary tree graph
        /// @return true if there exists an isomorphism between graph g and graph h and false otherwise
        friend bool isomorphism(binary_tree const &tree, binary_tree const &other)
        {
            return isomorphism(tree._graph, other._graph);
        }
 
    };

    template <typename DFSTreeVisitor>
    void depth_first_search(binary_tree<boost::no_property,boost::no_property> &tree,
      typename binary_tree<boost::no_property,boost::no_property>::vertex_descriptor  s, DFSTreeVisitor &vis);

    template <typename DFSTreeVisitor>
    void depth_first_search(binary_tree<boost::no_property,boost::no_property> const&tree,
      typename binary_tree<boost::no_property,boost::no_property>::vertex_descriptor  s, DFSTreeVisitor &vis);

    template <typename VertexProperty,typename DFSTreeVisitor>
        requires (!std::is_same_v<VertexProperty, boost::no_property>)
    void depth_first_search(binary_tree<VertexProperty,boost::no_property> &tree,
      typename binary_tree<VertexProperty,boost::no_property>::vertex_descriptor  s, DFSTreeVisitor &vis);

    template <typename VertexProperty, typename DFSTreeVisitor>
        requires (!std::is_same_v<VertexProperty, boost::no_property>)
    void depth_first_search(binary_tree<VertexProperty,boost::no_property> const&tree,
      typename binary_tree<VertexProperty,boost::no_property>::vertex_descriptor  s, DFSTreeVisitor &vis);

    template <typename EdgeProperty,typename DFSTreeVisitor>
        requires (!std::is_same_v<EdgeProperty, boost::no_property>)
    void depth_first_search(binary_tree<boost::no_property,EdgeProperty> &tree,
      typename binary_tree<boost::no_property,EdgeProperty>::vertex_descriptor  s, DFSTreeVisitor &vis);

    template <typename EdgeProperty, typename DFSTreeVisitor>
        requires (!std::is_same_v<EdgeProperty, boost::no_property>)
    void depth_first_search(binary_tree<boost::no_property,EdgeProperty> const&tree,
      typename binary_tree<boost::no_property,EdgeProperty>::vertex_descriptor  s, DFSTreeVisitor &vis);

    template <typename VertexProperty, typename EdgeProperty, typename DFSTreeVisitor>
        requires (!std::is_same_v<VertexProperty, boost::no_property> &&  !std::is_same_v<VertexProperty, boost::no_property>)
    void depth_first_search(binary_tree<VertexProperty,EdgeProperty> &tree,
      typename binary_tree<VertexProperty,EdgeProperty>::vertex_descriptor  s, DFSTreeVisitor &vis);

    template <typename VertexProperty, typename EdgeProperty, typename DFSTreeVisitor>
        requires (!std::is_same_v<VertexProperty, boost::no_property> &&  !std::is_same_v<VertexProperty, boost::no_property>)
    void depth_first_search(binary_tree<VertexProperty,EdgeProperty> const&tree,
      typename binary_tree<VertexProperty,EdgeProperty>::vertex_descriptor  s, DFSTreeVisitor &vis);


    namespace detail
    {
        template<typename Vertex, typename Edge, typename Generator>
        auto update_tree(
            binary_tree<Vertex,Edge>& tree, 
            std::vector<typename binary_tree<Vertex, Edge>::vertex_descriptor> leaves, 
            Generator& rng)
        {
            using tree_type = binary_tree<Vertex, Edge>;
            using vertex_descriptor = typename tree_type::vertex_descriptor;

            if(leaves.size() == 1 ){
                return leaves.front();
            } else {
                std::uniform_int_distribution<> distrib(1, leaves.size() -1 );
                int split = distrib(rng);

                std::vector<vertex_descriptor> left(leaves.begin(), leaves.begin() + split);
                std::vector<vertex_descriptor> right(leaves.begin() + split, leaves.end());

                auto parent = add_vertex(tree);
                if constexpr(std::is_same_v<Edge, boost::no_property>)
                    add_edges(tree, parent, update_tree(tree, left, rng), update_tree(tree, right, rng));
                else
                    add_edges(tree, parent, 
                        std::make_tuple(update_tree(tree, left, rng), Edge() ), 
                        std::make_tuple(update_tree(tree, right, rng), Edge() ));
                return parent;
            }
        }
    }

    /// @brief Generate a random binary tree with no vertex/edge property attached.
    /// @tparam Generator Random Number Generator
    /// @param n_leaves Number of leaves in the binary tree
    /// @param rng The random number generator
    /// @return A binary tree with its root vertex descriptor
    template<typename Vertex=boost::no_property, typename Edge=boost::no_property, typename Generator>
    std::tuple
    <
    quetzal::coalescence::binary_tree<Vertex, Edge>, 
    typename quetzal::coalescence::binary_tree<Vertex, Edge>::vertex_descriptor
    >
    get_random_binary_tree(int n_leaves, Generator& rng)
    {
    using tree_type = quetzal::coalescence::binary_tree<Vertex, Edge>;
    using vertex_descriptor = typename tree_type::vertex_descriptor;

    tree_type tree;
    std::vector<vertex_descriptor> leaves(n_leaves);
    std::transform(leaves.cbegin(), leaves.cend(), leaves.begin(), [&tree](auto) { return add_vertex(tree); });
    vertex_descriptor root = detail::update_tree(tree, leaves, rng);
    return std::tuple(std::move(tree), root);

  }
}

#endif