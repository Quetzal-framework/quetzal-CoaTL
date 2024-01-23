#include "quetzal/quetzal.hpp"
#include <cassert>
#include <filesystem>

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
    auto env = quetzal::geography::landscape<>::from_files({{"bio1", file1}, {"bio12", file2}}, times);
    std::cout << env << std::endl;

    // We indeed recorded 2 variables: bio1 and bio12
    assert(env.num_variables() == 2);

    // The semantic shares strong similarities with a raster
    landscape_type::latlon Bordeaux(44.5, 0.34);

    assert(env.contains(Bordeaux));
    assert(env.contains(env.to_centroid(Bordeaux)));

    // These little function-objects will soon be very handy to embed the GIS variables
    // into the simulation with quetzal::expressive
    const auto &f = env["bio1"].to_view();
    const auto &g = env["bio12"].to_view();

    // But for now just print their raw data if defined or any other value if undefined
    for (auto t : env.times())
    {
        for (auto x : env.locations())
        {
            f(x, t).value_or(0.0);
            g(x, t).value_or(env["bio12"].NA());
        }
    }
}
