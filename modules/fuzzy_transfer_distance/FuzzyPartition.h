// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef FUZZY_PARTITION_H
#define FUZZY_PARTITION_H

#include <numeric> // accumulate
#include <unordered_set>
#include <unordered_map>
#include "assert.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "zip_range.h"

#include "matrix.h"
#include "munkres.h"
#include "RestrictedGrowthString.h"

template<typename T>
bool operator==(const boost::numeric::ublas::matrix<T>& m, const boost::numeric::ublas::matrix<T>& n);

template<typename E> class FuzzyPartition;

template<typename E>
std::ostream& operator<<(std::ostream& os, const FuzzyPartition<E>& fuzzy_partition);

//! \brief Clusters are implicitly defined because of cluster merging (what would make unique IDs complicated to use)

/*!
 * \brief Template class usefull for formalizing that elements of a set can simultaneously belong to various clusters.
 *
 * Fuzzy partition are objects of interest when one wants to represent the fact that an element of a set can belong
 * to several clusters. This can be represented by a matrix M(i,j), with clusters in line and elements in column, where
 * the general term mu(i,j) represent the coefficient of membership of an element to a cluster. Coefficients memberships
 * summed on all clusters are equal to 1.
 * \warning In the present implementation, Clusters IDs are not memorized, because it would
 * complicate fuzzy partition operations as merging clusters. So clusters are stored in the order given by their
 * identifiers at construction, but the IDs themselves are not stored.
 *
 * \tparam Elements 	Elements identifiers.
 *
 */
template<typename Element>
class FuzzyPartition
{
	using matrix_type = boost::numeric::ublas::matrix<double>;
	using vector_type = boost::numeric::ublas::vector<double>;

public:

	using element_type = Element;

	/*!
	 * \brief     Construct the fuzz partition with given coefficients
	 * \warning   All vector have to be of same size.
	 * \warning   The vectors index gives the cluster index.
	 */
	FuzzyPartition(std::unordered_map<element_type, std::vector<double> > const& coefficients);

	//! \brief Constructor
	template<typename Cluster>
	FuzzyPartition(std::unordered_map<element_type, std::unordered_map<Cluster, double>> const& coefficients);

	/*! \brief Compute the fuzzy transfer distance between two fuzzy partition
	 *  \brief Partitons must have same elements number, but number of clusters can differe.
	 */
	double fuzzy_transfer_distance(FuzzyPartition const& other) const;

	//! \brief Returns the number of elements (columns) of the fuzzy partition (matrix)
	size_t nElements() const;

	//! \brief Returns the number of clusters (lines) of the fuzzy partition (matrix)
	size_t nClusters() const;

	//! \brief Return the elements of the fuzzy partition.
	const std::unordered_set<element_type>& elements() const;

	//! \brief Return the clusters indices
	std::unordered_set<int> clusters() const;

	//! \brief Modifies the object, merging clusters (summing lines) according to a restricted growth string object.
	FuzzyPartition & merge_clusters(RestrictedGrowthString const& rgs);

	//! \brief Comparison operator, returning true if two partitions have equal elements set and membership coefficent, else returns false.
	bool operator==(FuzzyPartition<element_type> const& other) const;


private:


	Matrix<double> weighted_bipartite_graph(FuzzyPartition const& other) const {
		using namespace boost::numeric::ublas;
		Matrix<double> costs(this->nClusters(), other.nClusters());

		for(unsigned int i = 0; i < this->nClusters(); ++i){
			matrix_row< const matrix<double> > v(m_membership_coefficients, i);

			for(unsigned int j = 0; j < other.nClusters(); ++j){
				matrix_row< const matrix<double> > w(other.m_membership_coefficients, j);
				costs(i,j) = compute_cost(v,w);
			}

		}
		return costs;
	}


