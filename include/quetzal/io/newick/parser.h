#ifndef __NEWICK_TREE_H_INCLUDED__
#define __NEWICK_TREE_H_INCLUDED__

#include <boost/spirit/home/x3.hpp>
#include "ast.h"
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/std_pair.hpp>

namespace quetzal::format::newick
{
  namespace parser
  {
    namespace x3 = boost::spirit::x3;

    static x3::rule<class branch, ast::node> const node{"node"};

    static auto const name     = x3::lexeme[x3::alpha >> *x3::alnum];
    static auto const length   = ':' >> x3::double_ | x3::attr(0.0);
    static auto const leaf     = name | x3::attr(std::string{});
    static auto const children = '(' >> (node % ',') >> ')' | x3::attr(ast::nodes{});
    static auto const node_def = children >> leaf >> -length;
    static auto const tree     = x3::skip(x3::blank)[node >> ';' >> x3::eoi];

    BOOST_SPIRIT_DEFINE(node);

    template <typename ExpectedAttributeType> void compatible(auto p)
    {
    static_assert(
        std::is_same_v<ExpectedAttributeType,
                       typename x3::traits::attribute_of<
                           decltype(x3::rule<struct _, ExpectedAttributeType>{} = p),
                           x3::unused_type>::type>);
    };

    void checks()
    {
      compatible<double>(length);
      compatible<std::string>(leaf);
      compatible<std::string>(name);
      compatible<ast::nodes>(children);
      compatible<ast::node>(tree);
    }
  } // end namespace parser


} // end namespace newick
#endif
