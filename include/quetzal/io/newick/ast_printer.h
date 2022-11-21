#ifndef __NEWICK_AST_PRINTER_H_INCLUDED__
#define __NEWICK_AST_PRINTER_H_INCLUDED__

#include "ast.h"
#include <iostream>

namespace quetzal::newick::ast
{
  constexpr int tabsize = 4;

  struct rexpr_printer
  {
      typedef void result_type;

      rexpr_printer(int indent = 0) : indent(indent) {}

      void operator()(rexpr const& ast) const
      {
          std::cout << '{' << std::endl;
          for (auto const& entry : ast.entries)
          {
              tab(indent+tabsize);
              std::cout << '"' << entry.first << "\" = ";
              boost::apply_visitor(rexpr_printer(indent+tabsize), entry.second);
          }
          tab(indent);
          std::cout << '}' << std::endl;
      }

      void operator()(std::string const& text) const
      {
          std::cout << '"' << text << '"' << std::endl;
      }

      void tab(int spaces) const
      {
          for (int i = 0; i < spaces; ++i)
              std::cout << ' ';
      }

      int indent;
  };
}
#endif
