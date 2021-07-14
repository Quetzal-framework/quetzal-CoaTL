// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __DISPERSAL_POLICY_H_INCLUDED__
#define __DISPERSAL_POLICY_H_INCLUDED__

#include "../utils/PointWithId.h"
#include "../utils/matrix_operation.h"

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/symmetric.hpp>

#include <vector>
#include <memory> // make_shared
#include <random> // std::discrete_distribution
#include <numeric> // std::transform std::accumulate

namespace quetzal
{
  namespace demography
  {
    ///
    /// @brief Various implementations of dispersal process implementing an interface
    /// compatible with the requirements of the \ref quetzal::demography::History simulation algorithm.
    ///
    namespace dispersal_policy
    {
      ///
      /// @brief Policy class to specialize the \ref quetzal::demography::History class for expansion of populations with very low densities.
      ///
      /// @remark Meant to be used through its helper function \ref individual_based::make_distance_based_dispersal()
      ///
      /// @ingroup demography
      ///
      /// @details Simulate the demographic history by sampling the next location of each migrant in a proability distribution.
      ///
      class individual_based
      {
      private:
        ///
        /// @brief Inner class implementing details of the individual-based expansion algorithm
        ///
        /// @remark Contains a heavy matrix, not meant to be copied around.
        ///
        /// @tparam Space type of geographic coordinates
        /// @tparam Matrix the type of matrix used for storing migration rates
        ///
        template<typename Space, typename Matrix>
        class Implementation
        {
        private:
          using coord_type = Space;
          // distance matrix type, can be sparse or symmetric
          using matrix_type = Matrix;
          // for easier access to matrix row/columns ID
          using point_ID_type = quetzal::utils::PointWithId<coord_type>;
          // owe the matrix, heavy object
          matrix_type m_matrix;
          // don't owe the vector
          std::vector<point_ID_type> const& m_points;
          // don't owe the space
          std::vector<coord_type> const& m_coords;
          // specific to individual-based: stores discrete distributions centered on departure
          using distribution_type = std::discrete_distribution<size_t>;
          mutable std::vector<distribution_type> m_distributions;
          ///
          /// @brief Make a weight matrix
          ///
          /// @remark The resulting matrix is meant to be used in discrete_distribution: no need to divide terms by row sum
          ///
          /// @tparam F functor with signature matrix_type::value_type (coord_type const& x, coord_type const&y)
          /// @param  coords vector of coordinates to compute distance between
          /// @param  w      functor F, usually a transform of geographic distance between two coordinates
          /// @return        a weight matrix
          ///
          template<typename F>
          matrix_type make_weight_matrix(std::vector<coord_type> const& coords, F w){
            matrix_type A (coords.size(), coords.size());
            for (unsigned i = 0; i < A.size1 (); ++ i)
            {
              auto x = coords.at(i);
              for (unsigned j = 0; j < A.size2(); ++ j)
              {
                auto y = coords.at(j);
                A (i, j) = w(x, y);
              }
            }
            return A;
          }
          ///
          /// @brief Builds the discrete distributions for sampling coords IDs
          ///
          /// @return a vector of discrete distributions
          ///
          std::vector<distribution_type> make_distributions() const
          {
            std::vector<distribution_type> dists;
            dists.reserve(m_matrix.size1());
            for (unsigned i = 0; i < m_matrix.size1 (); ++ i)
            {
              std::vector<double> weights;
              weights.reserve(m_matrix.size2());
              for (unsigned j = 0; j < m_matrix.size2(); ++ j)
              {
                weights.push_back(m_matrix(i, j));
              }
              dists.emplace_back(weights.cbegin(), weights.cend());
            }
            return dists;
          }
        public:
          ///
          /// @brief Forbid costly copy
          ///
          Implementation(const Implementation&) = delete;
          ///
          /// @brief Constructor
          ///
          /// @tparam F functor with signature matrix_type::value_type (coord_type const& x, coord_type const&y)
          ///
          template<typename F>
          Implementation(std::vector<point_ID_type> const& points, std::vector<coord_type> const& coords, F f):
          m_matrix( make_weight_matrix(coords, f) ),
          m_points(points),
          m_coords(coords),
          m_distributions(make_distributions())
          {}
          ///
          /// @brief Sample arrival coordinate conditionally to departure
          ///
          template<typename Generator>
          coord_type sample_arrival(Generator& gen, coord_type const& x) const
          {
            return m_coords[m_distributions[quetzal::utils::getIndexOfPointInVector(x, m_coords)].operator()(gen)];
          }
        }; // end inner class Implementation

