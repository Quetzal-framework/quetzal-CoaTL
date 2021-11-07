// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __POPULATIONS_ON_DISK_H_INCLUDED__
#define __POPULATIONS_ON_DISK_H_INCLUDED__

#include <map>
#include <sstream>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "assert.h"

namespace quetzal
{
	namespace demography
	{
		/*!
		* \brief Population size distribution in time and space, serialized in a sliding windows to limit RAM usage.
		*
		* \tparam Space    Demes identifiers.
		* \tparam Time     EqualityComparable, CopyConstructible.
		* \tparam Value    The type of the population size variable (int, unsigned int, double...)
		* \ingroup demography
		*/
		template<typename Space, typename Time, typename Value>
		class PopulationSizeOnDiskImplementation
		{
		private:

			Time m_last_flagged_time = 0;
			// only stores 2 time keys at the time, other are serialized
			std::unordered_map<Time, std::unordered_map<Space, Value> > m_populations;

			std::string get_archive_name(time_type t) const
			{
				const std::string prefix = "N_";
				const std::string extension = ".archive";
				return prefix + to_string(t) + extension;
			}

			void serialize_layer(time_type t)
			{
				const std::string filename = get_archive_name(t);
				// create and open a binary archive for output
				std::ofstream ofs(filename);
				// save data to archive
				boost::archive::binary_oarchive oa(ofs);
				// write class instance to archive
				oa << m_populations.at(t);
				// archive and stream closed when destructors are called, clear map
				m_populations.erase(t);
			}

			void deserialize_layer(time_type t)
			{
				std::string filename = get_archive_name(t);
				 // create and open an archive for input
				 std::ifstream ifs(filename);
				 boost::archive::binary_iarchive ia(ifs);
				 // read class state from archive
				 ia >> m_populations[t];
				 // archive and stream closed when destructors are called
			}

			void maybe_slide_window(time_type t)
			{
				// when the context just jumped forward in time:
				if( t > m_last_flagged_time)
				{
					// N(x, t) is being computed based in N(x, m_last_flagged_time) & m_last_flagged_time = t - 1
					if( t >= 2)
					{
						// you can serialize N(x, t - 2) because don't need it anymore
						serialize_layer( t - 2 );
					}
					// Remember that the context jumped
					m_last_flagged_time = t;
				}
				// when the context just jumped backward in time
				else if( t < m_last_flagged_time)
				{
					// N(x, t) is being computed based in N(x, current_time) & m_last_flagged_time = t - 1
					// You have to deserialize N(t) because you gonna need it
					deserialize_layer(t);
					// You can serialize the "previous" layer because you don't need it anymore
					serialize_layer( m_last_flagged_time ); // should always exists: ranges from 0 to t_sampling
					// Remember that the context jumped
					m_last_flagged_time = t;
				}
			}

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
			PopulationSizeOnDiskImplementation() = default;
			/**
			* \brief Set population size value at deme x at time t
			*/
			void set(coord_type const& x, time_type const& t, value_type N)
			{
				assert(N >= 0);
				maybe_slide_window(t);
				m_populations[t][x] = N;
			}
			/**
			* \brief Get population size value at deme x at time t
			*/
			value_type get(coord_type const& x, time_type const& t) const
			{
				maybe_slide_window(t);
				assert( is_defined(x,t) );
				return m_populations.at(t).at(x);
			}
			/**
			* \brief Get population size at deme x at time t.
			* \remark operator allows for more expressive mathematical style in client code
			*/
			value_type operator()(coord_type const& x, time_type const& t) const
			{
				maybe_slide_window(t);
				return get(x,t);
			}
			/**
			* \brief Population size at deme x at time t.
			* \return a reference to the value, initialized with value_type default constructor
			* \remark operator allows for more expressive mathematical style in client code
			*/
			value_type& operator()(coord_type const& x, time_type const& t)
			{
				maybe_slide_window(t);
				return m_populations[t][x];
			}
			/**
			* \brief Return the demes at which the population size was defined (>0) at time t.
			*/
			std::vector<coord_type> definition_space(time_type const& t) const
			{
				maybe_slide_window(t);
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
				maybe_slide_window(t);
				return 	   (!m_populations.empty())
				&& (m_populations.find(t) != m_populations.end())
				&& (m_populations.at(t).find(x) != m_populations.at(t).end());
			}
			}
		};
	} // namespace demography
} // namespace quetzal

#endif
