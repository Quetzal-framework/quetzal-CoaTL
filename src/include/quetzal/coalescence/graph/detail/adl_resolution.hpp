// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/////////////////////////////////////////////////////////////////////////// *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef QUETZAL_TREE_ADL_H_INCLUDED
#define QUETZAL_TREE_ADL_H_INCLUDED

namespace quetzal::coalescence::detail
{

namespace adl_resolution
{
void add_edge() = delete;
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
void edge() = delete;
} // namespace adl_resolution
} // namespace quetzal::coalescence::detail

#endif