#include "quetzal/quetzal.hpp"
#include <filesystem>
#include <cassert>

using namespace quetzal;

int main()
{
	auto file1 = std::filesystem::current_path() / "data/bio1.tif";
	auto file2 = std::filesystem::current_path() / "data/bio12.tif";

	// The raster have 10 bands that we will assign to 2001 ... 2010.
	std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

	// Initialize the landscape: for each var a key and a file, for all a time series.
	using landscape_type = quetzal::geography::landscape<>;
	auto env = quetzal::geography::landscape<>::from_files( { {"bio1", file1}, {"bio12", file2} }, times );
	std::cout << env << std::endl;

	// // Embed default demographic quantities along vertices (N_{xt}) and edges (Phi_{xyt})
	// auto graph1 = env.to_4_neighborhood_graph<>();
	// auto graph2 = env.to_8_neighborhood_graph<>();
	// auto graph3 = env.to_complete_graph<>();

	// auto weights graph.edges()
	// 	| [](const & e){ return { graph.source(e), graph.target(e)};  }
	// 	| [](const & p){ return { env.to_latlon(s), graph.to_latlon(t)};  }
	// 	| [](const & p){ return x.great_circle_distance_to(y);}
	// 	| [](const & d){ return quetzal::demography::dispersal_kernel::ExponentialPower(d, {.a=1., .b=5.5});}
	// 	| []()
}
