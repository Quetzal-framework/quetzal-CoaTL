#include "quetzal/demography.h"
#include <iostream>
#include <random>
#include <map>
// Define a customized transition matrix
struct transition_matrix
{
	// Two demes, but identified by string
	using coord_type = std::string;
	using time_type = int;
	// Mandatory interface with the quetzal algorithms:
	// Take a the coordinate of start
	// Returns the vector of possible arrival coordinates
	std::vector<coord_type> arrival_space(coord_type x)
	{
		// Whatever the coordinate to start from, redirect them either in Paris or A.A
		return {"Paris", "Ann Arbor"};
	}
	// Second mandatory interface: what is the probability to move from x to y?
	double operator()(coord_type x, coord_type y)
	{
		return 0.5; // 1/2 probability to change of location
	}
};
int main(){
	using coord_type = std::string;
	using time_type = int;
	using generator_type = std::mt19937;
	using quetzal::demography::demographic_policy::mass_based;
  using quetzal::demography::memory_policy::on_RAM;
	int N0 = 100;
	// Number of non-overlapping generations for the demographic simulation
	int nb_generations = 10;
	quetzal::demography::History<coord_type, mass_based, on_RAM> history("Paris", N0, nb_generations);
	// Growth function
	auto N = history.get_functor_N();
	auto growth = [N](auto& gen, coord_type x, time_type t){ return 2*N(x,t) ; };
	// Random number generation
	generator_type gen;
	// Define the transition probabilities
	transition_matrix M;
	// Pass everything to the simulation
	history.simulate_forward(growth, M, gen);
	return 0;
}
