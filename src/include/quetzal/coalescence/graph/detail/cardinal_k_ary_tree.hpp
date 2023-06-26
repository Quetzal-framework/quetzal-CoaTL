//=======================================================================
// Copyright 2018 Jeremy William Murphy
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef BOOST_GRAPH_K_ARY_TREE
#define BOOST_GRAPH_K_ARY_TREE

#include <boost/config.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_mutability_traits.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/detail/indexed_properties.hpp>
#include <boost/graph/named_function_params.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/concept/assert.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/range.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include <stack>
#include <utility>

#include "concepts.hpp"
#include "visit.hpp"
#include "detail_k_ary_tree.hpp"

namespace boost
{
  template <typename Graph>
  bool empty(typename graph_traits<Graph>::vertex_descriptor u, Graph const &)
  {
    return u == graph_traits<Graph>::null_vertex();
  }

  /// @brief Binary tree base definition for further specialization
  ///
  /// @tparam Vertex 
  /// @tparam Predecessor : should predecessors be stored (bidirectional) or not (forward)
  ///
  /// @remarks Properties were not considered when implementing the class, so it's completely missing.
  ///
  template <bool Predecessor, typename Vertex = std::size_t>
  class binary_tree;

  ///
  /// @brief Forward Binary Tree
  ///
  /// @tparam Vertex 
  ///
  /// @remark Does not store predecessors
  ///
  template <typename Vertex>
  class binary_tree<false, Vertex>
    : public detail::binary_tree_base<Vertex, detail::binary_tree_forward_node<binary_tree<false, Vertex> > >
  {
    private:

    using super_t = detail::binary_tree_base
    <
      Vertex, 
      detail::binary_tree_forward_node<binary_tree<false, Vertex> > 
    >;

    public:

      /// @brief Directed, undirected or bidirectional
      using directed_category = directed_tag;

      /// @brief The ways in which the vertices in the graph can be traversed.
      class traversal_category : public incidence_graph_tag, public vertex_list_graph_tag {};

      /// @brief The type for the objects used to identify edges in the graph.
      using edge_descriptor = typename super_t::edge_descriptor;

      /// @brief The type for the objects used to identity vertices in the graph.
      using vertex_descriptor = typename super_t::vertex_descriptor;

      /// @brief Inherit constructors
      using super_t::super_t;

      /// @brief Add a left edge to the parent vertex.
      /// @param parent The parent vertex
      /// @param child The child vertex
      /// @param g The binary tree graph
      /// @return The descriptor of the edge added.
      friend
      edge_descriptor
      add_left_edge(vertex_descriptor parent, vertex_descriptor child, binary_tree &g)
      {
        BOOST_ASSERT(parent != child);

        return g.add_left_edge(parent, child);
      }

      /// @brief Add a left edge to the parent vertex.
      /// @param parent The parent vertex
      /// @param child The child vertex
      /// @param g The binary tree graph
      /// @return The descriptor of the edge added.
      friend
      edge_descriptor
      add_right_edge(vertex_descriptor parent, vertex_descriptor child, binary_tree &g)
      {
        BOOST_ASSERT(parent != child);

        return g.add_right_edge(parent, child);
      }


      /// @name MutableGraph interface
      /// @{

      /// @brief Inserts the edge (u,v) into the graph.
      /// @param u Vertex u
      /// @param v Vertex v
      /// @param g Binary tree graph
      /// @return an edge descriptor pointing to the new edge.   
      ///         If the edge (u,v) is already in the graph, 
      ///         then the bool flag returned is false and the returned edge descriptor points to 
      ///         the already existing edge.
      friend
      std::pair<edge_descriptor, bool>
      add_edge(vertex_descriptor u, vertex_descriptor v, binary_tree &g)
      {
        return g.add_edge(u, v);
      }

      /// @brief Remove the edge (u,v) from the graph.
      /// @param u Vertex u
      /// @param v Vertex v
      /// @param g The binary tree graph
      friend
      void
      remove_edge(vertex_descriptor u, vertex_descriptor v, binary_tree &g)
      {
        g.remove_edge(u, v);
      }

      /// @brief Remove the edge e from the graph.
      /// @param e The edge in the graph.
      /// @param g The binary tree graph.
      friend
      void
      remove_edge(edge_descriptor e, binary_tree &g)
      {
        remove_edge(e.first, e.second, g);
      }

      /// @brief Remove all edges to and from vertex u from the graph.
      /// @param u The vertex u
      /// @param g The binary tree graph.
      friend
      void
      clear_vertex(vertex_descriptor u, binary_tree &g)
      {
        g.clear_vertex(u);
      }

      /// @} // MutableGraph interface
  }; // end class binary_tree<false, Vertex>


