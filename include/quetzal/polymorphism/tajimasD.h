// Copyright 2022 Arnaud Becheler    <arnaud.becheler@gmail.com>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __TAJIMAS_D_H_INCLUDED__
#define __TAJIMAS_D_H_INCLUDED__

#include <algorithm> // std::generate
#include <cmath> // sqrt

///
/// @brief Generate term a1 and a2 as of Tajima 1989 paper
///
/// @return a pair p where p.first is the term a1 and p.second the term a2
///
template<typename T=double>
class tajimasD
{
public:
  using value_type = T;
private:
  value_type _pi;
  value_type _S;
  value_type _n;
  std::pair<value_type, value_type> _a;
  value_type _b1;
  value_type _b2;
  value_type _c1;
  value_type _c2;
  value_type _e1;
  value_type _e2;
  value_type _D;
  ///
  /// @brief Generate term a1 and a2 as of Tajima 1989 paper
  ///
  /// @return a pair p where p.first is the term a1 and p.second the term a2
  ///
  constexpr std::pair<value_type, value_type> compute_a(double n)
  {
    std::make_pair(0.0, 0.0);
    std::generate(1, n, [i = 1, this] () mutable { _a.first += 1/i; _a.second += 1/(i*i); return i++;});
  }
  ///
  /// @brief Generate term b1 as of Tajima 1989 paper
  ///
  /// @param n the number of sequences in the sample
  ///
  constexpr auto compute_b1(double n){ return (n + 1.0) / (3.0 * ( n - 1.0)); }
  ///
  /// @brief Generate term b2 as in Tajima 1989 paper
  ///
  /// @param n the number of sequences in the sample
  ///
  constexpr auto compute_b2(double n){ return (2.0 * (n*n + n + 3.0)) / (9.0 * n * (n - 1.0)); }
  ///
  /// @brief Generate term c1 as in Tajima 1989 paper
  ///
  /// @param n the number of sequences in the sample
  ///
  constexpr auto compute_c1(value_type a1, value_type b1){ return b1 - 1.0 / a1;}
  ///
  /// @brief Generate term c2 as in Tajima 1989 paper
  ///
  /// @param n the number of sequences in the sample
  ///
  constexpr auto compute_c2(double n, value_type a1, value_type a2, value_type b2)
  {
    return b2 - (n + 2.0)/(a1*n) + a2/(a1*a1) ;
  }
  ///
  /// @brief Generate term e1 as in Tajima 1989 paper
  ///
  constexpr auto compute_e1(value_type c1, value_type a1){ return c1 / a1; }
  ///
  /// @brief Generate term e2 as in Tajima 1989 paper
  ///
  constexpr auto compute_e2(value_type a1, value_type a2, value_type c2){ return c2 / (a1*a1 + a2); }
  ///
  /// @brief Compute Tajima's D as in Tajima 1989 paper
  ///
  /// @param pi mean pairwise difference
  /// @param S number of segregating sites
  constexpr auto compute_D(value_type pi, value_type S, value_type a1, value_type e1, value_type e2)
  {
    return (pi - S/a1) / std::pow((e1 * S + e2 * S * (S - 1.0)), 0.5);
  }
  ///
  /// @brief Constructor
  ///
  /// @param pi mean pairwise difference
  /// @param S number of segregating sites
  /// @param n number of sequences in the sample
  ///
  tajimasD(double pi, double S, double n):
  _pi(pi),
  _S(S),
  _n(n),
  _a(compute_a(n)),
  _b1(compute_b1(n)),
  _b2(compute_b2(n)),
  _c1(compute_c1(_a.first, _a.second)),
  _c2(compute_c2(n, _a.first, _a.second, _b2)),
  _e1(compute_e1(_c1, _a.first)),
  _e2(compute_e2(_a.first, _a.second, _c2)),
  _D(compute_D()){}

}; // class TajimasD

#endif
