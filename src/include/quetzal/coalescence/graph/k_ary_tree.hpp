// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "detail/tree_traits.hpp"
#include "detail/visit.hpp"

#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/isomorphism.hpp>
#include <boost/graph/topological_sort.hpp>

#include <boost/graph/random.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/random_spanning_tree.hpp>

#include "detail/adl_resolution.hpp"
#include "detail/utils.hpp"

#include <iostream>
#include <algorithm>

#include <range/v3/all.hpp>

namespace quetzal::coalescence
{
	using no_property = boost::no_property;

	namespace detail
	{
    
    // Base class and common implementation
		template <class VertexProperty, class EdgeProperty, class CRTP>
		class k_ary_tree_common
		{
		protected:

			/// @brief The type of graph hold by the tree class
			using base = tree_traits::model<
				tree_traits::out_edge_list_type,
				tree_traits::vertex_list_type,
				tree_traits::directed_type,
				VertexProperty,
				EdgeProperty>;

			base _graph;

		public:
			/// @brief Default constructor
			explicit k_ary_tree_common() : _graph() {}

			/// @brief Constructs a tree with \f$n\f$ vertices
			explicit k_ary_tree_common(size_t n) : _graph(n) {}

			/// @brief Vertex descriptor ("node ID" in other words).
			using vertex_descriptor = typename base::vertex_descriptor;

			/// @brief Edge descriptor
			using edge_descriptor = typename base::edge_descriptor;

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
			/// @param other A k-ary tree graph.
			/// @return true if there exists an isomorphism between the two graphs and false otherwise
			bool is_isomorphic(k_ary_tree_common const &other) const
			{
				return boost::isomorphism(_graph, other._graph);
			}

			/// @brief Finds the root of the tree graph starting from a vertex \f$u\f$.
			/// @param u The vertex to start from.
			/// @remark This function will be an infinite loop if called on a recurrent
			///         tree (which is not a tree any more).
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
				return has_predecessor(u);
			}

			/// @brief Returns the number of out-edges of vertex \f$v\f$.
			degree_size_type out_degree(vertex_descriptor v) const
			{
				return boost::out_degree(v, _graph);
			}

			/// @brief Evaluates if vertex \f$u\f$ has a predecessor.
			/// @param u The vertex to evaluate
			/// @return True if u has a predecessor, false otherwise
			bool has_predecessor(vertex_descriptor u) const
			{
        auto range = boost::in_edges(u, _graph);
        return std::distance(range.first, range.second) > 0;
			}

			/// @brief The predecessor of vertex \f$u\f$
			/// @param u The vertex
			/// @return The vertex that is the predecessor of \f$u\f$.
			vertex_descriptor predecessor(vertex_descriptor u) const
			{
        auto range = boost::in_edges(u, _graph);
        assert(std::distance(range.first, range.second) == 1 );
        return source(*range.first);
			}

			/// @brief Evaluates if vertex \f$u\f$ has successors.
			/// @param u The vertex to evaluate
			/// @return True if u has successors, false otherwise
			bool has_successors(vertex_descriptor u) const
			{
        auto range = boost::out_edges(u, _graph);
        assert(std::distance(range.first, range.second) != 1);
        return std::distance(range.first, range.second) >= 2;
			}