  /// @brief Bidirectional Binary Tree
  ///
  /// @tparam Vertex 
  ///
  template <typename Vertex>
  class binary_tree<true, Vertex>
    : public detail::binary_tree_base<Vertex, detail::binary_tree_bidirectional_node<binary_tree<true, Vertex> > >
  {
    using super_t = detail::binary_tree_base<Vertex, detail::binary_tree_bidirectional_node<binary_tree<true, Vertex> > > ;

    public:

      /// @brief Directed, undirected or bidirectional
      using directed_category = bidirectional_tag ;

      /// @brief The ways in which the vertices in the graph can be traversed.
      class traversal_category : public bidirectional_graph_tag, public vertex_list_graph_tag {};

      /// @brief The type for the objects used to identify edges in the graph.
      using edge_descriptor = typename super_t::edge_descriptor;

      /// @brief The type for the objects used to identify vertices in the graph.
      using vertex_descriptor = typename super_t::vertex_descriptor;

      /// @brief The integer type for vertex degree.
      using degree_size_type = typename super_t::degree_size_type;

      using edge_parallel_category = typename super_t::edge_parallel_category;
      
      /// Inherits constructors
      using super_t::super_t;

      /// @brief Add a left edge to the parent vertex.
      /// @param parent The parent vertex
      /// @param child The child vertex
      /// @param g The binary tree graph
      /// @return The descriptor of the edge added.
      friend
      edge_descriptor
      add_left_edge(vertex_descriptor parent, vertex_descriptor child, binary_tree &g)
      {
        BOOST_ASSERT(parent != child);
        g.nodes[child].predecessor = parent;
        return g.add_left_edge(parent, child);
      }

      /// @brief Add a right edge to the parent vertex.
      /// @param parent The parent vertex
      /// @param child The child vertex
      /// @param g The binary tree graph
      /// @return The descriptor of the edge added
      friend
      edge_descriptor
      add_right_edge(vertex_descriptor parent, vertex_descriptor child, binary_tree &g)
      {
        BOOST_ASSERT(parent != child);
        g.nodes[child].predecessor = parent;
        return g.add_right_edge(parent, child);
      }

      /// @brief Finds the root of the tree graph starting from a vertex u.
      /// @param u The vertex to start from.
      /// @param g The binary tree graph.
      /// @remark This function will be an infinite loop if called on a recurrent
      ///         tree (which is not a tree any more).
      friend
      vertex_descriptor
      root(vertex_descriptor u, binary_tree const &g)
      {
        BOOST_ASSERT(!empty(u, g));

        while (has_predecessor(u, g)) {
          u = predecessor(u, g);
        }

        BOOST_ASSERT(u != graph_traits<binary_tree>::null_vertex());
        return u;
      }

      /// @brief Evaluate if a vertex is a left successor (child)
      /// @param u The vertex to be evaluated
      /// @param g The binary tree graph
      /// @return True if u is a left successoir, false otherwise.
      friend
      bool
      is_left_successor(vertex_descriptor u, binary_tree const &g)
      {
        BOOST_ASSERT(!empty(u, g));
        vertex_descriptor v = predecessor(u, g);
        return left_successor(v, g) == u;
      }

      /// @brief Evaluate if a vertex is a right successor (child)
      /// @param u The vertex to be evaluated
      /// @param g The binary tree graph
      /// @return True if u is a right successoir, false otherwise.
      friend
      bool
      is_right_successor(vertex_descriptor u, binary_tree const &g)
      {
        BOOST_ASSERT(!empty(u, g));

        vertex_descriptor v = predecessor(u, g);
        return right_successor(v, g) == u;
      }

      /// @brief Evaluates if the vertex has a predecessor (parent)
      /// @param u The vertex to evaluate
      /// @param g The binary tree graph
      /// @return True if u has a predecessor, false otherwise
      friend
      bool
      has_predecessor(vertex_descriptor u, binary_tree const &g)
      {
        BOOST_ASSERT(!empty(u, g));
        BOOST_ASSERT(u < g.nodes.size());
        return g[u].predecessor != graph_traits<binary_tree>::null_vertex();
      }

      /// @brief The predecessor of a given vertex
      /// @param u The vertex 
      /// @param g The binary tree graph.
      /// @return The vertex that is the predecessor of u.
      friend
      vertex_descriptor
      predecessor(vertex_descriptor u, binary_tree const &g)
      {
        BOOST_ASSERT(!empty(u, g));
        BOOST_ASSERT(u < g.nodes.size());

        return g[u].predecessor;
      }

    /// @name BidirectionalGraph interface
    /// @{

    private:

      struct make_in_edge_descriptor
      {
        make_in_edge_descriptor(vertex_descriptor target) : target(target) {}

        edge_descriptor operator()(vertex_descriptor source) const
        {
          return edge_descriptor(source, target);
        }

        vertex_descriptor target;
      };

    public:

      /// @brief Iterate through the in-edges.
      using in_edge_iterator = transform_iterator<make_in_edge_descriptor, vertex_descriptor const *, edge_descriptor>;

      /// @brief Provides iterators to iterate over the in-going edges of vertex u
      /// @param u The vertex u
      /// @param g The binary tree graph
      /// @return A pair of iterators
      /// @relates quetzal::coalescence::binary_tree
      friend
      std::pair<in_edge_iterator, in_edge_iterator>
      in_edges(vertex_descriptor u, binary_tree const &g)
      {
        auto const p = has_predecessor(u, g);
        return std::make_pair(in_edge_iterator(&g.nodes[u].predecessor, make_in_edge_descriptor(u)),
                              in_edge_iterator(&g.nodes[u].predecessor + p, make_in_edge_descriptor(u)));
      }

      /// @brief Returns the number of in-edges of vertex v in graph g.
      /// @param u The vertex u
      /// @param g The binary tree graph
      /// @return The number of in-edges.
      friend
      degree_size_type
      in_degree(vertex_descriptor u, binary_tree const &g)
      {
        return has_predecessor(u, g);
      }

      /// @brief Returns the number of in-edges plus out-edges.
      /// @param u The vertex u
      /// @param g The binary tree graph
      /// @return Returns the number of in-edges plus out-edges 
      friend
      degree_size_type
      degree(vertex_descriptor u, binary_tree const &g)
      {
        return in_degree(u, g) + out_degree(u, g);
      }
      /// @} // end BidirectionalGraph interface

      /// @name MutableGraph interface
      /// @{

      /// @brief Inserts the edge (u,v) into the graph.
      /// @remark If the graph disallows parallel edges, and the edge (u,v) is already in the graph,
      ///         then the bool flag returned is false and the returned edge descriptor points to the already existing edge.
      /// @param u Vertex u
      /// @param v Vertex v
      /// @param g Binary tree graph
      /// @return An edge descriptor pointing to the new edge
      friend
      std::pair<edge_descriptor, bool>
      add_edge(vertex_descriptor u, vertex_descriptor v, binary_tree &g)
      {
        if (!g.add_vertex(v) && predecessor(v, g) != g.null_vertex())
          return std::make_pair(edge_descriptor(), false);
        g.add_vertex(u);

        std::pair<edge_descriptor, bool> const result = g.add_edge_strict(u, v);
        if (result.second)
          g.nodes[v].predecessor = u;
        return result;
      }

      /// @brief Remove the edge (u,v) from the graph.
      /// @param u Vertex u
      /// @param v Vertex v
      /// @param g The binary tree graph
      friend
      void
      remove_edge(vertex_descriptor u, vertex_descriptor v, binary_tree &g)
      {
        BOOST_ASSERT(predecessor(v, g) == u);
        g.remove_edge(u, v);
        g.nodes[v].predecessor = super_t::null_vertex();
      }

      /// @brief Remove the edge e from the graph.
      /// @param e The edge to be removed
      /// @param g The binary tree graph
      friend
      void
      remove_edge(edge_descriptor e, binary_tree &g)
      {
        remove_edge(e.first, e.second, g);
      }

      /// @brief Remove all edges to and from vertex u from the graph.
      /// @param u Vertex u
      /// @param g The binary tree graph.
      friend
      void
      clear_vertex(vertex_descriptor u, binary_tree &g)
      {
        g.clear_childrens_predecessor(u);
        g.clear_vertex(u);
        g.nodes[u].predecessor = super_t::null_vertex();
      }

      /// @brief Remove u from the vertex set of the graph. 
      /// @note  Undefined behavior may result if there are edges remaining in the graph who's target is u. 
      ///        Typically the clear_vertex() function should be called first.
      /// @param u 
      /// @param g 
      friend
      void
      remove_vertex(vertex_descriptor u, binary_tree &g)
      {
        BOOST_ASSERT(in_degree(u, g) == 0);

        g.remove_vertex(u);
      }

      /// @} // MutableGraph interface

      /// @brief Clears the children's predecessor.
      /// @param u 
      void
      clear_childrens_predecessor(vertex_descriptor u)
      {
        for (int i = 0; i != 2; i++) {
          super_t::nodes[super_t::nodes[u].successors[i]].predecessor = super_t::null_vertex();
        }
      }
  };

