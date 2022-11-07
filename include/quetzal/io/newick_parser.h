#ifndef __NEWICK_PARSER_H_INCLUDED__
#define __NEWICK_PARSER_H_INCLUDED__

#include <boost/spirit/home/x3.hpp>

namespace quetzal::newick::parser
{
  namespace x3 = boost::spirit::x3;

  using x3::alpha;
  using x3::alnum;
  using x3::double_;
  using x3::rule;
  using x3::phrase_parse;
  using x3::space;

  rule<struct branch> branch{"branch"};
  rule<struct branch_set> branch_set{"branch_set"};

  auto name    = alpha >> *alnum;
  auto length  = ':' > double_;
  auto leaf    = -name;
  auto internal= '(' >> branch_set >> ')' >> -name;
  auto subtree = leaf | internal;
  auto tree    = subtree >> ';';

  auto const branch_def = subtree >> length;
  auto const branch_set_def = branch | branch >> ',' >> branch_set;

  BOOST_SPIRIT_DEFINE(branch, branch_set);
}

#endif
