
//  compiles with g++ -o example example1.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "../../../quetzal.h"

#include <memory>
#include <random>
#include <functional>  // std::plus

class GenerativeModel{

private:

  class Params{
  public:
    auto k() const {return m_k; }
    void k(unsigned int value) { m_k = value; }

    auto r() const { return m_r; }
    void r(unsigned int value) { m_r = value; }

    auto mu() const { return m_mu; }
    void mu(double value) { m_mu = value; }

    auto sigma() const { return m_sigma; }
    void sigma(double value) { m_sigma = value; }

  private:
    unsigned int m_k;
    unsigned int m_r;
    double m_sigma;
    double m_mu;
  };

  using generator_type = std::mt19937;
  using time_type = unsigned int;
  using key_type = std::string;
  using env_type = quetzal::geography::DiscreteLandscape<key_type, std::string>;
  using coord_type = typename env_type::coord_type;
  using N_type = unsigned int;
  using pop_size_type = quetzal::demography::PopulationSize<coord_type, time_type, N_type>;
  using flux_type = quetzal::demography::PopulationFlux<coord_type, time_type, N_type>;
  using forest_type = quetzal::coalescence::Forest<coord_type, unsigned int>;
  using dispersal_kernel_type = quetzal::random::TransitionKernel<quetzal::random::DiscreteDistribution<coord_type>>;
  std::shared_ptr<dispersal_kernel_type> m_dispersal_kernel;
  std::shared_ptr<env_type> m_env;

public:

  using param_type = Params;

  GenerativeModel(){
    std::set<std::string> times = {"present"};
    std::map<key_type, std::string> files = {{"prec","wc2.0_10m_prec_01_europe_agg_fact_10.tif"}};
    m_env = std::make_shared<env_type>(files, times);
    m_dispersal_kernel = std::make_shared<dispersal_kernel_type>();
  }

  auto make_prior() const {
    auto prior = [](auto& gen){
      Params params;
      params.k(std::uniform_int_distribution<>(1,100)(gen));
      params.r(2);
      params.mu(0);
      params.sigma(1000);
      return params;
    };
    return prior;
  }

  auto operator()(generator_type& gen, Params const& param) const {
    return simulate(gen, param);
  }

  forest_type simulate(generator_type& gen, Params const& param) const {

    const auto& X = m_env->geographic_definition_space();
    std::vector<time_type> T = {2001,2002,2003,2004,2005,2006,2007,2008,2009,2010};

    pop_size_type N;
    coord_type Bordeaux(44.0,0.33);
    coord_type origin(m_env->reproject_to_centroid(Bordeaux));
    N(origin, 2001) = 10;

    flux_type Phi;

    // Building mathematical expressions of space and time
    using quetzal::expressive::literal_factory;
    using quetzal::expressive::use;

    quetzal::expressive::literal_factory<const coord_type&, time_type> lit;

    // Growth expressions
    auto r = lit(param.r());
    auto K = lit(param.k());
    auto N_expr = use([&N](coord_type const& x, time_type t){return N(x,t);});
    auto g = N_expr*(lit(1)+r)/ (lit(1)+((r * N_expr)/K));

    auto sim_N_tilde = [g](generator_type& gen, coord_type const& x, time_type t){
      std::poisson_distribution<N_type> poisson(g(x,t));
      return poisson(gen);
    };

    // Dispersal expressions
    auto gaussian  = [&param](coord_type::km d){
      auto sigma = param.sigma();
      auto mu = param.mu();
      double pi = 3.14159265358979323846;
      return ( 1./(sigma*sqrt(2.*pi))*exp(-pow(d-mu,2.)/(2.*pow(sigma, 2.))));
    };

    auto distance = [](coord_type const& x, coord_type const& y){
      return x.great_circle_distance_to(y);
    };

    auto m = quetzal::expressive::compose(gaussian, distance);

    auto make_dispersal_distribution = [X, m](coord_type const& x){
      std::vector<double> w;
      w.reserve(X.size());
      for(auto const& it : X){
        w.push_back(m(x,it));
      }
      return quetzal::random::DiscreteDistribution<coord_type>(X,std::move(w));
    };

    // Demographic process
    for(auto t : T){
      unsigned int total_propagules = 0;
      for(auto x : X){
        auto N_tilde = sim_N_tilde(gen, x, t);
        if(N_tilde >= 1){
          total_propagules += N_tilde;
          for(unsigned int i = 1; i <= N_tilde; ++i){
            if( ! m_dispersal_kernel->has_distribution(x)){
              m_dispersal_kernel->set(x, make_dispersal_distribution(x));
            }
            coord_type y = m_dispersal_kernel->operator()(gen, x);
            Phi(x,y,t) += 1;
            time_type t2 = t; ++t2;
            N(y,t2) += 1;
          }
        }
      }
      if(total_propagules == 0){
        throw std::domain_error("Population went extinct before sampling");
      }
    }

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

    // Coalescence process, sampling time = 2010

    forest_type forest;
    coord_type Paris(48.5,2.2);
    coord_type sample_deme = m_env->reproject_to_centroid(Paris);

    unsigned int sample_size = 20;
    forest.insert(sample_deme, std::vector<unsigned int>(sample_size, 1));

    if(N(sample_deme, 2010) < sample_size){
      throw std::domain_error("Simulated population size inferior to sampling size");
    }

    time_type t = 2009;
    while( (forest.nb_trees() > 1) && (t > 2000) ){
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

      using quetzal::coalescence::BinaryMerger;

      for(auto const & x : forest.positions()){

        auto range = forest.trees_at_same_position(x);

        std::vector<unsigned int> v;
        for(auto it = range.first; it != range.second; ++it){
          v.push_back(it->second);
        }

        if(v.size() >= 2){
          auto last = BinaryMerger::merge(
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

  } // simulate

}; // GenerativeModel

int main(){

  std::mt19937 gen;
  GenerativeModel model;
  auto abc = quetzal::abc::make_ABC(model, model.make_prior());
  auto table = abc.sample_prior_predictive_distribution(2000, gen);

  auto eta = [](auto const& forest){
    std::vector<unsigned int> v;
    for(auto const& it: forest){
      v.push_back(it.second);
    }
    std::sort(v.begin(), v.end());
    return v;
  };

  auto sum_stats = table.compute_summary_statistics(eta);

  auto print = [](auto const& p){
    std::cout << p.mu() << "\t" << p.sigma() << "\t" << p.r() << "\t" << p.k() << std::endl;
  };

  auto pod = sum_stats.begin()->data();
  auto theta = sum_stats.begin()->param();
  std::cout << "mu\tsigma\tr\tk" << std::endl;
  print(theta);

  for(auto const& it : sum_stats){
    if(it.data() == pod){
      print(it.param());
    }
  }

  return 0;
}
