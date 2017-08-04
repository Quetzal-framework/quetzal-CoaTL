// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/ 

#ifndef __GEOLOADER_H_INCLUDED__
#define __GEOLOADER_H_INCLUDED__

#include <memory> //std::unique_ptr
#include <iostream> // std::cout

#include <gdal.h>
#include <gdal_priv.h> // GDALDataSet
#include <cpl_conv.h> // for CPLMalloc()

#include "CustomGDALDatasetDeleter.h"

class GeoLoader
{
public:
    static std::unique_ptr<GDALDataset, CustomGDALDatasetDeleter> read(const std::string& path){
        GDALAllRegister();
    	const char * cPath = path.c_str();
   		std::unique_ptr<GDALDataset, CustomGDALDatasetDeleter> poDataset((GDALDataset *) GDALOpen( cPath, GA_ReadOnly ));
   		return poDataset;
    }
};

#endif
