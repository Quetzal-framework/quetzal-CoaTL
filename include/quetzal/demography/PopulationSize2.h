// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __POPULATIONS_2_H_INCLUDED__
#define __POPULATIONS_2_H_INCLUDED__

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "assert.h"

namespace quetzal{
namespace demography {

/*!
 * \brief Population size distribution in time and space.
 *
 * \tparam Space    Demes identifiers.
 * \ingroup demography
 */
template<typename Space>
class PopulationSize
{

public:

	//! \typedef time type
	using time_type = unsigned int;

	//! \typedef space type
	using coord_type = Space;

	//! \typedef type of the population size variable
	using value_type = double;

	/**
	  * \brief Constructor
		*/
	PopulationSize(unsigned int nb_demes, unsigned_int nb_generations):
	m_nb_demes(nb_demes),
	m_nb_generations(nb_generations),
	m_populations(nb_demes*nb_generations, value_type())
	{}

	/**
	  * \brief Get population size value at deme x at time t
		*/
	value_type get(coord_type const& x, time_type const& t) const
	{
		return m_populations.at(get_index(x,t));
	}

	/**
	 * \brief Get population size at deme x at time t.
	 * \remark operator allows for more expressive mathematical style in client code
	 */
	value_type operator()(coord_type const& x, time_type const& t) const
	{
		return get(x,t);
	}

	/**
	 * \brief Population size at deme x at time t.
	 * \return a reference to the value, initialized with value_type default constructor
	 * \remark operator allows for more expressive mathematical style in client code
	 */
	value_type & operator()(coord_type const& x, time_type const& t)
	{
		return m_populations.at(get_index(x,t));
	}

	/**
	  * \brief Set population size value at deme x at time t
		*/
	void set(coord_type const& x, time_type const& t, value_type N)
	{
		m_populations.at(get_index(x,t))=N;
	}

private:
	unsigned int m_nb_demes;
	unsigned int m_nb_generations;
	const Context &  m_context;
	std::vector<value_type> m_populations;

	/**
	 * \brief Translates latitude/longitude to a vector index
	 * \return a reference to the value, initialized with value_type default constructor
	 * \remark operator allows for more expressive mathematical style in client code
	 */
	size_t get_index(const coord_type& x, const time_type& t )
	{
		return (m_nb_demes*t-1) m_context.order_relation(x)
	}

};

} // namespace demography
} // namespace quetzal

#endif
