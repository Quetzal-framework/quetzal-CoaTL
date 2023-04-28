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

#include <boost/graph/graphviz.hpp>

#include "detail/tree_traits.hpp"
#include "detail/cardinal_k_ary_tree.hpp"
#include <utility>
#include <algorithm>
#include <iterator>
#include <tuple>

namespace quetzal::coalescence
{
  using no_property = boost::no_property;

  namespace detail
  {

    template <typename T1, typename... Ts>
    std::tuple<Ts...> unshift_tuple(const std::tuple<T1, Ts...> &tuple)
    {
      return std::apply([](auto &&, const auto &...args)
                        { return std::tie(args...); },
                        tuple);
    }

    // Getting std::map::emplace to work with aggregate initialization for POD EdgeProperties
    template <typename T>
    struct tag
    {
      using type = T;
    };

    template <typename F>
    struct initializer
    {
      F f;
      template <typename T>
      operator T() &&
      {
        return std::forward<F>(f)(tag<T>{});
      }
    };

    template <typename F>
    initializer(F &&) -> initializer<F>;

    template <typename... Args>
    auto initpack(Args &&...args)
    {
      return initializer{[&](auto t)
                         {
                           using Ret = typename decltype(t)::type;
                           return Ret{std::forward<Args>(args)...};
                         }};
    }

    template <typename VertexDescriptor, typename VertexProperty>
    struct VertexManager
    {
      using vertex_descriptor = VertexDescriptor;
      using vertex_hashmap_type = std::map<vertex_descriptor, VertexProperty>;

      vertex_hashmap_type _property_map;

      template <typename... Args>
      void add_vertex_to_manager(vertex_descriptor v, Args &&...args)
      {
        _property_map[v] = {std::forward<Args>(args)...};
      }

      const VertexProperty &operator[](vertex_descriptor v) const
      {
        return _property_map.at(v);
      }

      VertexProperty &operator[](vertex_descriptor v)
      {
        return _property_map[v];
      }
    };

    template <typename EdgeDescriptor, typename EdgeProperty>
    struct EdgeManager
    {
      using edge_descriptor = EdgeDescriptor;
      using vertex_descriptor = typename EdgeDescriptor::first_type;
      using edge_hashmap_type = std::map<edge_descriptor, EdgeProperty>;

      edge_hashmap_type _property_map;

      template <typename... Args>
      void
      add_edges(EdgeDescriptor u, std::tuple<Args...> left, EdgeDescriptor v, std::tuple<Args...> right)
      {
        _property_map.emplace(std::piecewise_construct,
                              std::forward_as_tuple(u),
                              std::forward_as_tuple(std::apply([](auto &&...args)
                                                               { return initpack(args...); },
                                                               left)));
        _property_map.emplace(std::piecewise_construct,
                              std::forward_as_tuple(v),
                              std::forward_as_tuple(std::apply([](auto &&...args)
                                                               { return initpack(args...); },
                                                               right)));
      }

      template <typename VertexDescriptor>
      const EdgeProperty &operator[](const std::pair<VertexDescriptor, VertexDescriptor> &edge) const
      {
        return _property_map.at(edge);
      }

      template <typename VertexDescriptor>
      EdgeProperty &operator[](const std::pair<VertexDescriptor, VertexDescriptor> &edge)
      {
        return _property_map.at(edge);
      }
    };

    namespace adl_resolution
    {
      void add_left_edge() = delete;
      void add_right_edge() = delete;
      void add_vertex() = delete;
      void out_degree() = delete;
      void root() = delete;
      void is_left_successor() = delete;
      void is_right_successor() = delete;
      void has_predecessor() = delete;
      void predecessor() = delete;
      void depth_first_search() = delete;
      void isomorphism() = delete;
      void in_edges() = delete;
      void degree() = delete;

    }

    template <class VertexProperty, class EdgeProperty>
    class binary_tree_common
    {
    protected:
      using base = boost::bidirectional_binary_tree<>;
      base _graph;

