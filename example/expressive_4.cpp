#include "quetzal/quetzal.hpp"
#include <iostream>
#include <random>

using namespace quetzal;

int main()
{

    // defining some useful type aliases
    using key_type = std::string;
    using time_type = int;
    using landscape_type = geography::landscape<key_type, time_type>;
    using location_type = landscape_type::location_descriptor;

    // Load the suitability map
    auto file1 = std::filesystem::current_path() / "data/suitability.tif";
    auto file2 = std::filesystem::current_path() / "data/elevation.tif";

    // The raster has 10 bands that we will assign to 2001 ... 2011.
    std::vector<time_type> times(10);
    std::iota(times.begin(), times.end(), 2001);

    // Makes sure the rasters are aligned
    auto landscape = landscape_type::from_file({{"suit", file1}, {"DEM", file2}}, times);

    // lightweight functor returning empty optionals where NA
    auto s_view = landscape["suit"].to_view();
    auto e_view = landscape["DEM"].to_view();

    // We need to make choices here concerning how NA are handled
    auto suit = expressive::use([&](location_type x, time_type t) { return s_view(x, t).value_or(0.0); });
    auto elev = expressive::use([&](location_type x, time_type t) { return e_view(x, t).value_or(0.0); });

    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

    auto rafting_capacity = [&](location_type x, time_type t) {
        std::bernoulli_distribution d(0.1);          // give "false" 9/10 of the time
        if (suit(x, t) == 0.0 and elev(x, t) == 0.0) // ocean cell case
            return static_cast<double>(d(gen)) * 2;  // will (rarely) allow 2 individuals to survive in the ocean cell
        else if (suit(x, 0) == 0.0 and elev(x, t) > 0.0) // unhabitable continental cell case
            return 0.0;                                  // suitability is minimal, so should be the capacity
        else                                             // habitable continental cells
            return 100. * suit(x, 0);                    // simple rescaling by the max number of individuals in a cell
    };

    auto rafting_friction = [&](location_type x, time_type t) {
        if (suit(x, t) == 0.0 and elev(x, t) == 0.0)     // ocean cell case
            return 0.0;                                  // the raft should move freely
        else if (suit(x, 0) == 0.0 and elev(x, t) > 0.0) // hostile continental cell case
            return 1.00;                                 // max friction as the cell is not attractive
        else                                             // favorable continental cell case
            return 1.0 - suit(x, 0);                     // higher the suitability, easier the travel
    };

    // expressions can be passed to a simulator core and later invoked with a location_type and a time_type argument
    auto x = *landscape.locations().begin();
    auto t = *landscape.times().begin();

    std::cout << "Friction f( x = " << x << ", t = " << t << " ) = " << rafting_friction(x, t) << std::endl;
    std::cout << "Carrying capacity K( x = " << x << ", t = " << t << " ) = " << rafting_capacity(x, t) << std::endl;
}