  /// @brief Forward Binary Tree alias
  /// @tparam Vertex 
  template <typename Vertex = std::size_t>
  using forward_binary_tree = binary_tree<false, Vertex>;
  
  /// @brief Bidirectional Binary Tree alias
  /// @tparam Vertex
  template <typename Vertex = std::size_t>
  using bidirectional_binary_tree = binary_tree<true, Vertex>;
 
  // IncidenceGraph interface

  namespace detail
  {
    
    template <typename BinaryTree, typename Visitor>
    Visitor traverse_nonempty(vertex_descriptor_t<BinaryTree> u, BinaryTree const &g, Visitor vis)
    {
      vis(visit::pre, u);
      if (has_left_successor(u, g))
        vis = traverse_nonempty(left_successor(u, g), g, vis);
      vis(visit::in, u);
      if (has_right_successor(u, g))
        vis = traverse_nonempty(right_successor(u, g), g, vis);
      vis(visit::post, u);
      return vis;
    }


    template <typename BinaryTree>
    int traverse_step(visit &stage, vertex_descriptor_t<BinaryTree> &u, BinaryTree const &g)
    {
      BOOST_CONCEPT_ASSERT((concepts::BidirectionalBinaryTreeConcept<BinaryTree>));

      switch (stage)
      {

      case visit::pre:

        if (has_left_successor(u, g))
        {
          u = left_successor(u, g);
          return 1;
        }

        stage = visit::in;
        return 0;

      case visit::in:

        if (has_right_successor(u, g)) 
        {
          stage = visit::pre;
          u = right_successor(u, g);  
          return 1;
        } 

        stage = visit::post;
        return 0;

      case visit::post:

        if (is_left_successor(u, g))
        {
          stage = visit::in;
        }
        u = predecessor(u, g);
        return -1;
      }
    }

