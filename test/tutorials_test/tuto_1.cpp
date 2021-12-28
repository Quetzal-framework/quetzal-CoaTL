// file main.cpp

#include <quetzal/demography.h>

#include <iostream>
#include <random>

int main(){

  // Here we simulate a population expansion through a 2 demes landscape.

  // Use type aliasing for readability
  using coord_type = int;
  using time_type = unsigned int;
  using generator_type = std::mt19937;

  // choose the strategy to be used
  using quetzal::demography::dispersal_policy::individual_based;
  using quetzal::demography::memory_policy::on_RAM;

  // Initialize the history with 10 individuals introduced in deme x=1 at time t=2018
  quetzal::demography::History<coord_type, individual_based, on_RAM> history(1, 2018, 10);

  // Get a reference on the population sizes database
  auto N = std::cref(history.pop_sizes());

  // Capture it with a lambda expression to build a growth function
  auto growth = [N](auto& gen, coord_type x, time_type t){ return 2*N(x,t) ; };

  // Number of non-overlapping generations for the demographic simulation
  unsigned int nb_generations = 3;

  // Random number generation
  generator_type gen;

  // Stochastic dispersal kernel, purposely very simple
  // The geographic sampling space is {-1 , 1}, there is 50% chance to migrate
  auto sample_location = [](auto& gen, coord_type x, time_type t){
    std::bernoulli_distribution d(0.5);
    if(d(gen)){ x = -x; }
    return x;
  };

  history.expand(nb_generations, growth, sample_location, gen);

  std::cout << "Population flows from x to y at time t:\n\n"
            << history.flows()
            << std::endl;
}
