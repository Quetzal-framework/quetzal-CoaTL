#include "quetzal/quetzal.hpp"
#include <filesystem>
#include <cassert>
// #include <print>

namespace geo = quetzal::geography;

int main()
{
	auto file1 = std::filesystem::current_path() / "data/bio1.tif";
	auto file2 = std::filesystem::current_path() / "data/bio12.tif";

	// The raster have 10 bands that we will assign to 2001 ... 2010.
	std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

	// Initialize the landscape: for each var a key and a file, for all a time series.
	using landscape_type = geo::landscape<>;
	auto land = geo::landscape<>::from_files( { {"bio1", file1}, {"bio12", file2} }, times );
	std::cout << land << std::endl;

	// We convert the grid to a fully connected graph
	auto graph1 = geo::graph_from_grid( land, geo::connect_fully(), geo::isotropy() ); // dense, isotrope migration
	auto graph2 = geo::graph_from_grid( land, geo::connect_fully(), geo::anisotropy() ); // dense, anisotrope

	auto graph3 = geo::graph_from_grid( land, geo::connect_4_neighbors(), geo::isotropy() ); // sparse, isotrope migration
	auto graph4 = geo::graph_from_grid( land, geo::connect_4_neighbors(), geo::anisotropy() ); // sparse, anisotrope migration

	auto graph5 = geo::graph_from_grid( land, geo::connect_8_neighbors(), geo::isotropy() ); // sparse, isotrope migration
	auto graph6 = geo::graph_from_grid( land, geo::connect_8_neighbors(), geo::anisotropy() ); // sparse, anisotrope migration

	// A little helper function
	auto constexpr sphere_distance = [&]( auto const& p1, auto const& p2 ){ 
		return land.to_latlon( p1 ).great_circle_distance( land.to_latlon( p2 ) );}
	
	auto v = graph1.edges()
		| std::transform( [&](auto const& e){ sphere_distance( graph.source( e ), graph.target( e ) );} )
		| std::transform( [&](auto const& d){ return quetzal::demography::dispersal_kernel::exponential_power( d, {.a=1., .b=5.5} ) ;} )
		| std::ranges::to<std::vector>();

    //std::println("{}", v);
}
