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
  auto elev = expressive::use([s_view](location_type x, time_type t){ return s_view(x,t).value_or(0.0); });

  std::random_device rd;  // a seed source for the random number engine
  std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

  // 1) hostile environment above an isoline: particularly useful if 123.0 is a randomly sampled parameter to be estimated
  auto isoline_suitability = [suit, elev](location_type x, time_type t){
      return ( elev(x,t) >= 123.0 ) ? 0.0 : suit(x,t);  
  };

  // 2) random (could be persistent) small-scale suitable shelters within high-altitude snow cover
  auto shelter_suitability = [suit, elev, &gen](location_type x, time_type t){
    std::bernoulli_distribution d(0.1); // give "false" (equiv. "0.0") 9/10 of the time
    return ( elev(x,t) >= 123.0 ) ? static_cast<double>(d(gen)) * suit(x,t) : suit(x,t); // above 123 meters, suitability is most of the time 0., rarely habitable.
  };

  // 3) rare trans-oceanic dispersal occurrences - to be coupled with rafting_capacity to create a stochastic oceanic process
  auto rafting_friction = [suit, elev](location_type x, time_type t){
    if( suit(x,t) == 0.0 and elev(x,t) == 0.0  ) // ocean cells
      return 0.0; // move uniformely random across ocean cells
    else if( suit(x,0) == 0.0 and elev(x,t) > 0.0) // unhabitable continental cells
      return 1.00; // max friction as nobody would want to move there. Like Oregon.
    else // habitable continental cells
      return 1.0 - suit(x, 0); 
  };

  auto rafting_capacity= [suit, elev, &gen](location_type x, time_type t){
    std::bernoulli_distribution d(0.1); // give "false" (equiv. "0.0") 9/10 of the time
      if( suit(x,t) == 0.0 and elev(x,t) == 0.0  ) // ocean cells
        return static_cast<double>(d(gen)) * 2; // will allow rarely 2 individuals max on the cell
      else if( suit(x,0) == 0.0 and elev(x,t) > 0.0) // unhabitable continental cells
        return 0.0; // min carrying capacity because no resource
      else // habitable continental cells
        return 1.0 - suit(x, 0); 
  };

  // expressions can be passed to a simulator core and later invoked with a location_type and a time_type argument
  auto x = *landscape.locations().begin();
  auto t = *landscape.times().begin();

  std::cout << "Suitability w/ isoline ( x = " << x << ", t = " << t << " ) = " << isoline_suitability(x,t) << std::endl;
  std::cout << "Suitability w/ shelter ( x = " << x << ", t = " << t << " ) = " << shelter_suitability(x,t) << std::endl;
  std::cout << "Friction w/ rafting    ( x = " << x << ", t = " << t << " ) = " << rafting_friction(x,t) << std::endl;
  std::cout << "Carrying cap. of rafts ( x = " << x << ", t = " << t << " ) = " << rafting_capacity(x,t) << std::endl;
}