    public:
      /// @brief Default constructor
      binary_tree_common() : _graph() {}

      /// @brief Constructs a tree with \f$n\f$ vertices
      binary_tree_common(size_t n) : _graph(n) {}

      /// @brief Edge descriptor
      using edge_descriptor = typename base::edge_descriptor;

      /// @brief Vertex descriptor
      using vertex_descriptor = typename base::vertex_descriptor;

      /// @brief Degree size type
      using degree_size_type = typename base::degree_size_type;

      /// @brief The ways in which the vertices in the graph can be traversed.
      using traversal_category = typename base::traversal_category;

      /// @brief Bidirectional
      using directed_category = typename base::directed_category;

      /// @brief Iterate through the in-edges.
      using in_edge_iterator = base::in_edge_iterator;

      using edge_parallel_category = typename base::edge_parallel_category;

      /// @name Topology Lookup
      /// @{

      /// @brief Detects if there is a 1-to-1 mapping of the vertices in one graph to the vertices of another graph such that adjacency is preserved.
      /// @param other A binary tree graph.
      /// @return true if there exists an isomorphism between the two graphs and false otherwise
      bool is_isomorphic(binary_tree_common const &other) const
      {
        using detail::adl_resolution::isomorphism;
        return isomorphism(_graph, other._graph);
      }

      /// @brief Finds the root of the tree graph starting from a vertex \f$u\f$.
      /// @param u The vertex to start from.
      /// @remark This function will be an infinite loop if called on a recurrent
      ///         tree (which is not a tree any more).
      vertex_descriptor
      find_root_from(vertex_descriptor u) const
      {
        using detail::adl_resolution::root;
        return root(u, _graph);
      }

      /// @brief Returns the number of in-edges plus out-edges.
      /// @param u The vertex \f$u\f$
      /// @return Returns the number of in-edges plus out-edges.
      degree_size_type
      degree(vertex_descriptor u) const
      {
        using detail::adl_resolution::degree;
        return degree(u, _graph);
      }

      /// @brief Returns the number of in-edges of vertex \f$u\f$.
      /// @param u The vertex \f$u\f$
      /// @return The number of in-edges.
      degree_size_type
      in_degree(vertex_descriptor u) const
      {
        return has_predecessor(u);
      }

      /// @brief Returns the number of out-edges of vertex \f$v\f$.
      degree_size_type
      out_degree(vertex_descriptor v) const
      {
        using detail::adl_resolution::out_degree;
        return out_degree(v, _graph);
      }

      /// @brief Evaluates if vertex \f$u\f$ has a predecessor.
      /// @param u The vertex to evaluate
      /// @return True if u has a predecessor, false otherwise
      bool
      has_predecessor(vertex_descriptor u) const
      {
        using detail::adl_resolution::has_predecessor;
        return has_predecessor(u, _graph);
      }

      /// @brief The predecessor of vertex \f$u\f$
      /// @param u The vertex
      /// @return The vertex that is the predecessor of \f$u\f$.
      vertex_descriptor
      predecessor(vertex_descriptor u) const
      {
        using detail::adl_resolution::predecessor;
        return predecessor(u, _graph);
      }

      /// @brief Evaluate if vertex \f$u\f$ is a left successor.
      /// @param u The vertex to be evaluated
      /// @return True if u is a left successoir, false otherwise.
      bool
      is_left_successor(vertex_descriptor u) const
      {
        using detail::adl_resolution::is_left_successor;
        return is_left_successor(u, _graph);
      }

      /// @brief Evaluate if vertex \f$u\f$ is a right successor.
      /// @param u The vertex to be evaluated
      /// @return True if u is a right successoir, false otherwise.
      bool
      is_right_successor(vertex_descriptor u) const
      {
        using detail::adl_resolution::is_right_successor;
        return is_right_successor(u, _graph);
      }

      /// @}

      /// @name Iterators 
      /// @{

