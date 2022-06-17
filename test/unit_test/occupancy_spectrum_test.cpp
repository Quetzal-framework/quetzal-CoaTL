// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_MODULE occupancy_spectrum_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/coalescence/occupancy_spectrum.h>

BOOST_AUTO_TEST_SUITE( occupancy_spectrum )

BOOST_AUTO_TEST_CASE (occupancy_spectrum_support)
{
  // Type declaration
  using quetzal::coalescence::occupancy_spectrum::Support;
  using quetzal::coalescence::occupancy_spectrum::OccupancySpectrum;
  // Customed editor policy
  auto editor = [](OccupancySpectrum && M_j)
  {
    std::copy(M_j.begin(), M_j.end(), std::ostream_iterator<OccupancySpectrum::value_type>(std::cout, " "));
    std::cout << "\n";
  };
  Support support(5,10);
  support.generate(editor);
}

BOOST_AUTO_TEST_CASE (occupancy_spectrum__distribution_init)
{
  using quetzal::coalescence::occupancy_spectrum::ProbabilityDistribution;
  // initialization by default constructor
  ProbabilityDistribution<> a;
  // constructor
  ProbabilityDistribution<> b(5,10);
  // initialization by move constructor
  ProbabilityDistribution<> c(std::move(b)); // b can not be used again
  // deleted initialization by copy constructor
  // ProbabilityDistribution d(c); // uncomment will cause a compilation error
  // deleted assignment by copy assignment operator
  // a = b; // uncomment will cause a compilation error
  // assignment by move assignment operator
  ProbabilityDistribution<> e = std::move(c); // c should not be used anymore !
}

BOOST_AUTO_TEST_CASE (occupancy_spectrum_distribution)
{
  using quetzal::coalescence::occupancy_spectrum::ProbabilityDistribution;
  int n = 5;
  int m = 10;

  ProbabilityDistribution<> dist1(n, m);
  std::cout << "Throwing " << dist1.n() << " balls in " << dist1.m() << " urns.\n";
  std::cout << "Full distribution: "
            << dist1.support().size() << " configurations and weights sum to "
            << std::accumulate(dist1.weights().begin(), dist1.weights().end(), 0.0)
            << std::endl;

   std::cout << dist1 << "\n" << std::endl;

  // Sample from it :
  std::mt19937 g;
  auto spectrum = dist1(g);

  // Truncate the empty spectrum for memory optimization
  using quetzal::coalescence::occupancy_spectrum::filter_policy::return_always_true;
  using quetzal::coalescence::occupancy_spectrum::editor_policy::truncate_tail;
  ProbabilityDistribution<return_always_true, truncate_tail> dist2(n, m);

  std::cout << "Shorten spectra:\n" << dist2 << "\n" << std::endl;

  // Filter the occupancy spectrum with small probability.
  auto pred = [](double p){
    bool keep = false;
    if(p > 0.01){ keep = true; }
    return keep;
  };
  ProbabilityDistribution<decltype(pred)> dist3(n, m, pred);
  std::cout << "Approximated distribution:\n" << dist3 << "\n" << std::endl;

  // Combine strategies
  ProbabilityDistribution<decltype(pred), truncate_tail> dist4(n, m, pred);
  std::cout << "Combined strategies:\n" << dist4 << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE (memoize )
{
  std::mt19937 g;
  // build and copy sample
  auto spectrum_a = quetzal::coalescence::occupancy_spectrum::memoize::memoize<>(10,10)(g);
  // no need to build, sample directly in the memoized distribution, no copy
  auto const& spectrum_b = quetzal::coalescence::occupancy_spectrum::memoize::memoize<>(10,10)(g);
  (void)spectrum_b;
}

BOOST_AUTO_TEST_CASE (spectrum_creation_policy)
{
  std::mt19937 g;

  auto a = quetzal::coalescence::occupancy_spectrum::sampling_policy::on_the_fly::sample(10,10, g);
  for(auto const& it : a){
    std::cout << it << " ";
  }

  std::cout << std::endl;

  auto b = quetzal::coalescence::occupancy_spectrum::sampling_policy::in_memoized_distribution<>::sample(10,10,g);
  for(auto const& it : b){
    std::cout << it << " ";
  }
}
BOOST_AUTO_TEST_SUITE_END()
