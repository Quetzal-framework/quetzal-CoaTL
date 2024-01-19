#include "quetzal/quetzal.hpp"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <random>

using namespace quetzal;

int main()
{
    using time_type = int;
    using raster_type = geography::raster<time_type>;
    using latlon = typename raster_type::latlon;

    // The raster has 10 bands that we will assign to 2001 ... 2011.
    std::vector<time_type> times(10);
    std::iota(times.begin(), times.end(), 2001);

    auto input_file = std::filesystem::current_path() / "data/bio1.tif";
    auto output_dir = std::filesystem::current_path() / "my_sample";

    // Read the raster
    auto bio1 = raster_type::from_file(input_file, times);

    // Sampling points:
    auto p1 = latlon(48., 0.);
    auto p2 = latlon(51, 3.5);
    auto p3 = latlon(38.0, 9.0);

    bio1.to_shapefile({p1, p2, p3}, output_dir);
    std::cout << (std::filesystem::is_directory(output_dir) ? "success" : "error") << std::endl;

    // Clean up
    std::filesystem::remove_all(output_dir);
}
