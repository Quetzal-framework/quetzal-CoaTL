// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE newick_generator_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/io/newick/generator.hpp>
#include <quetzal/utils/random_tree.hpp>
#include <quetzal/io/newick/to_k_ary_tree.hpp>
#include <quetzal/io/newick/from_k_ary_tree.hpp>
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
      in_order();
      this->right->depth_first_search(pre_order, in_order, post_order);
    }
    post_order(*this);
  }
};


BOOST_AUTO_TEST_SUITE( newick_formatting )


BOOST_AUTO_TEST_CASE(newick_filters)
{
  namespace fmt = quetzal::format::newick;
  namespace detail = fmt::detail;

  std::string s1 = "(A:5,B:5)r;";
  std::string s2 = "(A:5,B[Comment]:5)r;";
  std::string s3 = "(A:5,B[Comment[Nested]]:5)r;";

  // Testing policies
  BOOST_CHECK(detail::is_balanced<detail::parenthesis>::check(s2));
  BOOST_CHECK(detail::is_balanced<detail::square_bracket>::check(s3));

  BOOST_CHECK(detail::is_balanced<detail::square_bracket>::check("(A:5,B[Comment:5r;") == false );
  BOOST_CHECK(detail::is_balanced<detail::parenthesis>::check("(A:5,B[Comment:5))r;") == false );

  BOOST_CHECK_EQUAL(fmt::remove_comments_of_depth<0>::edit(s2) , s2);
  BOOST_CHECK_EQUAL(fmt::remove_comments_of_depth<1>::edit(s3) , s1);
  BOOST_CHECK_EQUAL(fmt::remove_comments_of_depth<2>::edit(s3) , s2);
}


struct Fixture_simple_tree
{
  Fixture_simple_tree()
  {
    BOOST_TEST_MESSAGE( "setup fixture simple tree" );
    /* Topology :
    *             a
    *           /   \
    *          /     c
    *         /     / \
    *        b     d   e
     */

     a.data = 'a';
     b.data = 'b';
     c.data = 'c';
     d.data = 'd';
     e.data = 'e';

     a.left = &b ; 
     b.parent = &a;
     a.right = &c; 
     c.parent = &a;
     c.left = &d ; 
     d.parent = &c;
     c.right = &e; 
     e.parent = &c;
  }

  ~Fixture_simple_tree()
  {
    BOOST_TEST_MESSAGE( "teardown fixture simple tree" );
  }

  Node a, b, c, d, e;

}; // end FixtureHomozygoteParents


BOOST_FIXTURE_TEST_CASE(external_tree_no_label, Fixture_simple_tree)
{
  namespace newick = quetzal::format::newick;

  // Interfacing Quetzal generator with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};

  // We are not interested into formatting label or branch_length information
  newick::Formattable<Node> auto no_label = [](const Node& n ){return "";};
  newick::Formattable<Node> auto no_branch_length = [](const Node& n){return "";};

  // We declare a generator passing it the interfaces
  auto generator = newick::generator(has_parent, has_children, no_label, no_branch_length);

  // We expose its interface to the user-defined class DFS algorithm
  a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

  // We retrieve the formatted string
  auto result = generator.take_result();
  BOOST_CHECK_EQUAL(result, "(,(,));");
}


BOOST_FIXTURE_TEST_CASE(external_tree_print_label, Fixture_simple_tree)
{
  namespace newick = quetzal::format::newick;

  // Interfacing Quetzal generator with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};

  // We are not interested into formatting the branch_length information
  newick::Formattable<Node> auto no_branch_length = [](const Node& n){return "";};

  // This time we want to record the data field
  newick::Formattable<Node> auto print_label = [](const Node& n ){return std::string(1, n.data);};

  // We configure a generator passing it the interfaces
  auto generator = newick::generator(has_parent, has_children, print_label, no_branch_length);

  // We expose its interface to the user-defined class DFS algorithm
  a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

  // We retrieve the formatted string
  auto result = generator.take_result();
  BOOST_CHECK_EQUAL(result , "(b,(d,e)c)a;");
}