        ///
        /// @brief Copiable class interfaceable with the demographic simulation
        ///
        template<typename Space, typename Matrix>
        class Interface
        {
        private:
          using impl_type = Implementation<Space, Matrix>;
          using coord_type = Space;
          using point_ID_type = quetzal::utils::PointWithId<coord_type>;
          // makes copy of the interface is ok
          std::shared_ptr<impl_type> m_pimpl;
        public:
          ///
          /// @brief Constructor, to be invoked by helper functions
          ///
          template<typename F>
          Interface(std::vector<point_ID_type> const& points, std::vector<coord_type> const& coords, F f):
          m_pimpl(std::make_shared<impl_type>(points, coords, f) )
          {}
          ///
          /// @brief Sample an arrival coordinate in the dispersal distribution conditionally to departure x
          ///
          /// @param x the departure coordinate
          ///
          /// @remark Interface with the individual-based demographic history expand method
          ///
          template<typename Generator>
          coord_type operator()( Generator& gen, coord_type const& x)
          {
            return m_pimpl->sample_arrival(gen, x);
          }
        }; // end inner class Interface

        public:
          /// \typedef type used to represent effective population size and flows
          using value_type = unsigned int;
          ///
          /// @brief Build a dispersal kernel usable with class History specialized for individual-based policy
          ///
          /// @param  coords a vector of geographic points
          /// @param  f      a functor giving the migration probability of an individual between two points
          ///                The signature should be equivalent to double f(T const& x, T const& Y)
          /// @return        A dispersal kernel that is cheap to copy
          ///
          template<typename T, typename F>
          static auto make_distance_based_dispersal(std::vector<T> const& coords, F f)
          {
            using coord_type = T;
            using point_ID_type = quetzal::utils::PointWithId<coord_type>;
            std::vector<point_ID_type> points;
            points.reserve(coords.size());
            const auto& ref = coords;
            std::transform(coords.begin(), coords.end(), std::back_inserter(points),
            [ref](coord_type const& x){ return point_ID_type(ref, x); });
            using matrix_type = boost::numeric::ublas::symmetric_matrix<double>;
            return Interface<coord_type, matrix_type>(points, coords, f);
          }
        }; // end individual_based

      ///
      /// @brief Policy class to specialize the \ref quetzal::demography::History class for simulation of population with large density.
      ///
      /// @ingroup demography
      ///
      /// \details Simulate the demographic history by considering populations of individuals as divisible masses.
      ///
      class mass_based
      {
      private:
        ///
        /// @brief Inner class implementing details of the demographic simulation algorithm
        /// @tparam Space type of geographic coordinates
        /// @tparam Matrix the type of matrix used for storing migration rates
        ///
        template<typename Space, typename Matrix>
        class Implementation
        {
        private:
          using coord_type = Space;
          // distance matrix type, can be sparse or symmetric
          using matrix_type = Matrix;
          // for easier access to matrix row/columns ID
          using point_ID_type = quetzal::utils::PointWithId<coord_type>;
          // owe the matrix, heavy object
          matrix_type m_matrix;
          // don't owe the vector
          std::vector<point_ID_type> const& m_points;
          // don't owe the space
          std::vector<coord_type> const& m_coords;
          // oops what was that again ?
          mutable std::map<coord_type, std::vector<coord_type>> m_arrival_space_cash;
          ///
          /// @brief Make a weight matrix
          ///
          /// @tparam F functor with signature matrix_type::value_type (coord_type const& x, coord_type const&y)
          ///
          /// @param coords vector of coordinates to compute distance between
          /// @param w functor F, usually a transform of geographic distance between two coordinates
          ///
          /// @return a weight matrix
          ///
          template<typename F>
          matrix_type make_weight_matrix(std::vector<coord_type> const& coords, F w)
          {
            matrix_type A (coords.size(), coords.size());
            for (unsigned i = 0; i < A.size1 (); ++ i)
            {
              for (unsigned j = 0; j < A.size2(); ++ j)
              {
                auto x = coords.at(i);
                auto y = coords.at(j);
                A (i, j) = w(x, y);
              }
            }
            return quetzal::utils::divide_terms_by_row_sum(A);
          }
        public:
          ///
          /// @brief Forbid costly copy
          ///
          Implementation(const Implementation&) = delete;
          ///
          /// @brief Constructor
          ///
          template<typename F>
          Implementation(std::vector<point_ID_type> const& points, std::vector<coord_type> const& coords, F f) :
          m_matrix( make_matrix(coords, f) ),
          m_points(points),
          m_coords(coords)
          {}
          ///
          /// @brief Allows the policy to be interfaceable with the \ref quetzal::demography::History simulation algorithm.
          ///
          /// @param x departure coordinate
          ///
          /// @return a std::vector of coordinates reachable from x.
          ///
          auto arrival_space(coord_type const& x) const
          {
            auto it = m_arrival_space_cash.find(x);
            if(it != m_arrival_space_cash.end())
            {
              return it->second;
            }
            else
            {
              auto p = m_arrival_space_cash.emplace(x, retrieve_non_zero_arrival_space(x));
              return p.first->second;
            }
          }
          ///
          /// @brief Avoids iteration over coordinates having a null probability of dispersal
          ///
          auto retrieve_non_zero_arrival_space(coord_type const& x) const
          {
            using it1_t = typename matrix_type::const_iterator1;
            using it2_t = typename matrix_type::const_iterator2;
            auto i = quetzal::utils::getIndexOfPointInVector(x, m_coords);
            it1_t it1 = m_matrix.begin1();
            std::advance(it1, i);
            std::vector<coord_type> v;
            v.reserve(m_matrix.size2()); // worst case
            for (it2_t it2 = it1.begin(); it2 != it1.end(); it2++)
            {
              if(*it2 > 0.0) v.push_back(m_coords.at(it2.index2()));
            }
            return v;
          }
          ///
          /// @brief Returns the migration rate
          ///
          double migration_rate(coord_type const& x, coord_type const& y)
          {
            return m_matrix(quetzal::utils::getIndexOfPointInVector(x, m_coords), quetzal::utils::getIndexOfPointInVector(y, m_coords) );
          }
        }; // end of inner class Implementation

