#include "quetzal/quetzal.hpp"
#include <filesystem>
#include "assert.h"

using namespace quetzal;

int main()
{
	int start = 2001;
	int end = 2011
	int step = 1;
	auto file = std::filesystem::current_path() / "test/data/bio1.tif";

	auto bio1 = geography::raster::from_file(file, start, end);

	using latlon =  decltype(bio1)::latlon;

	std::cout << bio1 << std::endl;

	// There are 10 bands/layers/time periods
	assert( bio1.times().size() == 10 );

	// There are 9 cells/spatial coordinates
	assert( bio1.locations().size() == 9 );

	// You will typically have georeferenced sampling points
	auto point_1 = latlon(52., -5.);
	auto point_2 = lonlat(-5., 52.);
	assert(point_1 == point_2)

	// Defines a lambda expression for checking extent
	auto check = [&bio1](auto x){
		std::cout << "Point " << x << " is " 
				<< ( bio1.contains(x) ? "" : "not")
				<< "in bio1 extent" << std::endl;
	};

	check(point_1);
	auto point_3 = bio1.lonlat(-99., 99);
	check(point_3);

	// 1D location descriptors can be converted to 2D coordinate systems
	auto x = bio1.origin();
	std::cout << "All equivalent:\t" 
			  << x << "\t"
			  << bio1.to_rowcol(x) << "\t"
			  << bio1.to_colrow(x) << "\t"
	          << bio1.to_lonlat(x) << "\t"
			  << bio1.to_latlon(x) << std::endl;

	// Retrieve the raster value.
	std::optional<double> maybe_bio1 = bio1.at(x,t);

	// It may be a NA, let's check for it:
	std::cout << "bio1( " << x << "," << t << ") is "
			  << ( maybe_bio1.has_value() ? "" : "not")
			  << "defined." << std::endl;

	// If value is not defined (empty optional), let's use the raster NA value.
	std::cout << maybe_bio1.value_or(bio1.NA()) <<  std::endl;

}