      /// @brief Provides iterators to iterate over the in-going edges of vertex \f$u\f$.
      /// @param u The vertex \f$u\f$.
      /// @return A pair of iterators.
      std::pair<in_edge_iterator, in_edge_iterator>
      in_edges(vertex_descriptor u) const
      {
        using detail::adl_resolution::in_edges;
        return in_edges(u, _graph);
      }

      /// @}

      /// @name Algorithms
      /// @{ 
      
      /// @brief Performs a read-and-write depth-first traversal of the vertices starting at vertex \f$s\f$.
      /// @tparam DFSTreeVisitor
      /// @param s The vertex to start from.
      /// @param vis The visitor to apply.
      template <typename DFSTreeVisitor>
      void
      depth_first_search(vertex_descriptor s, DFSTreeVisitor &vis)
      {
        using detail::adl_resolution::depth_first_search;
        return depth_first_search(_graph, s, vis);
      }

      /// @brief Performs a read-only depth-first traversal of the vertices starting at vertex \f$s\f$.
      /// @tparam DFSTreeVisitor
      /// @param s The vertex to start from.
      /// @param vis The visitor to apply.
      template <typename DFSTreeVisitor>
      void
      depth_first_search(vertex_descriptor s, DFSTreeVisitor &vis) const
      {
        using detail::adl_resolution::depth_first_search;
        return depth_first_search(_graph, s, vis);
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

      static constexpr inline vertex_descriptor null_vertex()
      {
        return base::null_vertex();
      }

    };

  } // end namespace detail

  template <class VertexProperty, class EdgeProperty>
  class binary_tree
  {
  };

  /// @brief A binary tree class where no property is attached to either vertices nor edges.
  template <>
  class binary_tree<no_property, no_property> : public detail::binary_tree_common<no_property, no_property>
  {
    using base = detail::binary_tree_common<no_property, no_property>;

  public:

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit binary_tree() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit binary_tree(size_t n) : base(n) {}
    /// @}

    /// @name Topology Modifiers
    /// @{

    /// @brief Add a vertex to the graph
    vertex_descriptor add_vertex()
    {
      using detail::adl_resolution::add_vertex;
      return add_vertex(_graph);
    }

    /// @brief Add edges between the parent vertex and the two children.
    std::pair<edge_descriptor, edge_descriptor>
    add_edges(vertex_descriptor parent, vertex_descriptor left, vertex_descriptor right)
    {
      assert(parent != left);
      assert(parent != right);
      assert(left != right);
      return std::make_pair(add_left_edge(parent, left, _graph), add_right_edge(parent, right, _graph));
    }

    /// @}

  }; // end specialization binary_tree<boost::no::property, boost::no_property>

  /// @brief A binary tree class where property is attached to vertices.
  /// @tparam VertexProperty The type of information to store at each vertex.
  template <class VertexProperty>
    requires(!std::is_same_v<VertexProperty,no_property>)
  class binary_tree<VertexProperty, no_property> : public detail::binary_tree_common<VertexProperty, no_property>
  {
  private:
    using base = detail::binary_tree_common<VertexProperty, no_property>;
    detail::VertexManager<typename base::vertex_descriptor, VertexProperty> _vertex_manager;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base::vertex_descriptor;

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit binary_tree() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit binary_tree(size_t n) : base(n) {}

    /// @}

    /// @name Topology Modifiers 
    /// @{

    /// @brief Add a vertex and its properties to the graph
    template <typename... Args>
    vertex_descriptor add_vertex(Args &&...args)
    {
      using detail::adl_resolution::add_vertex;
      vertex_descriptor v = add_vertex(this->_graph);
      _vertex_manager.add_vertex_to_manager(v, std::forward<Args>(args)...);
      return v;
    }

    /// @brief Add edges between the parent vertex and the two children.
    std::pair<edge_descriptor, edge_descriptor>
    add_edges(vertex_descriptor parent, vertex_descriptor left, vertex_descriptor right)
    {
      assert(parent != left);
      assert(parent != right);
      assert(left != right);
      return {add_left_edge(parent, left, this->_graph), add_right_edge(parent, right, this->_graph)};
    }