			/// @brief The successors of vertex \f$u\f$
			/// @param u The vertex
			/// @return A pair of iterators on the vertices that are the successors of \f$u\f$.
			auto successors(vertex_descriptor u) const
			{
        return boost::make_iterator_range(out_edges(u, _graph))
                | boost::adaptors::transformed([this](auto it){return target(it); } );
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

			/// @name Iterators
			/// @{

			/// @brief Provides iterators to iterate over the in-going edges of vertex \f$u\f$.
			/// @param u The vertex \f$u\f$.
			/// @return A pair of iterators.
			std::pair<in_edge_iterator, in_edge_iterator> in_edges(vertex_descriptor u) const
			{
				using detail::adl_resolution::in_edges;
				return in_edges(u, _graph);
			}

			/// @}

			/// @name Algorithms
			/// @{

			/// @brief Performs a read-and-write depth-first traversal of the vertices starting at vertex \f$s\f$.
			/// @tparam DFSTreeVisitor
			/// @param start The vertex to start from.
			/// @param vis The visitor to apply.
			template <typename DFSTreeVisitor>
			void depth_first_search(vertex_descriptor start, DFSTreeVisitor &vis)
			{
        // convert tree visitor to graph visitor
        struct VisWrap : boost::default_dfs_visitor
        {
          k_ary_tree_common &_tree;
          DFSTreeVisitor &_tree_visitor;
          VisWrap(k_ary_tree_common &tree, DFSTreeVisitor &v) : _tree(tree), _tree_visitor(v) {}
          void discover_vertex(vertex_descriptor v, const base& g) { _tree_visitor(boost::visit::pre, v); }
          void tree_edge(const edge_descriptor & e, const base& g)  { _tree_visitor(boost::visit::in, _tree.target(e) ); }
          void finish_vertex(vertex_descriptor v, const base& g)  { _tree_visitor(boost::visit::post, v); }
        } graph_visitor(*this, vis);
        std::vector<boost::default_color_type> colors(boost::num_vertices(_graph));
        boost::iterator_property_map color_map(colors.begin(), boost::get(boost::vertex_index, _graph));
				return boost::depth_first_search(_graph, boost::visitor(graph_visitor).color_map(color_map).root_vertex(start));
			}

			/// @brief Performs a read-only depth-first traversal of the vertices starting at vertex \f$s\f$.
			/// @tparam DFSTreeVisitor
			/// @param start The vertex to start from.
			/// @param vis The visitor to apply.
			template <typename DFSTreeVisitor>
			void depth_first_search(vertex_descriptor start, DFSTreeVisitor &vis) const
			{
        // convert tree visitor to graph visitor
        struct VisWrap : boost::default_dfs_visitor
        {
          const k_ary_tree_common &_tree;
          DFSTreeVisitor &_tree_visitor;
          VisWrap(const k_ary_tree_common &tree, DFSTreeVisitor &v) : _tree(tree), _tree_visitor(v) {}
          void discover_vertex(vertex_descriptor v, const base& g) { _tree_visitor(boost::visit::pre, v); }
          void tree_edge(const edge_descriptor & e, const base& g)  { _tree_visitor(boost::visit::in, _tree.target(e) ); }
          void finish_vertex(vertex_descriptor v, const base& g)  { _tree_visitor(boost::visit::post, v); }
        } graph_visitor(*this, vis);
        std::vector<boost::default_color_type> colors(boost::num_vertices(_graph));
        boost::iterator_property_map color_map(colors.begin(), boost::get(boost::vertex_index, _graph));
				return boost::depth_first_search(_graph, boost::visitor(graph_visitor).color_map(color_map).root_vertex(start));
			}

			/// @}

			/// @name Input/Output
			/// @{

			/// @brief Print the tree to the graphviz format.
			void to_graphviz(std::ostream &out) const
			{
				using namespace boost;
				return boost::write_graphviz(
					out, *this,
					boost::make_label_writer(boost::get(vertex_bundle, *this)),
					boost::make_label_writer(boost::get(edge_bundle, *this)));
			}

			/// @}

			/// @brief Null vertex identifier
			/// @return A null vertex
			static constexpr inline vertex_descriptor null_vertex()
			{
				return base::null_vertex();
			}

      private:

      template <typename Generator>
      static auto update_tree(
          CRTP &tree,
          std::vector<vertex_descriptor> leaves,
          Generator &rng)
      {
        using tree_type = CRTP;

        if (leaves.size() == 1)
        {
          return leaves.front();
        }
        else
        {
          std::uniform_int_distribution<> distrib(1, leaves.size() - 1);
          int split = distrib(rng);

          std::vector<vertex_descriptor> left(leaves.begin(), leaves.begin() + split);
          std::vector<vertex_descriptor> right(leaves.begin() + split, leaves.end());

          auto parent = boost::add_vertex(tree._graph);
          if constexpr (std::is_same_v<EdgeProperty, boost::no_property>)
            tree.add_edges(parent, { update_tree(tree, left, rng), update_tree(tree, right, rng) });
          else
            tree.add_edges(parent,
                          { std::make_tuple(update_tree(tree, left, rng), EdgeProperty()),
                          std::make_tuple(update_tree(tree, right, rng), EdgeProperty()) } );
          return parent;
        }
      }

      public:

      template<typename Generator>
      static std::tuple<CRTP, vertex_descriptor> 
      make_random_tree(int n_leaves, int k_max, Generator& rng)
      {
        CRTP tree;
        std::vector<vertex_descriptor> leaves(n_leaves);
        std::transform(leaves.cbegin(), leaves.cend(), leaves.begin(), 
                       [&tree](auto){ return tree.add_vertex(); });
        vertex_descriptor root = update_tree(tree, leaves, rng);
        return std::tuple(std::move(tree), root);
      }



		}; // end class k_ary_tree_common

	} // end namespace detail


