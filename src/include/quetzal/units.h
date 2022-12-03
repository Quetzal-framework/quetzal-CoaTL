#ifndef PHYSIC_CONSTANTS_HPP
#define PHYSIC_CONSTANTS_HPP


#include <units/base_dimension.h>
#include <units/isq/si/prefixes.h>
#include <units/quantity.h>
#include <units/quantity_io.h>
#include <units/unit.h>
#include <iostream>
#include <type_traits>

using namespace units;

// namespace fps {
//
// struct foot : named_unit<foot, "ft"> {};
// struct yard : named_scaled_unit<yard, "yd", ratio(3), foot> {};
//
// struct dim_length : base_dimension<"L", foot> {};
//
// template<UnitOf<dim_length> U, Representation Rep = double>
// using length = quantity<dim_length, U, Rep>;
//
// }  // namespace fps


///
/// @brief Compile-time unit quantities for coalescence
///
namespace quetzal::units
{}

//     using namespace units;
//     ///
//     /// @brief New kind to represent probability as a more specific usage of a dimensionless quantity
//     ///
//     /// @note Quantities of kind generation are now clearly distinct from more generic usages of time quantities.
//     ///
//     struct foot : named_unit<foot, "ft"> {};
//     // struct desk : named_scaled_unit<desk, "desk", ratio(3, 10), square_metre> {};
//     // struct probability : units::kind<probability, "probability", ratio(1), units::dimensionless> {};
//
//     // User Defined Literals
//     // constexpr auto operator"" _pr(double p) { return units::kind<probability, double>(p); }
//     ///
//     /// @brief New kind to represent the more specific usage of a time quantity
//     ///
//     /// @note Quantities of kind generation are now clearly distinct from more generic usages of time quantities.
//     ///
//     // struct generation : units::kind<generation, units::isq::si::dim_time> {};
//
//     ///
//     /// @brief New kind to represent the more specific usage of a time quantity
//     ///
//     /// @note Quantities of kind generation are now clearly distinct from more generic usages of time quantities.
//     ///
//     // struct coalescence : units::kind<coalescence, units::isq::si::dim_time> {};
//
//     } // end namespace units
//   } //
// }

#endif
