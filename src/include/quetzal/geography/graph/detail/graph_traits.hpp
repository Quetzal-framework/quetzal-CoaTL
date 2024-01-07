// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>

namespace quetzal::geography::detail
{
    ///
    /// @brief Defines the common desired graph properties and constraints for a complete and sparse spatial graph
    ///
    struct common_graph_traits
    {
        /// @brief We want to enforce avoiding multi-graphs (edges with same end nodes)
        using out_edge_list_type = boost::setS;

        /// @brief We don't allow for inserting vertices except at the end and we don't remove vertices.
        ///        This means that neither reallocation cost nor stability are reasons for preferring listS to vecS.
        using vertex_list_type = boost::vecS;

        /// @brief We need bidirectionality for in-edges access
        /// @remark Bidirectionality trades off storage/insertion overhead for the ability to get in-edges
        ///         and in-degree without having to enumerate all vertices or edges
        using directed_type = boost::bidirectionalS;
    };

    ///
    /// @brief Defines the desired graph properties and constraints for a complete spatial graph
    ///
    struct dense_graph_traits : public common_graph_traits
    {
        /// @brief Better to use the adjacency_matrix for dense graphs (E is close to V^2) rather than adjacency_list
        template <class... Types>
        using model = boost::adjacency_matrix<Types...>;
    };

    ///
    /// @brief Defines the desired graph properties and constraints for a 8 or 4 neighbors spatial graph
    ///
    struct sparse_graph_traits : public common_graph_traits
    {
        /// @brief Better to use the adjacency_list for sparse graphs (E is closer to 4*V or 8*V than V^2)
        template <class... Types>
        using model = boost::adjacency_matrix<Types...>;
    };
}