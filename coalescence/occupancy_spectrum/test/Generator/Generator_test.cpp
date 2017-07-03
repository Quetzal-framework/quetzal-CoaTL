// Copyright 2017 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/


#include "../../Generator.h"

//! [Example]

#include <iostream>
#include <algorithm> // std::copy
#include <iterator>  // std::ostream_iterator

int main(){
  using quetzal::coalescence::occupancy_spectrum::Generator;
  using spectrum_type = Generator::occupancy_spectrum_type;

  auto spectrum_handler = [](spectrum_type && M_j){
    std::copy(M_j.begin(), M_j.end(), std::ostream_iterator<spectrum_type::value_type>(std::cout, " "));
    std::cout << "\n";
  };

  Generator::generate(5, 10, spectrum_handler);

  return 0;
}

//! [Example]
