#include "quetzal/quetzal.hpp"
#include <filesystem>
#include "assert.h"

using namespace quetzal;

int main()
{
	using time_type = int;
	using raster_type = geography::raster<time_type>;

	std::vector<time_type> times(10);
    std::iota(times.begin(), times.end(), 2001);
	auto file = std::filesystem::current_path() / "test/data/bio1.tif";

	auto bio1 = raster_type::from_file(file, times);

	std::cout << bio1 << std::endl;

	// There are 10 bands/layers/time periods
	assert( bio1.times().size() == 10 );

	// There are 9 cells/spatial coordinates
	assert( bio1.locations().size() == 9 );

	// You will typically have georeferenced sampling points
	using latlon =  typename raster_type::latlon;
	using lonlat =  typename raster_type::lonlat;

	auto point_1 = latlon(52., -5.);
	auto point_2 = lonlat(-5., 52.);

	assert(point_1 == point_2);

	// Defines a lambda expression for checking extent
	auto check = [&bio1](auto x){
		std::cout << "Point " << x << " is " 
				<< ( bio1.contains(x) ? "" : "not")
				<< "in bio1 extent" << std::endl;
	};

	check(point_1);
	auto point_3 = lonlat(-99., 99);
	check(point_3);

	// Coordinates
	auto x = bio1.to_descriptor(bio1.origin());
	auto t = bio1.times().front();

	// 1D location descriptors can be converted to 2D coordinate systems
	std::cout << "All equivalent:\t" 
			  << x << "\t"
			  << bio1.to_rowcol(x) << "\t"
			  << bio1.to_colrow(x) << "\t"
	          << bio1.to_lonlat(x) << std::endl;

	// Retrieve the raster value.
	std::optional<double> maybe_bio1 = bio1.at(x,t);

	// It may be a NA, let's check for it:
	std::cout << "bio1( " << x << "," << t << ") is "
			  << ( maybe_bio1.has_value() ? "" : "not")
			  << "defined." << std::endl;

	// If value is not defined (empty optional), let's use the raster NA value.
	std::cout << maybe_bio1.value_or(bio1.NA()) <<  std::endl;

}
