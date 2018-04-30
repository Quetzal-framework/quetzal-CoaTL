
//  compiles with g++ -o3 main.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "../../../quetzal.h"
#include "../../../modules/simulator/simulators.h"
#include "../../../modules/fuzzy_transfer_distance/FuzzyPartition.h"
#include <memory>
#include <random>
#include <functional>  // std::plus
#include <map>
#include <cmath>

namespace quetzal{
  namespace demography {
    namespace dispersal {

//nathan et al 2012, table 15.1
struct Logistic
{
  static double pdf(double r, double a, double b)
  {
    assert(a > 0 && b >2 && r >= 0);
    return (b/(2*M_PI*(a*a)*std::tgamma(2/b)*std::tgamma(1-2/b)))*(1/(1+(std::pow(r,b)/(std::pow(a,b)))));
  }
};

struct Gaussian
{
  static double pdf(double r, double a)
  {
    assert(a > 0 && r >= 0);
    return 1/(M_PI*a*a) * std::exp(-(r*r)/(a*a)) ;
  }
};

struct NegativeExponential
{
  static double pdf(double r, double a)
  {
    assert(a > 0 && r >= 0);
    return  (1/2*M_PI*a*a)*std::exp(-r/a);
  }
};

template<typename Kernel, typename Space, typename... Args>
quetzal::random::DiscreteDistribution<Space>
make_dispersal_location_random_distribution(Space const& x, std::vector<Space> const& demes, Args&&... args)
{
  std::vector<double> weights;
  weights.reserve(demes.size());
  for(auto const& it : demes)
  {
    weights.push_back(Kernel::pdf(std::forward<Args>(args)...));
  }
  return quetzal::random::DiscreteDistribution<Space>(demes, weights);
}

template<typename Kernel, typename Space, typename... Args>
quetzal::random::DiscreteDistribution<Space>
make_dispersal_kernel(std::vector<Space> const& demes, Args&&... args)
{
  assert(!demes.empty());
  using law_type =  quetzal::random::DiscreteDistribution<Space>;
  quetzal::random::TransitionKernel<law_type> kernel;
  for(auto const& it : demes)
  {
    kernel.set(it, make_dispersal_location_random_distribution(it, demes, std::forward<Args>(args)...));
  }
  return kernel;
}

}}}

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

    auto N0() const {return m_N0; }
    void N0(unsigned int N) { m_N0 = N; }

    auto k() const {return m_k; }
    void k(unsigned int value) { m_k = value; }

    auto r() const { return m_r; }
    void r(unsigned int value) { m_r = value; }

    auto a() const  { return m_a; }
    void a(double value) { m_a = value; }

  private:
    unsigned int m_k;
    unsigned int m_r;
    double m_a;
    unsigned int m_N0;
  };

  using generator_type = std::mt19937;

  // Spatio-temporal coordinates
  using time_type = unsigned int;
  using landscape_type = quetzal::geography::EnvironmentalQuantity<time_type>;
  using coord_type = typename landscape_type::coord_type;

  // Demographic simulation types
  using N_type = unsigned int;
  using history_type = quetzal::demography::History<coord_type, time_type, N_type>;
  using simulator_type = quetzal::simulators::IDDC_model_1<coord_type, time_type, N_type>;

  // Landscape
  landscape_type m_landscape;

  // Initial distribution
  coord_type m_x0 = coord_type(44.00, 0.20);
  time_type m_t0 = 2004;
  N_type m_N0;

  // Genetic sample
  using loader_type = quetzal::genetics::Loader<typename landscape_type::coord_type, typename quetzal::genetics::microsatellite>;
  using dataset_type = loader_type::return_type;
  dataset_type m_dataset;
  time_type m_sampling_time = 2008;

  // Coalescence simulation
  using tree_type = std::vector<coord_type>;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;
  forest_type m_forest;

public:

  // Interface for ABC module
  using param_type = Params;
  using result_type = forest_type;

  GenerativeModel():
  m_landscape("/home/Downloads/wc2.0_5m_bio/bio1.tif", {2000,2001})
  {
    std::string file = "/home/becheler/Documents/VespaVelutina/DataForAnalysis.csv";
    loader_type loader;
    auto dataset = loader.read(file);
    m_dataset = dataset.reproject(m_landscape);

    auto sampled_demes = m_dataset.get_sampling_points();
    for(auto const& it : sampled_demes)
    {
      m_forest.insert(it, std::vector<coord_type>(dataset.size(it)));
    }

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

  auto operator()(generator_type& gen, param_type const& param) const
  {
    simulator_type simulator(m_x0, m_t0, m_N0);
    using namespace quetzal::demography::dispersal;
    using kernel_type = Gaussian;
    auto heavy_kernel = make_dispersal_kernel<kernel_type>(m_landscape.geographic_definition_space(), param.a());
    auto light_kernel = [&heavy_kernel](auto& gen, coord_type x, time_type t){ return heavy_kernel(gen, x); };
    auto growth = make_growth_expression(param, simulator.size_history());

    simulator.coalesce(m_forest, growth, light_kernel, m_sampling_time, gen);

    std::unordered_map<coord_type, std::vector<double>> coeffs;
    for(auto const& it : m_landscape.geographic_definition_space())
    {
      coeffs[it].reserve(m_forest.nb_trees());
    }

    unsigned int cluster_id = 0;
    for(auto const& it1 : m_forest )
    {
      for(auto const& it2 : it1.second)
      {
        coeffs[it2][cluster_id] += 1;
      }
      cluster_id += 1;
    }

    for(auto & it1 : coeffs){
      unsigned int sum = std::accumulate(it1.second.begin(), it1.second.end(), 0);
      assert(sum > 0.0);
      for(auto & it2 : it1.second){
        it2 = it2/sum;
      }
    }

    return FuzzyPartition<coord_type>(coeffs);
  }

}; // GenerativeModel



int main()
{
  std::mt19937 gen;
  GenerativeModel model;

  auto prior = [](auto& gen){
    GenerativeModel::param_type params;
    params.N0(std::uniform_int_distribution<>(1,15)(gen));
    params.k(std::uniform_int_distribution<>(1,500)(gen));
    params.r(100);
    params.a(60);
    return params;
  };

  auto abc = quetzal::abc::make_ABC(model, prior);
  auto table = abc.sample_prior_predictive_distribution(1000, gen);

  auto eta = [](auto const& m_forest){

    std::vector<unsigned int> v;
    for(auto const& it: m_forest){
      v.push_back(it.second);
    }
    std::sort(v.begin(), v.end());
    return f;
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
