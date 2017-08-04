// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/ 

#ifndef __GEODATA_H_INCLUDED__
#define __GEODATA_H_INCLUDED__

#include <gdal.h>
#include <gdal_priv.h> // GDALDataSet
#include <cpl_conv.h> // for CPLMalloc()
#include <string>
#include <vector>
#include <algorithm> // min_element
#include <iostream>

#include "CustomGDALDatasetDeleter.h"
#include "Coords.h"
#include "GeoLoader.h"

class GeoData{

	// IMPORTANT X is longitude from west to east, Y is Latitude from North to South
	// begin indices to 0
	using xy_type = std::pair<unsigned int, unsigned int>;

public: 

	using coord_type = geography::Coords;
	using value_type = double;
	using time_type = unsigned int;

	using exception_type = std::string;

	// Read data from path to gdal data set
	GeoData(std::string const& path) : resource(std::move(GeoLoader::read(path))){
		resetGeoTransform();
	}
	

	coord_type origin() const {

		double adfGeoTransform[6];

		if( resource->GetGeoTransform( adfGeoTransform ) == CE_None ){
			double lon_0 = adfGeoTransform[0] ;
			double lat_0 = adfGeoTransform[3] ;

			return coord_type(lat_0, lon_0) ;

		}else { throw std::string("error in GeoData::origin"); }
	}
	

	coord_type pixel_size() const {

		double adfGeoTransform[6];

		if( resource->GetGeoTransform( adfGeoTransform ) == CE_None ){
			double lon_cell_resolution = adfGeoTransform[1] ;
			double lat_cell_resolution = adfGeoTransform[5] ;

			return coord_type(lat_cell_resolution, lon_cell_resolution) ;

		}else {
			throw std::string("error in GeoData::pixel_size");
		}
	}

	// Return accessible times for access along temporal dimension
	std::vector<time_type> times() const{

		time_type tmax = resource->GetRasterCount();
		std::vector<time_type> times;

		for(time_type t = 1; t <= tmax; ++t)
			 times.push_back(t);

		return(times);
	}


	// Return accessible geographic lat/lon coordinates, 
	// discards no data coords)
	std::vector<coord_type> valid_coords(time_type const& t) const {
		try{
		std::vector<coord_type> coordinates;
		for(int x = 0; x < resource->GetRasterXSize(); ++x){

			for(int y = 0; y < resource->GetRasterYSize(); ++y){

				if(std::isfinite(this->data(x, y, t))){
					auto coords = xy_to_lat_lon(x,y);
					coordinates.push_back(coords);
				}
			}
		}
		return coordinates;
		}
		catch(...){
			throw std::string("in GeoData::validCoords");
		}
	}


	std::pair<coord_type, coord_type> get_spatial_extent() const {
		coord_type top_left_corner = origin();

		double lat = top_left_corner.get_lat() + (resource->GetRasterYSize()) * pixel_size().get_lat();
		double lon = top_left_corner.get_lon() + (resource->GetRasterXSize()) * pixel_size().get_lon();

		coord_type bootom_right_corner = coord_type(lat, lon);

		return std::make_pair(top_left_corner, bootom_right_corner);
	}


	bool is_in_spatial_extent(coord_type const& coord) const {
		bool is_in_spatial_extent = false;

		std::pair<coord_type, coord_type> extremes = get_spatial_extent();
		if( coord.get_lon() >= extremes.first.get_lon() && coord.get_lon() < extremes.second.get_lon() &&
			coord.get_lat() <= extremes.first.get_lat() &&  coord.get_lat() > extremes.second.get_lat()){
			is_in_spatial_extent = true;
		}

		return(is_in_spatial_extent);

	}


	coord_type reproject_to_centroid(coord_type const& coord) const {

		if( !is_in_spatial_extent(coord)){
			throw exception_type("in GeoData::reproject_to_centroid, coordinates to be reprojected is not comprised in GeoData object spatial extent");
		}

		xy_type xy = lat_lon_to_xy(coord);
		coord_type latlon = xy_to_lat_lon(xy.first, xy.second);
		return latlon;
	}

