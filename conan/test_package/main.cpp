#include <quetzal/quetzal.h>

int main()
{
  using time_type = int;
  using landscape_type = quetzal::geography::DiscreteLandscape<std::string,time_type>;
  using coord_type = landscape_type::coord_type;
  landscape_type env( {{"temp", argv[1]}},   {time_type(0)} );
}
