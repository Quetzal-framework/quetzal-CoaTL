// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/ 

#ifndef __FLUX_H_INCLUDED__
#define __FLUX_H_INCLUDED__

#include <unordered_map>
#include "assert.h"

namespace demography {


/*!
 * \brief Defines a template class to store the effective migrants number across a landscape along time.
 *
 * \tparam Space    Demes identifiers.
 * \tparam Time     Decrementable (backward in time), incrementable (forward in time), EqualityComparable, CopyConstructible.
 * \tparam T        Type of the flux size (unsigned int, double...)
 *
 */
template<typename Space, typename Time, typename T>
class Flux
{

public:

	using time_type = Time;
	using coord_type = Space;
	using N_type = T;

	N_type flux_from_to(coord_type const& from, coord_type const& to, time_type t) const {
		assert(m_flux.count(t) == 1);
		assert(m_flux.at(t).count(to) == 1);
		assert(m_flux.at(t).at(to).count(from) == 1);
		return m_flux.at(t).at(to).at(from);
	}

	N_type& flux_from_to(coord_type const& from, coord_type const& to, time_type t){
		return m_flux[t][to][from];
	}

	// \brief Returns the distribution of migrants number converging to x at time t.
	std::unordered_map<coord_type, N_type> const& flux_to(coord_type const& x, time_type t) const {
		assert(m_flux.count(t) == 1);
		assert(m_flux.at(t).count(x) == 1);
		return m_flux.at(t).at(x);
	}

	std::unordered_map<coord_type, N_type> const& operator()(coord_type const& to, time_type t) const {
		assert(m_flux.count(t) == 1);
		assert(m_flux.at(t).count(to) == 1);
		return flux_to(to, t);
	}

	bool flux_to_is_defined(coord_type const& to, time_type const& t) const {
		return (m_flux.count(t) == 1) && (m_flux.at(t).count(to) == 1);
	}

private:

	// period//to_deme/from_deme
	std::unordered_map<time_type, 
		std::unordered_map<coord_type, 
			std::unordered_map<coord_type, N_type> > > m_flux;
};

} // demography

#endif