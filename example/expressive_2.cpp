#include "quetzal/quetzal.hpp"
#include <iostream>

using namespace quetzal;

int main()
{

  // defining some useful type aliases
  using time_type = int;
  using raster_type = geography::raster<time_type>;
  using location_type = raster_type::location_descriptor;

  // Transforms simple numbers into functions of space and time
  expressive::literal_factory<location_type,time_type> lit;

  // The raster has 10 bands that we will assign to 2001 ... 2011.
  std::vector<time_type> times(10);
  std::iota(times.begin(), times.end(), 2001);

  // Load the suitability map
  auto file = std::filesystem::current_path() / "data/suitability.tif";
  auto raster = raster_type::from_file(file, times);

  // We need to make choices here concerning how NA are handled
  auto f1 = raster.to_view();                                       // lightweight functor returning empty optionals for NA
  auto f2 = [f1](location_type x, time_type t){ return f1(x,t).value_or(0.0); };  // remap empty optionals (NA) to 0.0 suitability value
  auto f3 = expressive::use(f2);                                    // f3 has now access to math operators
  auto r = lit(2) * f3 + lit(1);                                    // the growth rate r is a linear function of the suitability

  // r can be later invoked with a location_type and a time_type argument
  auto x = *raster.locations().begin();
  auto t = *raster.times().begin();
  std::cout << "r( x = " << x << ", t = " << t << " ) = " << r(x,t) << std::endl;
}