  template <class VertexProperty, class EdgeProperty>
  class k_ary_tree
  {
  };

  /// @brief @anchor CoalescenceKaryTreeNoPropertyNoProperty A k-ary tree class with no information attached to either vertices nor edges.
  /// @details This class can be used as a simple way to describe the kind of topology that emerges from evolutionary relationships 
  ///          between a sample of sequences for a non-recombining locus. 
  ///          Vertices and edges embed no additional information.
  ///          This graph structure is bidirected and allows to model a forest of disconnected trees and isolated vertices.
  /// @invariant Guarantees that each vertex \f$v\f$ has exactly 0 or \f$ k \geq 2\f$ successors, never 1.
  /// @invariant Guarantees that each vertex \f$v\f$ has exactly 0 or 1 predecessor.
  /// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
  /// @remark As it would be too costly to enforce non-cyclicity, it is left to the user responsability not to introduce any cycle.
  /// @section ex1 Example
  /// @include coalescence_k_ary_tree_1.cpp
  /// @section ex2 Output
  /// @include coalescence_k_ary_tree_1.txt
  template <>
  class k_ary_tree<no_property, no_property> : public detail::k_ary_tree_common<no_property, no_property, k_ary_tree<no_property, no_property>>
  {
    using base = detail::k_ary_tree_common<no_property, no_property, k_ary_tree<no_property, no_property>>;

  public:

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit k_ary_tree() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit k_ary_tree(size_t n) : base(n) {}
    /// @}

    /// @name Topology Modifiers
    /// @{

    /// @brief Add a vertex to the graph
    vertex_descriptor add_vertex()
    {
      return boost::add_vertex(_graph);
    }

    /// @brief Add edges from the parent vertex (source) to the children (target)
    std::vector<edge_descriptor>
    add_edges(vertex_descriptor parent, std::vector<vertex_descriptor> children)
    {
      assert( children.size() > 1);
      for(auto const& c : children){ assert(parent != c); }

      std::vector<edge_descriptor> edges (children.size());
      std::transform(children.cbegin(), 
                     children.cend(), 
                     edges.begin(), 
                     [parent, this](auto c){ return boost::add_edge(parent, c, this->_graph).first; } );
      return edges;
    }

    /// @}

  }; // end specialization k_ary_tree<boost::no::property, boost::no_property>

  /// @brief @anchor CoalescenceKaryTreeVertexPropertyNoProperty A k-ary tree class with information attached to vertices.
  /// @tparam VertexProperty The type of information to store at each vertex.
  /// @details This class can be used as a simple way to describe the kind of topology and mutational process that emerge from evolutionary relationships 
  ///          between a sample of sequences for a non-recombining locus. 
  ///          Vertices embed a user-defined type of information. Edges embed no additional information.
  ///          This graph structure is bidirected and allows to model a forest of disconnected trees and isolated vertices.
  /// @invariant Guarantees that each vertex \f$v\f$ has exactly \f$0\f$ or \f$n \geq 2\f$ successors, never \f$1\f$.
  /// @invariant Guarantees that each vertex \f$v\f$ has exactly\f$0\f$ or\f$1\f$ predecessor.
  /// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
  /// @remark As it would be too costly to enforce non-cyclicity, it is left to the user responsability not to introduce any cycle.
  /// @section ex1 Example
  /// @include coalescence_k_ary_tree_2.cpp
  /// @section ex2 Output
  /// @include coalescence_k_ary_tree_2.txt
  template <class VertexProperty>
    requires(!std::is_same_v<VertexProperty,no_property>)
  class k_ary_tree<VertexProperty, no_property> : public detail::k_ary_tree_common<VertexProperty, no_property, k_ary_tree<VertexProperty, no_property>>
  {
  private:
    using base = detail::k_ary_tree_common<VertexProperty, no_property, k_ary_tree<VertexProperty, no_property>>;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base::vertex_descriptor;

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit k_ary_tree() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit k_ary_tree(size_t n) : base(n) {}

    /// @}

    /// @name Topology Modifiers 
    /// @{

    /// @brief Add a vertex and its properties to the graph
    auto add_vertex( const VertexProperty &p )
    {
      return boost::add_vertex( p, this->_graph);
    }

