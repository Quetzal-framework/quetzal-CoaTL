#ifndef __NEWICK_AST_H_INCLUDED__
#define __NEWICK_AST_H_INCLUDED__

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <iomanip>

namespace quetzal::newick::ast
{
  using nodes = std::vector<struct node>;

  struct node
  {
    nodes children;
    std::string name;
    double length;
  };

  // for debug output
  static inline std::ostream& operator<<(std::ostream& os, node const& n)
  {
      os << "node { [";

      for (auto sep = ""; auto& c : n.children)
          os << std::exchange(sep, ", ") << c;

      os << "], " << std::quoted(n.name) ;

      if (n.length != 0)
          os << ":" << n.length;
      return os << " }";
  }
} // namespace ast

BOOST_FUSION_ADAPT_STRUCT(quetzal::newick::ast::node, children, name, length)

#endif