    /// @}

    /// @name Property Lookup 
    /// @{

    /// @brief Read only access to the vertex property
    const VertexProperty &operator[](vertex_descriptor v) const
    {
      return _vertex_manager[v];
    }

    /// @brief Read and write access to the vertex property
    VertexProperty &operator[](vertex_descriptor v)
    {
      return _vertex_manager[v];
    }

    /// @}


  }; // end specialization binary_tree<Vertex, boost::no_property>


  /// @brief A binary tree class where property is attached to edges
  /// @tparam EdgeProperty The type of information to store at each edge.
  template <class EdgeProperty>
    requires(!std::is_same_v<EdgeProperty, no_property>)
  class binary_tree<no_property, EdgeProperty> : public detail::binary_tree_common<no_property, EdgeProperty>
  {
  private:
    using base = detail::binary_tree_common<no_property, EdgeProperty>;
    detail::EdgeManager<typename base::edge_descriptor, EdgeProperty> _edge_manager;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base::vertex_descriptor;

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializses a graph with 0 vertices.
    explicit binary_tree() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit binary_tree(size_t n) : base(n) {}

    /// @}

    /// @name Topology Modifiers 
    /// @n{

    /// @brief Add a vertex to the graph.
    vertex_descriptor add_vertex()
    {
      using detail::adl_resolution::add_vertex;
      return add_vertex(this->_graph);
    }

    /// @brief Add edges between the parent vertex and the two children.
    template <typename... Args>
    std::pair<edge_descriptor, edge_descriptor>
    add_edges(vertex_descriptor parent,
              std::tuple<vertex_descriptor, Args...> left,
              std::tuple<vertex_descriptor, Args...> right)
    {
      assert(parent != get<0>(left));
      assert(parent != get<0>(right));
      assert(get<0>(left) != get<0>(right));

      auto left_edge = add_left_edge(parent, get<0>(left), this->_graph);
      auto right_edge = add_right_edge(parent, get<0>(right), this->_graph);

      _edge_manager.add_edges(left_edge, detail::unshift_tuple(left), right_edge, detail::unshift_tuple(right));
      return {left_edge, right_edge};
    }

    /// @}

    /// @name Property Lookup 
    /// @{

    /// @brief Read only access to the edge property
    const EdgeProperty &operator[](const edge_descriptor &edge) const
    {
      return _edge_manager[edge];
    }

    /// @brief Read and write access to the edge property
    EdgeProperty &operator[](const edge_descriptor &edge)
    {
      return _edge_manager[edge];
    }

    /// @}

  }; // end specialization binary_tree<boost::no_property, Edge>

  /// @brief A binary tree class with property attached to both edges and vertices.
  /// @tparam VertexProperty The type of information to store at each vertex.
  /// @tparam EdgeProperty The type of information to store at each edge.
  template <class VertexProperty, class EdgeProperty>
    requires(!std::is_same_v<VertexProperty, no_property> && !std::is_same_v<EdgeProperty, no_property>)
  class binary_tree<VertexProperty, EdgeProperty> : public detail::binary_tree_common<VertexProperty, EdgeProperty>
  {
  private:
    using base = detail::binary_tree_common<VertexProperty, EdgeProperty>;
    detail::EdgeManager<typename base::edge_descriptor, EdgeProperty> _edge_manager;
    detail::VertexManager<typename base::vertex_descriptor, VertexProperty> _vertex_manager;

  public:
    /// @brief Edge descriptor
    using edge_descriptor = typename base::edge_descriptor;

    /// @brief Vertex descriptor
    using vertex_descriptor = typename base::vertex_descriptor;

    /// @name Constructors 
    /// @{

    /// @brief Default constructor. Initializes a graph with 0 vertices.
    explicit binary_tree() : base() {}

