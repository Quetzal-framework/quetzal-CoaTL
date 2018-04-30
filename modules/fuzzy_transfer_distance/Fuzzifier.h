// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr> 

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/ 

#ifndef __FUZZIFIER_H_INCLUDED__
#define __FUZZIFIER_H_INCLUDED__

#include "FuzzyPartition.h"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "assert.h"

namespace fuzzy_transfer_distance {

// Transform a collection to a fuzzy Partition : various elements can belong to various clusters
class Fuzzifier
{

public:

	template<typename Element, typename Cluster, typename Hash, typename KeyEqual, typename Alloc>
	static constexpr 
	FuzzyPartition<Element> fuzzifie(std::unordered_map<Element, std::vector<Cluster>, Hash, KeyEqual, Alloc> const& sample);

	template<typename Forest, typename CellDemeAccessor>
	static constexpr FuzzyPartition<typename Forest::position_type> fuzzifie(Forest const& forest, CellDemeAccessor const& accessor);

private:
	template<typename Category>
	using MapCounter = std::unordered_map<Category, unsigned int>;

	template<typename Cluster>
	static constexpr MapCounter<Cluster> count_occurences(std::vector<Cluster> const& v);

};


template<typename Element, typename Cluster, typename Hash, typename KeyEqual, typename Alloc>
constexpr FuzzyPartition<Element> Fuzzifier::fuzzifie(std::unordered_map<Element, std::vector<Cluster>, Hash, KeyEqual, Alloc> const& sample) {
	// Pre-conditions
	assert(sample.size() != 0);
	assert(std::all_of(sample.cbegin(), sample.cend(), []( std::pair<Element, std::vector<Cluster>> const& p ){ return p.second.size() != 0;}));

	std::unordered_map<Element, std::unordered_map<Cluster, double>> coeffs;
	for(auto const& it1 : sample){
		for (auto const& it2 : count_occurences(it1.second)) {
			coeffs[it1.first][it2.first] = static_cast<double>(it2.second)/static_cast<double>(it1.second.size());
		}
	}

	// Post-conditions
	assert(coeffs.size() != 0);
	assert(std::all_of(coeffs.cbegin(), coeffs.cend(), []( auto const& p ){ return p.second.size() != 0;}));

	return FuzzyPartition<Element>(coeffs);
}

template<typename Forest, typename CellDemeAccessor>
constexpr FuzzyPartition<typename Forest::position_type> Fuzzifier::fuzzifie(Forest const& forest, CellDemeAccessor const& f){
	
	assert(forest.nb_trees() >= 1);
	using Element = typename Forest::position_type; // each element is the deme at which leaves were sampled
	using Cluster = unsigned int; // each tree is a cluster
	auto const& cref  = f;
	std::unordered_map<Element, std::vector<Cluster>> sample;
	unsigned int g = 0;
	for(auto const& it : forest){
		it.second.access_leaves_by_DFS( [&sample, &g, cref](auto const& cell){ sample[cref(cell)].push_back(g);});	
		++g;
	}

	return fuzzifie(sample);
}


template<typename Cluster>
constexpr Fuzzifier::MapCounter<Cluster> Fuzzifier::count_occurences(std::vector<Cluster> const& v) {
	MapCounter<Cluster> counts;
	for(auto const& it : v){
		counts[it] += 1;
	}
	return counts;
}

} // namespace fuzzy_transfer_distance

#endif

