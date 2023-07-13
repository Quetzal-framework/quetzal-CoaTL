#include "quetzal/quetzal.hpp"
#include <filesystem>
#include <cassert>
#include <iostream>

using namespace quetzal;
            

int main()
{
	using time_type = int;
	using raster_type = geography::raster<time_type>;
	using location_descriptor = typename raster_type::location_descriptor;

	// The raster has 10 bands that we will assign to 2001 ... 2011.
	std::vector<time_type> times(10);
    std::iota(times.begin(), times.end(), 2001);

	auto input_file = std::filesystem::current_path() / "data/bio1.tif";

	// Read the raster
	auto bio1 = raster_type::from_file(input_file, times);

	// Define the quantity we want to print (it could be any function of space and time)
	auto transform = [](location_descriptor x, time_type t){ return std::optional( 2*x + t); };

	// Output file name
	auto output_file = std::filesystem::current_path() / "my_quantity.tif";

	// Write only between these times
	bio1.to_geotiff(transform, 2001, 2005, output_file);

	std::cout << (std::filesystem::exists("my_quantity.tif") ? "success" : "error") << std::endl;

	// Clean up
    std::filesystem::remove_all(output_file);

}
