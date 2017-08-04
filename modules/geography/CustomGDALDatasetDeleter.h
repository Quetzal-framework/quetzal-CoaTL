// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/ 

#ifndef __CUSTOMGDALDATASETDELETER_H_INCLUDED__
#define __CUSTOMGDALDATASETDELETER_H_INCLUDED__

#include <gdal.h>

class CustomGDALDatasetDeleter
{
public:
    void operator()(GDALDatasetH res) const {
        ::GDALClose(res);
    };
};

#endif
