// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __OCCUPANCY_SPECTRUM_DISTRIBUTION_H_INCLUDED__
#define __OCCUPANCY_SPECTRUM_DISTRIBUTION_H_INCLUDED__

#include "Generator.h"

#include <random> // discrete_distribution
#include <utility> // std::forward, std::move
#include <vector>
#include <cmath> // std::pow
#include <assert.h>
#include <iostream> // << operator implementation

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/random.hpp> // boost::multiprecision::pow

namespace quetzal{
namespace coalescence{
namespace occupancy_spectrum {

  using boost::multiprecision::cpp_int;
  using boost::multiprecision::cpp_dec_float_50;

  struct ReturnAlwaysTrue {
      bool operator()(double param) const
      {
        return true;
      }
  };

  struct Identity {
      using spectrum_type = Generator::occupancy_spectrum_type;
      static spectrum_type handle(spectrum_type&& M_j)
      {
          return std::move(M_j);
      }
  };

  struct RetrieveLastEmptyUrns {
    using spectrum_type = Generator::occupancy_spectrum_type;
    static spectrum_type handle(spectrum_type&& M_j)
    {
      auto first = --(M_j.end());
      while (first != M_j.begin() && *first == 0)
      {
        --first;
      }
      M_j.erase(++first, M_j.end());
      return std::move(M_j);
    }
  };

  template
  <
  class UnaryPredicate = ReturnAlwaysTrue,
  class SpectrumHandler = Identity,
  class Int = cpp_int,
  class Float = cpp_dec_float_50
  >
  class OccupancySpectrumDistribution {

    using SelfType = OccupancySpectrumDistribution<UnaryPredicate, SpectrumHandler, Int, Float>;
    using spectrum_type = typename Generator::occupancy_spectrum_type;
    using support_type = std::vector<spectrum_type>;
    using probabilities_type = std::vector<double>;

    unsigned int m_k;
    unsigned int m_N;
    UnaryPredicate m_pred;
    support_type m_support;
    probabilities_type m_probas;
    mutable std::discrete_distribution<size_t> m_dist;

  public:

    //! \remark Copy constructor
    OccupancySpectrumDistribution(const SelfType&) = delete;

    //! \remark Move constructor
    OccupancySpectrumDistribution(SelfType&&) = default;

    //! \remark Default constructor
    OccupancySpectrumDistribution() = default;

    //! \remark copy assignment operator
    SelfType & operator= (const SelfType& ) = delete;

    //! \remark move assignment operator
    SelfType & operator= (SelfType&& ) = default;

    //! \remark heavy constructor for large k
    OccupancySpectrumDistribution(unsigned int k, unsigned int N, UnaryPredicate pred = UnaryPredicate()) : m_k(k), m_N(N), m_pred(pred) {
      auto callback = make_callback(k, N, pred);
      Generator::generate(k, N, callback);
      m_dist = std::discrete_distribution<size_t>( m_probas.cbegin(), m_probas.cend() );
      assert(m_support.size() == m_probas.size());
    }

    template<typename Generator>
    const spectrum_type & operator()(Generator& g) const {
      return m_support[m_dist(g)];
    }

    unsigned int k() const {return m_k; }

    unsigned int N() const {return m_N; }

    const support_type& support() const {return m_support; }

    const probabilities_type & weights() const {return m_probas; }

    friend std::ostream& operator<<(std::ostream& os, SelfType const& dist){
        for(unsigned int i = 0; i < dist.support().size(); ++ i)
        {
          //os << "P( \t" << dist.support()[i] << "\t) = " << dist.weights()[i] << "\n";
          auto v = dist.support()[i];
          os << "P( \t";
          if ( !v.empty() ) {
            os << '[';
            std::copy (v.begin(), v.end(), std::ostream_iterator<unsigned int>(os, ", "));
            os << "\b\b]";
          }
           os << "\t) = " << dist.weights()[i] << "\n";
        }
        return os;
    }

  private:

    auto make_callback(unsigned int k, unsigned int N, UnaryPredicate pred){
      return [this, k, N](spectrum_type&& M_j){
        auto p = compute_probability(k, N, M_j);
        if(this->m_pred(p)){
          this->m_support.push_back(SpectrumHandler::handle(std::move(M_j)));
          this->m_probas.push_back(p);
        }
      };
    }

    static Int Factorial(int num) {
      Int fact = 1;
      for(int i=num; i>1; --i){
        fact *= i;
      }
      return fact;
    }


    static double compute_probability(unsigned int n, unsigned int m, spectrum_type const& m_js) {
      Float numerator(Factorial(m)*Factorial(n));
      Float denominator(1.0);
      int j = 0;
      for(auto m_j : m_js){
        denominator *= boost::multiprecision::pow(Float(Factorial(j)),Float(m_j)) * Float(Factorial(m_j));
        ++j;
      }
      denominator *= Float(std::pow(m,n));
      Float result = numerator/denominator;
      return static_cast<double>(result) ;
    }

  };

} // namespace occupancy_spectrum
} // namespace coalescence
} // namespace quetzal

#endif
