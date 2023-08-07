#include "quetzal/quetzal.hpp"
#include <cassert>
#include <iostream>
#include <cmath> // std::pow

int main()
{
  // This is used to transform literals into callables
  using quetzal::expressive::literal_factory;
  // This is used to make lambdas usable by expressive
  using quetzal::expressive::use;

  // This object will create little functions that can be called with (double, double) parameters
  literal_factory<double,double> lit;

  // a is no longer a literal: it's a callable always returning 42
  auto a = lit(42.); 
  // check a is indeeed callable with (double, double) parameters
  assert( a(1.0, 3.14) == 42.0);

  // This enriches the lambda interface by giving it access to mathematical operators
  auto f = use([](double x, double y){ return std::pow(x, y);});
  // check a is indeeed callable with (double, double) parameters
  assert( f(1.0, 3.14) == 1.0 );

  // Both a and f can now be invoked with the same interface (double, double) -> double

  // We can now compose them easily ...
  auto my_expression  = a * f ;

  // ... and call them possibly (much) later eg in the library's internals
  std::cout << my_expression(1.0, 3.14) << std::endl;
}