        ///
        /// @brief Copiable class interfaceable with the demographic simulation
        ///
        template<typename Space, typename Matrix>
        class Interface
        {
        private:
          using impl_type = Implementation<Space, Matrix>;
          using coord_type = Space;
          using point_ID_type = quetzal::utils::PointWithId<coord_type>;
          std::shared_ptr<impl_type> m_pimpl;
        public:
          ///
          /// @brief Constructor use discouraged: use instead the helper functions \ref make_distance_based_dispersal() and make_sparse_distance_based_dispersal
          ///
          template<typename F>
          Interface(std::vector<point_ID_type> const& points, std::vector<coord_type> const& coords, F f):
          m_pimpl(std::make_shared<impl_type>(points, coords, f) )
          {}
          ///
          /// @brief Migration rate, operator interfaceable with \ref History algorithm.
          ///
          double operator()(coord_type const& x, coord_type const& y)
          {
            return m_pimpl->migration_rate(x,y);
          }
          ///
          /// @brief Defines arrival space (that is, vecinity plus focal deme) from focal deme x.
          ///
          /// @remark Interface required in expansion algorithm.
          ///
          auto arrival_space(coord_type const& x) const
          {
            return m_pimpl->arrival_space(x);
          }
        }; // inner class Interface

      public:
        //! \typedef type used to represent effective population size and flows
        using value_type = unsigned int;
        ///
        /// @brief Build a dispersal kernel usable with class History specialized for mass-based simulation policy
        ///
        /// @param  coords a vector of geographic points
        /// @param  f      a functor giving the migration coefficient between two points
        ///                The signature should be equivalent to `double f(T const& x, T const& Y)`
        /// @return        A dispersal kernel
        ///
        template<typename T, typename F>
        static auto make_distance_based_dispersal(std::vector<T> const& coords, F f)
        {
          using coord_type = T;
          using point_ID_type = quetzal::utils::PointWithId<coord_type>;
          std::vector<point_ID_type> points;
          points.reserve(coords.size());
          const auto& ref = coords;
          std::transform(coords.begin(), coords.end(), std::back_inserter(points),
          [ref](coord_type const& x){ return point_ID_type(ref, x); });
          using matrix_type = boost::numeric::ublas::symmetric_matrix<double>;
          return Interface<coord_type, matrix_type>(points, coords, f);
        }

