#include "quetzal/quetzal.hpp"
#include <filesystem>
#include "assert.h"

using namespace quetzal;

int main()
{
	int start = 2001;
	int step = 1;
	auto file = std::filesystem::current_path() / "test/data/bio1.tif";

	auto bio1 = geography::variable::from_file(file, start, step);
	std::cout << bio1 << std::endl;

	// There are 10 bands/layers/temporal coordinates
	assert( bio1.times().size() == 10 );

	// There are 9 cells/spatial coordinates
	assert( bio1.locations().size() == 9 );

	// You will typically have georeferenced sampling points
	auto point_1 = bio1.latlon(52., -5.);
	auto point_2 = bio1.lonlat(-5., 52.);

	assert(point_1 == point_2);

	// Defines a lambda expression for printing
	auto assess = [&bio1](auto x){
		std::cout << "Point " << x << " is " 
				<< ( bio1.contains(x) ? "" : "not")
				<< "in bio1 extent" << std::endl;
	};

	assess(point_1);
	auto point_3 = bio1.lonlat(-99., 99);
	assess(point_3);

	// Location descriptors can be converted
	geography::variable::location_descriptor x = bio1.origin();
	std::cout << "All equivalent:\t" 
			  << x << "\t"
			  << bio1.to_rowcol(x) << "\t"
			  << bio1.to_colrow(x) << "\t"
	          << bio1.to_lonlat(x) << "\t"
			  << bio1.to_latlon(x) << std::endl;

	// Distance between two points on Earth with Haversine formula
	std::cout << "Point 1 distance to origin: " << point_1.great_circle_distance_to(x);

	// The class is callable to retrieve the variable value.
	std::optional<double> o = bio1(x,t);

	// It may be a NA, let's check for it:
	std::cout << "bio1( " << x << "," << t << ") is "
			  << ( o.has_value() ? "" : "not")
			  << "defined." << std::endl;

	// Use bio1 to define e.g. the dispersal friction coefficient
	// See mathematical composition with expressive
	auto friction = [&f = std::as_const(bio1)](auto x, auto t){ return f(x,t).value_or(1.);};
	std::cout << friction(point_1) << std::endl;
	assert( friction(point_3) == 1.);

	// The spatial grid can be used as a demic graph structure for demographic simulation
	auto graph = bio1.to_graph();

	// A lambda expression to transform edges properties by an arbitrary kernel, e.g. :
	auto f = [&graph = std::as_const(graph)](auto edge){ graph[edge].distance() > 100 ? 0. : 1.; };

	auto range = std::ranges::transform( graph.edges(), f );

	graph.vertices()
}
