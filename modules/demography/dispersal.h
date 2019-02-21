// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include <cmath>
#include <vector>
#include <unordered_map>
#include <random>

// SymmetricDistanceMatrix
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

namespace quetzal{
namespace demography {
namespace dispersal {

/* Class to represent the distance matrix between each pair of points in a set.
 *
 * @details Useful e.g. to implement distance based dispersal kernels.
 * @tparam Space the type of points
 * @tparam Value the type of the distance value between pairs of points
 */
template<typename Space, typename Value>
class SymmetricDistanceMatrix
{

public:

  //! @typdef the type of the matrix used in the implementation
  template<typename T>
  using matrix_type = boost::numeric::ublas::symmetric_matrix<T>;

  //! @typedef point data type
  using coord_type = Space;

  //! @typedef distance value type
  using value_type = Value;

  /* Construct the matrix by computing distance between each pair of points in the set.
   *
   * @tparam F the distance function, with signature equivalent to the following:
   *           `value_type f(const coord_type & x, const coord_type & y)`
   */
  template<typename F>
  SymmetricDistanceMatrix(std::vector<coord_type> const& points, F f):
  m_points(points),
  m_matrix(construct_matrix(points, f))
  {}

  //! Send matrix in a stream
  friend std::ostream& operator <<(std::ostream& Stream, const SymmetricDistanceMatrix & M)
  {
     Stream << M.m_matrix;
     return Stream;
  }

  /* Apply a functor over the matrix and return the resulting new matrix
   *
   * @tparam UnaryOperation the transformation to apply to each matrix value.
   * @param op UnaryOperation object. The signature should be equivalent to
   *           `Ret op(value_type)`, with `Ret` being possibly any Copiable type.
   */
  template<typename UnaryOperation>
  auto apply(UnaryOperation op) const
  {
   using other_type = typename std::result_of_t<UnaryOperation(std::remove_cv_t<std::remove_reference_t<const value_type &>>)>;
   matrix_type<other_type> m (m_points.size(), m_points.size());
   for (unsigned i = 0; i < m.size1 (); ++ i){
     for (unsigned j = 0; j <= i; ++ j){
       m (i, j) = op(m_matrix(i, j));
     }
   }
   return SymmetricDistanceMatrix<coord_type, other_type>(m_points, m);
  }

  //! Return the set of points
  std::vector<coord_type> const& points(){return m_points;}

  /* For a given point, give the distance to all other points
   * @param x the given point
   * @return a boost::numeric::ublas::row
   */
  auto at(coord_type const& x) const
  {
    return boost::numeric::ublas::row(m_matrix, ID(x));
  }

  value_type operator()(coord_type const& x, coord_type const& y) const {
    return m_matrix(ID(x),ID(y));
  }

private:

  std::vector<Space> m_points;
  matrix_type<Value> m_matrix;

  unsigned int ID(coord_type const& x) const {
    auto it = std::find(m_points.begin(), m_points.end(), x);
    assert( it != m_points.end());
    return std::distance(m_points.begin(), it);
  }

  template<typename F>
  auto construct_matrix(std::vector<coord_type> const& points, F f) const
  {
    matrix_type<value_type> m (points.size(), points.size());
    for (unsigned i = 0; i < m.size1 (); ++ i)
    {
      for (unsigned j = 0; j <= i; ++ j)
      {
        m (i, j) = f(points.at(i), points.at(j));
      }
    }
    return m;
  }

};

// @brief Constructs a symmetric matrix of distances between points of the space.
// @tparam Space Demes identifiers (geographic points)
// @tparam F a functor implementing a symetric distance relationship between two
//         geographic points.
// @param points the set of geographic points to consider
// @param f a functor giving the distance between two demes and returning any copiable type T.
//        The signature should be equivalent to the following `T f(Space const& a, Space const& b)`
// @return a symmetric distance matrix
template<typename Space, typename F>
auto make_symmetric_distance_matrix(std::vector<Space> const& points, F f){
  using value_type = typename std::result_of_t<F(std::remove_cv_t<std::remove_reference_t<const Space &>>,
                                                 std::remove_cv_t<std::remove_reference_t<const Space &>>)>;
  return SymmetricDistanceMatrix<Space, value_type>(points, f);
}


// @brief Sampling distribution in the demic structure
// @tparam Space the deme identifier type (may be integers, geographic coordinates...)
//
// Allows to sample a destination point conditioning on the present location
template<typename Space>
class DiscreteLocationSampler
{
public:

  // @typedef the deme identifier
  using coord_type = Space;

private:
  std::vector<coord_type> m_points;

  using ID_type = unsigned int;
  std::unordered_map<coord_type, ID_type> m_dico;

  using distribution_type = std::discrete_distribution<ID_type>;
  mutable std::vector<distribution_type> m_kernels;

  std::unordered_map<coord_type, unsigned int> make_dico(std::vector<coord_type> const& points) const
  {
    unsigned int position = 0;
    std::unordered_map<coord_type, unsigned int> m;
    for(auto const& it : points)
    {
      m.emplace(it, position);
      ++position;
    }
    return m;
  }

public:

  DiscreteLocationSampler(std::vector<coord_type> const& points, std::vector<distribution_type> && kernels):
  m_points(points),
  m_dico(make_dico(points)),
  m_kernels(kernels)
  {}

