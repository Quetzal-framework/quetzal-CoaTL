// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE demography_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/demography.h>

struct transition_matrix {
	using coord_type = std::string;
	using time_type = unsigned int;
	std::vector<coord_type> arrival_space(coord_type)
	{
		return {"Paris", "Ann Arbor"};
	}
	double operator()(coord_type, coord_type)
	{
		return 0.5; // 1/2 probability to change of location
	}
};

BOOST_AUTO_TEST_SUITE( demography )

// Allows for arbitrary coordinate type and arbitrary origin date.
// Does not require to reserve all the necessary space before hand.
BOOST_AUTO_TEST_CASE( population_size_default )
{
  using coord_type = std::string;
	using time_type = unsigned int;
	using size_type = int;

	time_type t0 = 2017;
	coord_type x0 = "Paris";
	size_type N0 = 12;

	quetzal::demography::PopulationSize<coord_type, time_type, size_type> N;
	BOOST_TEST(N.is_defined(x0, t0) == false );

	N(x0,t0) = N0;
	BOOST_TEST(N.is_defined(x0, t0) );
	BOOST_CHECK_EQUAL( N.get(x0,t0) , N0 );

	BOOST_CHECK_EQUAL( N.definition_space(t0).size() , 1 );

	N.set("Bordeaux", t0, 2*N0);

	auto X = N.definition_space(t0);
	std::copy(X.begin(), X.end(), std::ostream_iterator<coord_type>(std::cout, " "));
}

// Allows only unsigned int demes (ie requires an order relation of demes), origin date has to be 0, and the total
// number of generations has to be known beforehand.
BOOST_AUTO_TEST_CASE( population_size_optimized )
{
	unsigned int nb_demes = 50;
	unsigned int duration = 10;
	double N_0 = 1000;
	unsigned int x_0 = 0;

	quetzal::demography::PopulationSizeOptimized N(nb_demes, duration, x_0, N_0);
	assert( N(0,0) == 1000) );

	N(1,0) = 200;
	assert(N(1,0) == 200);

}

BOOST_AUTO_TEST_CASE( flow )
{
  using coord_type = std::string;
	using time_type = unsigned int;
	using value_type = int;

	time_type t = 2017;
	coord_type i = "Paris";
	coord_type j = "Bordeaux";

	quetzal::demography::Flow<coord_type, time_type, value_type> Phi;

	BOOST_TEST( ! Phi.flux_to_is_defined(i, t) );
	Phi.set_flux_from_to(i, j, t, 12);
	Phi.add_to_flux_from_to(j, j, t, 1);
	BOOST_TEST(Phi.flux_to_is_defined(j, t));

	BOOST_TEST(Phi.flux_from_to(i,j,t) == 12);
	BOOST_TEST(Phi.flux_from_to(j,j,t) == 1);

	for(auto const& it : Phi.flux_to(j,t)){
		std::cout << it.first << " \t->\t " << j << "\t=\t" << it.second << std::endl;
	}
}

BOOST_AUTO_TEST_CASE( individual_based_history_default_storage )
{
	 //! [individual_based_history_default_storage example]
  // Here we simulate a population oscillation between 2 demes: -1 and 1
  using coord_type = int;
  using time_type = unsigned int;
  using generator_type = std::mt19937;
  // Declaresan individual-based history
  using quetzal::demography::dispersal_policy::individual_based;
	// 10 individuals introduced at x=1, t=2018
  quetzal::demography::History<coord_type, time_type, individual_based> history(1, 2018, 10);
  // Declares a growth function
  auto N = std::cref(history.pop_sizes()); // light copiable for capture
  auto growth = [N](auto&, coord_type x, time_type t){ return 2*N(x,t) ; };
  // Number of non-overlapping generations for the demographic simulation
  unsigned int nb_generations = 3;
  // Random number generation
  generator_type gen;
  // Dispersal kernel in {-1,1}
  auto sample_location = [](auto& gen, coord_type x)
	{
   std::bernoulli_distribution d(0.5);
   if(d(gen)){ x = -x; }
   return x;
  };
	// Expand the history
  history.expand(nb_generations, growth, sample_location, gen);
	// Print the migration history
  std::cout << "Population flows from x to y at time t:\n\n" << history.flows() << std::endl;
  std::cout << "\nKnowing an indiviual was in deme 1 in 2021, where could it have been just before dispersal ?\n";
  std::cout << "Answer: it could have been in deme " << history.backward_kernel(1, 2021, gen) << std::endl;
	//! [individual_based_history_default_storage example]

}

BOOST_AUTO_TEST_CASE( mass_based_history_default_storage )
{
  using coord_type = std::string;
  using time_type = unsigned int;
  using generator_type = std::mt19937;
  // Initialize history: 10 individuals introduced at x=1, t=2018
  using quetzal::demography::dispersal_policy::mass_based;
  quetzal::demography::History<coord_type, time_type, mass_based> history("Paris", 2018, 10);
  // Growth function
  auto N = std::cref(history.pop_sizes());
  auto growth = [N](auto&, coord_type x, time_type t){ return 2*N(x,t) ; };
  // Number of non-overlapping generations for the demographic simulation
  unsigned int nb_generations = 3;
  // Random number generation
  generator_type gen;
  transition_matrix M;
  history.expand(nb_generations, growth, M, gen);
  std::cout << "Population flows from x to y at time t:\n\n" << history.flows() << std::endl;
  std::cout << "\nKnowing an indiviual was in Paris in 2021, where could it have been just before dispersal ?\n";
  std::cout << "Answer: it could have been in " << history.backward_kernel("Paris", 2021, gen) << std::endl;
}



BOOST_AUTO_TEST_SUITE_END()
