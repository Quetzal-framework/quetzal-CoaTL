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

namespace quetzal::geography
{
  /// @brief Property of a process independent of the direction of movement
  using isotropy = boost::undirectedS;

  /// @brief Property of a process dependent of the direction of movement
  using anisotropy = boost::directedS;
}