    /// @brief Add edges from the parent vertex (source) to the children (target)
    std::vector<edge_descriptor>
    add_edges(vertex_descriptor parent, std::vector<vertex_descriptor> children)
    {
      assert( children.size() > 1);
      for(auto const& c : children){ assert(parent != c); }

      std::vector<edge_descriptor> edges (children.size());
      std::transform(children.cbegin(), 
                     children.cend(), 
                     edges.begin(), 
                     [parent, this](auto c){ return boost::add_edge(parent, c, this->_graph).first; } );
      return edges;
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


  }; // end specialization k_ary_tree<Vertex, boost::no_property>


  /// @brief @anchor CoalescenceKaryTreeNoPropertyEdgeProperty A k-ary tree class with information attached to edges.
  /// @tparam EdgeProperty The type of information to store at each edge.
  /// @details This class can be used as a simple way to describe the kind of topology and mutational process that emerge from evolutionary relationships 
  ///          between a sample of sequences for a non-recombining locus. 
  ///          Vertices embed no additional information. Edges embed an user-defined type of information.
  ///          This graph structure is bidirected and allows to model a forest of disconnected trees and isolated vertices.
  /// @invariant Guarantees that each vertex \f$v\f$ has exactly \f$0\f$ or \f$n \geq 2\f$ successors, never \f$1\f$.
  /// @invariant Guarantees that each vertex \f$v\f$ has exactly\f$0\f$ or\f$1\f$ predecessor.
  /// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
  /// @remark As it would be too costly to enforce non-cyclicity, it is left to the user responsability not to introduce any cycle.
    /// @section ex1 Example
  /// @include coalescence_k_ary_tree_3.cpp
  /// @section ex2 Output
  /// @include coalescence_k_ary_tree_3.txt
  template <class EdgeProperty>
    requires(!std::is_same_v<EdgeProperty, no_property>)
  class k_ary_tree<no_property, EdgeProperty> : public detail::k_ary_tree_common<no_property, EdgeProperty, k_ary_tree<no_property, EdgeProperty>>
  {
  private:
    using base = detail::k_ary_tree_common<no_property, EdgeProperty, k_ary_tree<no_property, EdgeProperty>>;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base::vertex_descriptor;

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializses a graph with 0 vertices.
    explicit k_ary_tree() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit k_ary_tree(size_t n) : base(n) {}

    /// @}

    /// @name Topology Modifiers 
    /// @n{

    /// @brief Add a vertex to the graph.
    vertex_descriptor add_vertex()
    {
      return boost::add_vertex(this->_graph);
    }

    /// @brief Add edges from the parent vertex (source) to the children (target)
    std::vector<edge_descriptor>
    add_edges(vertex_descriptor parent, const std::vector<std::pair<vertex_descriptor, EdgeProperty>> &children)
    {
      assert( children.size() > 1);
      for(auto const& c : children){ assert(parent != c); }

      std::vector<edge_descriptor> edges (children.size());
      std::transform(children.cbegin(), 
                     children.cend(), 
                     edges.begin(), 
                     [parent, this](const auto& c){ 
                      return boost::add_edge(parent, c.first, c.second, this->_graph).first; 
                      } );
      return edges;
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

  }; // end specialization k_ary_tree<boost::no_property, Edge>


  /// @brief @anchor CoalescenceKaryTreeVertexPropertyEdgeProperty A k-ary tree class with information attached to vertices.
  /// @tparam VertexProperty The type of information to store at each vertex.
  /// @tparam EdgeProperty The type of information to store at each edge.
  /// @details This class can be used as a simple way to describe the kind of topology and mutational process that emerge from evolutionary relationships 
  ///          between a sample of sequences for a non-recombining locus. 
  ///          Vertices and edges embed additional information as user-defined types.
  ///          This graph structure is bidirected and allows to model a forest of disconnected trees and isolated vertices.
  /// @invariant Guarantees that each vertex \f$v\f$ has exactly \f$0\f$ or \f$n \geq 2\f$ successors, never \f$1\f$.
  /// @invariant Guarantees that each vertex \f$v\f$ has exactly\f$0\f$ or\f$1\f$ predecessor.
  /// @invariant Guarantees that if \f$(u,v)\f$ is an edge of the graph, then \f$(v,u)\f$ is also an edge.
  /// @remark As it would be too costly to enforce non-cyclicity, it is left to the user responsability not to introduce any cycle.
  /// @section ex1 Example
  /// @include coalescence_k_ary_tree_4.cpp
  /// @section ex2 Output
  /// @include coalescence_k_ary_tree_4.txt
  template <class VertexProperty, class EdgeProperty>
    requires(!std::is_same_v<VertexProperty, no_property> && !std::is_same_v<EdgeProperty, no_property>)
  class k_ary_tree<VertexProperty, EdgeProperty> : public detail::k_ary_tree_common<VertexProperty, EdgeProperty, k_ary_tree<no_property, EdgeProperty>>
  {
  private:
    using base = detail::k_ary_tree_common<VertexProperty, EdgeProperty, k_ary_tree<no_property, EdgeProperty>>;
 
  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base::vertex_descriptor;

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit k_ary_tree() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit k_ary_tree(size_t n) : base(n) {}