BOOST_FIXTURE_TEST_CASE(external_randomized_labels, Fixture_simple_tree)
{
  namespace newick = quetzal::format::newick;

  // Interfacing Quetzal generator with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};

  // Get a seed for the random number engine
  std::random_device rd;
  // Standard mersenne_twister_engine seeded with rd()
  std::mt19937 rng(rd());
  // Arbitrary banch length distribution
  std::uniform_real_distribution<> dis(0.0, 2.0);

  // Random data is generated for the branch length
  newick::Formattable<Node> auto branch_length = [&rng,&dis](const Node& n){return std::to_string(dis(rng));};
  // More sophisticated label formatting
  newick::Formattable<Node> auto label = [](const Node& n ){return std::string(1, n.data) + "[my[comment]]";};

  // Create the generator passing it the interfaces
  auto generator = newick::generator(has_parent, has_children, label, branch_length);

  // We expose its interface to the user-defined class DFS algorithm
  a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

  // We don't have an expectation because of the randomness, but you can always print it
  std::cout << generator.take_result() << std::endl;

}


BOOST_FIXTURE_TEST_CASE(external_tree_align_format, Fixture_simple_tree)
{
  namespace newick = quetzal::format::newick;

  // Interfacing Quetzal generator with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){return n.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& n){return n.left != nullptr && n.right != nullptr;};

  // Random data is generated for the branch length
  newick::Formattable<Node> auto branch_length = [](const Node& n){return "0.1";};
  // More sophisticated label formatting
  newick::Formattable<Node> auto label = [](const Node& n ){return std::string(1, n.data) + "[my[comment]]";};

  // Writes a root node branch length with a value of 0.0 and disable/remove nested comments
  using flavor = quetzal::format::newick::TreeAlign;

  auto generator = newick::generator(has_parent, has_children, label, branch_length, flavor());
  a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

  // We retrieve the formatted string
  auto result = generator.take_result();
  BOOST_CHECK_EQUAL(result , "(b[my]:0.1,(d[my]:0.1,e[my]:0.1)c[my]:0.1)a[my]:0.0;");
  // Enables the use of nested comments
  // using flavor = quetzal::format::newick::PAUP;

  // TODO: Requires that an unrooted tree begin with a trifurcation & no nested comments
  // TODO: using flavor = quetzal::format::newick::PHYLIP;

}

BOOST_FIXTURE_TEST_CASE(legacy_k_ary_comparison, Fixture_simple_tree)
{
  namespace newick = quetzal::format::newick;
  using Flavor = quetzal::format::newick::TreeAlign;

  // LEGACY 

  // Interfacing Quetzal generator with non-quetzal tree types
  std::predicate<Node> auto has_parent = [](const Node& n){ return n.parent; };
  std::predicate<Node> auto has_children = [](const Node& n){ return n.left && n.right ;};

  // We are not interested into formatting the branch_length or label information
  newick::Formattable<Node> auto no_branch_length = [](const Node& n){ return ""; };
  newick::Formattable<Node> auto print_label = [](const Node& n ){ return ""; };

  // We configure a generator passing it the interfaces
  auto generator = newick::generator(has_parent, has_children, print_label, no_branch_length, Flavor());

  // We expose its interface to the user-defined class DFS algorithm
  a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

  // We retrieve the formatted string
  std::string const legacy = generator.take_result();
  BOOST_CHECK_EQUAL(legacy , "(,(,)):0.0;");

  // NOW WITH BGL GRAPHS
  using G = quetzal::coalescence::k_ary_tree<>;

  enum {a,b,c,d,e,N};
  G tree(N);
  add_edge(a, b, tree);
  add_edge(a, c, tree);
  add_edge(c, d, tree);
  add_edge(c, e, tree);

  // Generate the newick string
  auto const bgl = newick::generate_from(tree, Flavor());
  BOOST_CHECK_EQUAL(legacy , bgl);
}

struct vertex_t { std::string bar; };
struct edge_t { double foo; };

BOOST_AUTO_TEST_CASE(k_ary_tree_default_property)
{
  namespace newick = quetzal::format::newick;
  // Default generate trees with string/double as properties
  auto [tree,root] = newick::to_k_ary_tree<>("(ant:17, (bat:31, cow:22):7, dog:22, (elk:33, fox:12):40);");
  // Generate the newick string
  using Flavor = quetzal::format::newick::TreeAlign;
  // auto s = newick::generate_from(tree, Flavor());
  // // Convert back, default would be no_property, so need something
  // auto [other_tree, other_root] = newick::to_k_ary_tree<vertex_t, edge_t>(s);
  // // Should be isomorphic
  // BOOST_TEST(tree.is_isomorphic(other_root));
}


BOOST_AUTO_TEST_SUITE_END()
