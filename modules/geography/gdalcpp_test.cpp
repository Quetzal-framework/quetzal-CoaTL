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

#include "gdalcpp.h"

int main(){
	std::string file_name("/home/becheler/dev/quetzal/modules/geography/test_data/wc2.0_10m_tavg/wc2.0_10m_tavg_01.tif");
	gdalcpp::Dataset geo(file_name);
/*
	GDALDataset  *poDataset;
	GDALAllRegister();
	poDataset = static_cast<GDALDataset*>(GDALOpen( file_name.c_str(), GA_ReadOnly ));
	if( poDataset == NULL )
	{
			std::cout << "oops" << std::endl;
	}
	*/
	std::cout << geo.driver_name() << std::endl;

}