        ///
        /// @brief Contruct a dispersal kernel compatible with the mass-based policy
        ///
        /// @details This implementation should be chosen when the migration coefficients
        ///          are expected to be null over most of the landscape.
        ///
        /// @param  coords a container of geopgrahic points
        /// @param  f      a functor giving the migration coefficient between two points
        ///                The signature should be equivalent to double f(T const& x, T const& Y)
        /// @return        A dispersal kernel
        ///
        template<typename T, typename F>
        static auto make_sparse_distance_based_dispersal(std::vector<T> const& coords, F f)
        {
          using coord_type = T;
          using point_ID_type = quetzal::utils::PointWithId<coord_type>;
          std::vector<point_ID_type> points;
          points.reserve(coords.size());
          const auto& ref = coords;
          std::transform(coords.begin(), coords.end(), std::back_inserter(points),
          [ref](coord_type const& x){ return point_ID_type(ref, x); });
          using matrix_type = boost::numeric::ublas::compressed_matrix<double>;
          return Interface<coord_type, matrix_type>(points, coords, f);
        }

        ///
        /// @brief Dispersal kernel compatible with the mass-based policy, but that restricts migration to the vecinity of each deme.
        //////
        /// @remark For more flexibility, vecinity is defined by an external callable (does not have to be only the four nearest neighbours)
        ///
        /// @details This implementation should be chosen when the scale of the migration is much less than the scale of a deme.
        ///          This class is not meant to be used directly, but instanciated through the use of the helper function \ref make_neighboring_migration()
        ///
        /// @param  coords a container of geopgrahic points
        /// @param  f      a functor giving the migration coefficient between two points
        ///                The signature should be equivalent to double f(T const& x, T const& Y)
        /// @return        A dispersal kernel
        ///
        template<typename Space, typename F1, typename F2>
        class neighboring_migration
        {
        private:
          double m_emigrant_rate;
          F1 m_friction;
          F2 m_get_neighbors;
        public:
          using coord_type = Space;
          // Constructor
          neighboring_migration(double emigrant_rate, F1 friction, F2 get_neighbors):
          m_emigrant_rate(emigrant_rate),
          m_friction(friction),
          m_get_neighbors(get_neighbors)
          {}
          ///
          /// @brief Defines arrival space (that is in this class case, vecinity plus focal deme) from coordinate x.
          ///
          /// @remark Interface required in expansion algorithm.
          ///
          std::vector<coord_type> arrival_space(coord_type const& x) const
          {
            auto v = m_get_neighbors(x);
            // let's add focal deme to have all possible arrival demes
            v.push_back(x);
            return v;
          }
          ///
          /// @brief Migration rate, operator interfaceable with \ref History algorithm.
          ///
          auto operator()(coord_type const& from, coord_type const& to)
          {
            if(from == to)
            {
              return 1.0 - m_emigrant_rate;
            }
            else
            {
              return m_emigrant_rate * friction_weights(from, to);
            }
          }
        private:
          double friction_weights(coord_type const& x, coord_type const& y) const
          {
            // TODO does it copy the whole vector?
            auto neighbors = m_get_neighbors(x);
            std::vector<double> v(neighbors.size(), 0.0);
            std::transform(neighbors.begin(), neighbors.end(), v.begin(), [this](coord_type const& z) { return 1.0/(this->m_friction(z));} );
            double sum = std::accumulate(v.begin(), v.end(), 0.0);
            return 1.0/(m_friction(y)*sum);
          }
        }; // end neighboring_migration
        ///
        /// @brief Helper function to create a dispersal kernel compatible with \ref History specialized for the mass-based policy, but that restricts migration to the vecinity of each deme.
        ///
        /// @details For more flexibility, vecinity is defined by an external callable (does not have to be only the four nearest neighbours).
        ///          This implementation should be chosen when the scale of the migration is much less than the scale of a deme.
        ///
        /// @tparam T Coordinates type
        /// @tparam U functor used to describe friction of a deme. Its signature should be equivalent to `double f(T const& x)`
        /// @tparam V functor giving the neighbours of a deme. Signature should be equivalent to `std::vector<T> f(T const& x)`
        ///
        template<typename T, typename U, typename V>
        static neighboring_migration<T, U, V> make_neighboring_migration(T, double emigrant_rate, U friction, V get_neighbors)
        {
          return neighboring_migration<T, U, V>(emigrant_rate, friction, get_neighbors);
        }
      }; // end class mass_based
    } // end namespace dispersal_policy
  } // end namespace demography
} // end namespace quetzal

#endif
