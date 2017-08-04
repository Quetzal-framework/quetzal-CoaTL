
//               math::expressive library
//          Copyright Pierre Marques 2016 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BACKPORT_UTILITY_H
#define BACKPORT_UTILITY_H

#include <utility>

#include "backport_core.h"

#ifdef USE_CPP14STD_BACKPORT

namespace std {
	namespace backport_details {
		// Stores a tuple of indices.	Used by tuple and pair, and by bind() to
		// extract the elements in a tuple.
		template <size_t... _Indexes>
		struct _Index_tuple { };

		// Concatenates two _Index_tuples.
		template <typename _Itup1, typename _Itup2>
		struct _Itup_cat;

		template <size_t... _Ind1, size_t... _Ind2>
		struct _Itup_cat<_Index_tuple<_Ind1...>, _Index_tuple<_Ind2...>> {
			using __type = _Index_tuple<_Ind1..., (_Ind2 + sizeof...(_Ind1))...>;
		};

		// Builds an _Index_tuple<0, 1, 2, ..., _Num-1>.
		template <size_t _Num>
		struct _Build_index_tuple
			: _Itup_cat<
				typename _Build_index_tuple<_Num / 2>::__type,
				typename _Build_index_tuple<_Num - _Num / 2>::__type
			>
		{};

		template <>
		struct _Build_index_tuple<1> { typedef _Index_tuple<0> __type;};

		template <>
		struct _Build_index_tuple<0> {typedef _Index_tuple<> __type;};
	}//std::backport_details::

	
	/// Class template integer_sequence
	template <typename _Tp, _Tp... _Idx>
	struct integer_sequence {
		typedef _Tp value_type;
		static constexpr size_t size() { return sizeof...(_Idx); }
	};

	template <typename _Tp, _Tp _Num, typename _ISeq = typename std::backport_details::_Build_index_tuple<_Num>::__type>
	struct _Make_integer_sequence;

	template <typename _Tp, _Tp _Num,	size_t... _Idx>
	struct _Make_integer_sequence<_Tp, _Num, std::backport_details::_Index_tuple<_Idx...>> {
		static_assert( _Num >= 0, "Cannot make integer sequence of negative length" );

		typedef integer_sequence<_Tp, static_cast<_Tp>(_Idx)...> __type;
	};

	/// Alias template make_integer_sequence
	template <typename _Tp, _Tp _Num>
	using make_integer_sequence = typename _Make_integer_sequence<_Tp, _Num>::__type;

	/// Alias template index_sequence
	template <size_t... _Idx>
	using index_sequence = integer_sequence<size_t, _Idx...>;

	/// Alias template make_index_sequence
	template <size_t _Num>
	using make_index_sequence = make_integer_sequence<size_t, _Num>;

	/// Alias template index_sequence_for
	template <typename... _Types>
	using index_sequence_for = make_index_sequence<sizeof...(_Types)>;

}//std::

#endif

#endif

