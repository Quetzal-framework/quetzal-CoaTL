
//  compiles with g++ -o3 two_demes.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "/home/becheler/dev/quetzal/quetzal.h"

#include <memory>
#include <random>
#include <functional>  // std::plus
#include <map>

/*
namespace quetzal {
namespace demography {

  template<typename Space, typename Time, typename Value>
  class History;

  template<typename Space, typename Time>
  class History<Space, Time, unsigned int>{

  public:

    using flow_type = PopulationFlux<Space, Time, unsigned int>;
    using N_type = PopulationSize<Space, Time, unsigned int>;
    using coord_type = Space;
    using time_type = Time;

  private:
    N_type m_sizes;
    flow_type m_flows;
    std::vector<Time> m_times;

  public:

    History(coord_type const& x, time_type const& t, unsigned int N){
      m_sizes(x,t) = N;
      m_times.push_back(t);
    }

    flow_type const& flows() const {return m_flows;}
    N_type const& N() const {return m_sizes;}
    time_type const& first_time() const {return m_times.front(); }
    time_type const& last_time() const {return m_times.back(); }

    template<typename Growth, typename Dispersal, typename Generator>
    void expand(unsigned int nb_generations, Growth sim_growth, Dispersal kernel, Generator& gen) {
      for(unsigned int g = 1; g != nb_generations; g++){
        auto t = *m_times.rbegin();
        auto t_next = t; ++ t_next;
        unsigned int landscape_count = 0;
        for(auto x : m_sizes.definition_space(t) ){
          auto N_tilde = sim_growth(gen, x, t);
          landscape_count += N_tilde;
          if(N_tilde >= 1){
            for(unsigned int ind = 1; ind <= N_tilde; ++ind){
              coord_type y = kernel(gen, x, t);
              m_flows(x, y, t) += 1;
              m_sizes(y, t_next) += 1;
            }
          }
        }
        if(landscape_count == 0){
          throw std::domain_error("Landscape populations went extinct before sampling");
        }
        m_times.push_back(t_next);
      }
    }
  };

} // namespace demography
} // namespace quetzal

namespace quetzal {
namespace coalescence {

template<typename... Args>
class Simulator;

template<typename Space, typename Time, typename Value>
class Simulator<quetzal::demography::History<Space, Time, Value>> {

public:
  using merger_type = quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>;
  using history_type = quetzal::demography::History<Space, Time, Value>;
  using coord_type = Space;
  using time_type = Time;

  template<typename Tree>
  using forest_type = quetzal::coalescence::Forest<coord_type, Tree>;

private:

  using discrete_distribution_type = quetzal::random::DiscreteDistribution<coord_type>;
  using backward_kernel_type = quetzal::random::TransitionKernel<time_type, discrete_distribution_type>;

  history_type const& m_history;
  backward_kernel_type m_kernel;

  auto make_backward_distribution(coord_type const& x, time_type const& t){
    std::vector<double> w;
    std::vector<coord_type> X;
    assert(m_history.flows().flux_to_is_defined(x,t));
    auto const& flux_to = m_history.flows().flux_to(x,t);
    w.reserve(flux_to.size());
    X.reserve(flux_to.size());
    for(auto const& it : flux_to){
      X.push_back(it.first);
      w.push_back(static_cast<double>(it.second));
    }
    return discrete_distribution_type(std::move(X),std::move(w));
  }

  template<typename Generator, typename Tree>
  void sim_backward_migration(Generator& gen, forest_type<Tree>& forest, time_type const& t){
    forest_type<Tree> new_forest;
    for(auto const it : forest){
      coord_type x = it.first;
      if( ! m_kernel.has_distribution(x, t)){
        m_kernel.set(x, t, make_backward_distribution(x, t));
      }
      new_forest.insert(m_kernel(gen, x, t), it.second);
    }
    assert(forest.nb_trees() == new_forest.nb_trees());
    forest = new_forest;
  }

  template<typename Generator, typename Tree>
  void coalesce(Generator& gen, forest_type<Tree>& forest, time_type const& t){
    for(auto const & x : forest.positions()){
      auto range = forest.trees_at_same_position(x);
      std::vector<Tree> v;
      for(auto it = range.first; it != range.second; ++it){
        v.push_back(it->second);
      }
      if(v.size() >= 2){
        auto last = merger_type::merge(v.begin(), v.end(), N(x, t), gen );
        forest.erase(x);
        for(auto it = v.begin(); it != last; ++it){
          forest.insert(x, *it);
        }
      }
    }
  }

public:

  Simulator(history_type const& history) : m_history(history){}

  template<typename Generator, typename Tree>
  forest_type<Tree> operator()(Generator& gen, forest_type<Tree> const& forest){
    auto t = m_history.last_time();
    while( (forest.nb_trees() > 1) && (t > m_history.first_time()) ){
      sim_backward_migration(gen, forest, t);
      coalesce(gen, forest, t);
      --t;
    }
    return forest;
  }

};

} // namespace coalescence
} // namespace quetzal
*/
class GenerativeModel{

private:

  class Params{
  public:
    auto k() const {return m_k; }
    void k(unsigned int value) { m_k = value; }

    auto r() const { return m_r; }
    void r(unsigned int value) { m_r = value; }

    auto m() const  { return m_m; }
    void m(double value) { m_m = value; }

  private:
    unsigned int m_k;
    unsigned int m_r;
    double m_m;
  };

  using generator_type = std::mt19937;

  // Spatio-temporal coordinates
  using coord_type = int;
  using time_type = unsigned int;

  // Demographic simulation types
  using history_type = quetzal::demography::History<coord_type, time_type, unsigned int>;

  // Coalescence simulation types
  using tree_type = unsigned int;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;

  // Demic structure
  std::vector<coord_type> m_demes = {-1,1};

  // Initial distribution
  coord_type m_x0 = 0;
  time_type m_t0 = 2000;
  unsigned int m_N0 = 100;
  unsigned int m_nb_generations = 10;

  // Genetic sample
  unsigned int m_sampling_size = 40;
  time_type m_sampling_time = 2009;
  coord_type m_sampling_deme = 1;

public:

  using param_type = Params;
  using result_type = forest_type;

  auto make_growth_expression(param_type const& param, history_type::N_type const& N) const {
    using quetzal::expressive::literal_factory;
    using quetzal::expressive::use;
    quetzal::expressive::literal_factory<coord_type, time_type> lit;
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

  auto make_dispersal_kernel(param_type const& param) const {
     auto k = [&param, this](auto& gen, coord_type x, time_type t){
      std::bernoulli_distribution d(param.m());
      if(d(gen)){ x = -x; }
      return x;
    };
    return k;
  }

  forest_type operator()(generator_type& gen, param_type const& param) const {
    history_type history(m_x0, m_t0, m_N0);
    auto growth = make_growth_expression(param, history.N());
    auto kernel = make_dispersal_kernel(param);
    history.expand(m_nb_generations, growth, kernel, gen);

    if( history.N()(m_sampling_deme, history.last_time()) < m_sampling_size){
      throw std::domain_error("Simulated population size inferior to sampling size");
    }

    std::vector<tree_type> trees(m_sampling_size, 1);
    forest_type forest;
    forest.insert(m_sampling_deme, trees);

    quetzal::coalescence::Simulator<history_type> coal(history);
    return coal(gen, forest);
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
  auto table = abc.sample_prior_predictive_distribution(1000000, gen);

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
