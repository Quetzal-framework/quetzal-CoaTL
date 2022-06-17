// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_MODULE demography_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/demography.h>


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

BOOST_AUTO_TEST_CASE( population_size_on_disk )
{
  // Data types declaration
  using coord_type = std::string;
	using time_type = unsigned int;
	using size_type = unsigned int;
  // Initializing some data
	time_type t0 = 0;
	coord_type x0 = "A";
	size_type N0 = 12;

	quetzal::demography::PopulationSizeOnDiskImplementation<coord_type, time_type, size_type> N;
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
}

BOOST_AUTO_TEST_CASE( population_size_on_disk_moving_windows )
{
  // Data types declaration
  using coord_type = int;
	using time_type = unsigned int;
	using size_type = unsigned int;
  // Initializing some data
	time_type t0 = 0;
	coord_type x0 = 0;
	size_type N0 = 1;

	quetzal::demography::PopulationSizeOnDiskImplementation<coord_type, time_type, size_type> N;
	BOOST_TEST( !N.is_defined(x0, t0) );
  // Set first population size
	N.set(x0, t0, N0);
	BOOST_TEST( N.is_defined(x0, t0) );
	BOOST_CHECK_EQUAL( N.get(x0,t0) , N0 );
	BOOST_CHECK_EQUAL( N.definition_space(t0).size() , 1 );
  // Set other population sizes
  for(unsigned int i=1; i <= 10; ++i)
  {
    N.set(i, i, 2*N.get(i-1,i-1));
  }
  BOOST_CHECK_EQUAL( N.definition_space(10).size() , 1 );
  BOOST_CHECK_EQUAL( N.get(10,10) , 1024 );
  BOOST_CHECK_EQUAL( N.get(1,1) , 2 );
  BOOST_CHECK_EQUAL( N.get(0,0) , 1 );
  BOOST_CHECK_EQUAL( N.get(5,5) , 32 );
}

BOOST_AUTO_TEST_CASE( flow )
{
  using coord_type = std::string;
	using time_type = unsigned int;
	using value_type = int;

	time_type t = 2017;
	coord_type i = "A";
	coord_type j = "B";

	quetzal::demography::FlowHashMapImplementation<coord_type, time_type, value_type> Phi;

	BOOST_TEST( ! Phi.flow_to_is_defined(i, t) );
	Phi.set_flow_from_to(i, j, t, 12);
	Phi.add_to_flow_from_to(j, j, t, 1);
	BOOST_TEST(Phi.flow_to_is_defined(j, t));
	BOOST_TEST(Phi.flow_from_to(i,j,t) == 12);
	BOOST_TEST(Phi.flow_from_to(j,j,t) == 1);

  std::cout << "Flows converging to " << j << " at time t=" << t << ":" <<std::endl;
	for(auto const& it : Phi.flow_to(j,t))
  {
		std::cout << "From "<< it.first << " to " << j << " = " << it.second << std::endl;
	}
}

BOOST_AUTO_TEST_CASE( flow_on_disk_implementation )
{
  using coord_type = std::string;
	using time_type = unsigned int;
	using value_type = int;

	time_type t = 0;
	coord_type i = "A";
	coord_type j = "B";

	quetzal::demography::FlowOnDiskImplementation<coord_type, time_type, value_type> Phi;

	BOOST_TEST( ! Phi.flow_to_is_defined(i, t) );
	Phi.set_flow_from_to(i, j, t, 12);
	Phi.add_to_flow_from_to(j, j, t, 1);
	BOOST_TEST(Phi.flow_to_is_defined(j, t));
	BOOST_TEST(Phi.flow_from_to(i,j,t) == 12);
	BOOST_TEST(Phi.flow_from_to(j,j,t) == 1);

  std::cout << "Flows converging to " << j << " at time t=" << t << ":" <<std::endl;
	for(auto const& it : Phi.flow_to(j,t))
  {
		std::cout << "From "<< it.first << " to " << j << " = " << it.second << std::endl;
	}
}

BOOST_AUTO_TEST_CASE( flow_on_disk_implementation_moving_windows )
{
  std::cout << "here" <<std::endl;
  using coord_type = unsigned int;
	using time_type = unsigned int;
	using value_type = unsigned int;

	quetzal::demography::FlowOnDiskImplementation<coord_type, time_type, value_type> Phi;

  for(unsigned int t=0; t <= 5; ++t)
  {
    for(unsigned int from=0; from <=5; ++from)
    {
      for(unsigned int to=0; to <= 5; ++to)
      {
        Phi.set_flow_from_to(from, to, t, from+to+t);
      }
    }
  }

	BOOST_TEST(Phi.flow_to_is_defined(5, 5));
	BOOST_TEST(Phi.flow_from_to(5,5,5) == 15);
  BOOST_TEST(Phi.flow_from_to(2,4,5) == 11);
  BOOST_TEST(Phi.flow_from_to(0,0,4) == 4);
  BOOST_TEST(Phi.flow_from_to(0,0,3) == 3);
  BOOST_TEST(Phi.flow_from_to(0,0,2) == 2);
  BOOST_TEST(Phi.flow_from_to(0,0,1) == 1);
  BOOST_TEST(Phi.flow_from_to(0,0,0) == 0);
  BOOST_TEST(Phi.flow_from_to(1,2,5) == 8);
  BOOST_TEST(Phi.flow_from_to(0,0,0) == 0);
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
  using quetzal::demography::demographic_policy::individual_based;
  // Declare memory should be allocated only on demand: ideal for very short histories
  using quetzal::demography::memory_policy::on_RAM;
	// 10 individuals introduced at x=1, t=0
  quetzal::demography::History<coord_type, individual_based, on_RAM> history(x_0, N_0, nb_generations);
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

BOOST_AUTO_TEST_SUITE_END()
