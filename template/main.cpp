#include "quetzal/quetzal.hpp"

#include <vector>
#include <string>
#include <filesystem>

int main()
{
    // Load the suitability and elevation maps
    auto file1 = std::filesystem::current_path() / "data/suitability.tif";
    auto file2 = std::filesystem::current_path() / "data/elevation.tif";

    // The rasters 10 bands are indexed by the year they represent: 2001 ... 2010
    std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

    // Initialize the landscape: for each spatial variable a string key and a file value, for all a time series.
    auto landscape = quetzal::geography::landscape<>::from_files({{"suit", file1}, {"DEM", file2}}, times);
    std::cout << landscape << std::endl;

    // Declares some type aliases to shorten notation
    using location_type = quetzal::geography::landscape<>::location_descriptor;
    using time_type= quetzal::geography::landscape<>::time_descriptor;

    // lightweight functors for suitability and digital elevation models that return empty optionals where NA are encounters
    auto suit_view = landscape["suit"].to_view();
    auto elev_view = landscape["DEM"].to_view();

    // We need to make choices here concerning how NA are handled
    auto suit = quetzal::expressive::use([&](location_type x, time_type t)
                                { return suit_view(x, t).value_or(0.0); });
    auto elev = quetzal::expressive::use([&](location_type x, time_type t)
                                { return elev_view(x, t).value_or(0.0); });

    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

    // Small-scale ice-free shelters for the species randomly pop above the snow cover at high-altitude (>123m)
    auto nunatak_suitability = [&](location_type x, time_type t)
    {
        std::bernoulli_distribution d(0.1); // give "false" 9/10 of the time
        bool is_nunatak = d(gen);
        return (elev(x, t) >= 123.0) ? static_cast<double>(is_nunatak) * suit(x, t) : suit(x, t);
    };

    // To allow dispersal across ocean, we can compose expressions:
    auto capacity_with_rafting = [&](location_type x, time_type t)
    {
        std::bernoulli_distribution d(0.1);              // give "false" 9/10 of the time
        if (suit(x, t) == 0.0 and elev(x, t) == 0.0)     // ocean cell case:
            return static_cast<double>(d(gen)) * 2;      //   will (rarely) allow 2 individuals to survive in the ocean cell
        else if (suit(x, 0) == 0.0 and elev(x, t) > 0.0) // unsuitable continental cell case:
            return 0.0;                                  //   suitability is minimal, so should be the capacity
        else                                             // habitable continental cells:
            return nunatak_suitability(x,t);             //   evaluates suitability simulating nunataks
    };

    // Account for different dispersal modes
    auto friction_with_rafting = [&](location_type x, time_type t)
    {
        if (suit(x, t) == 0.0 and elev(x, t) == 0.0)     // ocean cell case:
            return 0.0;                                  //   the raft should move freely on the water
        else if (suit(x, 0) == 0.0 and elev(x, t) > 0.0) // hostile continental cell case:
            return 1.00;                                 //   max friction as the cell is not attractive
        else                                             // favorable continental cell case:
            return 1.0 - suit(x, 0);                     //   higher the suitability, easier the travel
    };

   // Expressions can be evaluated with a location_type and a time_type argument:
   auto x = *landscape.locations().begin();
   auto t = *landscape.times().begin();
  
   std::cout << "Friction f( x = " << x << ", t = " << t << " ) = " << friction_with_rafting(x,t) << std::endl;
   std::cout << "Carrying capacity K( x = " << x << ", t = " << t << " ) = " << capacity_with_rafting(x,t) << std::endl;

    // Spatial graph

} 
