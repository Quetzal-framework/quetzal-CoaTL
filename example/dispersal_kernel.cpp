#include "quetzal/quetzal.hpp"
#include <iostream>
#include <mp-units/ostream.h>

using namespace quetzal;

int main()
{
	// Shorten notation
	using namespace mp_units::si::unit_symbols; // SI system: km, m, s 
	using quetzal::demography::dispersal_kernel::gaussian;
	using quetzal::geography::lonlat;

	// Dispersal kernels operate on distances between geographic coordinates
    constexpr auto source = lonlat(2.25, 48.9176);
    constexpr auto target = lonlat(2.25, 48.95);
	constexpr auto distance = source.great_circle_distance_to(target) * km;

	// Used to parametrize the kernel
	constexpr auto param = gaussian<>::param_type {.a=1000.*m};

	// Compute quantities
	auto p = gaussian<>::pdf(distance, param);
	auto d = gaussian<>::mean_dispersal_distance(param);

	std::cout << "Dispersal from " << source << " to " << target << " :\n"
			  << "distance is " << distance << "\n"
			  << "probability is " << p << "\n"
			  << "mean dispersal distance is " << d << std::endl; 
}
