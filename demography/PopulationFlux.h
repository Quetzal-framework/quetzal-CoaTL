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
#include "assert.h"

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
template<typename Space, typename Time, typename T>
class PopulationFlux
{

public:

	//! \typedef time type
	using time_type = Time;

	//! \typedf space type
	using coord_type = Space;

	//! \typedef type of the population size variable
	using value_type = T;

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
	value_type flux_from_to(coord_type const& from, coord_type const& to, time_type t) const {
		assert(m_flux.count(t) == 1);
		assert(m_flux.at(t).count(to) == 1);
		assert(m_flux.at(t).at(to).count(from) == 1);
		return m_flux.at(t).at(to).at(from);
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
		return m_flux[t][to][from];
	}

	/**
	 * \brief Get population size at deme x at time t.
	 * \remark operator allows for more expressive mathematical style in client code
	 * \section Example
	 * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	 * \section Output
	 * \include demography/test/PopulationSize/PopulationSize_test.output
	 */
	value_type& operator()(coord_type const& from,coord_type const& to, time_type const& t){
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
		return flux_from_to(from,to,t);
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
	std::unordered_map<coord_type, value_type> const& flux_to(coord_type const& x, time_type t) const {
		assert(m_flux.count(t) == 1);
		assert(m_flux.at(t).count(x) == 1);
		return m_flux.at(t).at(x);
	}

	/**
	  * \brief Check if the distribution of the value of the flux converging to deme x at time t is defined
		* \return true if the distribution is defined, else returns false
		* \section Example
	  * \snippet demography/test/PopulationFlux/PopulationFlux_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationFlux/PopulationFlux_test.output
		*/
	bool flux_to_is_defined(coord_type const& to, time_type const& t) const {
		return (m_flux.count(t) == 1) && (m_flux.at(t).count(to) == 1);
	}

private:

	// period//to_deme/from_deme
	std::unordered_map<time_type,
		std::unordered_map<coord_type,
			std::unordered_map<coord_type, value_type> > > m_flux;
};

} // demography
} // quetzal

#endif
