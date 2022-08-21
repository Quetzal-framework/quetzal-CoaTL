// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE polymorphism_test

#include <boost/test/unit_test.hpp>

#include <units/quantity.h> // percent
#include <units/generic/dimensionless.h> // percent

#include <quetzal/polymorphism.h>

namespace utf = boost::unit_test;

using namespace units;

BOOST_AUTO_TEST_SUITE( statistics )

BOOST_AUTO_TEST_CASE( tajimasD )
{
  // // @note Quetzal is not bound to specific data structures, so tests are easily defined
  // //
  // // @see example in https://ocw.mit.edu/courses/hst-508-quantitative-genomics-fall-2005/0900020a633e85338e9510495c2e01a6_tajimad1.pdf
  // using variant_matrix_type = std::map<std::string, std::vector<std::string>>;
  // variant_matrix_type data
  // {
  //   {subj0, {"ATAATAAAAA", "AATAATAAAA", "AAATAAAAAA", "AATAAAAAAA", "A"},
  //   {subj1, {"AAAAAAAATA", "AATAATAAAA", "AAATAAAAAA", "AAAAAAAAAA", "A"},
  //   {subj2, {"AAAATAAAAA", "TATAATAAAA", "AAATATAAAA", "AAAAAAAAAA", "A"},
  //   {subj3, {"AAAAAAAAAA", "AATAATAAAA", "AAATAAATAA", "ATAAAAAAAA", "A"},
  //   {subj4, {"AAAATAAAAA", "AAATATAAAA", "AAATAAAAAA", "AAAAAAAAAA", "A"},
  //   {subj5, {"AAAATAAAAA", "AAAAATAAAA", "AAAAAAAAAA", "AAAAATAAAA", "A"},
  //   {subj6, {"AAAAAATAAA", "AATAATAAAA", "AAATAAAAAA", "AAAAAAAAAA", "A"},
  //   {subj7, {"AAAAAAAAAA", "AAAAATAAAA", "AAATAAAAAA", "AAAAAAAAAT", "A"},
  //   {subj8, {"AAAAAAAAAA", "AAAAAAAAAA", "AAATAAAAAA", "AAAAAAAAAA", "A"},
  //   {subj9, {"AAAAAAAAAA", "AAAAATAAAA", "AAATAATAAA", "AAAAAAAAAA", "A"}
  // }
  //
  // // Expose iterators interface to statistics algorithms:
  // get_subject = [](){return it.first};
  // // Compute statistics
  // polymophism<stat::tajimasD> stats(data);
  //
  // BOOST_CHECK_EQUAL(polymophism.nb_sequences), 10);
  // BOOST_CHECK_EQUAL(polymophism.nb_sites), 41);
  //
  // BOOST_CHECK_EQUAL(polymophism.tajimasD.mean_pairwise_difference.value()), 3.888889);
  // BOOST_CHECK_EQUAL(polymophism.tajimasD.number_of_segregating_sites.value()), 16);
  // BOOST_CHECK_EQUAL(polymophism.tajimasD.fraction_of_segregating_sites.value()), 16/41);
  //

  //
  // BOOST_CHECK_EQUAL(polymophism.tajimasD.value(), -1.446172);
  // std::cout << polymophism.tajimasD.meaning() << std::endl;
  //
  int mean_pairwise_difference = 3.888889;
  int nb_segregating_sites = 16;
  int nb_sequences = 10;

  // Compute stats
  using quetzal::polymophism::statistics::tajimasD;
  auto stat = tajimasD(mean_pairwise_difference, nb_segregating_sites, nb_sequences);

  // Define test metrics
  auto error = [](auto computed, auto expected){return computed - expected;};
  auto tolerance = [](auto expected){return dimensionless<percent>(1 * expected / expected);};

  // Expected values
  auto a1 = 2.828968;
  auto a2 = 1.539768;
  auto b1 = 0.407407;
  auto b2 = 0.279012;
  auto c1 = 0.053922;
  auto c2 = 0.047227;
  auto e1 = 0.019061;
  auto e2 = 0.004949;
  auto D  = -1.446172;

  BOOST_CHECK_SMALL(error(stat.a1(), a1), tolerance(a1).number());
  BOOST_CHECK_SMALL(error(stat.a2(), a2), tolerance(a2).number());
  BOOST_CHECK_SMALL(error(stat.b1(), b1), tolerance(b1).number() ) ;
  BOOST_CHECK_SMALL(error(stat.b2(), b2), tolerance(b2).number() ) ;
  BOOST_CHECK_SMALL(error(stat.c1(), c1), tolerance(c1).number() ) ;
  BOOST_CHECK_SMALL(error(stat.c2(), c2), tolerance(c2).number() ) ;
  BOOST_CHECK_SMALL(error(stat.e1(), e1), tolerance(e1).number() ) ;
  BOOST_CHECK_SMALL(error(stat.e2(), e2), tolerance(e2).number() ) ;
  BOOST_CHECK_SMALL(error(stat.D() ,  D), tolerance( D).number() ) ;

}

BOOST_AUTO_TEST_CASE( tajimasD )
{

}
BOOST_AUTO_TEST_SUITE_END()
