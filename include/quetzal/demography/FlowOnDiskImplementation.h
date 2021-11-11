// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __POPULATION_FLOW_ON_DISK_H_INCLUDED__
#define __POPULATION_FLOW_ON_DISK_H_INCLUDED__

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/utility.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <unordered_map>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iostream>     // output operator

namespace quetzal
{
	namespace demography
	{
		/*!
		 * \brief Defines a template class to store the demographic flows number across a landscape along time.
		 *
		 * \tparam Space    Demes identifiers.
		 * \tparam Time     EqualityComparable, CopyConstructible.
		 * \tparam Value    The type of the population size variable (int, unsigned int, double...)
		 * \ingroup demography
		 */
		template<typename Space, typename Time, typename Value>
		class FlowOnDiskImplementation
		{
		public:
			//! \typedef time type
			using time_type = Time;
			//! \typedef space type
			using coord_type = Space;
			//! \typedef type of the population size variable
			using value_type = Value;
			//! The type of key for nested map member
			struct key_type
			{
				//! origin of the flow
				coord_type from;
				//! destination of the flow
				coord_type to;
				//! Default constructor needed to default construct inner nested map
				key_type() = default;
				//! Constructor
				key_type(coord_type const& origin, coord_type const& destination):
				from(origin),
				to(destination)
				{}
				//! EqualityComparable
				bool operator==(key_type const& other) const
				{
					return (
									 other.from == this->from &&
									 other.to == this->to
								 );
				}
				//! Serializable by boost
				friend class boost::serialization::access;
				template<class Archive>
				void serialize(Archive & ar, const unsigned int version)
				{
						ar & from;
						ar & to;
				}
			};
			/**
				* \brief Makes key_type hashable in unordered_map
				*/
			struct key_hash : public std::unary_function<key_type, std::size_t>
			{
			 std::size_t operator()(const key_type& k) const
			 {
				 size_t res = 17;
				 res = res * 31 + std::hash<coord_type>()( k.from );
				 res = res * 31 + std::hash<coord_type>()( k.to );
				 return res;
			 }
			};
		private:
			mutable std::pair<Time, Time> m_RAM_window = {0,1};
			//! \typedef type of the historical migration flows database
			using forward_flow_type = std::unordered_map<time_type, std::unordered_map<key_type, value_type, key_hash>>;
			//! \typedef type of the historical reverse flows database
			using backward_flow_type =  std::unordered_map<time_type, std::unordered_map<coord_type, std::unordered_map<coord_type, value_type>>>;
			//! historical migration flows database
			mutable forward_flow_type m_forward_flow;
			//! historical reverse flows database
			mutable backward_flow_type m_backward_flow;
		public:
			/**
			  * \brief Default constructor
				*/
			FlowOnDiskImplementation() = default;
			/**
			  * \brief Retrieves value of the flux from deme i to deme j at time t.
				*/
			value_type flux_from_to(coord_type const& from, coord_type const& to, time_type t) const
			{
				slide_RAM_window_to(t);
				assert(m_forward_flow.find(t) != m_forward_flow.end());
				assert(m_forward_flow.at(t).find(key_type(from, to)) != m_forward_flow.at(t).end());
				return m_forward_flow.at(t).at(key_type(from,to));
			}
			/**
			  * \brief Retrieves value of the flux from deme i to deme j at time t.
				* \return a reference on the value, initialized with value_type default constructor
				*/
			void set_flux_from_to(coord_type const& from, coord_type const& to, time_type t, value_type v)
			{
				slide_RAM_window_to(t);
				m_forward_flow[t][key_type(from, to)] = v;
				m_backward_flow[t][to][from] = v;
			}
			/**
			  * \brief Adds value v to the flux from deme i to deme j at time t.
				*/
			void add_to_flux_from_to(coord_type const& from, coord_type const& to, time_type t, value_type v)
			{
				slide_RAM_window_to(t);
				m_forward_flow[t][key_type(from, to)] += v;
				m_backward_flow[t][to][from] += v;
			}
			/**
			  * \brief Retrieves the distribution of the value of the flux converging to deme x at time t.
				* \return a const reference on an unordered_map with the key giving the origin deme and the value
				*         giving the value of the flow
				*/
			std::unordered_map<coord_type, value_type> const & flux_to(coord_type const& x, time_type t) const
			{
				slide_RAM_window_to(t);
				assert(flux_to_is_defined(x,t));
				return m_backward_flow.at(t).at(x);
			}
			/**
			  * \brief Check if the distribution of the value of the flux converging to deme x at time t is defined
				* \return true if the distribution is defined, else returns false
				*/
			bool flux_to_is_defined(coord_type const& to, time_type const& t) const
			{
				slide_RAM_window_to(t);
				auto it1 = m_backward_flow.find(t);
				if(it1 != m_backward_flow.end())
				{
					auto it2 = m_backward_flow.at(t).find(to);
					return it2 != m_backward_flow.at(t).end();
				}
				else
				{
					return false;
				}
			}
		private:
			/**
			  * \brief Returns a filename to store forward flow
				* \return true if the distribution is defined, else returns false
				*/
			std::string get_forward_flow_archive_name(time_type t) const
			{
				const std::string prefix = "M-forward";
				const std::string extension = ".archive";
				return prefix + std::to_string(t) + extension;
			}
			/**
			  * \brief Returns a filename to store backward flow
				* \return true if the distribution is defined, else returns false
				*/
			std::string get_backward_flow_archive_name(time_type t) const
			{
				const std::string prefix = "M-backward";
				const std::string extension = ".archive";
				return prefix + std::to_string(t) + extension;
			}
			/**
				* \brief Serializes the forward flow and backward flow function at time t
				*/
			void serialize_layer(time_type t) const
			{
				const std::string forward_filename = get_forward_flow_archive_name(t);
				const std::string backward_filename = get_backward_flow_archive_name(t);
				// create and open a binary archive for output
				std::ofstream forward_ofs(forward_filename, std::ios::binary);
				std::ofstream backward_ofs(backward_filename, std::ios::binary);
				// save data to archive
				boost::archive::binary_oarchive forward_oa(forward_ofs);
				boost::archive::binary_oarchive backward_oa(backward_ofs);
				// write class instance to archive
				forward_oa << m_forward_flow.at(t);
				backward_oa << m_backward_flow.at(t);
				// archive and stream closed when destructors are called, clear map
				m_forward_flow.erase(t);
				m_backward_flow.erase(t);
			}

			void deserialize_layer(time_type t) const
			{
				const std::string forward_filename = get_forward_flow_archive_name(t);
				const std::string backward_filename = get_backward_flow_archive_name(t);
				// create and open an archive for input
				std::ifstream forward_ifs(forward_filename, std::ios::binary);
				std::ifstream backward_ifs(backward_filename, std::ios::binary);
				boost::archive::binary_iarchive forward_ia(forward_ifs);
				boost::archive::binary_iarchive backward_ia(backward_ifs);
				// read class state from archive
				std::unordered_map<key_type, value_type, key_hash> forward_layer;
				std::unordered_map<coord_type, std::unordered_map<coord_type, value_type>> backward_layer;
				forward_ia >> forward_layer;
				backward_ia >> backward_layer;
				// archive and stream closed when destructors are called
				m_forward_flow.emplace(t,forward_layer);
				m_backward_flow.emplace(t, backward_layer);
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
					m_RAM_window.first = t - 1;
					m_RAM_window.second = t;
					// read both layers from disk
					deserialize_layer( m_RAM_window.first );
					deserialize_layer( m_RAM_window.second );
				}
			}
		};
	} // demography
} // quetzal
#endif
