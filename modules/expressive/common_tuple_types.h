
//               math::expressive library
//          Copyright Pierre Marques 2016 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef COMMON_TUPLE_TYPES_H
#define COMMON_TUPLE_TYPES_H

#include <tuple>
#include <type_traits>

namespace std {

//tuple join on common_type
template <typename...T, typename...U>
struct common_type<std::tuple<T...>, std::tuple<U...>> {
	using type = std::tuple<typename std::common_type<T, U>::type...>;
};

}//std::

#endif
