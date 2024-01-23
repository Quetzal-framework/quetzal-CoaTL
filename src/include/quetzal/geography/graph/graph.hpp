// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/////////////////////////////////////////////////////////////////////////// *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/iterator_range.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/isomorphism.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/topological_sort.hpp>

#include "detail/adl_resolution.hpp"
#include "detail/concepts.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <type_traits>
#include <ranges>

namespace quetzal::geography
{
using no_property = boost::no_property;

namespace detail
{
template <template <class...> class Lhs, template <class...> class Rhs> constexpr bool is_same_template = false;

template <template <class...> class Lhs> constexpr bool is_same_template<Lhs, Lhs> = true;

// Base class and common implementation
template <class CRTP, class VertexProperty, class EdgeProperty,
          template <class ...> class Representation, class Directed>
class graph_common
{

  protected:
    /// @brief The type of graph hold by the graph class
    using base_type =
        std::conditional_t<is_same_template<Representation, boost::adjacency_list>,
                           Representation<boost::setS, boost::vecS, Directed, VertexProperty, EdgeProperty>,
                           Representation<Directed, VertexProperty, EdgeProperty, no_property, std::allocator<bool>>>;

    base_type _graph;

  public:
    /// @brief Default constructor
    explicit graph_common() : _graph()
    {
    }

    /// @brief Constructs a graph with \f$n\f$ vertices
    explicit graph_common(size_t n) : _graph(n)
    {
    }

    /// @brief Vertex information
    template <typename T, typename U, typename V, typename W, typename X>
    using representation_type = Representation<T, U, V, W, X>;

    /// @brief Vertex information
    using vertex_property = VertexProperty;

    /// @brief Edge information
    using edge_property = EdgeProperty;

    /// @brief Is the graph directed or not
    using directed_category = Directed;

    /// @brief Vertex descriptor ("node ID" in other words).
    using vertex_descriptor = typename base_type::vertex_descriptor;

    /// @brief Edge descriptor
    using edge_descriptor = typename base_type::edge_descriptor;

    /// @brief Degree size type.
    using degree_size_type = typename base_type::degree_size_type;

    /// @brief The ways in which the vertices in the graph can be traversed.
    using traversal_category = typename base_type::traversal_category;

    /// @brief Iterate through the in-edges.
    using in_edge_iterator = typename base_type::in_edge_iterator;

    /// @brief Iterate through the out-edges.
    using out_edge_iterator = typename base_type::out_edge_iterator;

    using edge_parallel_category = typename base_type::edge_parallel_category;

    /// @name Topology Lookup
    /// @{

    /// @brief Number of vertices in the graph.
    /// @return The number of vertices.
    int num_vertices() const
    {
        return boost::num_vertices(_graph);
    }

    /// @brief Number of edges in the graph.
    /// @return The number of edges.
    int num_edges() const
    {
        return boost::num_edges(_graph);
    }

    /// @brief Detects if there is a 1-to-1 mapping of the vertices in one graph to the vertices of another graph such
    /// that adjacency is preserved.
    /// @param other A graph graph.
    /// @return true if there exists an isomorphism between the two graphs and false otherwise
    bool is_isomorphic(graph_common const &other) const
    {
        return boost::isomorphism(_graph, other._graph);
    }

    /// @brief Returns the number of in-edges plus out-edges.
    /// @param u The vertex \f$u\f$
    /// @return Returns the number of in-edges plus out-edges.
    degree_size_type degree(vertex_descriptor u) const
    {
        return boost::degree(u, _graph);
    }

    /// @brief Returns the number of in-edges of vertex \f$u\f$.
    /// @param u The vertex \f$u\f$
    /// @return The number of in-edges.
    degree_size_type in_degree(vertex_descriptor u) const
    {
        return boost::in_degree(u, _graph);
    }

    /// @brief Returns the number of out-edges of vertex \f$v\f$.
    degree_size_type out_degree(vertex_descriptor v) const
    {
        return boost::out_degree(v, _graph);
    }

