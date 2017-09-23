// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/


#include "../../DiscreteLandscape.h"

//! [Example]

#include <iostream>   // std::cout
#include <string>

int main()
{

	using time_type = unsigned int;
	using landscape_type = quetzal::geography::DiscreteLandscape<std::string, time_type>;
	landscape_type env( {{"bio1","../test_data/bio1.tif"},{"bio12","../test_data/bio12.tif"}},
	                    {2001,2002,2003,2004,2005,2006,2007,2008,2009,2010} );

	std::cout << env.quantities_nbr() << " quantities read from files" << std::endl;

  landscape_type::coord_type Bordeaux(44.5, 0.34);
  if(env.is_in_spatial_extent(Bordeaux)) {
    std::cout << "Centroid: " << env.reproject_to_centroid(Bordeaux) << std::endl;
  }

  auto f = env["bio1"];
  auto g = env["bio12"];

  auto times = env.temporal_definition_space();
  auto space = env.geographic_definition_space();

  for(auto const& t : times){
    for(auto const& x : space){
      if( f(x,t) <= 200. && g(x,t) <= 600.){
        std::cout << x << "\t" << t << "\t" << f(x,t) << "\t" << g(x,t) << std::endl;
      }
    }
  }

	return 0;
}

//! [Example]