    template <typename BinaryTree, typename Visitor>
    Visitor traverse(vertex_descriptor_t<BinaryTree> u, BinaryTree const &g, Visitor vis)
    {
      if (empty(u, g))
        return vis;

      auto root = u;

      visit stage = visit::pre;
      vis(stage, u);

      do 
      {    
        traverse_step(stage, u, g);
        vis(stage, u);
      } while (u != root || stage != visit::post);

      return vis;
    }


    template <typename BinaryTree0, typename BinaryTree1>
    bool bifurcate_isomorphic_nonempty(
      vertex_descriptor_t<BinaryTree0> u, BinaryTree0 const &g,
      vertex_descriptor_t<BinaryTree1> v, BinaryTree1 const &h)
    {
      BOOST_CONCEPT_ASSERT((concepts::ForwardBinaryTreeConcept<BinaryTree0>));
      BOOST_CONCEPT_ASSERT((concepts::ForwardBinaryTreeConcept<BinaryTree1>));
      BOOST_ASSERT(!empty(u, g));
      BOOST_ASSERT(!empty(v, h));

      if (has_left_successor(u, g)) {
        if (has_left_successor(v, h)) {
          if (!bifurcate_isomorphic_nonempty(left_successor(u, g), g,
                                             left_successor(v, h), h))
            return false;
        } else
          return false;
      } else if (has_left_successor(u, g))
        return false;

      if (has_right_successor(u, g)) {
        if (has_right_successor(v, h)) {
          if (!bifurcate_isomorphic_nonempty(right_successor(u, g), g,
                                             right_successor(v, h), h))
            return false;
        } else
          return false;
      } else if (has_right_successor(u, g))
        return false;

      return true;
    }


