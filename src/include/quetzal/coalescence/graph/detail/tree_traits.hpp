// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef QUETZAL_TREE_TRAITS_H_INCLUDED
#define QUETZAL_TREE_TRAITS_H_INCLUDED

#include <boost/graph/adjacency_list.hpp>

namespace quetzal::coalescence::detail
{
    ///
    /// @brief Defines the desired graph properties and constraints for a coalescent tree
    ///
    struct tree_traits
    {
        /// @brief Trees are sparse graph in nature, adjacency_matrix would not be justified here
        template <class... Types>
        using model = boost::adjacency_list<Types...>;

        /// @brief We want to enforce avoiding multi-graphs (edges with same end nodes)
        using out_edge_list_type = boost::setS;

        /// @brief We don't allow for inserting vertices except at the end and we don't remove vertices.
        ///        This means that neither reallocation cost nor stability are reasons for preferring listS to vecS.
        using vertex_list_type = boost::vecS;

        /// @brief Coalescent trees are directed acyclic graphs but we need bidirectionality for in-edges access
        /// @remark Bidirectionality trades off storage/insertion overhead for the ability to get in-edges
        ///         and in-degree without having to enumerate all vertices or edges
        using directed_type = boost::bidirectionalS;
    };
}

#endif