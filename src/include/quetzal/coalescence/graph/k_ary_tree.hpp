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
#include <boost/graph/isomorphism.hpp>
#include <iostream>

namespace quetzal::coalescence
{
	namespace details
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
	}

	///
  /// @brief A class adapted for simulating coalescent trees as a rooted K-ary tree, where each node can hold at most k number of children.
	///
  /// @remark
	///          - Since topology (pure structure) matter, a coalescence tree is more than a data container.
	///          - This class inherits from as a boost::graph with specialized edge and vertex properties defined in details#tree_traits
  ///          - The simulation interface intentionally does not allow for removing edges or vertices,
	///         but you may access the underlying boost graph object to do so.
	///
	template <class VertexProperties = boost::no_property,
						class EdgeProperties   = boost::no_property>
	struct k_ary_tree : public	details::tree_traits::model<
	                      details::tree_traits::out_edge_list_type, details::tree_traits::vertex_list_type,
										    details::tree_traits::directed_type, VertexProperties, EdgeProperties>
	 {
		 /// @brief Properties of an edge, e.g. a structure representing the series of demes visited or simply the branch length.
		 using edge_properties   = EdgeProperties;
		 /// @brief Properties of a vertex, e.g. a structure representing the mutational state.
		 using vertex_properties = VertexProperties;
		 /// @brief The type of graph hold by the tree class
		 using self_type = details::tree_traits::model <
		       details::tree_traits::out_edge_list_type, details::tree_traits::vertex_list_type,
			     details::tree_traits::directed_type, vertex_properties, edge_properties >;

		 ///
		 /// @brief Print the tree to the graphviz format
		 /// @remarks Intends to hide the bundles writer syntax
		 ///
		 void to_graphviz(std::ostream& out) const
		 {
				 using namespace boost;
				 return boost::write_graphviz(
						 out, *this,
						 boost::make_label_writer(boost::get(vertex_bundle, *this)),
						 boost::make_label_writer(boost::get(edge_bundle, *this)));
		 }

		 ///
		 /// @brief Returns true if there exists an isomorphism between this and other and false otherwise.
		 ///
		 bool is_isomorphic(const self_type& other) noexcept
		 {
			 return boost::isomorphism(*this, other);
		 }

	 }; // end class Tree
} // end namespace quetzal::coalescence

#endif