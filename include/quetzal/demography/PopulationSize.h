// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __POPULATIONS_H_INCLUDED__
#define __POPULATIONS_H_INCLUDED__

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "assert.h"

namespace quetzal
{
	namespace demography
	{
		/*!
		* \brief Population size distribution in time and space.
		*
		* \tparam Space    Demes identifiers.
		* \tparam Time     EqualityComparable, CopyConstructible.
		* \tparam Value    The type of the population size variable (int, unsigned int, double...)
		* \ingroup demography
		*/
		template<typename Space, typename Time, typename Value>
		class PopulationSize
		{
			std::unordered_map<Time, std::unordered_map<Space, Value> > m_populations;
		public:
			//! \typedef time type
			using time_type = Time;
			//! \typedef space type
			using coord_type = Space;
			//! \typedef type of the population size variable
			using value_type = Value;
			/**
			* \brief Default constructor
			*/
			PopulationSize() = default;
			/**
			* \brief Set population size value at deme x at time t
			*/
			void set(coord_type const& x, time_type const& t, value_type N)
			{
				assert(N >= 0);
				m_populations[t][x] = N;
			}
			/**
			* \brief Get population size value at deme x at time t
			*/
			value_type get(coord_type const& x, time_type const& t) const
			{
				assert( is_defined(x,t) );
				return m_populations.at(t).at(x);
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
			value_type& operator()(coord_type const& x, time_type const& t)
			{
				return m_populations[t][x];
			}
			/**
			* \brief Return the demes at which the population size was defined (>0) at time t.
			*/
			std::vector<coord_type> definition_space(time_type const& t) const
			{
				std::vector<coord_type> v;
				for(auto const& it : m_populations.at(t) )
				{
					if(it.second > 0 ){v.push_back(it.first);}
				}
				return v;
			}
			/**
			* \brief Checks if population size is defined at deme x at time t
			* \return True  if variable is defined, else false.
			*/
			bool is_defined(coord_type const& x, time_type const& t) const
			{
				return 	   (!m_populations.empty())
				&& (m_populations.find(t) != m_populations.end())
				&& (m_populations.at(t).find(x) != m_populations.at(t).end());
			}
		};
	} // namespace demography
} // namespace quetzal

#endif