	template<typename V, typename W>
	double compute_cost(V const& v, W const& w) const {
		assert(v.size() == w.size());

		std::vector<double> distances;

		for(auto && t : zip_range(v, w) ) {
			distances.push_back(std::abs( boost::get<0>(t) - boost::get<1>(t) ));
		}

		return std::accumulate(distances.begin(), distances.end(), 0.0);
	}


	template<typename T>
	Matrix<T> hungarian_algorithm_resolution(Matrix<T> costs) const {
		Munkres<T> solver;
		solver.solve(costs);
		return(costs);
	}


	template<typename T>
	double compute_solution_cost(Matrix<T> const& solution, Matrix<T> const& costs) const {
		double total_cost = 0.0;
		for (unsigned int i = 0 ; i < solution.rows(); ++i ) {
			int row_matches = 0;
			for ( unsigned int j = 0 ; j < solution.columns() ; ++j ) {
				if ( solution(i,j) == 0 ){
					++row_matches;
					total_cost += costs(i,j);
				}
			}
			assert(row_matches == 0 || row_matches == 1);
		}
		return total_cost;
	}


	matrix_type build_coefficients_matrix(std::unordered_set<element_type> const& elements, std::vector<std::vector<double>> const& coefficients){
		assert(coefficients.size() == elements.size());

		unsigned int n_clusters = coefficients.front().size();

		for(auto const& it : coefficients) {
			assert(it.size() == n_clusters);
		}

		matrix_type m( n_clusters, elements.size(), 0);
		for(unsigned int j = 0; j < elements.size(); ++j){
			for(unsigned int i = 0; i < n_clusters; ++i ){
				m(i,j) = coefficients.at(j).at(i);
			}
		}
		return m;
	}

	std::unordered_set<element_type> retrieve_elements(std::unordered_map<element_type, std::vector<double> > const& coefficients) const {
		std::unordered_set<element_type> elems;
		for(auto const& it : coefficients){
			elems.insert(it.first);
		}
		return elems;
	}

	matrix_type build_coefficients_matrix(std::unordered_map<element_type, std::vector<double> > const& coefficients){

		unsigned int n_rows = coefficients.begin()->second.size();
		unsigned int n_cols = coefficients.size();

		for(auto const& it : coefficients) {
			assert(it.second.size() == n_rows);
		}

		matrix_type m( n_rows, n_cols, 0);

		unsigned int i = 0;
		unsigned int j = 0;

		for(auto const& it1 : coefficients){
			for(auto const& it2 : it1.second) {
				m(i,j) = it2;
				++i;
			}
			i = 0;
			++j;
		}
		return m;
	}

	template<typename Cluster>
	auto retrieve_elements(std::unordered_map<element_type, std::unordered_map<Cluster, double>> const& coefficients) const {
		std::unordered_set<element_type> elements;
		for(auto const& it : coefficients){
			elements.insert(it.first);
		}
		return(elements);
	}

	template<typename Cluster>
	matrix_type build_coefficients_matrix(std::unordered_map<element_type, std::unordered_map<Cluster, double>> const& coefs){
		assert(coefs.size() != 0);
		assert(std::all_of(coefs.cbegin(), coefs.cend(), []( auto const& p ){ return p.second.size() != 0;}));

		unsigned int n_cols = coefs.size();

		std::set<Cluster> s;
		for(auto const& it1 : coefs){
			for(auto const& it2 : it1.second){
				s.insert(it2.first);
			}
		}

		unsigned int n_rows = s.size();
		matrix_type m( n_rows, n_cols, 0.);

		unsigned int i = 0;
		unsigned int j = 0;

		for(auto const& it1 : coefs){
			for(auto const& it2 : it1.second) {
				m(i,j) = it2.second;
				++i;
			}
			i = 0;
			++j;
		}
		return m;
	}

	void check_coefficients_validty(){
		double sum = 0.;
		for(unsigned int j = 0; j < m_membership_coefficients.size2(); ++j){
			for(unsigned int i = 0; i < m_membership_coefficients.size1(); ++i){
				sum += m_membership_coefficients(i,j);
			}
			//assert(std::fabs(sum - 1) < 1E-3);
			sum = 0;
		}
	}

