#include "quetzal/polymorphism.hpp"
#include <cassert>

int main()
{
  int nb_sites = 41;
  double mean_pairwise_difference = 3.888889;
  int number_of_segregating_sites = 16;
  int nb_sequences = 10;

  auto stat = quetzal::polymorphism::statistics::tajimasD(mean_pairwise_difference,
                                                    number_of_segregating_sites,
                                                    nb_sequences);

  assert(stat.get() == -1.446172);
}
