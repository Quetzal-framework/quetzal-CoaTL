// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __DISPERSAL_KERNELS_HPP_INCLUDED__
#define __DISPERSAL_KERNELS_HPP_INCLUDED__

#include <cmath> // M_PI std::gamma std::pow
#include <cassert>

namespace quetzal
{
  namespace demography
  {
    namespace dispersal_kernel
    {
      /// @brief Gaussian dispersal kernel
      ///
      /// @ingroup demography
      ///
      struct Gaussian
      {
        /// @brief Gaussian dispersal kernel parameter
        class param_type
        {
          double _a;
        public:

          /// @brief Gaussian dispersal kernel parameter constructor
          param_type (double a): _a(a) {};

          /// @brief Get Gaussian dispersal kernel parameter
          double a() const {return _a;}
          void a(double value) { _a = value ;}
        };

        /// @brief Gaussian dispersal kernel parameter pdf
        static double pdf(double r, param_type const& p)
        {
          double a = p.a();
          assert(a > 0 && r >= 0);
          return 1/(M_PI*a*a) * std::exp(-(r*r)/(a*a)) ;
        }
      };

      /// @brief Logistic dispersal kernel
      ///
      /// @ingroup demography
      ///
      struct Logistic
      {
        class param_type
        {
          double _a;
          double _b;
        public:
          double a() const {return _a;}
          double b() const {return _b;}
          void a(double value) { _a = value;}
          void b(double value) { _b = value;}
        };

        static double pdf(double r, param_type const& p)
        {
          double a = p.a();
          double b = p.b();
          assert(a > 0 && b >2 && r >= 0);
          return (b/(2*M_PI*(a*a)*std::tgamma(2/b)*std::tgamma(1-2/b)))*(1/(1+(std::pow(r,b)/(std::pow(a,b)))));
        }

      };

      /// @brief NegativeExponential dispersal kernel
      ///
      /// @ingroup demography
      ///
      struct NegativeExponential
      {
        class param_type
        {
          double _a;
        public:
          double a() const {return _a;}
          void a(double value) { _a = value;}
        };

        static double pdf(double r, param_type const& p)
        {
          double a = p.a();
          assert(a > 0 && r>= 0);
          return 1/(2*M_PI*a*a) * std::exp(-r/a) ;
        }

      };

      /// @brief ExponentialPower dispersal kernel
      ///
      /// @ingroup demography
      ///
      struct ExponentialPower
      {
        class param_type
        {
          double _a;
          double _b;
        public:
          double a() const {return _a;}
          double b() const {return _b;}
          void a(double value) { _a = value;}
          void b(double value) { _b = value;}
        };

        static double pdf(double r, param_type const& p)
        {
          double a = p.a();
          double b = p.b();
          assert(a > 0 && b > 0 && r >= 0);
          return b/(2*M_PI*a*a*std::tgamma(2/b)) * std::exp(-std::pow(r,b)/std::pow(a,b));
        }

      };


      /// @brief TwoDt dispersal kernel
      ///
      /// @ingroup demography
      ///
      struct TwoDt
      {
        class param_type
        {
          double _a;
          double _b;
        public:
          double a() const {return _a;}
          double b() const {return _b;}
          void a(double value) { _a = value;}
          void b(double value) { _b = value;}
        };

        static double pdf(double r, param_type const& p)
        {
          double a = p.a();
          double b = p.b();
          assert(a > 0 && b > 1 && r >= 0);
          return (b-1)/(M_PI*a*a) * std::pow( (1 + (r*r)/(a*a)) , -b);
        }
      };

      /// @brief InversePowerLaw dispersal kernel
      ///
      /// @ingroup demography
      ///
      struct InversePowerLaw
      {
        class param_type
        {
          double _a;
          double _b;
        public:
          double a() const {return _a;}
          double b() const {return _b;}
          void a(double value) { _a = value;}
          void b(double value) { _b = value;}
        };

        static double pdf(double r, param_type const& p)
        {
          double a = p.a();
          double b = p.b();
          assert(a > 0 && b > 2 && r >= 0);
          return (b-2)*(b-1)/(2*M_PI*a*a) * std::pow( (1 + r/a) , -b);
        }
      };

      /// @brief UndefinedInversePowerLaw dispersal kernel
      ///
      /// @ingroup demography
      ///
      struct UndefinedInversePowerLaw
      {
        class param_type
        {
          double _a;
          double _b;
        public:
          double a() const {return _a;}
          double b() const {return _b;}
          void a(double value) { _a = value;}
          void b(double value) { _b = value;}
        };

        static double pdf(double r, param_type const& p)
        {
          double a = p.a();
          double b = p.b();
          assert(a > 0 && b > 2 && r >= 0);
          return std::pow( r/a , -b);
        }
      };

      /// @brief Lognormal dispersal kernel
      ///
      /// @ingroup demography
      ///
      struct Lognormal
      {
        class param_type
        {
          double _a;
          double _b;
        public:
          double a() const {return _a;}
          double b() const {return _b;}
          void a(double value) { _a = value;}
          void b(double value) { _b = value;}
        };

        static double pdf(double r, param_type const& p)
        {
          double a = p.a();
          double b = p.b();
          assert(a > 0 && b > 0 && r >= 0);
          return (1/(std::pow(2*M_PI, 3/2)*b*r*r)) * std::exp(- (std::log(r/a)*std::log(r/a) / (2*b*b) ) );
        }
      };

      /// @brief GaussianMixture dispersal kernel
      ///
      /// @ingroup demography
      ///
      struct GaussianMixture
      {
        class param_type
        {
          double _a1;
          double _a2;
          double _p;
        public:
          double a1() const {return _a1;}
          double a2() const {return _a2;}
          double p() const {return _p;}
          void a1(double value) { _a1 = value;}
          void a2(double value) { _a2 = value;}
          void p(double value) { _p = value;}
        };

        static double pdf(double r, param_type const& param)
        {
          double a1 = param.a1();
          double a2 = param.a2();
          double p = param.p();
          assert(a1 > 0 && a2 > 0 && p > 0 && p < 1 && r >= 0);
          return p/(M_PI*a1*a1) * std::exp(-(r*r)/(a1*a1)) + (1-p)/(M_PI*a2*a2) * std::exp(-(r*r)/(a2*a2)) ;
        }
      };
    } // namespace dispersal_kernel
  } // namespace demography
} // namespace quetzal

#endif
