// Copyright 2018 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

//  compiles with g++ -o3 cross_val.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "DemonstrationModel.h"
#include "../../../modules/demography/dispersal.h"
#include "../../../abc.h"

using model_type = DemonstrationModel<quetzal::demography::dispersal::Gaussian, std::mt19937>;

struct Wrapper{
  // Interface for ABC module
  using param_type = model_type::param_type;
  using result_type = model_type::result_type;
  using generator_type = model_type::generator_type;

  model_type& m_model;

  Wrapper(model_type& model): m_model(model){};

  result_type operator()(generator_type& gen, param_type const& param) const {
    return m_model(gen, param);
  }

};

struct Prior {
  template<typename Generator>
  auto operator()(Generator& gen) const
  {
    model_type::param_type params;
    // locus 11 has max diversity: 8
    params.N0(8);
    params.k(std::uniform_int_distribution<>(1,500)(gen));
    params.r(std::uniform_real_distribution<>(1.0, 20.0)(gen));
    params.a(std::uniform_real_distribution<>(100.0, 1000.0)(gen));
    return params;
  }
};

int main()
{
  using result_type = model_type::result_type;

  std::mt19937 gen;
  std::string bio_file = "/home/becheler/Documents/VespaVelutina/wc2.0_10m_prec_01_europe_agg_fact_5.tif";
  model_type::landscape_type landscape(bio_file, std::vector<model_type::time_type>(1));

  std::string file = "/home/becheler/Documents/VespaVelutina/dataForAnalysis.csv";
  //std::string file = "/home/becheler/dev/quetzal/modules/genetics/microsat_test.csv";
  model_type::loader_type loader;
  auto dataset = loader.read(file);

  model_type model(landscape, dataset);

  model.introduction_point(model_type::coord_type(44.00, 0.20), 2004);
  model.sampling_time(2008);

  Prior prior;
  Wrapper wrap(model);

  auto abc = quetzal::abc::make_ABC(wrap, prior);

  auto table = abc.sample_prior_predictive_distribution(20, gen);

  model_type::param_type true_param;
  true_param.N0(8);
  true_param.k(250);
  true_param.r(10);
  true_param.a(500);

  std::vector<result_type> pods;

  for(int i = 0; i != 10; ++i){
    try {
      pods.push_back(model(gen, true_param));
    }catch(...){
      std::cerr << "one pod less" << std::endl;
    }
  }

  // headers
  std::cout << "pod\tr\tk\tN0\ta\t";
  auto const& loci = dataset.loci();
  std::copy(loci.begin(), loci.end(), std::ostream_iterator<std::string>(std::cout, "\t"));
  std::cout << "\n";

  auto distance = [](result_type const& o, result_type const& s){
    assert(o.size() == s.size());
    std::vector<double> v;
    for(unsigned int i = 0; i < o.size(); ++ i )
    {
      v.push_back(o.at(i).fuzzy_transfer_distance(s.at(i)));
    }
    return v;
  };


  unsigned int id = 0 ;
  for(auto const& it : pods)
  {
    auto ftds = table.compute_distance_to(it, distance);
    for(auto const& it2 : ftds)
    {
      auto const& p = it2.param();
      std::cout << id << "\t"
                << std::to_string(p.r()) << "\t"
                << std::to_string(p.k()) << "\t"
                << std::to_string(p.N0()) << "\t"
                << std::to_string(p.a()) << "\t";

      std::copy(it2.data().begin(), it2.data().end(), std::ostream_iterator<double>(std::cout, "\t"));
      std::cout << "\n";
    }
    id += 1;
  }

  return 0;
}
