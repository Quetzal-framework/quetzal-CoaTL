#include "quetzal/quetzal.hpp"
#include <cassert>
#include <iostream>
#include <cmath> // std::pow

int main()
{
  using quetzal::expressive::literal_factory;
  using quetzal::expressive::use;

  // This creates little callables (double, double)
  literal_factory<double,double> lit;

  auto a = lit(42.); // a is no longer a simple double, it's an expression
  auto f = use([](double x, double y){ return std::pow(x, y);});

  // Both a and f can now be invoked with the same interface (double, double) -> double
  assert( a(1.0, 3.14) == 42.0);
  assert( f(1.0, 3.14) == 1.0 );

  // We can now compose them easily ...
  auto my_expression  = a * f ;

  // ... and call them (much) later eg in the library's internals
  std::cout << my_expression(1.0, 3.14) << std::endl;
}
