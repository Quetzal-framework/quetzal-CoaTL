// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef NEWICK_FROM_KARY_TREE_H_INCLUDED
#define NEWICK_FROM_KARY_TREE_H_INCLUDED

#include "quetzal/io/newick/ast.hpp"
#include "quetzal/io/newick/parser.hpp"
#include "quetzal/io/newick/generator.hpp"
#include "quetzal/coalescence/graph/k_ary_tree.hpp"
#include "quetzal/coalescence/graph/binary_tree.hpp"

#include <concepts>
#include <string>
#include <algorithm>

namespace quetzal::format::newick
{

    //
    // @brief Generate a Newick string from a k-ary tree with no properties attached to edges or vertices
    //
    std::string generate_from(quetzal::coalescence::k_ary_tree<boost::no_property, boost::no_property> const &graph, auto flavor)
    {
        using tree_type = quetzal::coalescence::k_ary_tree<boost::no_property, boost::no_property>;
        using vertex_type = tree_type::vertex_descriptor;

        namespace newick = quetzal::format::newick;

        // Data access
        std::predicate<vertex_type> auto has_parent = [&graph](vertex_type const &v)
        { return graph.has_predecessor(v); };
        std::predicate<vertex_type> auto has_children = [&graph](vertex_type v)
        { return graph.has_successors(v); };
        newick::Formattable<vertex_type> auto branch_length = [](vertex_type)
        { return ""; };
        newick::Formattable<vertex_type> auto label = [](vertex_type v)
        { return ""; };

        // We declare a generator passing it the data interfaces
        newick::generator gen{has_parent, has_children, label, branch_length, flavor};
        using Gen = decltype(gen);
        using Stack = std::stack<int>;
        Stack nth_child;

        // We expose its interface to the boost DFS algorithm
        struct VisWrap : boost::default_dfs_visitor
        {
            Gen &gen_;
            Stack &stack_;
            VisWrap(Gen &gen, Stack &s) : gen_(gen), stack_(s) {}
            void discover_vertex(vertex_type v, tree_type const &) const
            {
                stack_.push(0);
                gen_.pre_order()(v);
            }
            void finish_vertex(vertex_type v, tree_type const &) const
            {
                gen_.post_order()(v);
                stack_.pop();
            }
            void tree_edge(tree_type::edge_descriptor e, tree_type const &g) const
            {
                if (stack_.top()++ > 0)
                    gen_.in_order()();
            }
        } vis{gen, nth_child};

        depth_first_search(graph, boost::visitor(vis));
        return gen.take_result();
    }

    //
    // @brief Generate a Newick string from a k-ary tree with no properties attached to edges or vertices
    //
    template<typename Flavor>
    std::string generate_from(quetzal::coalescence::binary_tree<boost::no_property, boost::no_property> const &graph,
                              typename quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>::vertex_descriptor root, 
                              Flavor flavor)
    {
        using tree_type = quetzal::coalescence::binary_tree<boost::no_property, boost::no_property>;
        using vertex_type = typename tree_type::vertex_descriptor;
        namespace newick = quetzal::format::newick;

        // Data access
        std::predicate<vertex_type> auto has_parent = [&graph](vertex_type const &v)
        { return graph.has_predecessor(v); };

        std::predicate<vertex_type> auto has_children = [&graph](vertex_type v)
        { return static_cast<bool>(graph.out_degree(v)); };

        newick::Formattable<vertex_type> auto branch_length = [](vertex_type)
        {  return ""; };

        newick::Formattable<vertex_type> auto label = [](vertex_type)
        { return ""; };

        // We declare a generator passing it the data interfaces
        newick::generator gen{has_parent, has_children, label, branch_length, flavor};
        using Gen = decltype(gen);
        
        // We expose its interface to the boost DFS algorithm
        struct VisWrap
        {
            std::reference_wrapper<Gen> _generator;
            boost::visit _previous;
            VisWrap(Gen &gen) : _generator(gen) {}
            void operator()(boost::visit stage, vertex_type v)
            {
                switch (stage)
                {
                case boost::visit::pre:
                    _generator.get().pre_order()(v);
                    _previous = boost::visit::pre;
                    break;
                case boost::visit::in:
                    if (_previous == boost::visit::post)
                        _generator.get().in_order()();
                    _previous = boost::visit::in;
                    break;
                case boost::visit::post:
                    _generator.get().post_order()(v);
                    _previous = boost::visit::post;
                    break;
                }
            }
        } vis{gen};

        graph.depth_first_search(root, vis);
        return gen.take_result();
    }

