// file main.cpp
#include "quetzal/demography.h"
#include <iostream>
#include <random>
// Here we simulate a population expansion through a 2 demes landscape.
int main(){
  // Use type aliasing for readability
  using coord_type = int;
  using time_type = unsigned int;
  using generator_type = std::mt19937;
  // choose the strategy to be used
  using quetzal::demography::demographic_policy::individual_based;
  using quetzal::demography::memory_policy::on_RAM;
  // Random number generator
  generator_type gen;
  // Number of non-overlapping generations for the demographic simulation
  int nb_generations = 3;
  // Coordinate of the origin of introduction
  coord_type x0 = 1;
  // Number of individuals introduced
  int N0 = 10;
  // Initialize the history:
  quetzal::demography::History<coord_type, individual_based, on_RAM> history(x0, N0, nb_generations);
  // Get a light callable object that behaves as a function of space and time
  auto N = history.get_functor_N();
  // Constant growth rate
  int r = 100;
  // Capture it with a lambda expression to build a growth function
  auto growth = [N,r](auto& gen, coord_type x, time_type t){ return r*N(x,t) ; };
  // Stochastic dispersal lambda function, purposely very simple
  auto sample_location = [](auto& gen, coord_type x){
    // 50% chance to move to the other deme
    std::bernoulli_distribution d(0.5);
    // The geographic sampling space is {-1 , 1}
    if(d(gen)){ x = -x; }
    return x;
  };
  // run the simulation
  history.simulate_forward(growth, sample_location, gen);
  // Output
  time_type t = nb_generations -1;
  for(auto x : history.distribution_area(t))
  {
    std::cout << "N(" << x << "," << nb_generations -1 << ") = " << N(x,t) << std::endl;
  }
  return 0;
} // end main
