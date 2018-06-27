// Copyright 2018 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __QUETZAL_DEMONSTRATION_MODEL_H_INCLUDED__
#define __QUETZAL_DEMONSTRATION_MODEL_H_INCLUDED__

#include "../../../expressive.h"
#include "../../../modules/simulator/simulators.h"
#include "../../../modules/demography/SymmetricDistanceMatrix.h"
#include "../../../modules/demography/DistanceBasedLocationKernel.h"
#include "../../../modules/fuzzy_transfer_distance/FuzzyPartition.h"
#include "../../../modules/fuzzy_transfer_distance/RestrictedGrowthString.h"

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

  using result_type = std::vector<FuzzyPartition<coord_type>> ;

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
  mutable std::map<unsigned int, std::discrete_distribution<unsigned int>> m_distribs;
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
  m_distances(quetzal::demography::dispersal::make_symmetric_distance_matrix(*m_demes, [](auto const& x, auto const& y) {return x.great_circle_distance_to(y);} ) )
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

    auto fuzzifie(forest_type const& forest, locus_ID_type const& locus) const {
    std::map<coord_type, std::vector<double>> coeffs;

    for(auto const& it : m_forests.at(locus).positions())
    {
      coeffs[it].resize(forest.nb_trees());
    }

    unsigned int cluster_id = 0;
    for(auto const& it1 : forest )
    {
      for(auto const& it2 : it1.second)
      {
        coeffs[it2][cluster_id] += 1;
      }
      cluster_id += 1;
    }

    for(auto & it1 : coeffs){
      double sum = std::accumulate(it1.second.begin(), it1.second.end(), 0.0);
      assert(sum > 0.0);
      for(auto & it2 : it1.second){
        it2 = it2/sum;
      }
    }
    return FuzzyPartition<coord_type>(coeffs);
  }


    unsigned int factorial(unsigned int n) const
    {
        if (n == 0)
           return 1;
        return n * factorial(n - 1);
    }

    int countP(int n, int k) const
    {
      // Base cases
      if (n == 0 || k == 0 || k > n)
         return 0;
      if (k == 1 || k == n)
          return 1;

      // S(n+1, k) = k*S(n, k) + S(n, k-1)
      return  k*countP(n-1, k) + countP(n-1, k-1);
    }

    int Bell(unsigned int n) const
    {
      int sum = 0;
      for(unsigned int k = 1; k <= n; ++k)
      {
        sum += countP(n, k);
      }
      return sum;
    }

    double pdf(unsigned int k, unsigned int n) const
    {
      double a = static_cast<double>(factorial(k));
      double b = static_cast<double>(Bell(n));
      double e = 2.71828;
      double p = std::pow(k,n)/(a*b*e);
      return p;
    }

    auto sample_partition(unsigned int n, Generator& gen) const
    {
      if(m_distribs.find(n) == m_distribs.end()){
        std::vector<double> w;
        size_t size = 10000;
        w.resize(size);
        w[0] = 0;
        for(unsigned int k = 1; k < size; ++k)
        {
          w[k] = pdf(k,n);
        }
        m_distribs[n] = std::discrete_distribution<unsigned int>(w.begin(), w.end());
      }

      unsigned int K = m_distribs.at(n)(gen);

      std::uniform_int_distribution<> unif(1, K);
      std::vector<unsigned int> Cs;
      for(unsigned int i = 1; i <= n; ++i )
      {
        Cs.push_back(unif(gen));
      }

      std::map<unsigned int, unsigned int> ids;
      std::vector<unsigned int> rgs;
      rgs.resize(Cs.size());

      unsigned int block_ID = 0;
      for(unsigned int i = 0; i < Cs.size(); ++i){
        auto it = ids.find(Cs[i]);
        if( it != ids.end() ){
          rgs[i] = it->second;
        }else {
          rgs[i] = block_ID;
          ids[Cs[i]] = block_ID;
          ++block_ID;
        }
      }
      return RestrictedGrowthString(rgs);
    }

public:
  auto operator()(generator_type& gen, param_type const& param) const
  {
    simulator_type simulator(m_x0, m_t0, param.N0());

    // Dispersal patterns
    using  quetzal::demography::dispersal::DiscreteLocationKernelFactory;
    auto kernel = DiscreteLocationKernelFactory::make<coord_type, quetzal::geography::GeographicCoordinates::km, DispersalKernel>(m_distances, param);

    auto light_kernel = [&kernel, param](auto& gen, coord_type x, time_type t){
      return kernel(x, gen);
    };

    // Growth patterns
    auto growth = make_growth_expression(param, simulator.size_history());

    auto merge_binop = [](const tree_type &parent, const tree_type &child)
    {
      tree_type copy = parent;
      copy.insert( copy.end(), child.begin(), child.end() );
      return copy;
    };

    result_type fps;

    std::map<coord_type, unsigned int> counts;
    for(auto const& it : m_dataset->get_sampling_points()){
      counts[it] = (m_dataset->size(it)) * 2;
    }

    auto const& history = simulator.simulate_demography(counts, growth, light_kernel, m_sampling_time, gen);

    for(auto const& locus : m_dataset->loci() ){
      auto updated_forest = simulator.coalescence_process(m_forests.at(locus), history, merge_binop, gen);
      auto S_sim = fuzzifie(updated_forest, locus);
      //std::cout << "Simulated fuzzy Partiton:\n" << S_sim << std::endl;
      if(S_sim.nClusters() > 1 )
      {
        S_sim.merge_clusters(sample_partition(S_sim.nClusters(), gen));
      }
      fps.push_back(S_sim);
    }

    //std::cout << "Aggregated simulated fuzzy Partiton:\n" << S_sim << std::endl;

    return fps;

  }

private:
  FuzzyPartition<coord_type> fuzzifie_data(DemonstrationModel::dataset_type::locus_ID_type const& locus) const {
    using cluster_type = unsigned int;
    std::set<cluster_type> clusters;
    std::map<coord_type, std::vector<double>> coeffs;

    auto frequencies = m_dataset->frequencies_discarding_NA(locus);
    unsigned int n_clusters = m_dataset->allelic_richness(locus);

    // preparing coeffs clusters vectors
    // and preparing clusters set
    for(auto const& it1 : frequencies)
    {
      coeffs[it1.first].resize(n_clusters);
      for(auto const& it2: it1.second){
        clusters.insert(it2.first);
      }
    }

    // fill the coefficients
    for(auto & it1 : coeffs){
      for(auto const& it2 : frequencies[it1.first]){
        auto allele = it2.first;
        auto freq = it2.second;
        auto it_pos = std::find(clusters.cbegin(), clusters.cend(), allele);
        assert(it_pos != clusters.end());
        auto index = std::distance(clusters.begin(), it_pos);
        it1.second[index] = freq;
      }
    }

    return FuzzyPartition<coord_type>(coeffs);
  }


};

#endif
