
//               math::expressive library
//          Copyright Pierre Marques 2016 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BACKPORT_FUNCTIONAL_H
#define BACKPORT_FUNCTIONAL_H

#include <functional>

#include "backport_core.h"

#ifdef USE_CPP14STD_BACKPORT

namespace std {

//void overloads of std's operator functors

	struct __is_transparent;	// undefined

	template<>
	struct plus<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) + std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) + std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) + std::forward<_Up>(__u); }
		typedef __is_transparent is_transparent;
	};

		/// One of the @link arithmetic_functors math functors@endlink.
	template <>
	struct minus<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) - std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) - std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) - std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

		/// One of the @link arithmetic_functors math functors@endlink.
	template<>
	struct multiplies<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) * std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) * std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) * std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link arithmetic_functors math functors@endlink.
	template<>
	struct divides<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) / std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) / std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) / std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link arithmetic_functors math functors@endlink.
	template<>
	struct modulus<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) % std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) % std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) % std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link arithmetic_functors math functors@endlink.
	template<>
	struct negate<void> {
		template <typename _Tp>
		constexpr auto operator()(_Tp&& __t) const
		noexcept(noexcept(-std::forward<_Tp>(__t)))
		-> decltype(-std::forward<_Tp>(__t))
		{ return -std::forward<_Tp>(__t); }

		typedef __is_transparent is_transparent;
	};


	/// One of the @link comparison_functors comparison functors@endlink.
	template<>
	struct equal_to<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) == std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) == std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) == std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link comparison_functors comparison functors@endlink.
	template<>
	struct not_equal_to<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) != std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) != std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) != std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link comparison_functors comparison functors@endlink.
	template<>
	struct greater<void>
	{
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) > std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) > std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) > std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link comparison_functors comparison functors@endlink.
	template<>
	struct less<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) < std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) < std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) < std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link comparison_functors comparison functors@endlink.
	template<>
	struct greater_equal<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) >= std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) >= std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) >= std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link comparison_functors comparison functors@endlink.
	template<>
	struct less_equal<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) <= std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) <= std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) <= std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};




	/// One of the @link logical_functors Boolean operations functors@endlink.
	template<>
	struct logical_and<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) && std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) && std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) && std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link logical_functors Boolean operations functors@endlink.
	template<>
	struct logical_or<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) || std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) || std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) || std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	/// One of the @link logical_functors Boolean operations functors@endlink.
	template<>
	struct logical_not<void> {
		template <typename _Tp>
		constexpr auto operator()(_Tp&& __t) const
		noexcept(noexcept(!std::forward<_Tp>(__t)))
		-> decltype(!std::forward<_Tp>(__t))
		{ return !std::forward<_Tp>(__t); }

		typedef __is_transparent is_transparent;
	};


	template <>
	struct bit_and<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) & std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) & std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) & std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	template <>
	struct bit_or<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) | std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) | std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) | std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	template <>
	struct bit_xor<void> {
		template <typename _Tp, typename _Up>
		constexpr auto operator()(_Tp&& __t, _Up&& __u) const
		noexcept(noexcept(std::forward<_Tp>(__t) ^ std::forward<_Up>(__u)))
		-> decltype(std::forward<_Tp>(__t) ^ std::forward<_Up>(__u))
		{ return std::forward<_Tp>(__t) ^ std::forward<_Up>(__u); }

		typedef __is_transparent is_transparent;
	};

	template<typename _Tp = void>
    struct bit_not : public unary_function<_Tp, _Tp> {
    	constexpr _Tp operator()(const _Tp& __x) const { return ~__x; }
	};

	template <>
	struct bit_not<void> {
		template <typename _Tp>
		constexpr auto operator()(_Tp&& __t) const
		noexcept(noexcept(~std::forward<_Tp>(__t)))
		-> decltype(~std::forward<_Tp>(__t))
		{ return ~std::forward<_Tp>(__t); }

		typedef __is_transparent is_transparent;
	};

}//std::

#endif

#endif

