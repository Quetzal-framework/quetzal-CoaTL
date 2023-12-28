#include "quetzal/quetzal.hpp"
#include <filesystem>
#include <cassert>

using namespace quetzal;

	//  Expected dataSet structure
	//
	//  * origin at Lon -5, Lat 52.
	//  * 9 cells
	//  * 10 layers
	//  * pixel size (-5, 5)
	//  * East and South limits ARE NOT in spatial extent
	//
	//
	// (origin)	    -5        0       5      10
	// 		   \   /         /  	 /		/
	// 			\ / 		/		/	   /
	// 		52	 * ------ * ----- * ---- *
	// 			 |    .   |   .   |   .
	// 			 |   c0   |  c1   |  c2
	// 		47	 * ------ * ----- * ---- *
	// 			 |    .   |   .   |   .
	// 			 |   c3   |  c4   |  c5
	// 		42	 * ------ * ----- * ---- *
	// 			 |   .    |   .   |   .
	// 			 |   c6   |  c7   |  c8
	// 		37	 *        *       *      *
	// 
	//
	//          90
	//          |         (+)
	// 	    0 --------------> 180    X size positive in decimal degree (east direction positive)
	//          |                    Y size negative in decimal degree (south direction negative)
	//          |
	// 			|
	// 			|
	// 	   (-)  v        
	// 		    0                     

int main()
{
	using time_type = int;
	using raster_type = geography::raster<time_type>;

	// The raster has 10 bands that we will assign to 2001 ... 2011.
	std::vector<time_type> times(10);
    std::iota(times.begin(), times.end(), 2001);

	auto file = std::filesystem::current_path() / "data/bio1.tif";

	// Read the raster
	auto bio1 = raster_type::from_file(file, times);

	std::cout << bio1 << std::endl;

	// Check there are 10 bands/layers/time periods
	assert( std::ranges::distance(bio1.times()) == 10 );

	// There are 9 cells/spatial coordinates
	assert( std::ranges::distance(bio1.locations()) == 9 );

	// You will typically have georeferenced sampling points
	using latlon =  typename raster_type::latlon;
	using lonlat =  typename raster_type::lonlat;

	auto point_1 = latlon(52., -5.);
	auto point_2 = lonlat(-5., 52.);

	assert(point_1 == point_2);

	// Defines a lambda expression for checking extent
	auto check = [&bio1](auto x){
		std::cout << "Point " << x << " is " 
				<< ( bio1.contains(x) ? " " : "not ")
				<< "in bio1 extent" << std::endl;
	};

	check(point_1);
	auto point_3 = lonlat(-99., 99);
	check(point_3);

	// Computing distance will come handy for spatial graphs
	std::cout << point_1 << " is " 
			  << point_1.great_circle_distance_to(point_3)
			  << " km away from " << point_3 << std::endl;

	// Coordinates
	auto x = bio1.to_descriptor(bio1.origin());
	auto t = bio1.times().front();

	// 1D location descriptors can be converted to 2D coordinate systems
	std::cout << "All equivalent:\n\t" 
			  << x << "\n\t"
			  << bio1.to_rowcol(x) << "\n\t"
			  << bio1.to_colrow(x) << "\n\t"
			  << bio1.to_latlon(x) << "\n\t"
	          << bio1.to_lonlat(x) << std::endl;

	// Retrieve the raster value, that may be defined.
	std::optional<double> maybe_bio1 = bio1.at(x,t);

	// It may be a NA, let's check for it:
	std::cout << "bio1(" << x << "," << t << ") is "
			  << ( maybe_bio1.has_value() ? "" : "not")
			  << "defined." << std::endl;

	// If value is not defined (empty optional), let's use the raster NA value.
	std::cout << maybe_bio1.value_or(bio1.NA()) <<  std::endl;

}
