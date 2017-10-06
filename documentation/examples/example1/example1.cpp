
//  compiles with g++ -o example example1.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "../../../quetzal.h"

#include <random>
#include <functional>  // std::plus
#include <iterator> // iterator_traits
class GenerativeModel{

private:

  class Params{
  public:
    auto r() const { return m_r; }
    void r(unsigned int value) { m_r = value; }

    auto mu() const { return m_mu; }
    void mu(double value) { m_mu = value; }

    auto sigma() const { return m_sigma; }
    void sigma(double value) { m_sigma = value; }

  private:
    unsigned int m_r;
    double m_sigma;
    double m_mu;
  };

  using generator_type = std::mt19937;
  using time_type = unsigned int;
  using key_type = std::string;
  using env_type = quetzal::geography::DiscreteLandscape<key_type, time_type>;
  using coord_type = typename env_type::coord_type;
  using N_type = unsigned int;
  using pop_size_type = quetzal::demography::PopulationSize<coord_type, time_type, N_type>;
  using flux_type = quetzal::demography::PopulationFlux<coord_type, time_type, N_type>;
  using forest_type = quetzal::coalescence::Forest<coord_type, unsigned int>;

public:

  using param_type = Params;

  auto make_prior() const {
    auto prior = [](auto& gen){
      Params params;
      params.r(std::uniform_real_distribution<double>(2.,10.)(gen));
      params.sigma(std::uniform_real_distribution<double>(1000.,100000.)(gen));
      params.mu(std::uniform_real_distribution<double>(0.,1000.)(gen));
      return params;
    };
    return prior;
  }

  auto operator()(generator_type& gen, Params const& param) const {
    return simulate(gen, param);
  }

  forest_type simulate(generator_type& gen, Params const& param) const {

    env_type E({{"bio1","bio1.tif"},{"bio12","bio12.tif"}},
               {2001,2002,2003,2004,2005,2006,2007,2008,2009,2010});

    const auto& X = E.geographic_definition_space();
    const auto& T = E.temporal_definition_space();

    pop_size_type N;
    coord_type Bordeaux(44.0,0.33);
    coord_type origin(E.reproject_to_centroid(Bordeaux));
    N(origin, 2001) = 10;

    flux_type Phi;

    // Building mathematical expressions of space and time
    using quetzal::expressive::literal_factory;
    using quetzal::expressive::use;

    quetzal::expressive::literal_factory<const coord_type&, time_type> lit;

    // Growth expressions
    auto r = lit(param.r());
    //auto k = (use(E["bio1"]) + use(E["bio12"]))/lit(2);
    auto k = lit(10);
    auto N_expr = use([&N](coord_type const& x, time_type t){return N(x,t);});
    auto g = N_expr*(lit(1)+r)/ (lit(1)+((r * N_expr)/k));

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

    using quetzal::random::TransitionKernel;
    using quetzal::random::DiscreteDistribution;
    TransitionKernel<DiscreteDistribution<coord_type>> dispersal_kernel;

    // Demographic process
    for(auto t : T){
      for(auto x : X){
        auto N_tilde = sim_N_tilde(gen, x, t);
        for(unsigned int i = 1; i <= N_tilde; ++i){
          if( ! dispersal_kernel.has_distribution(x)){
            dispersal_kernel.set(x, make_dispersal_distribution(x));
          }
          coord_type y = dispersal_kernel(gen, x);
          Phi(x,y,t) += 1;
          time_type t2 = t; ++t2;
          N(y,t2) += 1;
        }
      }
    }

    auto make_backward_distribution = [&Phi](coord_type const& x, time_type t){
      std::vector<double> w;
      std::vector<coord_type> X;
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
    coord_type sample_deme = E.reproject_to_centroid(Paris);

    unsigned int sample_size = 5;
    forest.insert(sample_deme, std::vector<unsigned int>(sample_size, 1));

    if(N(origin, 2010) < sample_size){
      throw std::domain_error("Simulated population size inferior to sampling size");
    }

    time_type t = 2009;
    while( (forest.nb_trees() > 1) && (t > 2000) ){
      TransitionKernel<time_type, DiscreteDistribution<coord_type>> backward_kernel;

      std::cout << "Forest before migration:" << std::endl;
      for(auto const& it : forest){
        std::cout << it.first << " " << it.second << "\n";
      }
      std::cout << std::endl;

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

      std::cout << "Forest after migration:" << std::endl;
      for(auto const& it : forest){
        std::cout << it.first << " " << it.second << "\n";
      }
      std::cout << std::endl;

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
  auto table = abc.sample_prior_predictive_distribution(30, gen);

  unsigned int id = 0;
  for(auto const& it : table){
    std::cout << "Sim " << id << "\t"
              << "mu = " << it.param().mu() << "\t"
              << "r = " << it.param().r() << "\t"
              << "sigma = " << it.param().sigma() << "\nData:\n";
    auto const& forest = it.data();
    for(auto const& it2 : forest ){
        std::cout << it2.first << " <----> " << it2.second << "\n";
    }
    std::cout << std::endl;
    ++id;
  }

  return 0;
}
