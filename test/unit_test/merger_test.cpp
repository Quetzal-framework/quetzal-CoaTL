// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE merger_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/coalescence/merger_policy.h>

BOOST_AUTO_TEST_SUITE( mergers )

BOOST_AUTO_TEST_CASE(binary_merger_with_strings)
{
  // will produce binary trees
  using quetzal::coalescence::merger_policy::BinaryMerger;
  // let's play with trees of expressions
  using node_type = std::string;
  // define the sequence of lineages to coalesce: here they are simple letters
  std::vector<node_type> nodes = {"a", "b", "c", "d"};
  // define the population size
  int N = 3;
  // a classic random number generator
  std::mt19937 gen;

  // Print the initial state
  std::cout << "\nNodes at sampling time :\n";
  std::copy(nodes.begin(), nodes.end(), std::ostream_iterator<node_type>(std::cout, "\n"));
  std::cout << "\n";

  // perform one step of coalescence and print out the result
  auto last = BinaryMerger::merge(nodes.begin(), nodes.end(), N, gen);
  std::cout << "\nAfter one binary merge generation:\n";
  // print the remaining lineages
  for(auto it = nodes.begin(); it != last; ++it)
  {
    std::cout << *it << std::endl;
  }
}

BOOST_AUTO_TEST_CASE (simultaneous_multiple_merger_on_the_fly_with_strings)
{
  // let's play with strings as node data
  using node_type = std::string;
  // randomly sample (simulate) an occupancy spectrum when needed
  using quetzal::coalescence::occupancy_spectrum::sampling_policy::on_the_fly;
  // policy that will produce multiple merge events dictated by the occupancy spectrum
  using SMM = quetzal::coalescence::merger_policy::SimultaneousMultipleMerger<on_the_fly>;
  // define the sequence of lineages to coalesce: here they are simple letters
  std::vector<node_type> nodes = {"a", "b", "c", "d"};
  // define the population size
  int N = 3;
  // a classic random number generator
  std::mt19937 gen;

  // Print initial state
  std::cout << "\nNodes at sampling time :\n";
  std::copy(nodes.begin(), nodes.end(), std::ostream_iterator<node_type>(std::cout, "\n"));
  std::cout << "\n";

  // Simulate one generation of merge
  auto last = SMM::merge(nodes.begin(), nodes.end(), N, gen);
  std::cout << "\nAfter one simultaneous multiple merge generation:\n";
  // print the remaining lineages
  for(auto it = nodes.begin(); it != last; ++it)
  {
    std::cout << *it << std::endl;
  }
}

BOOST_AUTO_TEST_SUITE_END()
