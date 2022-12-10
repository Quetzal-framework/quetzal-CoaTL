// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef TREE_BGL_H_INCLUDED
#define TREE_BGL_H_INCLUDED

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <iostream>

namespace quetzal::coalescence
{
	///
	/// @brief Defines the desired properties and constraints for a coalescent graph
	///
	struct tree_traits
	{
		/// @brief Trees are sparse graph in nature, adjacency_matrix would not be justified here
		template <class... Types> using model = boost::adjacency_list<Types...>;
		/// @brief We want to enforce avoiding multi-graphs (edges with same end nodes)
		using out_edge_list_type              = boost::setS;
		/// @brief We don't allow for inserting vertices except at the end and we don't remove vertices.
		///        This means that neither reallocation cost nor stability are reasons for preferring listS to vecS.
		using vertex_list_type                = boost::vecS;
		/// @brief Coalescent trees are directed acyclic graphs
		using directed_type                   = boost::directedS;
	};

	///
  /// @brief A graph with properties suited to represent a coalescent tree
  ///
	template <class VertexProperties = boost::no_property,
						class EdgeProperties   = boost::no_property>
	struct Tree {
		/// @brief Properties of an edge, like the demes visited
		using edge_properties   = EdgeProperties;
		/// @brief Properties of a vertex, like the mutational state
		using vertex_properties = VertexProperties;
		/// @brief The type of graph hold by the tree class
		using graph_type =
			tree_traits::model<tree_traits::out_edge_list_type, tree_traits::vertex_list_type,
												 tree_traits::directed_type, vertex_properties, edge_properties>;

		private:
			graph_type _graph;

		public:
			///
			/// @brief Add a vertex to the tree
			///
			auto add_vertex(auto&&... args) {
					return boost::add_vertex(std::forward<decltype(args)>(args)..., _graph);
			}
			///
			/// @brief Add an edge to the tree
			///
			/// @note order of vertices determines edge orientation
			///
			auto add_edge(auto&&... args) {
					return boost::add_edge(std::forward<decltype(args)>(args)..., _graph);
			}

			///
			/// @brief Print the tree to the graphviz format
			///
			void to_graphviz(std::ostream& out) const
			{
					using namespace boost;
					return boost::write_graphviz(
							out, _graph,
							boost::make_label_writer(boost::get(vertex_bundle, this->_graph)),
							boost::make_label_writer(boost::get(edge_bundle, this->_graph)));
			}
	};

} // end namespace quetzal::coalescence

#endif
