#include "quetzal/quetzal.hpp"
#include <filesystem>
#include <cassert>
//#include <print>

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

	// We convert the grid to a fully connected graph
	auto graph = env.to_complete_graph<>();

	// A little helper function
	auto constexpr sphere_distance = [&]( auto const& p1, auto const& p2 ){ 
		return env.to_latlon( p1 ).great_circle_distance( env.to_latlon( p2 ) );}
	
	auto v = graph.edges()
		| std::transform( [&](auto const& e){ sphere_distance( graph.source( e ), graph.target( e ) );} )
		| std::transform( [&](auto const& d){ return quetzal::demography::dispersal_kernel::exponential_power( d, {.a=1., .b=5.5} ) ;} )
		| std::ranges::to<std::vector>();

    //std::println("{}", v);
}