	std::unordered_set<element_type> m_elements;
	matrix_type m_membership_coefficients;

	friend std::ostream& operator<< <> (std::ostream&, FuzzyPartition<element_type> const&);

};






template <typename Element>
FuzzyPartition<Element>::FuzzyPartition(std::unordered_map<element_type, std::vector<double> > const& coefficients) :
m_elements(retrieve_elements(coefficients)),
m_membership_coefficients(build_coefficients_matrix(coefficients))
{
	assert(m_membership_coefficients.size2() == m_elements.size());
	assert(m_membership_coefficients.size1() == coefficients.cbegin()->second.size());
	check_coefficients_validty();
}

template<typename Element>
template<typename Cluster>
FuzzyPartition<Element>::FuzzyPartition(std::unordered_map<element_type, std::unordered_map<Cluster, double>> const& coefficients) :
m_elements(retrieve_elements(coefficients)),
m_membership_coefficients(build_coefficients_matrix(coefficients))
{
	assert(coefficients.size() != 0);
	assert(std::all_of(coefficients.cbegin(), coefficients.cend(), []( auto const& p ){ return p.second.size() != 0;}));
	check_coefficients_validty();
}

template<typename Element>
double FuzzyPartition<Element>::fuzzy_transfer_distance(FuzzyPartition const& other) const {
	assert(this->nElements() == other.nElements());
	auto costs = this->weighted_bipartite_graph(other);
	auto best_assignment = hungarian_algorithm_resolution(costs);
	return (compute_solution_cost(best_assignment, costs));
}

template<typename Element>
size_t FuzzyPartition<Element>::nElements() const {
	return m_elements.size();
}

template<typename Element>
size_t FuzzyPartition<Element>::nClusters() const {
	return m_membership_coefficients.size1();
}

template<typename Element>
const std::unordered_set<Element>& FuzzyPartition<Element>::elements() const {
	return m_elements;
}

template<typename Element>
std::unordered_set<int> FuzzyPartition<Element>::clusters() const {
	std::unordered_set<int> clusters_set;
	for(unsigned int i = 0; i < m_membership_coefficients.size1(); ++i){
		clusters_set.insert(i);
	}
	return clusters_set;
}

template<typename Element>
FuzzyPartition<Element> & FuzzyPartition<Element>::merge_clusters(RestrictedGrowthString const& rgs){
	assert( rgs.size() == this->nClusters() );
	matrix_type new_coefficients(rgs.nBlocks(), this->nElements(), 0);
	for(unsigned int i = 0; i < m_membership_coefficients.size1(); ++i){
		for(unsigned int j = 0; j < m_membership_coefficients.size2(); ++j){
			new_coefficients(rgs.at(i), j) += m_membership_coefficients(i,j);
		}
	}
	m_membership_coefficients.swap(new_coefficients);
	check_coefficients_validty();
	return *this;
}

template<typename Element>
bool FuzzyPartition<Element>::operator==(FuzzyPartition<element_type> const& other) const {
	return (this->m_elements == other.m_elements && this->m_membership_coefficients == other.m_membership_coefficients);
}

template<typename E>
std::ostream& operator<<(std::ostream& os, const FuzzyPartition<E>& fuzzy_partition)
{
    os << fuzzy_partition.m_membership_coefficients;
    return os;
}

template<typename T>
bool operator==(const boost::numeric::ublas::matrix<T>& m, const boost::numeric::ublas::matrix<T>& n)
{
  bool returnValue =
    (m.size1() == n.size1()) &&
    (m.size2() == n.size2());

  if (returnValue)
  {
    for (unsigned int i = 0; returnValue && i < m.size1(); ++i)
    {
      for (unsigned int j = 0; returnValue && j < m.size2(); ++j)
      {
        returnValue &= m(i,j) == n(i,j);
      }
    }
  }
  return returnValue;
}

#endif
