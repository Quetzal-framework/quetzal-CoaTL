// Copyright 2018 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

//! [Demonstrate use]
#include "quetzal/quetzal.h"
#include <random> // rng
#include <iostream>
#include <fstream>
#include <vector>

// Namespace shortnames
namespace geo = quetzal::geography;
namespace schemes = quetzal::sampling_scheme;
namespace expr = quetzal::expressive;

// Parameter 1 : path to geoTiff environmental dataset
int main(int argc, char* argv[])
{
  std::cout << "0 ... Reading " << argc << " input parameters" << std::endl;
  // Initalize random number generator
  std::random_device rd;
  std::mt19937 gen(rd());

  /******************************
  * Geospatial dataset
  *****************************/
  std::cout << "1 ... Reading environment file " << argv[1] << std::endl;
  using time_type = int;
  using landscape_type = geo::DiscreteLandscape<std::string,time_type>;
  using coord_type = landscape_type::coord_type;
  landscape_type env( {{"temp", argv[1]}},   {time_type(0)} );

  /******************************
  * Simulator configuration
  *****************************/
  std::cout << "2 ... Initializing spatial coalescence simulator" << std::endl;
  // Declaring simulation policies (policy-based design)
  using demographic_policy = quetzal::demography::dispersal_policy::mass_based;
  using coalescence_policy = quetzal::coalescence::newick_with_distance_to_parent<coord_type, time_type>;
  using memory_policy      = quetzal::demography::memory_policy::on_disk;
  using simulator_type     = quetzal::ForwardBackwardSpatiallyExplicit<coord_type, demographic_policy, coalescence_policy, memory_policy>;
  // Initial distribution
  coord_type x_0 = env.reproject_to_centroid(coord_type(40.0, -3.0));
  // Number of introduced gene copies
  demographic_policy::value_type N_0 = 100;
  // Number of generations
  unsigned int t_0 = 0;
  unsigned int nb_generations = 30;
  unsigned int sampling_time = 29;
  // Simulator initialization
  simulator_type simulator(x_0, N_0, nb_generations);
  // Ancestral wright-fisher population size
  simulator.ancestral_Wright_Fisher_N(N_0);

  /******************************
  * Niche and growth functions
  *****************************/
  std::cout << "3 ... Initializing demographic growth niche functions" << std::endl;
  // A literal factory is used to transforms literals into function of space and time
  using expr::literal_factory;
  using expr::use;
  literal_factory<coord_type, time_type> lit;
  // Growth rate is constant across space and time
  auto r = lit(5);
  // Retrieve temperature function of space (homogeneous across time)
  auto temp = env["temp"];
  // Wrap it to ignore time argument
  auto s = [temp](coord_type x, time_type){return temp(x, 0) >= 0.0 ? temp(x,0) : 0.0 ;};
  // Compose it to get a carrying capacity expression
  auto K = use(s)*lit(5);
  // Retrieve the population size functor
  auto N = simulator.get_functor_N();
  // Compose everything in a logistic growth expression
  auto logistic = N * ( lit(1) + r ) / ( lit(1) + ( (r * N)/K ));
  // Add random fluctuations
  auto sim_children = [logistic](auto& gen, coord_type const&x, time_type t){
    std::poisson_distribution<unsigned int> poisson(logistic(x,t));
    return poisson(gen);
  };

  /******************
  * Dispersal
  ******************/
  std::cout << "4 ... Initializing dispersal kernel" << std::endl;
  // Define an emigrant rate
  double emigrant_rate = 0.1;
  // Define the friction according to environmental features
  auto is_temp_high = [temp](coord_type x, time_type){return temp(x, 0) >= 15.0 ? true : false ;};
  auto friction = [is_temp_high](coord_type const& x){return is_temp_high(x,0) ? 0.3 : 0.7 ;};
  // Capture an expression to find nearest neighbours
  auto env_ref = std::cref(env);
  auto get_neighbors = [env_ref](coord_type const& x){return env_ref.get().direct_neighbors(x);};
  // Build a dispersal kernel object
  auto dispersal = demographic_policy::make_neighboring_migration(coord_type(), emigrant_rate, friction, get_neighbors);

  /**************************
  * Demographic expansion
  **************************/
  std::cout << "5 ... Simulating demographic expansion" << std::endl;
  simulator.simulate_forward_demography(sim_children, dispersal, gen);

  /************************************************************
  *  Sample uniformely at random across the distribution area
  ************************************************************/
  std::cout << "6 ... Simulating sampling scheme" << std::endl;
  // Declare sampling size
  unsigned int n = 50;
  // Retrieve the non-empty demes at sampling time
  std::vector<coord_type> distribution_area = simulator.distribution_area(sampling_time);
  // Declare a functor to get the population size at sampling time
  auto last_N = [N, sampling_time](coord_type const& x){return N(x, sampling_time);};
  // Give the required information to the sampling scheme simulator
  auto unif_sampler = quetzal::sampling_scheme::make_unif_constrained_sampler(distribution_area, last_N, n);
  // Generate a random sample
  auto sample = unif_sampler(gen);

  /**********************
  *  Coalescence
  **********************/
  std::cout << "8 ... Begining coalescence" << std::endl;
  // Coalesce sampled gene copies aaaaall the way to the MRCA
  auto results = simulator.coalesce_to_mrca<>(sample, sampling_time, gen);
  // Output trees newick formulas
  std::ofstream file;
  file.open ("trees.txt");
  file << results;
  file.close();
  std::cout << "Done" << std::endl;

  /**********************
  *  Data visualization
  **********************/
  std::cout << "7 ... Exporting GIS data for visualization" << std::endl;
  std::string N_filename = "N.tif";
  // Make it easy to visualize using eg the R package raster
  env.export_to_geotiff(N, t_0, sampling_time, [&simulator](time_type const& t){return simulator.distribution_area(t);}, N_filename);
  // Easy to visualize with eg readOGR R function
  env.export_to_shapefile(sample, "sample.shp");

  return 0;
}
//! [Demonstrate use]
