// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __OCCUPANCY_SPECTRUM_ALGORITHM_H_INCLUDED__
#define __OCCUPANCY_SPECTRUM_ALGORITHM_H_INCLUDED__

#include <vector>
#include <utility>

namespace quetzal{

namespace coalescence{

namespace occupancy_spectrum{

struct Generator{
  using occupancy_spectrum_type = std::vector<unsigned int>;
  // n balls, m urns, Policy handles occupancy_spectrum_type
  template<class Callback>
  static void generate(unsigned int n, unsigned int m, Callback& callback){
    occupancy_spectrum_type M_j(n+1);
    f(n, m, n, M_j, callback);
  }

private:

  // j_max = nombre de balles dans l'urne la plus remplie.
  template<class Callback>
  static void f(unsigned int n, unsigned int m, unsigned int j_max, occupancy_spectrum_type& v, Callback& callback){

    if(m == 0 && n == 0){
      // probleme resolu
      callback(std::move(v));
      return;
    }

    if (m != 0){

      if (n == 0) {
        auto copy = v;
        copy[0] = m; // ici n = 0 et m = 0, donc probleme resolu
        callback(std::move(copy));
        return;

      } else {
        if( j_max > 0){
          // vertical descent
          for(unsigned int i = n/j_max; i >= 1; --i){

            auto copy = v;
            copy[j_max] = i;

            if(m >= i){
              unsigned int balls_left = n - i*j_max;
              if(balls_left < j_max){
                f( balls_left, m-i, balls_left, copy, callback);
              }else {
                f( balls_left, m-i, j_max-1, copy, callback);
              }
            }
          }
        }
      }
    }

    // horizontal descent
    if( j_max == 0 && n > 0 ){
      // unsolvable
      return;
    }

    auto copy = v;
    copy[j_max] = 0;
    f(n, m, j_max-1, copy, callback);

  }

};

} // namespace occupancy_spectrum
} // namespace coalescence
} // namespace quetzal

#endif
