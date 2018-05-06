// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __POPULATION_FLUX_H_INCLUDED__
#define __POPULATION_FLUX_H_INCLUDED__

#include <unordered_map>
#include <cassert>
#include <functional>
#include <algorithm>

namespace quetzal{
namespace demography {


/*!
 * \brief Defines a template class to store the effective migrants number across a landscape along time.
 *
 * \tparam Space    Demes identifiers.
 * \tparam Time     EqualityComparable, CopyConstructible.
 * \tparam Value    The type of the population size variable (int, unsigned int, double...)
 * \ingroup demography
 * \section Example
 * \snippet demography/test/PopulationFlux/PopulationFlux_test.cpp Example
 * \section Output
 * \include demography/test/PopulationFlux/PopulationFlux_test.output
 */
template<typename Space, typename Time, typename Value>
class PopulationFlux
{

public:

	//! \typedef time type
	using time_type = Time;

	//! \typedf space type
	using coord_type = Space;

	//! \typedef type of the population size variable
	using value_type = Value;

	/**
	  * \brief Default constructor
		* \section Example
	  * \snippet demography/test/PopulationFlux/PopulationFlux_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationFlux/PopulationFlux_test.output
		*/
	PopulationFlux() = default;

	/**
	  * \brief Retrieves value of the flux from deme i to deme j at time t.
		* \section Example
	  * \snippet demography/test/PopulationFlux/PopulationFlux_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationFlux/PopulationFlux_test.output
		*/
	value_type flux_from_to(coord_type const& from, coord_type const& to, time_type t) const
	{
		auto key = std::make_tuple(t, from, to);
		assert(m_flows.count(key) == 1);
		return(m_flows.at(key));
	}

	/**
	  * \brief Retrieves value of the flux from deme i to deme j at time t.
		* \return a reference on the value, initialized with value_type default constructor
		* \section Example
	  * \snippet demography/test/PopulationFlux/PopulationFlux_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationFlux/PopulationFlux_test.output
		*/
	value_type& flux_from_to(coord_type const& from, coord_type const& to, time_type t){
		//return m_flux[t][to][from];
		return m_flows[key_type(t, from, to)];
	}

	/**
	 * \brief Get population size at deme x at time t.
	 * \remark operator allows for more expressive mathematical style in client code
	 * \section Example
	 * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	 * \section Output
	 * \include demography/test/PopulationSize/PopulationSize_test.output
	 */
	value_type& operator()(coord_type const& from,coord_type const& to, time_type const& t)
	{
		return flux_from_to(from,to,t);
	}

	/**
	 * \brief Get population size at deme x at time t.
	 * \remark operator allows for more expressive mathematical style in client code
	 * \section Example
	 * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	 * \section Output
	 * \include demography/test/PopulationSize/PopulationSize_test.output
	 */
	value_type operator()(coord_type const& from,coord_type const& to, time_type const& t) const {
		return flux_from_to(from, to, t);
	}

	/**
	  * \brief Retrieves the distribution of the value of the flux converging to deme x at time t.
		* \return a const reference on an unordered_map with giving the origin deme and the value
		*         giving the value of the flux
		* \section Example
	  * \snippet demography/test/PopulationFlux/PopulationFlux_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationFlux/PopulationFlux_test.output
		*/
	std::unordered_map<coord_type, value_type> flux_to(coord_type const& x, time_type t) const
	{
		assert(flux_to_is_defined(x,t));
		std::unordered_map<coord_type, value_type> result;
		for(auto const& it : m_flows){
			if(it.first.time == t && it.first.to == x)
			{
				result[it.first.from] = it.second;
			}
		}
		return result;
	}

	/**
	  * \brief Check if the distribution of the value of the flux converging to deme x at time t is defined
		* \return true if the distribution is defined, else returns false
		* \section Example
	  * \snippet demography/test/PopulationFlux/PopulationFlux_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationFlux/PopulationFlux_test.output
		*/
	bool flux_to_is_defined(coord_type const& to, time_type const& t) const
	{
		auto predicate = [t, to](auto const& it){
			return (it.first.time == t && it.first.to == to);
		};
		auto it = std::find_if(m_flows.begin(), m_flows.end(), predicate);
		return it != m_flows.end();
	}

	auto begin() const
	{
		return m_flows.cbegin();
	}

	auto end() const
	{
		return m_flows.cend();
	}

private:

	struct key_type
	{
		time_type time;
		coord_type from;
		coord_type to;

		key_type(time_type const& t, coord_type const& origin, coord_type const& destination):
		time(t),
		from(origin),
		to(destination)
		{}

		bool operator==(key_type const& other) const
		{
			return (
							 other.time == this->time &&
							 other.from == this->from &&
							 other.to == this->to
						 );
		}
	};

	struct key_hash : public std::unary_function<key_type, std::size_t>
	{
	 std::size_t operator()(const key_type& k) const
	 {
		 size_t res = 17;
		 res = res * 31 + std::hash<time_type>()( k.time );
		 res = res * 31 + std::hash<coord_type>()( k.from );
		 res = res * 31 + std::hash<coord_type>()( k.to );
		 return res;
	 }
	};

	using map_type = std::unordered_map<const key_type, value_type, key_hash>;
	map_type m_flows;

};

} // demography
} // quetzal

#endif
