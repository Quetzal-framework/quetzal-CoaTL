

#include "../../../quetzal.h"

#include <random>

class GenerativeModel{

private:

  struct Params{
    auto r() const {return m_r;}
    auto sigma() const {return m_sigma;}
    auto mu() const {return m_mu;}

    unsigned int m_r;
    double m_sigma;
    double m_mu;
  };

public:

  auto prior() const {
    auto prior = [](auto& gen){
      Params params;
      params.r(std::uniform_real_distribution<double>(1.,5.)(gen));
      params.sigma(std::uniform_real_distribution<double>(1.,50.)(gen));
      params.mu(std::uniform_real_distribution<double>(1.,10.)(gen));
      return params;
    };
    return prior;
  }

  template<typename Generator>
  auto operator()(Generator& gen, Params const& param) const {
      return generateRandomDemography(gen, param);
  }


  template<typename Generator>
  auto generateRandomDemography(Generator& gen, Params const& param) const {

    Env E = read_env({"temperature.tif", "rainfall.tif"});

    using space_type = typename Env::space_type;
    const auto& X = E.geographic_space();

    using time_type = typename Env::time_type;
    const auto& T = E.temporal_space();

    using N_type = unsigned int;
    PopulationSize<space_type, time_type, N_type> N;
    N(X.begin(), T.begin()) = 10;

    PopulationFlux<space_type, time_type, N_type> Phi;
/*
    literal_factory<space_type, time_type> lit;

    auto r = lit(param.r());
    auto k = (use(std::cref(E[1]))+use(std::cref(E[2])))/lit(2);
    auto N_expr = use(std::cref(N));
    auto g = N_expr*(lit(1)+r)/ (lit(1)+((r * N_expr)/k));

    auto sim_N_tilde = [g](space_type const& x, time_type t, auto& gen){
      std::poisson_distribution<N_type> poisson(g(x,t));
      return poisson(gen);
    };

    auto gaussian  = [&param](auto x){
      auto sigma = param.sigma();
      auto mu = param.mu();
      double pi = 3.14159265358979323846;
      return ( 1./(sigma * sqrt(2. * pi)) * exp( - pow(x-mu,2.)/(2. * pow(sigma, 2.))));
    };

    auto distance = [](space_type const& x, space_type const& y){
      return x.distance_to(y);
    };

    auto m = compose(gaussian, distance);
    auto kernel = make_kernel<memoize>(X, m);
    auto kernel2 = make_dispersal_kernel<>(X, m);

    auto weights = [m](space_type const& x, auto const& space){
      std::vector<double> w;
      w.reserve(space.size());
      for(auto const& y : space){
        w.push_back(m(x,y));
      }
      return w;
    };

    auto source = [weights](space_type const& x, auto const& space){
      return std::discrete_distribution<size_t>(weights(x, space));
    };

    ExpandingTransitionKernel<space_type, DiscreteDistribution, decltype(source)>
    auto kernel = [](space_type const& x, auto& gen){
      DiscreteDistribution<space_type> dis()
    };

    for(auto t : T){
      for(auto x : N.definition_space(t)){
        auto N_tilde = sim_N_tilde(x,t);
        for(auto i = 1; i <= N_tilde; ++i){
          auto y = kernel(x,gen);
          Phi(x,y,t) += 1;
          t2 = t; ++t2;
          N(y,t2) += 1;
        }
      }
    }
  */
    return std::make_pair(N,Phi);

  }

};

int main(){

  std::mt19937 gen;
  GenerativeModel model;
  auto abc = abc::make_ABC(model, model.prior());
  auto table = abc.sample_prior_predictive_distribution(30, gen);

  return 0;
}
