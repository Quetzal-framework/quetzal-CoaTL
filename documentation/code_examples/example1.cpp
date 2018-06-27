// Copyright 2018 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

//  compiles with g++ -o3 cross_val.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "../../quetzal.h"

#include <memory>      // std::unique_ptr
#include <functional>  // std::plus
#include <map>         // std::map

template<typename DispersalKernel, typename Generator>
class DemonstrationModel{

public:

  /********************************************************
  / Type aliases are defined hereafter for better clarity
  *********************************************************/

  // Random number generator
  using generator_type = Generator;

  // Temporal coordinates
  using time_type = unsigned int;

  // Explicit space represention
  using landscape_type = quetzal::geography::EnvironmentalQuantity<time_type>;

  // Coordinate system (actually lat/lon)
  using coord_type = typename landscape_type::coord_type;

  // Type of population sizes
  using N_type = unsigned int;

  // Type of historical simulation database
  using history_type = quetzal::demography::History<coord_type, time_type, N_type, quetzal::demography::Flow<coord_type, time_type, N_type>>;

  // Type of the demo-genetic simulator
  using simulator_type = quetzal::simulators::IDDC_model_1<coord_type, time_type, N_type>;

  // Type of the dispersal kernel to use
  using kernel_type = DispersalKernel;

  // Genetic sample
  using loader_type = quetzal::genetics::Loader<coord_type, quetzal::genetics::microsatellite>;
  using dataset_type = loader_type::return_type;
  using locus_ID_type = dataset_type::locus_ID_type;

  // In the FTD methodology, a genealogy is only represented by the partition
  // of its leaves (sampled geographic coordinate)
  using tree_type = std::vector<coord_type>;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;

  using result_type = std::vector<forest_type> ;

  // Nested class for representing the model parameters
  // Inherits from the dispersal kernel parameters
  class param_type : public kernel_type::param_type {

  public:
    // Number of introduced individuals
    auto N0() const {return m_N0; }
    void N0(unsigned int N) { m_N0 = N; }

    // Constant carrying capacity along the landscape
    auto k() const {return m_k; }
    void k(unsigned int value) { m_k = value; }

    // Constant growth rate along the landscape
    auto r() const { return m_r; }
    void r(unsigned int value) { m_r = value; }

  private:
    unsigned int m_k;
    unsigned int m_r;
    unsigned int m_N0;
  }; // end Params class

  /*******************************
  / Here are defined members data
  *******************************/
private:
  const landscape_type & m_landscape;
  std::unique_ptr<dataset_type> m_dataset;
  std::unique_ptr<std::vector<coord_type>> m_demes;
  std::map<locus_ID_type, forest_type> m_forests;
  quetzal::demography::dispersal::SymmetricDistanceMatrix<coord_type, typename quetzal::geography::GeographicCoordinates::km> m_distances;

  // Point of introduction
  coord_type m_x0;

  // Time of introduction
  time_type m_t0;

  // Sampling time
  time_type m_sampling_time;

public:

  DemonstrationModel(const landscape_type & landscape, dataset_type const& dataset):
  m_landscape(landscape),
  m_dataset(reproject(dataset)),
  m_demes(std::make_unique<std::vector<coord_type>>(landscape.geographic_definition_space())),
  m_forests(make_forests(*m_dataset)),
  m_distances(quetzal::demography::dispersal::make_symmetric_distance_matrix(
    *m_demes,
    [](auto const& x, auto const& y) {return x.great_circle_distance_to(y);} ))
  {}

  // Set the spatio-temporal introduction point, reprojecting x0 to its cell centroid
  auto& introduction_point(coord_type const& x0, time_type t0){
    m_x0 = m_landscape.reproject_to_centroid(x0);
    m_t0 = t0;
    return *this;
  }

  // Set the sampling time
  auto& sampling_time(time_type t){
    m_sampling_time = t;
    return *this;
  }

private:

  // Reproject the sampling points to the centroid of the environmental cell
  std::unique_ptr<dataset_type> reproject(dataset_type const& data){
    auto ptr = std::make_unique<dataset_type>(data);
    ptr->reproject(m_landscape);
    return ptr;
  }

  // Construct a forest at a locus from the given dataset
  forest_type make_forest(dataset_type const& data, dataset_type::locus_ID_type locus) const {
    forest_type forest;
    for(auto const& x : data.get_sampling_points())
    {
      for(auto const& individual : data.individuals_at(x))
      {
        auto alleles = individual.alleles(locus);
        if(alleles.first.get_allelic_state() > 0) forest.insert(x, std::vector<coord_type>(1, x));
        if(alleles.second.get_allelic_state() > 0) forest.insert(x, std::vector<coord_type>(1, x));
      }
    }
    return forest;
  }

