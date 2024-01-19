// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#pragma once

#include <cassert>
#include <cmath> // M_PI tgamma pow

#include <mp-units/math.h>            // mp_units::tgamma, exp etc
#include <mp-units/systems/isq/isq.h> // QuantityOf<isq::length>
#include <mp-units/systems/si/si.h>
namespace quetzal
{
namespace demography
{
namespace dispersal_kernel
{
using namespace mp_units;
// enables the use of shortened units notation: km, m, s
using namespace mp_units::si::unit_symbols;

/// @brief Gaussian dispersal location kernel (thin-tailed)
/// @ingroup demography
/// @details To be used as a reference against leptokurtic kernels.
///          Suitable for diffusion-based dispersal or complete random-walk during a constant time.
template <QuantityOf<isq::length> Distance = mp_units::quantity<mp_units::si::metre>> struct gaussian
{

    /// @brief Dispersal location kernel parameter
    struct param_type
    {
        /// @brief Scale parameter homogeneous to a distance
        Distance a;
    };

    /// @brief Probability density function
    /// @param r The distance radius from the source to the target
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{1}{\pi a^2}.exp(-\frac{r^2}{a^2}) \f$
    static constexpr auto pdf(Distance r, param_type const &p)
    {
        assert(p.a > 0. * m && r >= 0. * m);
        return (1. / (M_PI * p.a * p.a)) * exp(-(r * r) / (p.a * p.a));
    }

    /// @brief Mean dispersal distance
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{a\sqrt{\pi}}{2} \f$
    static constexpr Distance mean_dispersal_distance(param_type const &p)
    {
        using std::pow;
        assert(p.a > 0. * m);
        return (p.a * pow(M_PI, 0.5)) / 2.;
    }
};

/// @brief Logistic dispersal location kernel (fat-tailed, power-law tail)
/// @ingroup demography
/// @details Suitable for frequent long-distance dispersal events and weak effect of distance close to the source.
template <QuantityOf<isq::length> Distance = mp_units::quantity<mp_units::si::metre>> struct logistic
{

    /// @brief Dispersal location kernel parameter
    struct param_type
    {
        /// @brief Scale parameter homogeneous to a distance
        Distance a;

        /// @brief Shape parameter determining the shape of the curve (weight of long-distance events).
        double b;
    };

    /// @brief Probability density function
    /// @param r The distance radius from the source to the target
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{b}{(2\pi a^2 \Gamma(\frac{2}{b}) \Gamma(1-\frac{2}{b}))} (1 +
    /// \frac{r^b}{a^b} )^{-1} \f$
    static constexpr auto pdf(Distance r, param_type const &p)
    {
        using std::exp;
        using std::log;
        using std::pow;
        using std::tgamma;
        Distance a = p.a;
        double b = p.b;
        assert(a > 0. * m && b > 2. && r >= 0. * m);
        return (b / (2. * M_PI * (a * a) * tgamma(2. / b) * tgamma(1. - 2. / b))) *
               (1. / (1. + (pow(r, b) / (pow(a, b)))));
    }

    /// @brief Mean dispersal distance
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$
    /// a~\frac{\Gamma(\frac{3}{b})~\Gamma(1-\frac{3}{b})}{\Gamma(\frac{2}{b})~\Gamma(1-\frac{2}{b})} \f$
    static constexpr Distance mean_dispersal_distance(param_type const &p)
    {
        using std::tgamma;
        Distance a = p.a;
        double b = p.b;
        assert(a > 0. * m && b > 2);
        return (a * tgamma(3. / b) * tgamma(1. - 3. / b)) / (tgamma(2. / b) * tgamma(1. - 2. / b));
    }
};

/// @brief Negative Exponential dispersal location kernel (exponential tail fatness)
/// @ingroup demography
/// @details To be used as reference against more fat-tailed kernels.
///          Suitable for representing a travel at constant speed in a random direction with a constant
///          stopping rate, or a correlated random walk with settlement.
template <QuantityOf<isq::length> Distance = mp_units::quantity<mp_units::si::metre>> struct negative_exponential
{
    /// @brief Dispersal location kernel parameter
    struct param_type
    {
        /// @brief Scale parameter homogeneous to a distance
        Distance a;
    };

    /// @brief Probability density function
    /// @param r The distance radius from the source to the target
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{1}{2 \pi a^2} exp(-\frac{r}{a}) \f$
    static constexpr auto pdf(Distance r, param_type const &p)
    {
        using std::exp;
        Distance a = p.a;
        assert(a > 0. * m && r >= 0. * m);
        return 1. / (2. * M_PI * a * a) * exp(-r / a);
    }

    /// @brief Mean dispersal distance
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ 2a \f$
    static constexpr Distance mean_dispersal_distance(param_type const &p)
    {
        Distance a = p.a;
        assert(a > 0. * m);
        return 2. * a;
    }
};

/// @brief Exponential Power dispersal location kernel (\f$b > 1\f$ : thin-tailed, \f$b < 1\f$ : fat-tailed. Always
/// thinner than power laws.)
/// @ingroup demography
/// @details Suitable for pollen dispersal. Gaussian and Exponential are special cases, making it suitable for shape
/// comparisons.
template <QuantityOf<isq::length> Distance = mp_units::quantity<mp_units::si::metre>> struct exponential_power
{
    /// @brief Dispersal location kernel parameter
    struct param_type
    {
        /// @brief Scale parameter homogeneous to a distance
        Distance a;

        /// @brief Shape parameter determining the shape of the curve (weight of long-distance events).
        double b;
    };

    /// @brief Probability density function
    /// @param r The distance radius from the source to the target
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{b}{2 \pi a^2 \Gamma(\frac{2}{b})} exp(-\frac{r^b}{a^b}) \f$
    static constexpr auto pdf(Distance r, param_type const &p)
    {
        using std::pow;
        using std::tgamma;
        Distance a = p.a;
        double b = p.b;
        assert(a > 0. * m && b > 0. && r >= 0. * m);
        return b / (2. * M_PI * a * a * tgamma(2. / b)) * exp(-pow(r, b) / pow(a, b));
    }

    /// @brief Mean dispersal distance
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ a ~ \frac{\Gamma(\frac{3}{b})}{\Gamma(\frac{2}{b})} \f$
    static constexpr Distance mean_dispersal_distance(param_type const &p)
    {
        using std::tgamma;
        Distance a = p.a;
        double b = p.b;
        assert(a > 0. * m && b > 0.);
        return a * tgamma(3. / b) / tgamma(2. / b);
    }
};

/// @brief TwoDt dispersal location kernel (fat-tailed, power-law tail)
/// @ingroup demography
/// @details Suitable for seed dispersal. Obtained as a continuous mixture of Gaussian kernels with variance
///          parameters distributed as the inverse of a Gamma distribution.
template <QuantityOf<isq::length> Distance = mp_units::quantity<mp_units::si::metre>> struct two_dt
{
    /// @brief Dispersal location kernel parameter
    struct param_type
    {
        /// @brief Scale parameter homogeneous to a distance
        Distance a;

        /// @brief Shape parameter determining the shape of the curve (weight of long-distance events).
        double b;
    };

    /// @brief Probability density function
    /// @param r The distance radius from the source to the target
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{b-1}{\pi a^2} (1+\frac{r^2}{a^2})^{-b} \f$
    static constexpr auto pdf(Distance r, param_type const &p)
    {
        using std::pow;
        Distance a = p.a;
        double b = p.b;
        assert(a > 0. * m && b > 1. && r >= 0. * m);
        return (b - 1.) / (M_PI * a * a) * pow((1. + (r * r) / (a * a)), -b);
    }

    /// @brief Mean dispersal distance
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ a ~ \frac{\sqrt{\pi_PI}}{2}\frac{\Gamma(b-\frac{3}{2})}{\Gamma(b-1)} \f$
    /// @warning Return \f$ \infty \f$ if \f$ b < 3/2 \f$
    static constexpr Distance mean_dispersal_distance(param_type const &p)
    {
        using std::pow;
        using std::tgamma;
        Distance a = p.a;
        double b = p.b;
        assert(a > 0. * m && b > 1.);
        if (b < 3. / 2.)
        {
            return std::numeric_limits<double>::infinity();
        }
        return (a * pow(M_PI, 0.5) * tgamma(b - 3. / 2.)) / (2. * tgamma(b - 1.));
    }
};

/// @brief Inverse Power Law dispersal location kernel (fat-tailed, power-law tail)
/// @ingroup demography
/// @details Suitable for very fat tails.
template <QuantityOf<isq::length> Distance = mp_units::quantity<mp_units::si::metre>> struct inverse_power_law
{
    /// @brief Dispersal location kernel parameter
    struct param_type
    {
        /// @brief Scale parameter homogeneous to a distance
        Distance a;

        /// @brief Shape parameter determining the shape of the curve (weight of long-distance events).
        double b;
    };

    /// @brief Probability density function
    /// @param r The distance radius from the source to the target
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{(b-2)(b-1)}{2 \pi a^2} (1+\frac{r}{a})^{-b} \f$
    static constexpr auto pdf(Distance r, param_type const &p)
    {
        using std::pow;
        Distance a = p.a;
        double b = p.b;
        assert(a > 0. * m && b > 2. && r >= 0. * m);
        return pow(1. + r / a, -b) * (b - 2.) * (b - 1.) / (2. * M_PI * a * a);
    }

    /// @brief Mean dispersal distance
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{2a}{b-3} \f$
    /// @warning Return \f$ \infty \f$ if \f$ b < 3 \f$
    static constexpr Distance mean_dispersal_distance(param_type const &p)
    {
        Distance a = p.a;
        double b = p.b;
        assert(a > 0. * m && b > 2.);
        if (b < 3.)
        {
            return std::numeric_limits<double>::infinity();
        }
        return 2. * a / (b - 3.);
    }
};

/// @brief Lognormal dispersal location kernel (fat-tailed)
/// @ingroup demography
/// @details Suitable for seed dispersal, particularly when the peak of the distribution is not at zero distance from
/// the source.
template <QuantityOf<isq::length> Distance = mp_units::quantity<mp_units::si::metre>> struct lognormal
{
    /// @brief Dispersal location kernel parameter
    struct param_type
    {
        /// @brief Scale parameter homogeneous to a distance
        Distance a;

        /// @brief Shape parameter determining the shape of the curve (weight of long-distance events).
        double b;
    };

    /// @brief Probability density function
    /// @param r The distance radius from the source to the target
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{1}{ (2\pi)^{3/2}br^2} exp( - \frac{log(r/a)^2}{2b^2} ) \f$
    // static constexpr auto pdf(Distance r, param_type const& p)
    // {
    //   Distance a = p.a;
    //   double b = p.b;
    //   assert(a > 0. * m && b > 0. && r >= 0. * m);
    //   return (1./(pow(2.*M_PI, 3./.2)*b*r*r)) * exp(- (mp_units::log(r/a)*mp_units::log(r/a) / (2.*b*b) ) );
    // }

    /// @brief Mean dispersal distance
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ a ~ exp(\frac{b^2}{2}) \f$
    static constexpr Distance mean_dispersal_distance(param_type const &p)
    {
        using std::exp;
        Distance a = p.a;
        double b = p.b;
        assert(a > 0. * m && b > 0.);
        return a * exp(b * b / 2.);
    }
};

/// @brief Gaussian Mixture dispersal location kernel (leptokurtic, never fat-tailed)
/// @ingroup demography
/// @details Used in theoretical studies.
template <QuantityOf<isq::length> Distance = mp_units::quantity<mp_units::si::metre>> struct gaussian_mixture
{
    /// @brief Dispersal location kernel parameter
    struct param_type
    {
        /// @brief First scale parameter homogeneous to a distance
        Distance a1;
        /// @brief Second scale parameter homogeneous to a distance
        Distance a2;
        /// @brief Shape parameter
        double p;

      public:
    };

    /// @brief Probability density function
    /// @param r The distance radius from the source to the target
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{p}{\pi a_1^2} exp(- \frac{r^2}{a_1^2}) + \frac{1-p}{\pi a_2^2}
    /// exp(\frac{r^2}{a_2^2}) \f$
    static constexpr auto pdf(Distance r, param_type const &param)
    {
        using std::exp;
        Distance a1 = param.a1;
        Distance a2 = param.a2;
        double p = param.p;
        assert(a1 > 0. * m && a2 > 0. * m && p > 0. && p < 1. && r >= 0. * m);
        return p / (M_PI * a1 * a1) * exp(-(r * r) / (a1 * a1)) +
               (1. - p) / (M_PI * a2 * a2) * exp(-(r * r) / (a2 * a2));
    }

    /// @brief Mean dispersal distance
    /// @param p Parameters of the distribution
    /// @return The value of the expression \f$ \frac{\sqrt{\pi}}{2}~ (pa_1+(1-p)a_2) \f$
    static constexpr Distance mean_dispersal_distance(param_type const &param)
    {
        using std::pow;
        Distance a1 = param.a1;
        Distance a2 = param.a2;
        double p = param.p;
        assert(a1 > 0. * m && a2 > 0. * m && p > 0. && p < 1.);
        return pow(M_PI, 0.5) * (p * a1 + (1. - p) * a2) / 2.;
    }
};
} // namespace dispersal_kernel
} // namespace demography
} // namespace quetzal