#ifndef __NEWICK_PARSER_H_INCLUDED__
#define __NEWICK_PARSER_H_INCLUDED__

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

namespace quetzal::newick::ast
{
  ///////////////////////////////////////////////////////////////////////////
  //  Our node struct
  ///////////////////////////////////////////////////////////////////////////
  struct node
  {
      std::string name;
      double length;
  };
  using boost::fusion::operator<<;
}

// Tell fusion about our node struct to make it a first-class citizen.
BOOST_FUSION_ADAPT_STRUCT(quetzal::newick::ast::node, name, length)

///////////////////////////////////////////////////////////////////////////////
//  Our node parser
///////////////////////////////////////////////////////////////////////////////
namespace quetzal::newick::parser
{
  namespace x3 = boost::spirit::x3;

  x3::rule<struct branch> branch{"branch"};

  auto name     = x3::lexeme[x3::alpha >> *x3::alnum]; // to be improved later
  auto length   = ':' >> x3::double_;
  auto leaf     = -name;
  auto internal = '(' >> (branch % ',') >> ')' >> -name;
  auto subtree  = internal | leaf;
  auto tree     = x3::skip(x3::blank)[subtree >> -length >> ';' >> x3::eoi];

  auto branch_def = subtree >> -length;

  BOOST_SPIRIT_DEFINE(branch);
}

#endif