  // Construct all forest at all loci from the given dataset
  std::map<locus_ID_type, forest_type> make_forests(dataset_type const& data) const {
    std::map<locus_ID_type, forest_type> m;
    for(auto const& it : data.loci()){
      m.emplace(it, make_forest(data, it));
    }
    return m;
  }

  auto make_growth_expression(param_type const& param, history_type::N_type const& N) const
  {
    using quetzal::expressive::literal_factory;
    using quetzal::expressive::use;

    literal_factory<coord_type, time_type> lit;

    auto r = lit(param.r());
    auto K = lit(param.k());
    auto N_cref = std::cref(N);
    auto N_expr = use([N_cref](coord_type x, time_type t){return N_cref(x,t);});
    auto g = N_expr*(lit(1)+r)/ (lit(1)+((r * N_expr)/K));
    auto sim_N_tilde = [g](generator_type& gen, coord_type x, time_type t){
      std::poisson_distribution<history_type::N_type::value_type> poisson(g(x,t));
      return poisson(gen);
    };

    return sim_N_tilde;
  }

public:
  result_type operator()(generator_type& gen, param_type const& param) const
  {
    simulator_type simulator(m_x0, m_t0, param.N0());

    // Dispersal patterns
    using quetzal::demography::dispersal::DiscreteLocationKernelFactory;

    // Kernel transition that is costly to pass as a copy argument
    auto heavy_kernel = quetzal::demography::dispersal::DiscreteLocationKernelFactory::make<coord_type, quetzal::geography::GeographicCoordinates::km, DispersalKernel>(m_distances, param);

    // Little wrapper that is cheaper to copy and pass around
    auto light_kernel = [&heavy_kernel, param](auto& gen, coord_type x, time_type t){
      return heavy_kernel(x, gen);
    };

    // Just make a copiable expression representing growth patterns
    auto growth = make_growth_expression(param, simulator.size_history());

    // What is a "merge" event, in the sense of the data structure representing a genealogy ?
    auto merge_binop = [](const tree_type &parent, const tree_type &child)
    {
      tree_type copy = parent;
      copy.insert( copy.end(), child.begin(), child.end() );
      return copy;
    };

    // construct the geographic distribution of sampled gene copies
    // it is used to check that the demography has reached the sampling points
    std::map<coord_type, unsigned int> counts;
    for(auto const& it : m_dataset->get_sampling_points()){
      counts[it] = (m_dataset->size(it)) * 2;
    }

    // Simulate demographic history
    auto const& history = simulator.simulate_demography(counts, growth, light_kernel, m_sampling_time, gen);

    // One genealogical forest by locus (possibly incomplete coalescence)
    result_type output;

    // Loop over loci to simulate the genetic history conditionally to the demography
    for(auto const& locus : m_dataset->loci() ){
      auto coalesced_forest = simulator.coalescence_process(m_forests.at(locus), history, merge_binop, gen);
      output.push_back(coalesced_forest);
    }

    // return the updated forest
    return output;

  }

};

template<typename Model>
struct Wrapper{
  using model_type = Model;
  // Interface for ABC module
  using param_type = typename model_type::param_type;
  using result_type = typename model_type::result_type;
  using generator_type = typename model_type::generator_type;

  model_type& m_model;

  Wrapper(model_type& model): m_model(model){};

  result_type operator()(generator_type& gen, param_type const& param) const {
    return m_model(gen, param);
  }

};

template<typename Model>
struct Prior {

  template<typename Generator>
  auto operator()(Generator& gen) const
  {
    using model_type = Model;
    using param_type = typename model_type::param_type;
    param_type params;
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
  using model_type = DemonstrationModel<quetzal::demography::dispersal::Gaussian, std::mt19937>;

  std::mt19937 gen;

  // Precise environmental files
  std::string bio_file = "europe_precipitation.tif";

  // Build the landscape from files
  model_type::landscape_type landscape(bio_file, std::vector<model_type::time_type>(1));

  // Read the genetic dataset
  std::string file = "microsat_test.csv";
  model_type::loader_type loader;
  auto dataset = loader.read(file);

  // Build the simulation model
  model_type model(landscape, dataset);

  // Precise some context information
  model.introduction_point(model_type::coord_type(44.00, 0.20), 2004);
  model.sampling_time(2008);

  // Build the prior, make the model easily copiable, build an abc procedure
  Prior<model_type> prior;
  Wrapper<model_type> wrap(model);
  auto abc = quetzal::abc::make_ABC(wrap, prior);

  // The reference table contains 20 model outputs
  auto table = abc.sample_prior_predictive_distribution(20, gen);

  return 0;
}
