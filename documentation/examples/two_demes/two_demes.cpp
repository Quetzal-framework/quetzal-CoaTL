
//  compiles with g++ -o two_demes.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "/home/becheler/dev/quetzal/quetzal.h"

#include <memory>
#include <random>
#include <functional>  // std::plus
#include <map>

class GenerativeModel{

private:

  class Params{
  public:
    auto k() const {return m_k; }
    void k(unsigned int value) { m_k = value; }

    auto r() const { return m_r; }
    void r(unsigned int value) { m_r = value; }

    auto m() const  { return m_m; }

    void m(double value) {
      assert(value >= 0 && value <= 1);
      m_m = value;
    }

  private:
    unsigned int m_k;
    unsigned int m_r;
    double m_m;
  };

  using generator_type = std::mt19937;
  using coord_type = unsigned int;
  using time_type = unsigned int;
  using N_type = unsigned int;

  using pop_size_type = quetzal::demography::PopulationSize<coord_type, time_type, N_type>;
  using flux_type = quetzal::demography::PopulationFlux<coord_type, time_type, N_type>;
  using tree_type = unsigned int;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;

  coord_type m_x0 = 0;
  time_type m_t0 = 2000;
  time_type m_t_last = 2010;
  N_type m_init = 100;
  N_type m_sampling_size = 30;
  std::vector<coord_type> m_demes = {0,1};
  coord_type m_sampling_deme = 1;
  time_type m_sampling_time = 2009;

public:

  using param_type = Params;

  forest_type operator()(generator_type& gen, param_type const& param) const {
    return simulate(gen, param);
  }

  auto make_growth_expression(param_type const& param, pop_size_type & N) const {
    using quetzal::expressive::literal_factory;
    using quetzal::expressive::use;

    quetzal::expressive::literal_factory<coord_type, time_type> lit;

    auto r = lit(param.r());
    auto K = lit(param.k());
    auto N_expr = use([&N](coord_type x, time_type t){return N(x,t);});

    auto g = N_expr*(lit(1)+r)/ (lit(1)+((r * N_expr)/K));

    auto sim_N_tilde = [g](generator_type& gen, coord_type x, time_type t){
      std::poisson_distribution<N_type> poisson(g(x,t));
      return poisson(gen);
    };

    return sim_N_tilde;
  }

  void demographic_process(pop_size_type& N, flux_type& Phi, param_type const& param, generator_type& gen) const {

    auto dispersal_kernel = [&param, &gen, this](coord_type x){
      std::bernoulli_distribution d(param.m());
      bool change_deme = d(gen);
        if(x == this->m_demes[0]){
          if(change_deme){
            return this->m_demes[1];
          }else{
            return this->m_demes[0];
          }
        }else{
          if(change_deme){
            return this->m_demes[0];
          }else{
            return this->m_demes[1];
          }
        }
    };

    auto sim_N_tilde = make_growth_expression(param, N);

    for(time_type t = m_t0; t != m_t_last; ++t){
      unsigned int landscape_count = 0;

      for(auto i : m_demes){

        auto N_tilde = sim_N_tilde(gen, i, t);

        landscape_count += N_tilde;

        if(N_tilde >= 1){
          for(unsigned int ind = 1; ind <= N_tilde; ++ind){
            coord_type j = dispersal_kernel(i);
            Phi(i, j, t) += 1;
            N(j, t+1) += 1;
          }
        }
      }
      if(landscape_count == 0){
        throw std::domain_error("Landscape populations went extinct before sampling");
      }
    }
  }