	// Access to one value at one place at one time
	// Reading value one by one is not efficient
	value_type operator()(coord_type const& latlon, time_type t) const {

		int bandID = int(t);
		assert(bandID != 0);
		
		auto xy = lat_lon_to_xy(latlon);
		return read(xy.first, xy.second, bandID) ;
	}


	value_type min() const {
		std::vector<value_type> minima ;

		for(auto it : times())
			minima.push_back( min(it) );

		return *(std::min_element(minima.begin(), minima.end())) ;
	}

	value_type max() const {
		std::vector<value_type> maxima ;

		for(auto it : times())
			maxima.push_back( max(it) );

		return *(std::min_element(maxima.begin(), maxima.end())) ;
	}


private:

	std::shared_ptr<GDALDataset> resource;
	std::vector<double> gT{std::vector<double>(6, 0.)};

	value_type min(GeoData::time_type t) const {
		int bandID = int(t);
		assert(bandID != 0);
		return get_min_max(t).first ;
	}

	value_type max(GeoData::time_type t) const {
		int bandID = int(t);
		assert(bandID != 0);
		return get_min_max(t).second ;
	}

	std::pair<value_type, value_type> get_min_max(GeoData::time_type t) const {
		int bandID = int(t);
		assert(bandID !=0);

		GDALRasterBand  *poBand;
		poBand = resource->GetRasterBand( bandID );

		int nBlockXSize;
		int nBlockYSize;
		poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );

		int boolean_GotMin;
		int boolean_GotMax;
		double adfMinMax[2];
		adfMinMax[0] = poBand->GetMinimum( &boolean_GotMin );
		adfMinMax[1] = poBand->GetMaximum( &boolean_GotMax );

		if( ! (boolean_GotMin && boolean_GotMax) ){ 
			GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
		}
		
		std::pair<value_type, value_type> min_max = std::make_pair(adfMinMax[0], adfMinMax[1]);

		return min_max ;
	}


	// Convert row and col to latitude/longitude
	coord_type xy_to_lat_lon(unsigned int x, unsigned int y) const {

		unsigned int col = x;
		unsigned int row = y;

   		auto lon = gT[1] * col + gT[2] * row + gT[1] * 0.5 + gT[2] * 0.5 + gT[0];
   		auto lat = gT[4] * col + gT[5] * row + gT[4] * 0.5 + gT[5] * 0.5 + gT[3];

		coord_type geo = coord_type(lat, lon);
		return(geo);
	}


	// Convert latitude/longitude to row and col
	xy_type lat_lon_to_xy(coord_type const& geo) const {
		
		int col = (geo.get_lon() - gT[0]) / gT[1];
		int row = (geo.get_lat() - gT[3]) / gT[5];

		return std::make_pair(col, row);
	}

	// Access to one value at one place at one time
	// Reading value one by one is not efficient
	value_type data(unsigned int x, unsigned int y, time_type t) const {

		int bandID = int(t);
		assert(bandID !=0);
		
		return read(x, y, bandID) ;

	}

	
	// Read 1 value using GDAL API. To change if access is too slow
	value_type read(unsigned int x, unsigned int y, int bandID) const {
		assert(x < (unsigned)(resource->GetRasterXSize()) );
		assert(y < (unsigned)(resource->GetRasterYSize()) );

		GDALRasterBand  *poBand = resource->GetRasterBand( bandID );
		assert(poBand && "in GeoData, reading dataset, unable to access band");

		int nXSize = 1;
		auto line = (float *) CPLMalloc(sizeof(float)*nXSize);
		poBand->RasterIO( GF_Read, x, y, 1, 1, line, nXSize, 1, GDT_Float32, 0, 0 );
		double val = double(*line);

		CPLFree(line);

		return(val);
	}




	// Reset geoTransform from resource
	void resetGeoTransform(){

		double geoTransform[6];

		if( resource->GetGeoTransform( geoTransform ) == CE_None ){

			for(unsigned int i = 0; i <= 5; ++i)
				gT[i] = geoTransform[i];

		}else{
			throw std::string("unable to reset geoTransform");
		}
	}

};

#endif

