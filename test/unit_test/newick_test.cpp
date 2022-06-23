// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE coalescence_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/io.h>
#include <random>
#include <concepts>

// Simplistic tree for testing
struct Node
{
  Node *parent;
  Node *left;
  Node *right;
  char data;

  template<class Op1, class Op2, class Op3>
  void depth_first_search(Op1 pre_order, Op2 in_order, Op3 post_order){
    pre_order(*this);
    if(this->left != nullptr && this->right != nullptr)
    {
      this->left->depth_first_search(pre_order, in_order, post_order);
      in_order();
      this->right->depth_first_search(pre_order, in_order, post_order);
      in_order();
      post_order(*this);
    }
  }
} ;


BOOST_AUTO_TEST_SUITE( format )

BOOST_AUTO_TEST_CASE(newick_filters)
{
  namespace fmt = quetzal::format;

  std::string s1 = "(A:5,B:5)r;";
  std::string s2 = "(A:5,B[Comment]:5)r;";
  std::string s3 = "(A:5,B[Comment[Nested]]:5)r;";

  // Testing policies
  BOOST_CHECK(fmt::is_balanced<fmt::parenthesis>::check(s2));
  BOOST_CHECK(fmt::is_balanced<fmt::square_bracket>::check(s3));

  BOOST_CHECK(fmt::is_balanced<fmt::square_bracket>::check("(A:5,B[Comment:5r;") == false );
  BOOST_CHECK(fmt::is_balanced<fmt::parenthesis>::check("(A:5,B[Comment:5))r;") == false );

  BOOST_CHECK_EQUAL(fmt::newick::remove_comments_of_depth<0>::edit(s2) , s2);
  BOOST_CHECK_EQUAL(fmt::newick::remove_comments_of_depth<1>::edit(s3) , s1);
  BOOST_CHECK_EQUAL(fmt::newick::remove_comments_of_depth<2>::edit(s3) , s2);
}

BOOST_AUTO_TEST_CASE(newick_formatting)
{
  namespace newick = quetzal::format::newick;

  /* Topology :
  *             a
  *           /  \
  *          /    c
  *         /      \\
  *        b       d e
   */

   Node a; a.data = 'a';
   Node b; b.data = 'b';
   Node c; c.data = 'c';
   Node d; d.data = 'd';
   Node e; e.data = 'e';

   a.left = &b ; b.parent = &a;
   a.right = &c; c.parent = &a;
   c.left = &d ; d.parent = &c;
   c.right = &e; e.parent = &c;

  // Simplest case

  // Interface Quetzal formatter with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};

  // No data to format, just the topology
  newick::Formattable<Node> auto no_label = [](const Node& n ){return "";};
  newick::Formattable<Node> auto no_branch_length = [](const Node& n){return "";};

  // Create a formatter
  auto formatter_1 = newick::make_formatter(has_parent, has_children, no_label, no_branch_length);
  // Expose its interface to your data-specific DFS algorithm
  a.depth_first_search(formatter_1.pre_order(), formatter_1.in_order(), formatter_1.post_order());

  // Retrieving the string
  std::string s = formatter_1.get();
  BOOST_CHECK_EQUAL(s , "(,(,));");

  // Non-trivial data acquisition and formatting

  // Get a seed for the random number engine
  std::random_device rd;
  // Standard mersenne_twister_engine seeded with rd()
  std::mt19937 gen(rd());
  // Arbitrary banch length distribution
  std::uniform_real_distribution<> dis(0.0, 2.0);
  // Random data generation
  newick::Formattable<Node> auto branch_length = [&gen,&dis](const Node& n){return std::to_string(dis(gen));};
  // More sophisticated label formatting
  newick::Formattable<Node> auto label = [](const Node& n ){return std::string(1, n.data) + "[my[comment]]";};
  // Call the formatter
  auto formatter_2 = newick::make_formatter(has_parent, has_children, label, branch_length);
  a.depth_first_search(formatter_2.pre_order(), formatter_2.in_order(), formatter_2.post_order());
  std::cout << formatter_2.get() << std::endl;

  // Extra customizations (policy-based design)

  // Writes a root node branch length with a value of 0.0 and disable nested comments
  // using flavor = quetzal::format::newick::TreeAlign;
  // Enables the use of nested comments
  // using flavor = quetzal::format::newick::PAUP;
  // TODO: Requires that an unrooted tree begin with a trifurcation & no nested comments
  // TODO: using flavor = quetzal::format::newick::PHYLIP;

  // auto formatter_3 = newick::make_formatter<flavor>(has_parent, has_children, label, branch_length);
  // a.depth_first_search(formatter_3.pre_order(), formatter_3.in_order(), formatter_3.post_order());
  // std::cout << formatter_3.get() << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