    /// @brief Returns the edge between two vertices of the graph if the edge exists
    /// @param u The first vertex
    /// @param u The second vertex
    /// @return An optional edge descriptor \f$e\f$.
    std::optional<edge_descriptor> edge(vertex_descriptor u, vertex_descriptor v) const
    {
        auto p = boost::edge(u, v, _graph);
        return p.second ? std::make_optional(p.first) : std::nullopt;
    }

    /// @brief Returns the source vertex of a directed edge
    /// @param e The edge
    /// @return The source vertex of the edge \f$e\f$.
    vertex_descriptor source(edge_descriptor e) const
    {
        return boost::source(e, _graph);
    }

    /// @brief Returns the target vertex of a directed edge
    /// @param e The edge
    /// @return The target vertex of the edge \f$e\f$.
    vertex_descriptor target(edge_descriptor e) const
    {
        return boost::target(e, _graph);
    }

    /// @}

    /// @name Topology Modification
    /// @{

    //// @brief Add a new vertex to the graph, returning the vertex_descriptor for the new vertex.
    vertex_descriptor add_vertex()
    {
        return boost::add_vertex(_graph);
    }

    //// @brief Remove all edges to and from vertex \f$u\f$ from the graph.
    /// @param u The vertex
    void clear_vertex(vertex_descriptor u)
    {
        return boost::clear_vertex(u, _graph);
    }

    //// @brief Remove vertex u from the graph, also removing all edges to and from vertex \f$u\f$
    /// @param u The vertex to remove.
    void remove_vertex(vertex_descriptor u)
    {
        clear_vertex(u);
        return boost::remove_vertex(u, _graph);
    }

    //// @brief Inserts the edge \f$(u,v)\f$ into the graph if it does not exist, and returns an edge descriptor
    /// pointing to the new edge.
    /// @param u Source vertex
    /// @param u Target vertex
    edge_descriptor add_edge(vertex_descriptor u, vertex_descriptor v)
    {
        return boost::add_edge(u, v, _graph).first;
    }

    //// @brief Remove the edge \f$(u,v)\f$ from the graph.
    /// @param u Source vertex
    /// @param u Target vertex
    void remove_edge(vertex_descriptor u, vertex_descriptor v)
    {
        return boost::remove_edge(u, v, _graph);
    }

    //// @brief Remove the edge \f$e\f$ from the graph.
    /// @param e The edge to remove
    void remove_edge(edge_descriptor e)
    {
        return boost::remove_edge(e, _graph);
    }

    /// @}

    /// @name Iterators
    /// @{

    /// @brief Provides a range to iterate over the in-going edges of vertex \f$u\f$.
    /// @param u The vertex \f$u\f$.
    /// @return A range
    auto in_edges(vertex_descriptor u) const
    {
        return boost::make_iterator_range(boost::in_edges(u, _graph));
    }

    /// @brief Provides a range to iterate over the out-going edges of vertex \f$u\f$.
    /// @param u The vertex \f$u\f$.
    /// @return A range
    auto out_edges(vertex_descriptor u) const
    {
        return boost::make_iterator_range(boost::out_edges(u, _graph));
    }

    /// @brief Provides a range to iterate over the vertices of the graph
    /// @return A range
    auto vertices() const
    {
        return boost::make_iterator_range(boost::vertices(_graph));
    }

    /// @brief Provides a range to iterate over the edges of the graph
    /// @return A range
    auto edges() const
    {
        auto [first, last] = boost::edges(_graph);
        return std::ranges::subrange(first, last);
    }

    /// @}

