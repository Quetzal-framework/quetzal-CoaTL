// Copyright 2018 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#include "quetzal.h"

#include <boost/math/special_functions/binomial.hpp> // binomial coefficient

#include <random>

#include <iostream>
#include <algorithm>
#include <cassert>
#include <vector>

int main(int argc, char* argv[])
{

  /******************************
   * Geospatial dataset
   *****************************/
  std::cout << "Reading " << argc << " input parameters" << std::endl;

  using time_type = unsigned int;
  using landscape_type = quetzal::geography::DiscreteLandscape<std::string,time_type>;
  using coord_type = landscape_type::coord_type;
  landscape_type env( {{"rain", argv[1]}},   {time_type(0)} );

  std::cout << "1 ... Environment file " << argv[1] << " read" << std::endl;

  /******************************
   * Simulator configuration
   *****************************/
  using demographic_policy = quetzal::demography::strategy::mass_based;
  using coalescence_policy = quetzal::coalescence::policies::distance_to_parent<coord_type, time_type>;
  using simulator_type = quetzal::simulators::SpatiallyExplicit<coord_type, time_type, demographic_policy, coalescence_policy>;

  coord_type x_0(40.0, -3.0);
  x_0 = env.reproject_to_centroid(x_0);
  time_type t_0 = 2000;
  demographic_policy::value_type N_0 = 100;

  simulator_type simulator(x_0, t_0, N_0);
  simulator.ancestral_Wright_Fisher_N(30);

  std::cout << "2 ... Spatial coalescence simulator initialized" << std::endl;

  /******************************
   * Niche and growth functions
   *****************************/
  using quetzal::expressive::literal_factory;
  using quetzal::expressive::use;
  literal_factory<coord_type, time_type> lit;

  auto rain = env["rain"];
  auto s = [rain](coord_type x, time_type){return rain(x, 0);};
	unsigned int factor = 5;

  auto K = use(s)*lit(factor);
  auto r = lit(5);

  auto pop_sizes = simulator.pop_size_history();
  auto N = use( [pop_sizes](coord_type x, time_type t){ return pop_sizes(x,t);} );

  auto g = N * ( lit(1) + r ) / ( lit(1) + ( (r * N)/K ));
  auto sim_children = [g](auto& gen, coord_type const&x, time_type t){
    std::poisson_distribution<unsigned int> poisson(g(x,t));
    return poisson(gen);
  };

  std::cout << "3 ... Niche an growth functions initialized" << std::endl;

  std::random_device rd;
  std::mt19937 gen(rd());

  /******************
   * Dispersal
   ******************/
  auto const& space = env.geographic_definition_space();
  coord_type::km radius = 60.0;
  using quetzal::geography::memoized_great_circle_distance;
  auto distance = [](coord_type const& a, coord_type const& b) -> double {return memoized_great_circle_distance(a,b);};
  auto weight = [radius](coord_type::km d) -> double { if(d < radius){return 1.0;} else {return 0.0;} ;};
	auto f = [weight, distance](coord_type const& a, coord_type const& b){
    if(a == b){
      return 500.0;
    }else{
      return weight(distance(a,b));
    }
  };
  auto dispersal = demographic_policy::make_sparse_distance_based_dispersal(space, f);

  std::cout << "4 ... Dispersal kernel initialized" << std::endl;

  /**************************
  * Demographic expansion
  **************************/
  unsigned int t_sampling = 2100;
  simulator.expand_demography(t_sampling, sim_children, dispersal, gen);

  std::cout << "5 ... Demographic expansion finished" << std::endl;

  /**********************
  *  Sampling schemes
  **********************/
  std::vector<coord_type> distribution_area = pop_sizes.get().definition_space(t_sampling);
  unsigned int n = 200;
  auto last_N = [N, t_sampling](coord_type const& x){return N(x, t_sampling);};

  // Uniform sampling
  auto unif_sampler = quetzal::sampling_scheme::make_unif_constrained_sampler(distribution_area, last_N, n);
  auto global_sample = unif_sampler(gen);

  // Clustered sampling

  // Radius sampling in Spain
  coord_type::km radius_1 = 200.0;
  auto weight_1 = [radius_1](coord_type::km d) -> double { if(d < radius_1){return 1.0;} else {return 0.0;} ;};
  auto f1 = [weight_1, distance, x_0](coord_type const& x){return weight_1(distance(x_0, x));};
  auto spain_sampler = quetzal::sampling_scheme::make_constrained_sampler(distribution_area, f1, last_N, n/2);
  auto spain_sample = spain_sampler(gen);

  // Radius sampling around Sicilia
  coord_type sicilia = env.reproject_to_centroid(coord_type(37.3, 15.0));
  coord_type::km radius_2 = 200.0;
  auto weight_2 = [radius_2](coord_type::km d) -> double { if(d < radius_2){return 1.0;} else {return 0.0;} ;};
  auto f2 = [weight_2, distance, sicilia](coord_type const& x){return weight_2(distance(sicilia, x));};
  auto sicilia_sampler = quetzal::sampling_scheme::make_constrained_sampler(distribution_area, f2, last_N, n/2);
  auto sicilia_sample = sicilia_sampler(gen);

  auto merged_sample = spain_sample;
  for(auto const& it: sicilia_sample){
    merged_sample[it.first] += it.second;
  }

  std::cout << "6 ... Sample schemes simulated" << std::endl;

  /**********************
   *  Data visualization
   **********************/
  std::string N_filename = "/home/becheler/dev/sampling_project/output/N.tif";
  env.export_to_geotiff(N, t_0, t_sampling, [&pop_sizes](time_type const& t){return pop_sizes.get().definition_space(t);}, N_filename);

  env.export_to_shapefile(global_sample, "uniform.shp");
  env.export_to_shapefile(spain_sample, "spain.shp" );
  env.export_to_shapefile(sicilia_sample, "sicilia.shp");

  std::cout << "7 ... GIS data visualization exported" << std::endl;

  /**********************
   *  Coalescence
   **********************/

  std::cout << "8 ... Begin coalescence" << std::endl;

  std::cout << simulator.coalesce_to_mrca(merged_sample, t_sampling, gen) << std::endl;

  return 0;

}