    template <typename BinaryTree0, typename BinaryTree1>
    bool bifurcate_isomorphic(
                      vertex_descriptor_t<BinaryTree0> u, BinaryTree0 const &g,
                      vertex_descriptor_t<BinaryTree1> v, BinaryTree1 const &h)
    {
      BOOST_CONCEPT_ASSERT((concepts::BidirectionalBinaryTreeConcept<BinaryTree0>));
      BOOST_CONCEPT_ASSERT((concepts::BidirectionalBinaryTreeConcept<BinaryTree1>));

      if (empty(u, g)) return empty(v, h);
      if (empty(v, h)) return false;
      auto root0 = u;
      visit visit0 = visit::pre;
      visit visit1 = visit::pre;
      while (true) {
        traverse_step(visit0, u, g);
        traverse_step(visit1, v, h);
        if (visit0 != visit1) return false;
        if (u == root0 && visit0 == visit::post) return true;
      }
    }
  } // end namespace detail


  /// @brief performs a depth-first traversal of the vertices in a directed graph
  /// @tparam Vertex 
  /// @tparam DFSTreeVisitor 
  /// @param g The binary tree graph
  /// @param s The vertex to start from
  /// @param vis The visitor to apply
  template <typename Vertex, typename DFSTreeVisitor>
  void
  depth_first_search(binary_tree<false, Vertex> &g, vertex_descriptor_t<binary_tree<false, Vertex>> s, DFSTreeVisitor &vis)
  {
    if (!empty(s, g))
      vis = detail::traverse_nonempty(s, g, vis);
  }

  template <typename Vertex, typename DFSTreeVisitor>
  void
  depth_first_search(binary_tree<false, Vertex> const&g, vertex_descriptor_t<binary_tree<false, Vertex>> s, DFSTreeVisitor &vis)
  {
    if (!empty(s, g))
      vis = detail::traverse_nonempty(s, g, vis);
  }

  /// @brief performs a depth-first traversal of the vertices in a directed graph
  /// @tparam Vertex 
  /// @tparam DFSTreeVisitor 
  /// @param g The binary tree graph
  /// @param s The vertex to start from
  /// @param vis The visitor to apply
  template <typename Vertex, typename DFSTreeVisitor>
  void
  depth_first_search(binary_tree<true, Vertex> &g, vertex_descriptor_t<binary_tree<true, Vertex>> s, DFSTreeVisitor &vis)
  {
    vis = detail::traverse(s, g, vis);
  }

  template <typename Vertex, typename DFSTreeVisitor>
  void
  depth_first_search(binary_tree<true, Vertex> const&g, vertex_descriptor_t<binary_tree<true, Vertex>> s, DFSTreeVisitor &vis)
  {
    vis = detail::traverse(s, g, vis);
  }

  /// @brief Detects if there is a 1-to-1 mapping of the vertices in one graph to the vertices of another graph such that adjacency is preserved. 
  /// @tparam Vertex0 
  /// @tparam Vertex1 
  /// @param g A binary tree graph
  /// @param h A binary tree graph
  /// @return true if there exists an isomorphism between graph g and graph h and false otherwise
  template <typename Vertex0, typename Vertex1>
  bool
  isomorphism(binary_tree<false, Vertex0> const &g, binary_tree<false, Vertex1> const &h)
  {
    if (num_vertices(g) != num_vertices(h))
      return false;
    return num_vertices(g) == 0
           || detail::bifurcate_isomorphic_nonempty(0, g, 0, h);
  }

  /// @brief Detects if there is a 1-to-1 mapping of the vertices in one graph to the vertices of another graph such that adjacency is preserved. 
  /// @tparam Vertex0 
  /// @tparam Vertex1 
  /// @param g A binary tree graph
  /// @param h A binary tree graph
  /// @return true if there exists an isomorphism between graph g and graph h and false otherwise
  template <typename Vertex0, typename Vertex1>
  bool
  isomorphism(binary_tree<true, Vertex0> const &g, binary_tree<true, Vertex1> const &h)
  {
    if (num_vertices(g) != num_vertices(h))
      return false;
    return num_vertices(g) == 0 ||
           detail::bifurcate_isomorphic(0, g, 0, h);
  }
}

#endif // #ifndef BOOST_GRAPH_K_ARY_TREE

