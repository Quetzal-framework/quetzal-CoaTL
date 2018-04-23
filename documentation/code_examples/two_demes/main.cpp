
//  compiles with g++ -o3 main.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "../../../quetzal.h"
#include "../../../modules/simulator/simulators.h"

#include <memory>
#include <random>
#include <functional>  // std::plus
#include <map>


template<typename Space, typename Time, typename Value>
std::ostream& operator <<(std::ostream& stream, const quetzal::demography::PopulationFlux<Space, Time, Value>& flows)
{
  for(auto const& it : flows){
    stream << it.first.time << "\t" << it.first.from <<  "\t" << it.first.to << "\t" << it.second << "\n";
  }
    return stream;
}

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
  using N_type = unsigned int;
  using history_type = quetzal::demography::History<coord_type, time_type, N_type>;

  // Coalescence simulation types
  using tree_type = unsigned int;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;

  using simulator_type = quetzal::simulators::IDDC_model_1<coord_type, time_type, N_type>;

  // Demic structure
  std::vector<coord_type> m_demes = {-1,1};

  // Initial distribution
  coord_type m_x0 = -1;
  time_type m_t0 = 2000;
  N_type m_N0 = 100;

  // Genetic sample
  time_type m_sampling_time = 2009;
  unsigned int m_sampling_size = 40;
  coord_type m_sampling_deme = 1;

public:

  using param_type = Params;
  using result_type = forest_type;

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

  auto make_dispersal_kernel(param_type const& param) const
  {
     auto kernel = [&param, this](auto& gen, coord_type x, time_type t){
      std::bernoulli_distribution d(param.m());
      if(d(gen)){ x = -x; }
      return x;
    };
    return kernel;
  }

  forest_type operator()(generator_type& gen, param_type const& param) const
  {
    simulator_type simulator(m_x0, m_t0, m_N0);

    auto growth = make_growth_expression(param, simulator.size_history());
    auto kernel = make_dispersal_kernel(param);

    forest_type forest;
    forest.insert(m_x0, std::vector<tree_type>(m_sampling_size, 1));

    return simulator.coalesce(forest, growth, kernel, m_sampling_time, gen);
  }

}; // GenerativeModel



int main()
{

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
