// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __ENVIRONMENTAL_QUANTITY_H_INCLUDED
#define __ENVIRONMENTAL_QUANTITY_H_INCLUDED

#include "gdalcpp.h"
#include "GeographicCoordinates.h"

#include <algorithm>

namespace quetzal {
namespace geography {

// Functor of space and time, with discrete definition space
template<typename Time>
class EnvironmentalQuantity : public gdalcpp::Dataset {

public:
  //! \typedef coordinate type
  using coord_type = GeographicCoordinates;

  //! \typedef time type
  using time_type = Time;

  //! \typedef decimal degree type
  using decimal_degree = typename GeographicCoordinates::decimal_degree;

private:

  struct XY{
    XY(unsigned int col, unsigned int row) : x(col), y(row){}
    unsigned int x;
    unsigned int y;
    bool operator==(const XY& other) const {
        if(x == other.x && y == other.y) return true;
        return false;
      }
  };

  struct Resolution{
      Resolution(decimal_degree x_res, decimal_degree y_res) : x(x_res), y(y_res){}
      decimal_degree x;
      decimal_degree y;
  };

  struct Extent{
			Extent(decimal_degree xmin, decimal_degree xmax, decimal_degree ymin, decimal_degree ymax):
			x_min(xmin),
			x_max(xmax),
			y_min(ymin),
			y_max(ymax){}

			decimal_degree x_min;
			decimal_degree x_max;
			decimal_degree y_min;
			decimal_degree y_max;
	};

  std::vector<XY> m_valid_XY;
  std::vector<GeographicCoordinates> m_valid_LonLat;
  std::vector<time_type> m_times;
  std::vector<double> m_gT;
  Extent m_extent;

  std::vector<XY> get_domain(unsigned int bandID){
    assert( bandID < depth());
    std::vector<XY> v;
    for(unsigned int x = 0; x < width(); ++x){
      for( unsigned int y = 0; y < height(); ++y){
        if(std::isfinite(read(x, y, bandID))){
          v.emplace_back(x,y);
        }
      }
    }
  }

  bool has_same_domain_at_all_times(){
    bool answer = true;
    if(depth()>1){
      std::vector<unsigned int> bands(depth());
      std::iota(bands.begin(), bands.end(), 0);
      auto have_same_domain = [this](unsigned int i, unsigned int j){
        auto s_i = this->get_domain(i);
        auto s_j = this->get_domain(j);
        return std::equal(s_i.begin(), s_i.end(), s_j.begin());
      };
      answer = std::all_of(bands.begin(), bands.end(), have_same_domain);
    }
    return answer;
  }


  // Read 1 value using GDAL API. To change if access is too slow
  double read(unsigned int x, unsigned int y, unsigned int bandID) const {
    assert(x < width() );
    assert(y < height() );
    GDALRasterBand  *poBand = this->get().GetRasterBand( bandID );
    assert(poBand && "Unable to access band");
    int nXSize = 1;
    auto line = (float *) CPLMalloc(sizeof(float)*nXSize);
    poBand->RasterIO( GF_Read, x, y, 1, 1, line, nXSize, 1, GDT_Float32, 0, 0 );
    double val = double(*line);
    CPLFree(line);
    return(val);
  }

  // Top left corner and bottom right corner geographic coordinates
  Extent compute_extent() const {
    auto lon_min = this->origin().lon();
    auto lat_min = this->origin().lat();
    auto lon_max = lon_min + this->width() * this->resolution().x;
    auto lat_max = lat_min + this->height() * this->resolution().y;
    return Extent(lon_min, lon_max, lat_min, lat_max);
  }

  // Convert row x and col y to geographic coordinate
  GeographicCoordinates to_lat_lon(unsigned int x, unsigned int y) const {
    auto lon = m_gT[1] * x + m_gT[2] * y + m_gT[1] * 0.5 + m_gT[2] * 0.5 + m_gT[0];
    auto lat = m_gT[4] * x + m_gT[5] * y + m_gT[4] * 0.5 + m_gT[5] * 0.5 + m_gT[3];
    return GeographicCoordinates(lat, lon);
  }

  // Convert latitude/longitude to col/row indices
  XY to_xy(GeographicCoordinates const& c) const {
    int col = (c.lon() - m_gT[0]) / m_gT[1];
    int row = (c.lat() - m_gT[3]) / m_gT[5];
    return XY(col, row);
  }

  std::vector<GeographicCoordinates> to_lat_lon(std::vector<XY> const& xy) const {
    std::vector<GeographicCoordinates> out;
    for(auto const& it : xy){
      out.emplace_back(it.y, it.x);
    }
    return out;
  }

public:

  EnvironmentalQuantity(const std::string& dataset_name, std::vector<time_type> const& times):
  gdalcpp::Dataset(dataset_name),
  m_valid_XY(get_domain(0)),
  m_valid_LonLat(to_lat_lon(m_valid_XY)),
  m_times(times),
  m_gT(affine_transformation_coefficients()),
  m_extent(compute_extent())
  {
    std::sort(m_times.begin(), m_times.end());

    if(times.size() != depth()){
     throw std::runtime_error("Wrong number of times.");
    }

    if( !has_same_domain_at_all_times()){
      throw std::runtime_error("All times should have same definition space");
    }
  }

  double operator()(GeographicCoordinates const& c, Time const& t){
    auto xy = to_xy(c);
    ptrdiff_t pos = distance(m_times.begin(), find(m_times.begin(), m_times.end(), t));
    assert(pos >= m_times.size());
    return read(xy.x, xy.y, pos);
  }

  const std::vector<GeographicCoordinates>& geographic_definition_space() const {
    return m_valid_LonLat;
  }

  const std::vector<time_type>& temporal_definition_space() const {
    return m_times;
  }

  // Geographic coordinates of the top left corner.
  GeographicCoordinates origin() const {
    return GeographicCoordinates(m_gT[3], m_gT[0]) ;
  }

  // Pixel size in x and y
  Resolution resolution() const {
      return Resolution(m_gT[1], m_gT[5]);
  }

  // Check if a given geographic coordinate is in the landscape spatial extent
  bool is_in_spatial_extent(GeographicCoordinates const& c) const {
    bool is_in_spatial_extent = false;
    auto extent = this->extent();
    if( c.lon() >= extent.x_min && c.lon() < extent.x_max &&
        c.lat() <= extent.y_max && c.lat() > extent.y_min      )
    {
      is_in_spatial_extent = true;
    }
    return(is_in_spatial_extent);
  }

  // Coordinate of the centroid of the landscape cell to which the given coordinate belongs
  GeographicCoordinates reproject_to_centroid(GeographicCoordinates const& c) const {
    assert(is_in_spatial_extent(c));
    auto xy = to_xy(c);
    return to_lat_lon(xy.first, xy.second);
  }

  const Extent& extent() const {
    return m_extent;
  }

};

} // namespace quetzal
} // namespace geography

#endif
