// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE simulator_test

#include <quetzal.h>

#include <boost/test/included/unit_test.hpp>
namespace utf = boost::unit_test;

#include <random>
#include <algorithm>

BOOST_AUTO_TEST_SUITE( utils )

BOOST_AUTO_TEST_SUITE( sampling_scheme )


BOOST_AUTO_TEST_CASE( param_base_class )
{
  quetzal::sampling_scheme::param_base_class sample(100);
  BOOST_CHECK_EQUAL(sample.sampling_size(), 100);

  sample.sampling_size(20);
  BOOST_CHECK_EQUAL(sample.sampling_size(), 20);
}


BOOST_AUTO_TEST_CASE( uniform_at_random, * utf::tolerance(0.1) )
{
  using sampling_type = quetzal::sampling_scheme::uniform_at_random;
  std::vector<int> space = {1,2,3,4,5,6,7,8,9,10};
  sampling_type d(10000);
  std::mt19937 gen;
  auto sample = d(space, gen);

  // Checking distribution central tendency
  int sum = std::accumulate(sample.begin(), sample.end(), 0, [](auto a, auto const& p){return a + p.first * p.second;});
  double mean = static_cast<double>(sum) / 10000.0;
  BOOST_TEST(mean == 5.5);

  // Changing the parameter value for another sampling
  sampling_type::param_type p = d.param();
  p.sampling_size(100);
  auto more_sample = d(space, p, gen);
}

BOOST_AUTO_TEST_CASE( clustered_sampling )
{

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
