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

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "assert.h"
#include <iostream>
 #include <fstream>

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
			// Data in this windows are in RAM and allow for read and write access
			mutable std::pair<Time, Time> m_RAM_window = {0,1};
			// only stores 2 time keys at the time, other are serialized
			mutable std::unordered_map<Time, std::unordered_map<Space, Value> > m_populations;

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
				slide_RAM_window_to(t);
				m_populations[t][x] = N;
			}
			/**
			* \brief Population size at deme x at time t.
			* \return a reference to the value, initialized with value_type default constructor
			* \remark operator allows for more expressive mathematical style in client code
			*/
			value_type& operator()(coord_type const& x, time_type const& t)
			{
				slide_RAM_window_to(t);
				return m_populations[t][x];
			}
			/**
			* \brief Get population size value at deme x at time t
			*/
			value_type get(coord_type const& x, time_type const& t) const
			{
				slide_RAM_window_to(t);
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
			* \brief Return the demes at which the population size was defined (>0) at time t.
			*/
			std::vector<coord_type> definition_space(time_type const& t) const
			{
				slide_RAM_window_to(t);
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

		private:

			std::string get_archive_name(time_type t) const
			{
				const std::string prefix = "N-";
				const std::string extension = ".archive";
				return prefix + std::to_string(t) + extension;
			}

			void serialize_layer(time_type t) const
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

			void deserialize_layer(time_type t) const
			{
				std::string filename = get_archive_name(t);
				 // create and open an archive for input
				 std::ifstream ifs(filename, std::ios::binary);
				 boost::archive::binary_iarchive ia(ifs);
				 // read class state from archive
				 std::unordered_map<Space, Value> layer;
				 ia >> layer;
				 // archive and stream closed when destructors are called
				 m_populations[t] = layer;
			}

			void slide_RAM_window_to(time_type t) const
			{
				// window is well positioned
				if(t == m_RAM_window.first || t == m_RAM_window.second )
				{
					// do nothing, just read the data
					return;
				}
				// windows has to go forward
				else if( t == m_RAM_window.second + 1)
				{
					// we know for sure we can serialize this
					serialize_layer( m_RAM_window.first );
					// slide the window
					m_RAM_window.first += 1;
					m_RAM_window.second += 1;
					// we don't know if we are in forward history simulation or forward reading
					try { deserialize_layer( m_RAM_window.second);
					} catch (const std::exception& e) {
						std::cout << "layer did not exist on disk, I assume it's forward simulation time" << std::endl;
					}
				}
				// windows has to go backward
				else if( t == m_RAM_window.first - 1)
				{
					// we know for sure we can serialize this
					serialize_layer( m_RAM_window.second );
					// we know for sure we should be able to deserialize this
					deserialize_layer( m_RAM_window.first - 1 );
					// slide back the window
					m_RAM_window.first -= 1;
					m_RAM_window.second -= 1;
				}
				// current windows is totally not aligned: assume it's (for now) only random reading access
				else
				{
					// erase both layers from RAM
					serialize_layer( m_RAM_window.first );
					serialize_layer( m_RAM_window.second );
					// slide the windows
          if(t == 0)
					{
						m_RAM_window.first = 0;
						m_RAM_window.second = 1;
					}else{
						m_RAM_window.first = t - 1;
						m_RAM_window.second = t;
					}
					// read both layers from disk
					deserialize_layer( m_RAM_window.first );
					deserialize_layer( m_RAM_window.second );
				}
			}
		}; // PopulationSizeOnDiskImplementation
	} // namespace demography
} // namespace quetzal

#endif