    /// @brief Null vertex identifier
    /// @return A null vertex
    static constexpr inline vertex_descriptor null_vertex()
    {
        return base_type::null_vertex();
    }

}; // end class graph_common

} // end namespace detail

// Base for template partial specialization
template <class VertexProperty, class EdgeProperty,
          template <typename, typename, typename, typename, typename> class Representation, class Directed>
class graph
{
};

/// @brief @anchor SpatialGraphNoPropertyNoProperty A graph class with no information attached to either vertices nor
/// edges.
/// @details This graph structure is bidirected and allows to model a forest of disconnected graphs and isolated
/// vertices.
/// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
template <template <typename, typename, typename, typename, typename> class Representation, class Directed>
class graph<no_property, no_property, Representation, Directed>
    : public detail::graph_common<graph<no_property, no_property, Representation, Directed>, no_property, no_property,
                                  Representation, Directed>
{
  private:
    using self_type = graph<no_property, no_property, Representation, Directed>;
    using base_type = detail::graph_common<self_type, no_property, no_property, Representation, Directed>;

  public:
    /// @name Constructors
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit graph() : base_type()
    {
    }

    /// @brief Construct graph with \f$n\f$ vertices
    explicit graph(size_t n) : base_type(n)
    {
    }
    /// @}

}; // end specialization graph<boost::no::property, boost::no_property>

/// @brief @anchor SpatialGraphVertexPropertyNoProperty A graph class with information attached to vertices.
/// @tparam VertexProperty The type of information to store at each vertex.
/// @details Vertices embed a user-defined type of information. Edges embed no additional information.
///          This graph structure is bidirected and allows to model a forest of disconnected graphs and isolated
///          vertices.
/// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
template <class VertexProperty, template <typename, typename, typename, typename, typename> class Representation,
          class Directed>
    requires(!std::is_same_v<VertexProperty, no_property>)
class graph<VertexProperty, no_property, Representation, Directed>
    : public detail::graph_common<graph<VertexProperty, no_property, Representation, Directed>, VertexProperty,
                                  no_property, Representation, Directed>
{
  private:
    using self_type = graph<VertexProperty, no_property, Representation, Directed>;
    using base_type = detail::graph_common<self_type, VertexProperty, no_property, Representation, Directed>;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base_type::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base_type::vertex_descriptor;

    /// @name Constructors
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit graph() : base_type()
    {
    }

    /// @brief Construct graph with \f$n\f$ vertices
    explicit graph(size_t n) : base_type(n)
    {
    }

    /// @}

    /// @name Topology Modifiers
    /// @{

    /// @brief Add a vertex and its properties to the graph
    auto add_vertex(const VertexProperty &p)
    {
        return boost::add_vertex(p, this->_graph);
    }

    /// @}

    /// @name Property Lookup
    /// @{

    /// @brief Read only access to the vertex property
    const VertexProperty &operator[](vertex_descriptor v) const
    {
        return this->_graph[v];
    }

    /// @brief Read and write access to the vertex property
    VertexProperty &operator[](vertex_descriptor v)
    {
        return this->_graph[v];
    }

    /// @}

}; // end specialization graph<Vertex, no_property>

/// @brief @anchor SpatialGraphNoPropertyEdgeProperty A graph class with information attached to edges.
/// @tparam EdgeProperty The type of information to store at each edge.
/// @details Vertices embed no additional information. Edges embed an user-defined type of information.
///          This graph structure is bidirected and allows to model a forest of disconnected graphs and isolated
///          vertices.
/// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
template <class EdgeProperty, template <typename, typename, typename, typename, typename> class Representation,
          class Directed>
    requires(!std::is_same_v<EdgeProperty, no_property>)
class graph<no_property, EdgeProperty, Representation, Directed>
    : public detail::graph_common<graph<no_property, EdgeProperty, Representation, Directed>, no_property, EdgeProperty,
                                  Representation, Directed>
{
  private:
    using self_type = graph<no_property, EdgeProperty, Representation, Directed>;
    using base_type = detail::graph_common<self_type, no_property, EdgeProperty, Representation, Directed>;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base_type::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base_type::vertex_descriptor;

    /// @name Constructors
    /// @{

    /// @brief Default constructor. Initializses a graph with 0 vertices.
    explicit graph() : base_type()
    {
    }

    /// @brief Construct graph with \f$n\f$ vertices
    explicit graph(size_t n) : base_type(n)
    {
    }

    /// @}

    /// @name Topology Modifiers
    /// @n{

    //// @brief Inserts the edge \f$(u,v)\f$ into the graph if it does not exist, and returns an edge descriptor
    /// pointing to the new edge.
    /// @param u Source vertex
    /// @param u Target vertex
    edge_descriptor add_edge(vertex_descriptor u, vertex_descriptor v, const EdgeProperty &p)
    {
        return boost::add_edge(u, v, p, this->_graph).first;
    }

    /// @}

    /// @name Property Lookup
    /// @{

    /// @brief Read only access to the edge property
    const EdgeProperty &operator[](const edge_descriptor &edge) const
    {
        return this->_graph[edge];
    }

    /// @brief Read and write access to the edge property
    EdgeProperty &operator[](const edge_descriptor &edge)
    {
        return this->_graph[edge];
    }

    /// @}

}; // end specialization graph<boost::no_property, Edge>

/// @brief @anchor SpatialGraphVertexPropertyEdgeProperty A graph class with information attached to vertices.
/// @tparam VertexProperty The type of information to store at each vertex.
/// @tparam EdgeProperty The type of information to store at each edge.
/// @details Vertices and edges embed additional information as user-defined types.
///          This graph structure is bidirected and allows to model a forest of disconnected graphs and isolated
///          vertices.
/// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
template <class VertexProperty, class EdgeProperty,
          template <typename, typename, typename, typename, typename> class Representation, class Directed>
    requires(!std::is_same_v<VertexProperty, no_property> and !std::is_same_v<EdgeProperty, no_property>)
class graph<VertexProperty, EdgeProperty, Representation, Directed>
    : public detail::graph_common<graph<VertexProperty, EdgeProperty, Representation, Directed>, VertexProperty,
                                  EdgeProperty, Representation, Directed>
{
  private:
    using self_type = graph<VertexProperty, EdgeProperty, Representation, Directed>;
    using base_type = detail::graph_common<self_type, VertexProperty, EdgeProperty, Representation, Directed>;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base_type::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base_type::vertex_descriptor;

    /// @name Constructors
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.

    explicit graph() : base_type()
    {
    }

    /// @brief Construct graph with \f$n\f$ vertices
    explicit graph(size_t n) : base_type(n)
    {
    }

    ///@}

    /// @name Topology Modifiers
    /// @{

    /// @brief Add a vertex and its properties to the graph
    auto add_vertex(const VertexProperty &p)
    {
        return boost::add_vertex(p, this->_graph);
    }

    //// @brief Inserts the edge \f$(u,v)\f$ into the graph if it does not exist, and returns an edge descriptor
    /// pointing to the new edge.
    /// @param u Source vertex
    /// @param u Target vertex
    /// @param p The edge property
    edge_descriptor add_edge(vertex_descriptor u, vertex_descriptor v, const EdgeProperty &p)
    {
        return boost::add_edge(u, v, p, this->_graph).first;
    }

    /// @}

    /// @name Property Lookup
    /// @{

    /// @brief Read only access to the vertex property
    const VertexProperty &operator[](vertex_descriptor v) const
    {
        return this->_graph[v];
    }

    /// @brief Read and write access to the vertex property
    VertexProperty &operator[](vertex_descriptor v)
    {
        return this->_graph[v];
    }

    /// @brief Read only access to the edge property
    const EdgeProperty &operator[](const edge_descriptor &edge) const
    {
        return this->_graph[edge];
    }

    /// @brief Read and write access to the edge property
    EdgeProperty &operator[](const edge_descriptor &edge)
    {
        return this->_graph[edge];
    }

    /// @}
};

} // end namespace quetzal::geography