    ///@}

    /// @name Topology Modifiers 
    /// @{

    /// @brief Add a vertex and its properties to the graph
    vertex_descriptor
    add_vertex(const VertexProperty &p)
    {
      return boost::add_vertex( p , this->_graph);
    }

    /// @brief Add edges from the parent vertex (source) to the children (target)
    std::vector<edge_descriptor>
    add_edges(vertex_descriptor parent,
              std::vector<std::tuple<vertex_descriptor, EdgeProperty>> children)
    {
      assert( children.size() > 1);
      for(auto const& c : children){ assert(parent != c); }

      std::vector<edge_descriptor> edges (children.size());
      std::transform(children.cbegin(), 
                     children.cend(), 
                     edges.begin(), 
                     [parent, this](const auto& c){ 
                      return boost::add_edge(parent, std::get<0>(c), std::get<1>(c), this->_graph).first; 
                      } );
      return edges;
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

  namespace detail
  {
    template <typename Vertex, typename Edge, typename Generator>
    auto update_tree(
        k_ary_tree<Vertex, Edge> &tree,
        std::vector<typename k_ary_tree<Vertex, Edge>::vertex_descriptor> leaves,
        Generator &rng)
    {
      using tree_type = k_ary_tree<Vertex, Edge>;
      using vertex_descriptor = typename tree_type::vertex_descriptor;

      if (leaves.size() == 1)
      {
        return leaves.front();
      }
      else
      {
        std::uniform_int_distribution<> distrib(1, leaves.size() - 1);
        int split = distrib(rng);

        std::vector<vertex_descriptor> left(leaves.begin(), leaves.begin() + split);
        std::vector<vertex_descriptor> right(leaves.begin() + split, leaves.end());

        vertex_descriptor parent;
        
        if constexpr (std::is_same_v<Vertex, boost::no_property>)
          parent = tree.add_vertex();
        else
          parent = tree.add_vertex(Vertex());

        if constexpr (std::is_same_v<Edge, boost::no_property>)
          tree.add_edges(parent, update_tree(tree, left, rng), update_tree(tree, right, rng));
        else
          tree.add_edges(parent, { { update_tree(tree, left, rng), Edge() }, { update_tree(tree, right, rng), Edge()} } );
        return parent;
      }
    }
  } // end namespace detail

  /// @brief Generate a random k-ary tree.
  /// @tparam Generator Random Number Generator
  /// @param n_leaves Number of leaves in the k-ary tree
  /// @param rng The random number generator
  /// @return A k-ary tree with its root vertex descriptor
  template <typename Vertex = boost::no_property, typename Edge = boost::no_property, typename Generator>
  std::pair< quetzal::coalescence::k_ary_tree<Vertex, Edge>, typename quetzal::coalescence::k_ary_tree<Vertex, Edge>::vertex_descriptor>
  get_random_k_ary_tree(int n_leaves, Generator &rng)
  {
    using tree_type = quetzal::coalescence::k_ary_tree<Vertex, Edge>;
    using vertex_descriptor = typename tree_type::vertex_descriptor;

    tree_type tree;

    std::vector<vertex_descriptor> leaves(n_leaves);

    std::transform(leaves.cbegin(), leaves.cend(), leaves.begin(),
      [&tree](auto)
      {
        if constexpr (std::is_same_v<Vertex, boost::no_property>)
          return tree.add_vertex();
        else
          return tree.add_vertex(Vertex());
      });

    vertex_descriptor root = detail::update_tree(tree, leaves, rng);
    return std::tuple(std::move(tree), root);
  }
	
} // end namespace quetzal::coalescence
