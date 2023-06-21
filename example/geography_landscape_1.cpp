#include "quetzal/quetzal.hpp"
#include <filesystem>
#include "assert.h"

using namespace quetzal;

int main()
{
	// What type you want to use to identify the variable: could also be e.g. an integer
	using key_type = std::string;

	// What type you want to use to identify the variable: could also be e.g. a time period
	using time_type = int;

	// Let's define a shorter alias
	using landscape_type = quetzal::geography::landscape<key_type, time_type>;

	auto file1 = std::filesystem::current_path() / "data/bio1.tif";
	auto file2 = std::filesystem::current_path() / "data/bio12.tif";

	// Initialize the landscape: for each var a key and a file, for all a time series.
	auto env = landscape_type::from_files( { {"bio1", file1}, {"bio12", file2} }, {2001, 2002, 2003,2004,2005,2006,2007,2008,2009,2010} );

	// We indeed recorded 2 variables: bio1 and bio12
	assert(env.size() == 2);

	// The semantic shares strong similarities with a raster
	landscape_type::latlon Bordeaux(44.5, 0.34);

	assert(env.contains(Bordeaux));
	assert(env.contains( env.to_centroid(Bordeaux) ) );
	assert(env.times().size() == 10);
	assert(env.locations().size() == 9);

	// These little function-objects will soon be very handy to embed the GIS variables into the simulation with quetzal::expressive
	const auto& f = env["bio1"].to_view();
	const auto& g = env["bio12"].to_view();

	// But for now just print their raw data if defined or any other value if undefined
	for (auto t : env.times())
	{
		for (auto x : env.locations())
		{
			f(x,t).value_or(0.0);
			g(x,t).value_or(env["bio12"].NA());
		}
	}
}