    //
    // @brief Generate a Newick string from a k-ary tree with properties attached to edges and vertices
    //
    template<class VertexProperty, class EdgeProperty, typename Flavor>
    requires (!std::is_same_v<VertexProperty, boost::no_property> &&  !std::is_same_v<VertexProperty, boost::no_property>)
    std::string generate_from(quetzal::coalescence::binary_tree<VertexProperty, EdgeProperty> const &graph,
                              typename quetzal::coalescence::binary_tree<VertexProperty, EdgeProperty>::vertex_descriptor root, 
                              Flavor flavor)
    {
        using tree_type = quetzal::coalescence::binary_tree<VertexProperty, EdgeProperty>;
        using vertex_descriptor = typename tree_type::vertex_descriptor;
        using edge_descriptor = typename tree_type::edge_descriptor;
        namespace newick = quetzal::format::newick;

        // Data access
        std::predicate<vertex_descriptor> auto has_parent = [&graph](vertex_descriptor const &v)
        { return graph.has_predecessor(v); };

        std::predicate<vertex_descriptor> auto has_children = [&graph](vertex_descriptor v)
        { return static_cast<bool>(graph.out_degree(v)); };

        newick::Formattable<vertex_descriptor> auto branch_length = [&graph = std::as_const(graph)](vertex_descriptor v)
        {  
            std::string s = graph.has_predecessor(v)? graph[ std::make_pair(graph.predecessor(v), v)].label() : "" ;
            return s;
        };

        newick::Formattable<vertex_descriptor> auto label = [&graph](vertex_descriptor v)
        { return graph[v].label(); };

        // We declare a generator passing it the data interfaces
        newick::generator gen{has_parent, has_children, label, branch_length, flavor};
        using Gen = decltype(gen);
        
        // We expose its interface to the boost DFS algorithm
        struct VisWrap
        {
            std::reference_wrapper<Gen> _generator;
            boost::visit _previous;
            VisWrap(Gen &gen) : _generator(gen) {}
            void operator()(boost::visit stage, vertex_descriptor v)
            {
                switch (stage)
                {
                case boost::visit::pre:
                    _generator.get().pre_order()(v);
                    _previous = boost::visit::pre;
                    break;
                case boost::visit::in:
                    if (_previous == boost::visit::post)
                        _generator.get().in_order()();
                    _previous = boost::visit::in;
                    break;
                case boost::visit::post:
                    _generator.get().post_order()(v);
                    _previous = boost::visit::post;
                    break;
                }
            }
        } vis{gen};

        graph.depth_first_search(root, vis);
        return gen.take_result();
    }

    // ///
    // /// @brief Generate a Newick string from a k-ary tree with properties
    // ///
    // template<class VertexProperties, class EdgeProperties>
    // std::string generate_from(quetzal::coalescence::k_ary_tree<VertexProperties, EdgeProperties> graph)
    // {
    //   namespace newick = quetzal::format::newick;

    //   using vertex_t = typename quetzal::coalescence::k_ary_tree<>::vertex_descriptor;

    //   std::predicate<vertex_t>      auto has_parent    = [&graph](vertex_t v){return graph.has_parent(v);};
    //   std::predicate<vertex_t>      auto has_children  = [&graph](vertex_t v){return graph.has_children(v);};
    //   newick::Formattable<vertex_t> auto label         = [&graph](vertex_t v ){return graph[v];};
    //   newick::Formattable<vertex_t> auto branch_length = [&graph](vertex_t v)
    //   {
    //     std::string result;
    //     auto [it1, it2] = out_edges(v, graph);
    //     assert(std::distance(it1,it2) <= 1);
    //     if(std::distance(it1,it2) == 1) result = std::to_string(graph[*it1]);
    //     return result;
    //   };

    //   // We declare a generator passing it the interfaces
    //   auto generator = newick::make_generator(has_parent, has_children, label, branch_length);

    //   // We expose its interface to the user-defined class DFS algorithm
    //   graph.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

    // }

} // end namespace quetzal::format::newick

#endif