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

namespace abc {

	template<typename T>
  struct Identity {
      T operator()(T&& a) const { return std::move(a); }
			const T& operator()(T const& a) const { return a; }
  };

	template<class ModelType, class ParametersJointDistributionType>
	class ABC{

	private:

		using model_type = ModelType;
		using param_type = typename model_type::param_type;
		using priors_type = ParametersJointDistributionType;

		mutable priors_type priors;
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

		ABC(model_type const& m, priors_type const& p)  :
		priors(p),
		generative_model(m)
		{}

		//! \remark Rubin(1984) likelyhood-free rejection sampler
		//! \remark assume observed data takes values in a finite or countable set D
		//! \remark ObservedDataType and simulated_data_type should be EqualityComparable
		//! \remark see 'Approximate Bayesian Computations methods, Marin et al 2011, Algorithm 1)
		template<class ObsType, typename Generator>
		std::vector<param_type> rubin_rejection_sampler(unsigned int N, ObsType const& observed_data, Generator& g) const {
			std::vector<param_type> sample;
			sample.reserve(N);
			while(sample.size() != N)
			{
				auto params = param_type(priors(g));
				if( observed_data == generative_model(g, params))
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
			using simulated_data_type = typename std::result_of<ModelType(Generator&, const param_type&)>::type;
			using summary_stat_type = typename std::result_of<EtaType(simulated_data_type&&)>::type;

			ReferenceTable<PriorPredictiveDistributionSample, param_type, summary_stat_type> table;

			for(unsigned int i = 0; i < N; ++i)
			{
				auto params = param_type(priors(g));
				table.emplace_back(params, eta(generative_model(g, params)));
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

#endif
