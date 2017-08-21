// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o abc_test abc_test.cpp -std=c++14 -Wall

#include <random>
#include "abc.h"
#include <iostream>

int main () {

  using data_type = unsigned int;
  using model_type = std::poisson_distribution<data_type>;
  using param_type = typename model_type::param_type;
  std::mt19937 g;

  // True data generation
  auto true_lambda = param_type(5);
  model_type model;
  auto obs_data = model(g, true_lambda);

  std::uniform_real_distribution<double> prior_on_lambda(1.,100.);
  auto abc = abc::make_ABC(model, prior_on_lambda);

  // Use Rubin algorithm
  auto posterior_rubin = abc.rubin_rejection_sampler(30, obs_data, g);

  // Use Pritchard algorithm with distance and rejection threshold
  auto rho = [](data_type a, data_type b){return std::abs(static_cast<int>(a) - static_cast<int>(b) ); };
  auto posterior_pritchard = abc.pritchard_rejection_sampler(30, obs_data, rho, 1, g);

  // Generate a reference table, sequence of prior predictive distribution samples
  auto table = abc.sample_prior_predictive_distribution(30, g);
  auto eta = [](data_type d){return d; }; // useless identity sumstats
  auto sumstats = table.compute_summary_statistics(eta);
  auto distances = sumstats.compute_distance_to(eta(obs_data), rho);

  // Format output
  auto format = [](auto const& sample){
    std::cout << sample.param().mean() << "\t" << sample.data() << std::endl;
  };
  
  (void)format;

  //activate for printing distances table
  //std::for_each(distances.cbegin(), distances.cend(), format);

  return 0;

}
