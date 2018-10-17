// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __SIMULATORS_UTILS_H_INCLUDED__
#define __SIMULATORS_UTILS_H_INCLUDED__

#include "../demography/History.h"
#include "../coalescence/containers/Forest.h"
#include "../coalescence/policies/merger.h"

#include <map>

namespace quetzal {

  template<typename Env, typename F>
  struct neighbor_migration
  {

  	using coord_type = typename Env::coord_type;
    using env_type = Env;
    using F_type = F;

    env_type const& _landscape;
    F_type _f;

    neighbor_migration(env_type const& env, F f):
    _landscape(env),
    _f(f)
    {}

    template<typename T>
  	std::vector<coord_type> arrival_space(coord_type const& x, const T&) const
  	{
      auto v = _landscape.four_nearest_defined_cells(x);
      v.push_back(x);
      return v;
  	}

    template<typename T>
  	double operator()(coord_type const& x, coord_type const& y, T const& t) const
  	{
  		auto v = arrival_space(x,t);
      auto binop = [this, t](auto const& a, auto const& b){return a + _f(b,t);};
      double sum = std::accumulate(v.begin(), v.end(), 0.0, binop);
      return _f(y,t) / sum;
  	}

  };

template<typename Env, typename F>
auto make_neighbor_migration(Env const& env, F f )
{
  return neighbor_migration<Env, F>(env, f);
}

class cell_JC69
{
private:
	int m_t = 0;
	char m_allelic_state;

public:
	cell_JC69(int t): m_t(t) {}
	cell_JC69() = default;
	int time() const {return m_t;}
	char allelic_state() const {return m_allelic_state;}
	auto& allelic_state(char a)
	{
		m_allelic_state = a;
		return *this;
	}
};



/*
 * Set of mutation kernel policies to update the mutational states along a tree.
 */
namespace mutation_kernel{
  /* @brief Mute allelic states in the Arlequin standard data format.
  */
  class standard_data
  {
  public:
    // @typedef typde of the mutational state
    using state_type = bool;
    class cell_type
    {
    private:
    	int m_time = 0;
    	state_type m_allelic_state=false;

    public:
    	cell_type(int t): m_time(t) {}
    	cell_type() = default;
    	int time() const {return m_time;}
    	state_type allelic_state() const {return m_allelic_state;}
    	auto& allelic_state(state_type a)
    	{
    		m_allelic_state = a;
    		return *this;
    	}
    };
    /* @brief Sample a new mutational state
    * @param state the initial mutational state
    * @param nb_mutations the number of mutational event to simulate
    * @return the updated mutational state
    */
    template<typename UnusedRNG>
    static bool mute(state_type state, unsigned int nb_mutations, UnusedRNG&)
    {
      if ( nb_mutations % 2 != 0)
      {
        state = !state;
      }
      return state;
    }
  };


 /* @brief JC69 mutational kernel
  */
  class JC69
  {
    static const std::map<char, std::string> dico;
  public:
    // @typedef allelic state type
    using state_type = char;
    /*
     * @brief Sample a new mutational state
     * @param state the initial mutational state
     * @param nb_mutations the number of mutational events to simulate
     * @param gen a random number generator
     * @tparam Generator a random number generator
     * @return
     */
    template<typename Generator>
    static state_type mute(state_type state, unsigned int nb_mutations, Generator & gen)
    {
      assert( dico.find(state) != dico.end() );
      for(unsigned int i = 0; i < nb_mutations; ++i)
      {
        std::uniform_int_distribution dist(0,2);
        state = dico.at(state)[dist(gen)];
      }
      return state;
    }
  };

} // namespace mutation_kernel



//! Generic sampling schemes simulation
namespace sampling_scheme {

  //! Base class for sampling schemes parameter classes
  class param_base_class {
  private:
    unsigned int _n;
  public:
    /* Constructor
     * @param n sampling size
     */
    param_base_class(unsigned int n): _n(n) {}

    param_base_class() = default;

    /* Read the sampling size parameter
    */
    unsigned int sampling_size() const
    {
      return _n;
    }

    /* Set the sampling size parameter
    */
    void sampling_size(unsigned int n)
    {
      this->_n = n;
    }
  };


//! Policy to sample coordinates uniformely at random in a space
class uniform_at_random {
public:

