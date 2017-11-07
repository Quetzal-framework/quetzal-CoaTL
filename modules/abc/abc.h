// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __ABC_H_INCLUDED__
#define __ABC_H_INCLUDED__

#include "ReferenceTable.h"

#include <tuple>
#include <vector>
#include <type_traits> // result_of
#include <stdexcept> // std::domain_error

namespace quetzal {
namespace abc {

	template<typename T>
  struct identity {
      T operator()(T&& a) const { return std::move(a); }
			const T& operator()(T const& a) const { return a; }
  };


	/*!
	 * \brief ABC object for Approximate Bayesian Computations.
	 *
	 * Associates a generative (simulation) model with a possibly multidimensional
	 * prior distribution.
	 *
	 * Simulation models are abstracted to the concept of \ref abc "GenerativeModel".
	 * \tparam ModelType a type that meets the requirements of \ref abc "GenerativeModel".
	 * \tparam PriorType a type that meets the requirements of \ref abc "PriorDistribution"
	 * \remark ABC objects can be used with the helper function `make_ABC`
	 *
	 * \ingroup abc
	 * \section Example
	 * \snippet abc/test/ABC/test.cpp Example
	 * \section Output
	 * \include abc/test/ABC/test.output
	 */
	template<class ModelType, class PriorType>
	class ABC{

	public:

		//! \typedef model type used for simulating data
		using model_type = ModelType;

		//! \typedef model parameter type
		using param_type = typename model_type::param_type;

		//! \typedef simulated data type
		using simulated_data_type = typename model_type::return_type;

		//! \typedef prior distribution type for sampling model parameters
		using prior_type = PriorType;

	private:

		mutable prior_type priors;
		mutable model_type generative_model;

		//! \remark Represent a couple {parameter, generated data}
		template<class ParamType, class DataType>
		class PriorPredictiveDistributionSample{

		private:

			using param_type = ParamType;
			using data_type = DataType;

			param_type m_param;
			data_type m_data;

		public:
			PriorPredictiveDistributionSample(param_type const& p, data_type const& d) :
			m_param(p),
			m_data(d)
			{}

			PriorPredictiveDistributionSample(param_type && p, data_type&& d) noexcept :
			m_param(std::move(p)),
			m_data(std::move(d))
			{}

			PriorPredictiveDistributionSample(param_type const& p, data_type&& d) noexcept :
			m_param(p),
			m_data(std::move(d))
			{}

		const param_type & param() const {return m_param; }

		const data_type & data() const {return m_data; }

		}; // end class PriorPredictiveDistributionSample


	public:

	 /**
		 * \brief Constructor
		 * \param m the model to be used
		 * \param p the prior distribution
		 * \section Example
		 * \snippet abc/test/ABC/test.cpp Example
	 	 * \section Output
	 	 * \include abc/test/ABC/test.output
		 */
		ABC(model_type const& m, prior_type const& p)  :
		priors(p),
		generative_model(m)
		{}

		/**
			* \brief Rubin(1984) likelyhood-free rejection sampler
			*
			* Performs rejection assuming observed data takes values in a finite or countable set \f$D\f$
			* as defined in
			* [Approximate Bayesian Computations methods, Marin et al 2011](https://link.springer.com/article/10.1007%2Fs11222-011-9288-2?LI=true),
			* Algorithm 1.
			*
			* \param n the number of parameters to accept.
			* \param y the observed data from which to draw inference.
			* \return a vector of accepted parameter values.
			*
			* \remark ObservedDataType and simulated_data_type should be [EqualityComparable](http://en.cppreference.com/w/cpp/concept/EqualityComparable)
			*
			* \section Example
			* \snippet abc/test/ABC/test.cpp Example
			* \section Output
			* \include abc/test/ABC/test.output
			*/
		template<class ObservedDataType, typename Generator>
		std::vector<param_type> rubin_rejection_sampler(unsigned int n, ObservedDataType const& y, Generator& g) const {
			std::vector<param_type> sample;
			sample.reserve(n);
			while(sample.size() != n)
			{
				auto params = param_type(priors(g));
				if( y == generative_model(g, params))
				{
					sample.push_back(params);
				}
			}
			return sample;
		}

		//! \remark Pritchard & al (1999) likelyhood free rejection sampler.
		//! \remark Extension of Rubin sampler to the case of continuous sample spaces
		//! \remark see 'Approximate Bayesian Computations methods, Marin et al 2011, Algorithm 2)
		template<class ObsType, class RhoType, class EtaType, class T, class Generator>
		std::vector<param_type> pritchard_rejection_sampler(
			unsigned int N,
			ObsType const& y,
			RhoType const& rho,
			T const& epsilon,
			Generator& g,
			EtaType const& eta
		) const {

			std::vector<param_type> sample;
			sample.reserve(N);

			while(sample.size() != N)
			{
				auto params = param_type(priors(g));
				if( rho(eta(y), eta(generative_model(g, params))) <= epsilon )
				{
					sample.push_back(params);
				}
			}
			return sample;
		}


		//! \remark Pritchard & al (1999) likelyhood free rejection sampler.
		//! \remark Extension of Rubin sampler to the case of continuous sample spaces
		//! \remark see 'Approximate Bayesian Computations methods, Marin et al 2011, Algorithm 2)
		template<class ObsType, class RhoType, class T, class Generator>
		std::vector<param_type> pritchard_rejection_sampler(unsigned int N, ObsType const& y, RhoType const& rho,	T const& epsilon,	Generator& g) const {
			using simulated_data_type = typename std::result_of<ModelType(Generator&, const param_type&)>::type;
			using eta_type = Identity<simulated_data_type>;
			eta_type eta;
			return pritchard_rejection_sampler(N, y, rho, epsilon, g, eta);
		}

		//! \remark Generation of reference table from prior predictive distribution
		//! \remark see Algorithm 1 in ABC Random Forests for Bayesian Parameter Inference (Marin et al 2016)
		//! Returns of ReferenceTable
		template<class EtaType, typename Generator>
		auto sample_prior_predictive_distribution(unsigned int N, Generator& g, EtaType const& eta) const {

			using simul_type = typename std::result_of_t<ModelType(Generator&, const param_type&)>;
			using sumstat_type = typename std::result_of_t<EtaType(simul_type&&)>;

			ReferenceTable<PriorPredictiveDistributionSample, param_type, sumstat_type> table;
			unsigned int i = 0;
			while(i < N)
			{
				auto params = param_type(priors(g));
				try{
					table.emplace_back(params, eta(generative_model(g, params)));
					++i;
				}
				catch (std::domain_error& e){
					std::cerr << e.what() << std::endl;
				}
			}

			return table;
		}

		//! \remark Generation of reference table from prior predictive distribution
		//! \remark see Algorithm 1 in ABC Random Forests for Bayesian Parameter Inference (Marin et al 2016)
		template<typename Generator>
		auto sample_prior_predictive_distribution(unsigned int N, Generator& g) const {
			using simulated_data_type = typename std::result_of<ModelType(Generator&, const param_type&)>::type;
			using eta_type = Identity<simulated_data_type>;
			eta_type eta;
			return sample_prior_predictive_distribution(N, g, eta);
		}

	}; // end class ABC definition

	template<class ModelType, class ParametersJointDistributionType>
	auto make_ABC(ModelType const& model,	ParametersJointDistributionType const& prior)
	{
			return ABC<ModelType, ParametersJointDistributionType>(model, prior);
	}

} // namespace abc
} // namespace quetzal

#endif
