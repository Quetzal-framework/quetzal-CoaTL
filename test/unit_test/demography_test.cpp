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

//#include <concepts>

template<typename T>
concept bool MyConcept()
{
    return requires(T a, T b)
    {
        { a == b } -> bool;
        { a != b } -> bool;
   };
}

struct transition_matrix
{
	using coord_type = std::string;
	std::vector<coord_type> arrival_space(coord_type)
	{
		return {"A", "B"};
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
  // Data types declaration
  using coord_type = std::string;
	using time_type = unsigned int;
	using size_type = unsigned int;
  // Initializing some data
	time_type t0 = 2017;
	coord_type x0 = "A";
	size_type N0 = 12;

	quetzal::demography::PopulationSizeHashMapImplementation<coord_type, time_type, size_type> N;
	BOOST_TEST( !N.is_defined(x0, t0) );
  // Set first population size
	N.set(x0, t0, N0);
	BOOST_TEST( N.is_defined(x0, t0) );
	BOOST_CHECK_EQUAL( N.get(x0,t0) , N0 );
	BOOST_CHECK_EQUAL( N.definition_space(t0).size() , 1 );
  // Set second population time
	N.set("B", t0, 2*N0);
  BOOST_CHECK_EQUAL( N.definition_space(t0).size() , 2 );
  // Retrieve definition space (demes with population size > 0)
  auto X = N.definition_space(t0);
  std::cout << "Distribution area at time t0: {";
	std::copy(X.begin(), X.end(), std::ostream_iterator<coord_type>(std::cout, ","));
  std::cout << "}\n" << std::endl;
}

// BOOST_AUTO_TEST_CASE( population_size_optimized )
// {
//   using coord_type = unsigned int;
// 	std::vector<coord_type> demes = {0,1,2,3,4,5,6};
// 	unsigned int duration = 10;
// 	double N_0 = 1000;
// 	coord_type x_0 = 0;
// 	// Constructor
// 	quetzal::demography::PopulationSizeVectorImplementation N(demes, duration);
// 	assert( N(0,0) == 1000 );
// 	// Assignment
// 	N(x_0, 0) = N_0;
// 	assert(N(0,0) == N_0);
// }

BOOST_AUTO_TEST_CASE( flow )
{
  using coord_type = std::string;
	using time_type = unsigned int;
	using value_type = int;

	time_type t = 2017;
	coord_type i = "A";
	coord_type j = "B";

	quetzal::demography::Flow<coord_type, time_type, value_type> Phi;

	BOOST_TEST( ! Phi.flux_to_is_defined(i, t) );
	Phi.set_flux_from_to(i, j, t, 12);
	Phi.add_to_flux_from_to(j, j, t, 1);
	BOOST_TEST(Phi.flux_to_is_defined(j, t));
	BOOST_TEST(Phi.flux_from_to(i,j,t) == 12);
	BOOST_TEST(Phi.flux_from_to(j,j,t) == 1);

  std::cout << "Flows converging to " << j << " at time t=" << t << ":" <<std::endl;
	for(auto const& it : Phi.flux_to(j,t))
  {
		std::cout << "From "<< it.first << " to " << j << " = " << it.second << std::endl;
	}
}

BOOST_AUTO_TEST_CASE( individual_based_history_default_storage )
{
	 //! [individual_based_history_default_storage example]
  // Here we simulate a population oscillation between 2 demes: -1 and 1
  using coord_type = int;
  using generator_type = std::mt19937;
  generator_type gen;
  // Number of non-overlapping generations for the demographic simulation
  coord_type x_0 = 1;
  unsigned int nb_generations = 3; // times: 0 - 1 - 2
  unsigned int N_0 = 10;
  // Declare an individual-based history where each gene copy is dispersed independently
  using quetzal::demography::dispersal_policy::individual_based;
  // Declare memory should be allocated only on demand: ideal for very short histories
  using quetzal::memory::on_demand;
	// 10 individuals introduced at x=1, t=0
  quetzal::demography::History<coord_type, individual_based, on_demand> history(x_0, N_0, nb_generations);
  // Declare a growth function
  auto N = history.get_functor_N(); // light copiable for capture
  auto growth = [N](auto& gen, coord_type x, auto t){ return 2*N(x,t) ; };
  // Dispersal kernel, samples arrival location in {-1,1}
  auto sample_location = [](auto& gen, coord_type x)
	{
   std::bernoulli_distribution d(0.5);
   if(d(gen)){ x = -x; }
   return x;
  };
	// Expand the history
  history.simulate_forward(growth, sample_location, gen);
	// Print the migration history
  std::cout << "\nKnowing an indiviual was in deme 1 at t=2, simulate its location at time t=1?\n";
  std::cout << "Location at time 1: " << history.backward_kernel(1, 2, gen) << std::endl;
	//! [individual_based_history_default_storage example]

}

// const bool pre_allocated_implemented = false;
//
// BOOST_AUTO_TEST_CASE( mass_based_history_default_storage, * utf::enable_if<pre_allocated_implemented>() )
// {
//   using time_type = unsigned int;
//   using coord_type = std::string;
//   using generator_type = std::mt19937;
//   // Number of non-overlapping generations for the demographic simulation
//   unsigned int nb_generations = 5;
//   std::vector<coord_type> demes {"A","B"};
//   unsigned int N0 = 10;
//   coord_type x0 = "A";
//   // Declare a mass-based history, where gene copies are assumed to be divisible quantities
//   using quetzal::demography::dispersal_policy::mass_based;
//   // Declare memory should be pre-allocated: best for long histories
//   using quetzal::memory::pre_allocated;
//   quetzal::demography::History<coord_type, mass_based, pre_allocated> history(x0, N0, nb_generations, demes);
//   // Growth function
//   auto N = history.get_functor_N();
//   auto growth = [N](auto&, coord_type x, time_type t){ return 2*N(x,t) ; };
//   // Random number generation
//   generator_type gen;
//   transition_matrix M;
//   history.simulate_forward(growth, M, gen);
//   std::cout << "\nKnowing an indiviual was in deme A at time 4, simulate its location at time 3?\n";
//   std::cout << "Location at t=3: " << history.backward_kernel("A", 4, gen) << std::endl;
// }

BOOST_AUTO_TEST_SUITE_END()
