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
  Node *parent = nullptr;
  Node *left = nullptr;
  Node *right = nullptr;
  char data;

  template<class Op1, class Op2, class Op3>
  void depth_first_search(Op1 pre_order, Op2 in_order, Op3 post_order)
  {
    pre_order(*this);
    if(this->left != nullptr && this->right != nullptr)
    {
      this->left->depth_first_search(pre_order, in_order, post_order);
      in_order(*this);
      this->right->depth_first_search(pre_order, in_order, post_order);
      in_order(*this);
    }
    post_order(*this);
  }
};


BOOST_AUTO_TEST_SUITE( newick_formatting )


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


struct Fixture_simple_tree
{
  Fixture_simple_tree()
  {
    BOOST_TEST_MESSAGE( "setup fixture simple tree" );
    /* Topology :
    *             a
    *           /  \
    *          /    c
    *         /      \\
    *        b       d e
     */

     a.data = 'a';
     b.data = 'b';
     c.data = 'c';
     d.data = 'd';
     e.data = 'e';

     a.left = &b ; b.parent = &a;
     a.right = &c; c.parent = &a;
     c.left = &d ; d.parent = &c;
     c.right = &e; e.parent = &c;
  }

  ~Fixture_simple_tree()
  {
    BOOST_TEST_MESSAGE( "teardown fixture simple tree" );
  }

  Node a, b, c, d, e;

}; // end FixtureHomozygoteParents


BOOST_FIXTURE_TEST_CASE(no_label, Fixture_simple_tree)
{
  namespace newick = quetzal::format::newick;

  // Interfacing Quetzal formatter with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};

  // We are not interested into formatting label or branch_length information
  newick::Formattable<Node> auto no_label = [](const Node& n ){return "";};
  newick::Formattable<Node> auto no_branch_length = [](const Node& n){return "";};

  // We declare a formatter passing it the interfaces
  auto formatter = newick::make_formatter(has_parent, has_children, no_label, no_branch_length);

  // We expose its interface to the user-defined class DFS algorithm
  a.depth_first_search(formatter.pre_order(), formatter.in_order(), formatter.post_order());

  // We retrieve the formatted string
  BOOST_CHECK_EQUAL(formatter.get(), "(,(,));");
}


BOOST_FIXTURE_TEST_CASE(print_label, Fixture_simple_tree)
{
  namespace newick = quetzal::format::newick;

  // Interfacing Quetzal formatter with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};

  // We are not interested into formatting the branch_length information
  newick::Formattable<Node> auto no_branch_length = [](const Node& n){return "";};

  // This time we want to record the data field
  newick::Formattable<Node> auto print_label = [](const Node& n ){return std::string(1, n.data);};

  // We configure a formatter passing it the interfaces
  auto formatter = newick::make_formatter(has_parent, has_children, print_label, no_branch_length);

  // We expose its interface to the user-defined class DFS algorithm
  a.depth_first_search(formatter.pre_order(), formatter.in_order(), formatter.post_order());

  // We retrieve the formatted string
  BOOST_CHECK_EQUAL(formatter.get() , "(b,(d,e)c)a;");
}


BOOST_FIXTURE_TEST_CASE(randomized_labels, Fixture_simple_tree)
{
  namespace newick = quetzal::format::newick;

  // Interfacing Quetzal formatter with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};

  // Get a seed for the random number engine
  std::random_device rd;
  // Standard mersenne_twister_engine seeded with rd()
  std::mt19937 gen(rd());
  // Arbitrary banch length distribution
  std::uniform_real_distribution<> dis(0.0, 2.0);

  // Random data is generated for the branch length
  newick::Formattable<Node> auto branch_length = [&gen,&dis](const Node& n){return std::to_string(dis(gen));};
  // More sophisticated label formatting
  newick::Formattable<Node> auto label = [](const Node& n ){return std::string(1, n.data) + "[my[comment]]";};

  // Create the formatter passing it the interfaces
  auto formatter = newick::make_formatter(has_parent, has_children, label, branch_length);

  // We expose its interface to the user-defined class DFS algorithm
  a.depth_first_search(formatter.pre_order(), formatter.in_order(), formatter.post_order());

  // We don't have an expectation because of the randomness, but you can always print it
  std::cout << formatter.get() << std::endl;

}


BOOST_FIXTURE_TEST_CASE(customized_labels, Fixture_simple_tree)
{
  namespace newick = quetzal::format::newick;

  // Interfacing Quetzal formatter with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};

  // Random data is generated for the branch length
  newick::Formattable<Node> auto branch_length = [](const Node& n){return "0.1";};
  // More sophisticated label formatting
  newick::Formattable<Node> auto label = [](const Node& n ){return std::string(1, n.data) + "[my[comment]]";};

  // Writes a root node branch length with a value of 0.0 and disable nested comments
  using flavor = quetzal::format::newick::TreeAlign;

  // Enables the use of nested comments
  // using flavor = quetzal::format::newick::PAUP;
  // TODO: Requires that an unrooted tree begin with a trifurcation & no nested comments
  // TODO: using flavor = quetzal::format::newick::PHYLIP;

  auto formatter = newick::make_formatter(has_parent, has_children, label, branch_length, flavor());
  a.depth_first_search(formatter.pre_order(), formatter.in_order(), formatter.post_order());

  // We retrieve the formatted string
  BOOST_CHECK_EQUAL(formatter.get() , "(b[my]:0.1,(d[my]:0.1,e[my]:0.1)c[my]:0.1)a[my]:0.0;");
}

BOOST_AUTO_TEST_SUITE_END()