  forest_type simulate(generator_type& gen, param_type const& param) const {

    pop_size_type N;

    N(m_x0, m_t0) = m_init;

    flux_type Phi;

    demographic_process(N, Phi, param, gen);

    if( N(m_sampling_deme, m_t_last) < m_sampling_size){
      throw std::domain_error("Simulated population size inferior to sampling size");
    }

    std::vector<tree_type> leaves(m_sampling_size, 1);
    forest_type forest;
    forest.insert(m_sampling_deme, std::vector<unsigned int>(m_sampling_size, 1));

    auto make_backward_distribution = [&Phi](coord_type const& x, time_type t){
      std::vector<double> w;
      std::vector<coord_type> X;
      assert(Phi.flux_to_is_defined(x,t));
      auto const& flux_to = Phi.flux_to(x,t);
      w.reserve(flux_to.size());
      X.reserve(flux_to.size());
      for(auto const& it : flux_to){
        X.push_back(it.first);
        w.push_back(static_cast<double>(it.second));
      }
      return quetzal::random::DiscreteDistribution<coord_type>(std::move(X),std::move(w));
    };

    auto t = m_sampling_time;
    while( (forest.nb_trees() > 1) && (t > m_t0) ){
      quetzal::random::TransitionKernel<time_type, quetzal::random::DiscreteDistribution<coord_type>> backward_kernel;

      forest_type new_forest;
      for(auto const it : forest){
        coord_type x = it.first;
        if( ! backward_kernel.has_distribution(x, t)){
          backward_kernel.set(x, t, make_backward_distribution(x, t));
        }
        coord_type y = backward_kernel(gen, x, t);
        new_forest.insert(y, it.second);
      }

      assert(forest.nb_trees() == new_forest.nb_trees());
      forest = new_forest;

      using merger_type = quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>;

      for(auto const & x : forest.positions()){

        auto range = forest.trees_at_same_position(x);

        std::vector<unsigned int> v;
        for(auto it = range.first; it != range.second; ++it){
          v.push_back(it->second);
        }
        if(v.size() >= 2){
          auto last = merger_type::merge(
            v.begin(),
            v.end(),
            N(x, t),
            0,
            std::plus<unsigned int>(),
            gen
          );

          forest.erase(x);
          for(auto it = v.begin(); it != last; ++it){
            forest.insert(x, *it);
          }
        }

        }
      --t;
    }

    return forest;

  }

}; // GenerativeModel



int main(){

  std::mt19937 gen;
  GenerativeModel model;

  auto prior = [](auto& gen){
    GenerativeModel::param_type params;
    params.k(std::uniform_int_distribution<>(1,500)(gen));
    params.r(100);
    params.m(0.1);
    return params;
  };

  auto abc = quetzal::abc::make_ABC(model, prior);
  auto table = abc.sample_prior_predictive_distribution(200000, gen);

  auto eta = [](auto const& forest){
    std::vector<unsigned int> v;
    for(auto const& it: forest){
      v.push_back(it.second);
    }
    std::sort(v.begin(), v.end());
    return v;
  };

  auto sum_stats = table.compute_summary_statistics(eta);

  auto to_json_str = [](auto const& p){
    return "{\"r\":"+ std::to_string(p.r()) +
           ",\"k\":" + std::to_string(p.k()) + "}";
  };

  //multiple rejections
  auto true_param = prior(gen);
  true_param.k(50);
  std::vector<std::vector<unsigned int>> pods;

  for(int i = 0; i != 100; ++i){
    try {
      pods.push_back(eta(model(gen, true_param)));
    }catch(...){
      std::cerr << "one pod less" << std::endl;
    }
  }

  std::string buffer = "{";
  unsigned int pod_id = 1;
  for(auto const& it1 : pods){
    buffer = buffer + "\"" + std::to_string(pod_id) + "\":[";
    bool is_empty = true;
    for(auto const& it2 : sum_stats){
      if(it2.data() == it1){
        buffer += to_json_str(it2.param());
        buffer += ",";
        is_empty = false;
      }
    }
    if(!is_empty){
        buffer.pop_back();
    }
    buffer += "],";
    pod_id += 1;
  }
  buffer.pop_back();
  buffer += "}";

  std::cout << buffer << std::endl;
  return 0;
}
