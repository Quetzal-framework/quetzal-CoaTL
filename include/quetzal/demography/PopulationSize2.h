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

#include <vector>
#include "assert.h"

namespace quetzal
{
	namespace demography
	{
		/*!
		* \brief Population size distribution in time and space.
		* \tparam Space    Demes identifier, usually GeographicCoordinates
		* \ingroup demography
		*/
		template<typename Space>
		class PopulationSizeVectorImplementation
		{
		private:
			unsigned int m_nb_demes;
			unsigned int m_nb_generations;
			std::vector<value_type> m_populations;
			/**
			* \brief Get the index of the point
			* \param x the point
			* \param g the generation number
			*/
			size_t get_index(const point_type& x, const time_type& g ) const
			{
				return m_nb_demes*g + x.getId()
			}

		public:
			//! \typedef geographic coordinates type
			using coord_type = Space;
			//! \typedef indexable wrapper around coordinates
			using point_type = quetzal::utils::PointWithId<Space>;
			//! \typedef time type
			using time_type = unsigned int;
			//! \typedef type of the population size variable
			using value_type = double;
			/*!
			\brief Distribution of population size in space and time
			\param n number of demes in the landscape
			\param g number of generations simulation is supposed to run
			\remark pre-allocates a std::vector<double> of n*g elements
			*/
			PopulationSize_2(unsigned int nb_demes, unsigned_int nb_generations):
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
		};
	} // namespace demography
} // namespace quetzal

	#endif
