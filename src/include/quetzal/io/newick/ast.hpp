// Copyright 2022 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#pragma once

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <iomanip>

namespace quetzal::format::newick::ast
{

  ///
  /// @brief A container of children nodes
  ///
  using nodes = std::vector<struct node>;

  ///
  /// @brief Abstract Syntax Tree structure
  struct node
  {
    ///
    /// @brief The type of vertex described by the AST
    ///
    using vertex_property = std::string;

    ///
    /// @brief The type of edge described by the AST
    ///
    using edge_property = double;

    ///
    /// @brief A container of nodes
    ///
    nodes children;

    ///
    /// @brief the name of the node
    ///
    vertex_property name;

    ///
    ///@brief the length of the branch
    ///
    edge_property distance_to_parent;

  };

  ///
  /// @brief Print a n-ary tree graphically
  ///
  static inline std::ostream& print_subtree(node const& node, const std::string& prefix, std::ostream& os)
  {
    // if tree is more than a simple root node
    if ( !node.children.empty())
    {
      // print a prefix growing with depth
      os << prefix;

      size_t nb_children = node.children.size();

      // Bifurcation if more than 1 child
      if (nb_children > 1) os << "├──";

      for (size_t i = 0; i < nb_children; ++i)
      {
        const auto& child = node.children[i];
        // If not the last child
        if (i < nb_children - 1)
        {
          // If not the first child
          if (i > 0) os << prefix << "├──";

          bool should_print_branch = nb_children > 1 && !child.children.empty();
          auto new_prefix = prefix + (should_print_branch ? "│\t" : "\t");
          os << child.distance_to_parent << "──" << std::quoted(child.name) << "\n";
          print_subtree(child, new_prefix, os);
        }
        // If the last child
        else
        {
          if(nb_children > 1) os << prefix;
          os << "└──" << child.distance_to_parent << "──" << std::quoted(child.name) << "\n";
          print_subtree(child, prefix + "\t", os);
        }
      }
    }
    return os;
  }

  ///
  /// @brief Ostream operator
  ///
  static inline std::ostream& operator<<(std::ostream& os, node const& n)
  {
    os << std::quoted(n.name) << "\n";
    return print_subtree(n, "", os);
  }

} // namespace ast

BOOST_FUSION_ADAPT_STRUCT(quetzal::format::newick::ast::node, children, name, distance_to_parent)