    /// @brief Construct graph with \f$n\f$ vertices
    explicit binary_tree(size_t n) : base(n) {}

    ///@}

    /// @name Topology Modifiers 
    /// @{

    /// @brief Add a vertex and its properties to the graph
    template <typename... Args>
    vertex_descriptor
    add_vertex(Args &&...args)
    {
      using detail::adl_resolution::add_vertex;
      vertex_descriptor v = add_vertex(this->_graph);
      _vertex_manager.add_vertex_to_manager(v, std::forward<Args>(args)...);
      return v;
    }

    /// @brief Add edges between the parent vertex and the two children.
    template <typename... Args>
    std::pair<edge_descriptor, edge_descriptor>
    add_edges(vertex_descriptor parent,
              std::tuple<vertex_descriptor, Args...> left,
              std::tuple<vertex_descriptor, Args...> right)
    {
      assert(parent != get<0>(left));
      assert(parent != get<0>(right));
      assert(get<0>(left) != get<0>(right));
      auto left_edge = add_left_edge(parent, get<0>(left), this->_graph);
      auto right_edge = add_right_edge(parent, get<0>(right), this->_graph);
      _edge_manager.add_edges(left_edge, detail::unshift_tuple(left), right_edge, detail::unshift_tuple(right));
      return {left_edge, right_edge};
    }

    /// @}

    /// @name Property Lookup
    /// @{

    /// @brief Read only access to the vertex property
    const VertexProperty &operator[](vertex_descriptor v) const
    {
      return _vertex_manager[v];
    }

    /// @brief Read and write access to the vertex property
    VertexProperty &operator[](vertex_descriptor v)
    {
      return _vertex_manager[v];
    }

    /// @brief Read only access to the edge property
    const EdgeProperty &operator[](const edge_descriptor &edge) const
    {
      return _edge_manager[edge];
    }

    /// @brief Read and write access to the edge property
    EdgeProperty &operator[](const edge_descriptor &edge)
    {
      return _edge_manager[edge];
    }

    /// @}
  };

  namespace detail
  {
    template <typename Vertex, typename Edge, typename Generator>
    auto update_tree(
        binary_tree<Vertex, Edge> &tree,
        std::vector<typename binary_tree<Vertex, Edge>::vertex_descriptor> leaves,
        Generator &rng)
    {
      using tree_type = binary_tree<Vertex, Edge>;
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

        auto parent = tree.add_vertex();
        if constexpr (std::is_same_v<Edge, boost::no_property>)
          tree.add_edges(parent, update_tree(tree, left, rng), update_tree(tree, right, rng));
        else
          tree.add_edges(parent,
                         std::make_tuple(update_tree(tree, left, rng), Edge()),
                         std::make_tuple(update_tree(tree, right, rng), Edge()));
        return parent;
      }
    }
  }

  /// @brief Generate a random binary tree with no vertex/edge property attached.
  /// @tparam Generator Random Number Generator
  /// @param n_leaves Number of leaves in the binary tree
  /// @param rng The random number generator
  /// @return A binary tree with its root vertex descriptor
  template <typename Vertex = boost::no_property, typename Edge = boost::no_property, typename Generator>
  std::tuple<
      quetzal::coalescence::binary_tree<Vertex, Edge>,
      typename quetzal::coalescence::binary_tree<Vertex, Edge>::vertex_descriptor>
  get_random_binary_tree(int n_leaves, Generator &rng)
  {
    using tree_type = quetzal::coalescence::binary_tree<Vertex, Edge>;
    using vertex_descriptor = typename tree_type::vertex_descriptor;

    tree_type tree;
    std::vector<vertex_descriptor> leaves(n_leaves);
    std::transform(leaves.cbegin(), leaves.cend(), leaves.begin(), [&tree](auto)
                   { return tree.add_vertex(); });
    vertex_descriptor root = detail::update_tree(tree, leaves, rng);
    return std::tuple(std::move(tree), root);
  }
}

#endif