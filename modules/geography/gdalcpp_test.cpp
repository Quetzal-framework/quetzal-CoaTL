// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o gdalcpp_test gdalcpp_test.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include <assert.h>
#include <iostream>
#include <string>
#include <algorithm>

#include "gdalcpp.h"

int main(){
	std::string file_name("/home/becheler/dev/quetzal/modules/geography/test_data/wc2.0_10m_tavg/wc2.0_10m_tavg_01.tif");
	gdalcpp::Dataset data(file_name);
	std::cout << "Name:\t" << data.dataset_name() << std::endl;
	std::cout << "Driver:\t " << data.driver_name() << std::endl;
	std::cout << "Width:\t" << data.width() << std::endl;
	std::cout << "Height:\t" << data.height() << std::endl;
	std::cout << "Depth:\t" << data.depth() << std::endl;
	std::cout << "Coefs:\t";
	for(auto const& it : data.affine_transformation_coefficients()){
			std::cout << it << " ";
	}
	std::cout << std::endl;
}