  // @brief samples a destination in the location probability distribution
  // @tparam Generator a random number generator
  // @param x the present location
  // @param gen a random number generator
  // @return a deme sampled in the discrete probability distribution of arrivals.
  template<typename Generator>
  coord_type operator()(coord_type const& x, Generator & gen) const
  {
    auto id = m_dico.at(x);
    auto & dist = m_kernels.at(id);
    return m_points.at(dist(gen));
  }

};

// @brief Constructs a discrete location kernel
// @tparam M a distance matrix
// @tparam Kernel a dispersal kernel
// @return a DiscreteLocationSampler
//
template<typename Kernel, typename M>
auto make_discrete_location_sampler(M const& d, typename Kernel::param_type const& p)
{
  auto f = [p](auto r){return Kernel::pdf(r, p);};
  auto weights = d.apply(f);
  std::vector<std::discrete_distribution<unsigned int>> discrete_kernels;
  discrete_kernels.reserve(weights.points().size());
  for(auto const& x : weights.points())
  {
    auto x_weights = weights.at(x);
    discrete_kernels.push_back(std::discrete_distribution<unsigned int>(x_weights.begin(), x_weights.end()));
  }
  return DiscreteLocationSampler<typename M::coord_type>(weights.points(), std::move(discrete_kernels));
}


//nathan et al 2012, table 15.1
struct Gaussian
{
  class param_type{
    double _a;
  public:
    double a() const {return _a;}
    void a(double value) { _a = value ;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    assert(a > 0 && r >= 0);
    return 1/(M_PI*a*a) * std::exp(-(r*r)/(a*a)) ;
  }
};

struct Logistic
{
  class param_type{
    double _a;
    double _b;
  public:
    double a() const {return _a;}
    double b() const {return _b;}
    void a(double value) { _a = value;}
    void b(double value) { _b = value;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    double b = p.b();
    assert(a > 0 && b >2 && r >= 0);
    return (b/(2*M_PI*(a*a)*std::tgamma(2/b)*std::tgamma(1-2/b)))*(1/(1+(std::pow(r,b)/(std::pow(a,b)))));
  }

};

struct NegativeExponential
{
  class param_type{
    double _a;
  public:
    double a() const {return _a;}
    void a(double value) { _a = value;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    assert(a > 0 && r>= 0);
    return 1/(2*M_PI*a*a) * std::exp(-r/a) ;
  }

};

struct ExponentialPower
{
  class param_type{
    double _a;
    double _b;
  public:
    double a() const {return _a;}
    double b() const {return _b;}
    void a(double value) { _a = value;}
    void b(double value) { _b = value;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    double b = p.b();
    assert(a > 0 && b > 0 && r >= 0);
    return b/(2*M_PI*a*a*std::tgamma(2/b)) * std::exp(-std::pow(r,b)/std::pow(a,b));
  }

};

struct TwoDt
{
  class param_type{
    double _a;
    double _b;
  public:
    double a() const {return _a;}
    double b() const {return _b;}
    void a(double value) { _a = value;}
    void b(double value) { _b = value;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    double b = p.b();
    assert(a > 0 && b > 1 && r >= 0);
    return (b-1)/(M_PI*a*a) * std::pow( (1 + (r*r)/(a*a)) , -b);
  }
};

struct InversePowerLaw
{
  class param_type{
    double _a;
    double _b;
  public:
    double a() const {return _a;}
    double b() const {return _b;}
    void a(double value) { _a = value;}
    void b(double value) { _b = value;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    double b = p.b();
    assert(a > 0 && b > 2 && r >= 0);
    return (b-2)*(b-1)/(2*M_PI*a*a) * std::pow( (1 + r/a) , -b);
  }
};

struct UndefinedInversePowerLaw
{
  class param_type{
    double _a;
    double _b;
  public:
    double a() const {return _a;}
    double b() const {return _b;}
    void a(double value) { _a = value;}
    void b(double value) { _b = value;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    double b = p.b();
    assert(a > 0 && b > 2 && r >= 0);
    return std::pow( r/a , -b);
  }
};


struct Lognormal
{
  class param_type{
    double _a;
    double _b;
  public:
    double a() const {return _a;}
    double b() const {return _b;}
    void a(double value) { _a = value;}
    void b(double value) { _b = value;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    double b = p.b();
    assert(a > 0 && b > 0 && r >= 0);
    return (1/(std::pow(2*M_PI, 3/2)*b*r*r)) * std::exp(- (std::log(r/a)*std::log(r/a) / (2*b*b) ) );
  }
};

struct GaussianMixture
{
  class param_type{
    double _a1;
    double _a2;
    double _p;
  public:
    double a1() const {return _a1;}
    double a2() const {return _a2;}
    double p() const {return _p;}
    void a1(double value) { _a1 = value;}
    void a2(double value) { _a2 = value;}
    void p(double value) { _p = value;}
  };

  static double pdf(double r, param_type const& param)
  {
    double a1 = param.a1();
    double a2 = param.a2();
    double p = param.p();
    assert(a1 > 0 && a2 > 0 && p > 0 && p < 1 && r >= 0);
    return p/(M_PI*a1*a1) * std::exp(-(r*r)/(a1*a1)) + (1-p)/(M_PI*a2*a2) * std::exp(-(r*r)/(a2*a2)) ;
  }
};

}}}