  /* @brief parameter of the sampling scheme
  */
  class param_type : public param_base_class {
    using param_base_class::param_base_class;
  };

private:
  param_type _param;

public:
  uniform_at_random() = default;

  uniform_at_random(unsigned int n) : _param(n) {}

  param_type param() const { return _param; }

  void param(unsigned int n){ _param = param_type(n); }

    /* @brief Sample points in a discrete space, uniformely at random
   * @remark a same coordinates can be sampled several times
   *
   * @tparam Cont a container of coordinates
   * @tparam Generator a RandomNumberGenerator
   *
   * @param space coordinates to be sampled
   * @param n the number of points to sample
   * @param gen a random number generator
   *
   * @return a 'std::map<Cont::value_type, unsigned int>' giving the sampling intensities
   * at the sampled coordinates.
   */
	template<typename Cont, typename Generator>
	auto operator()(Cont const& space, param_type const& p, Generator& gen)
	{
    assert(p.sampling_size() > 0);

		using coord_type = typename Cont::value_type;
		auto l = space.size();

		std::uniform_int_distribution<unsigned int> dist(0, l-1);
		std::map<coord_type, unsigned int> sample;

		for(unsigned int i = 0; i < p.sampling_size() ; ++i)
		{
			auto it = space.begin();
			std::advance(it, dist(gen));
			sample[*it] += 1;
		}
		return sample;
	}

  template<typename Cont, typename Generator>
  auto operator()(Cont const& space, Generator& gen)
  {
    return this->operator()(space, this->param(), gen);
  }
};

/* Policy to design a sampling scheme where sampling points form clusters
 * @tparam Grid he spatial grid type to use to reproject sampling points.
 */
template<typename Grid>
class clustered_sampling {
public:

  using coord_type = typename Grid::coord_type;

  /* @brief parameter of the sampling scheme
  */
  class param_type : public param_base_class {
  private:
    unsigned int _nb_clusters;
    double _var;
    const Grid & _spatial_grid;

  public:

    /* Constructor
    * @param n sampling size
    * @param nb_clusters the number of clusters in which disperse sampling points
    * @param var the gaussian variance to disperse sampling points around clusters
    * @param spatial_grid the spatial grid to use to reproject sampling points.
     */
    param_type(unsigned int n, unsigned int nb_clusters, double var, Grid const& spatial_grid):
    param_base_class::param_base_class(n),
    _nb_clusters(nb_clusters),
    _var(var),
    _spatial_grid(spatial_grid) {}

    //! Read the number of clusters
    unsigned int nb_clusters() const
    {
      return _nb_clusters;
    }

    //! Set the number of clusters
    void nb_clusters(unsigned int nb_cluster)
    {
      this->_nb_clusters = nb_clusters;
    }

    //! Read the variance of the gaussian dispersion around clusters
    double var() const
    {
      return _var;
    }

    //! Set the variance of the gaussian dispersion around clusters
    void var(double v)
    {
      this->_var = v;
    }

  }; // end of param_type inner class

	template<typename Cont, typename Generator>
	static auto sample (Cont const& space, param_type const& p, Generator& gen)
	{

    assert(p.nb_clusters() <= p.sampling_size());
		auto l = space.size();

		std::vector<coord_type> seeds;
		std::uniform_int_distribution<unsigned int> dist(0, l-1);
		for(unsigned int i = 0; i < p.nb_clusters(); ++i)
		{
			auto it = space.begin();
			std::advance(it, dist(gen));
			seeds.push_back(*it);
		}

		std::normal_distribution gaussian(0.0, p.var());
		std::uniform_int_distribution<unsigned int> sample_a_seed(0, p.nb_clusters() - 1);
		std::map<coord_type, unsigned int> sample;
		unsigned int count = 0;
		while(count < p.sampling_size())
		{
			auto seed_id = sample_a_seed(gen);
			auto x = seeds.at(seed_id);
			x.lat() += gaussian(gen);
			x.lon() += gaussian(gen);
			if( p.spatial_grid().is_in_spatial_extent(x))
      {
				x = p.spatial_grid().reproject_to_centroid(x);
				sample[x] += 1;
				count += 1;
			}
		}
		return sample;
	}

};

} // namespace sampling_scheme

} // namespace quetzal

const std::map<char, std::string> quetzal::mutation_kernel::JC69::dico = { {'A', "TGC"}, {'T', "ACG"}, {'G', "ATC"}, {'C', "ATG"} };

#endif
