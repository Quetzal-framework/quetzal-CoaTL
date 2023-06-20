// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "io/newick/parser.hpp"
#include "io/newick/generator.hpp"
#include "io/newick/from_tree_graph.hpp"
#include "io/newick/to_k_ary_tree.hpp"

#include "io/genetics/wrapper.hpp"

namespace quetzal
{
  ///
  /// @brief Parsers and generators for input/output.
  ///
  namespace format
  {
    ///
    /// @brief Generic components for generating Newick strings
    ///
    namespace newick
    {}

    ///
    /// @brief Data-structures for genetic datasets
    ///
    namespace genetics
    {}
  }
}
