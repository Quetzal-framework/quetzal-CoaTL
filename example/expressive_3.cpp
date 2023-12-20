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
  auto landscape = landscape_type::from_files( {{"suit", file1}, {"DEM", file2}}, times);

  // lightweight functor returning empty optionals where NA
  auto s_view = landscape["suit"].to_view(); 
  auto e_view = landscape["DEM"].to_view(); 

  // We need to make choices here concerning how NA are handled
  auto suit = expressive::use([s_view](location_type x, time_type t){ return s_view(x,t).value_or(0.0); });
  auto elev = expressive::use([e_view](location_type x, time_type t){ return e_view(x,t).value_or(0.0); });

  std::random_device rd;  // a seed source for the random number engine
  std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

  // 1) hostile environment above an isoline: particularly useful if 123.0 is a randomly sampled parameter to be estimated
  auto isoline_suitability = [suit, elev](location_type x, time_type t){
      return ( elev(x,t) >= 123.0 ) ? 0.0 : suit(x,t);  
  };

  // 2) small-scale suitable ice-free shelters randomly popping above the snow cover at high-altitude
  auto nunatak_suitability = [suit, elev, &gen](location_type x, time_type t){
    std::bernoulli_distribution d(0.1); // give "false" 9/10 of the time
    bool is_nunatak = d(gen);
    return ( elev(x,t) >= 123.0 ) ? static_cast<double>(is_nunatak) * suit(x,t) : suit(x,t);
  };

  // expressions can be passed to a simulator core and later invoked with a location_type and a time_type argument
  auto x = *landscape.locations().begin();
  auto t = *landscape.times().begin();

  std::cout << "Suitability w/ isoline ( x = " << x << ", t = " << t << " ) = " << isoline_suitability(x,t) << std::endl;
  std::cout << "Suitability w/ shelter ( x = " << x << ", t = " << t << " ) = " << nunatak_suitability(x,t) << std::endl;
}
