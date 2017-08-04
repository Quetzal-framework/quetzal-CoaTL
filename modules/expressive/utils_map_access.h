
//               math::expressive library
//          Copyright Pierre Marques 2016 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef UTILS_MAP_ACCESS_H
#define UTILS_MAP_ACCESS_H

//utilitaire d'accès simplifié à une map
namespace utils {
namespace map_access {
	
template <typename M> 
typename M::mapped_type const* operator/(M const& map, typename M::key_type const& key) {
   typename M::const_iterator it = map.find(key);
   return (it != map.end()) ? &it->second : nullptr;
}

template <typename M> 
typename M::mapped_type* operator/(M& map, typename M::key_type const& key) {
   typename M::iterator it = map.find(key);
   return (it != map.end()) ? &it->second : nullptr;
}

}//utils::map_access::
}//utils::

using namespace utils::map_access;

#endif
