// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "detail/graph_traits.hpp"

#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <boost/graph/isomorphism.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/connected_components.hpp>

#include "detail/adl_resolution.hpp"

#include <iostream>
#include <algorithm>

#include <range/v3/all.hpp>

namespace quetzal::geography
{
	using no_property = boost::no_property;

	namespace detail
	{
    
    // Base class and common implementation
		template <class VertexProperty, class EdgeProperty, class CRTP, class Density>
		class graph_common
		{
		protected:

			/// @brief The type of graph hold by the graph class
			using base = typename Density::model<
				Density::out_edge_list_type,
				Density::vertex_list_type,
				Density::directed_type,
				VertexProperty,
				EdgeProperty>;

			base _graph;

		public:
			/// @brief Default constructor
			explicit graph_common() : _graph() {}

			/// @brief Constructs a graph with \f$n\f$ vertices
			explicit graph_common(size_t n) : _graph(n) {}

			/// @brief Vertex descriptor ("node ID" in other words).
			using vertex_descriptor = typename base::vertex_descriptor;

			/// @brief Edge descriptor
			using edge_descriptor = typename base::edge_descriptor;

      /// @brief Vertex information 
      using vertex_property = VertexProperty;

      /// @brief Edge information 
      using edge_property = EdgeProperty;

			/// @brief Degree size type.
			using degree_size_type = typename base::degree_size_type;

			/// @brief The ways in which the vertices in the graph can be traversed.
			using traversal_category = typename base::traversal_category;

			/// @brief The graph is bidirected.
			using directed_category = typename base::directed_category;

			/// @brief Iterate through the in-edges.
			using in_edge_iterator = typename base::in_edge_iterator;

			/// @brief Iterate through the out-edges.
			using out_edge_iterator = typename base::out_edge_iterator;

			using edge_parallel_category = typename base::edge_parallel_category;

			/// @name Topology Lookup
			/// @{

			/// @brief Detects if there is a 1-to-1 mapping of the vertices in one graph to the vertices of another graph such that adjacency is preserved.
			/// @param other A graph graph.
			/// @return true if there exists an isomorphism between the two graphs and false otherwise
			bool is_isomorphic(graph_common const &other) const
			{
				return boost::isomorphism(_graph, other._graph);
			}

			/// @brief Finds the root of the graph graph starting from a vertex \f$u\f$.
			/// @param u The vertex to start from.
			/// @remark This function will be an infinite loop if called on a recurrent graph
			vertex_descriptor find_root_from(vertex_descriptor u) const
			{
        std::vector<vertex_descriptor> order;
        topological_sort(this->_graph, back_inserter(order));
        return order.back();
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
        auto p = boost::edge( u, v, _graph );
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

      //// @brief Inserts the edge \f$(u,v)\f$ into the graph if it does not exist, and returns an edge descriptor pointing to the new edge.
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

			/// @}

			/// @brief Null vertex identifier
			/// @return A null vertex
			static constexpr inline vertex_descriptor null_vertex()
			{
				return base::null_vertex();
			}

		}; // end class graph_common

	} // end namespace detail


  template <class VertexProperty, class EdgeProperty, class Density>
  class graph
  {
  };

  /// @brief @anchor SpatialGraphNoPropertyNoProperty A graph class with no information attached to either vertices nor edges.
  /// @details This graph structure is bidirected and allows to model a forest of disconnected graphs and isolated vertices.
  /// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
  template <class Density>
  class graph<no_property, no_property, Density> : public detail::graph_common<no_property, no_property, graph<no_property, no_property, Density>, Density>
  {
    using base = detail::graph_common<no_property, no_property, graph<no_property, no_property, Density>, Density>;

  public:

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit graph() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit graph(size_t n) : base(n) {}
    /// @}

  }; // end specialization graph<boost::no::property, boost::no_property>

  /// @brief @anchor SpatialGraphVertexPropertyNoProperty A graph class with information attached to vertices.
  /// @tparam VertexProperty The type of information to store at each vertex.
  /// @details Vertices embed a user-defined type of information. Edges embed no additional information.
  ///          This graph structure is bidirected and allows to model a forest of disconnected graphs and isolated vertices.
  /// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
  template <class VertexProperty, class Density>
    requires(!std::is_same_v<VertexProperty,no_property>)
  class graph<VertexProperty, no_property, Density> : public detail::graph_common<VertexProperty, no_property, graph<VertexProperty, no_property, Density>, Density>
  {
  private:
    using base = detail::graph_common<VertexProperty, no_property, graph<VertexProperty, no_property, Density>, Density>;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base::vertex_descriptor;

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit graph() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit graph(size_t n) : base(n) {}

    /// @}

    /// @name Topology Modifiers 
    /// @{

    /// @brief Add a vertex and its properties to the graph
    auto add_vertex( const VertexProperty &p )
    {
      return boost::add_vertex( p, this->_graph);
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


  }; // end specialization graph<Vertex, boost::no_property>


  /// @brief @anchor SpatialGraphNoPropertyEdgeProperty A graph class with information attached to edges.
  /// @tparam EdgeProperty The type of information to store at each edge.
  /// @details Vertices embed no additional information. Edges embed an user-defined type of information.
  ///          This graph structure is bidirected and allows to model a forest of disconnected graphs and isolated vertices.
  /// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
  template <class EdgeProperty, class Density>
    requires(!std::is_same_v<EdgeProperty, no_property>)
  class graph<no_property, EdgeProperty, Density> : public detail::graph_common<no_property, EdgeProperty, graph<no_property, EdgeProperty, Density>, Density>
  {
  private:
    using base = detail::graph_common<no_property, EdgeProperty, graph<no_property, EdgeProperty, Density>, Density>;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base::vertex_descriptor;

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializses a graph with 0 vertices.
    explicit graph() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit graph(size_t n) : base(n) {}

    /// @}

    /// @name Topology Modifiers 
    /// @n{

    //// @brief Inserts the edge \f$(u,v)\f$ into the graph if it does not exist, and returns an edge descriptor pointing to the new edge.
    /// @param u Source vertex
    /// @param u Target vertex
    edge_descriptor add_edge(vertex_descriptor u, vertex_descriptor v, const EdgeProperty & p) 
    {
      return boost::add_edge(p, u, v, this->_graph).first;
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
  ///          This graph structure is bidirected and allows to model a forest of disconnected graphs and isolated vertices.
  /// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
  template <class VertexProperty, class EdgeProperty, class Density>
    requires(!std::is_same_v<VertexProperty, no_property> && !std::is_same_v<EdgeProperty, no_property>)
  class graph<VertexProperty, EdgeProperty, Density> : public detail::graph_common<VertexProperty, EdgeProperty, graph<no_property, EdgeProperty, Density>, Density>
  {
  private:
    using base = detail::graph_common<VertexProperty, EdgeProperty, graph<no_property, EdgeProperty, Density>, Density>;
 
  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base::vertex_descriptor;

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    
    explicit graph() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit graph(size_t n) : base(n) {}

    ///@}

    /// @name Topology Modifiers 
    /// @{

    /// @brief Add a vertex and its properties to the graph
    auto add_vertex( const VertexProperty &p )
    {
      return boost::add_vertex( p, this->_graph);
    }

    //// @brief Inserts the edge \f$(u,v)\f$ into the graph if it does not exist, and returns an edge descriptor pointing to the new edge.
    /// @param u Source vertex
    /// @param u Target vertex
    /// @param p The edge property
    edge_descriptor add_edge(vertex_descriptor u, vertex_descriptor v, const EdgeProperty & p) 
    {
      return boost::add_edge(p, u, v, this->_graph).first;
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
