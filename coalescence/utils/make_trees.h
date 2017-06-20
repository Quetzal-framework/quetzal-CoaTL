// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __MAKE_TREES_H_INCLUDED__
#define __MAKE_TREES_H_INCLUDED__

#include "utils/Tree.h"
#include "utils/Forest.h"


namespace coalescence {

  /*!
   * \brief Build a vector of n tree nodes each encapsulating a default-constructed data.
   *
   * Usefull e.g. when k lineages have to be constructed from scratch.
   *
   * \tparam DataType    The type of data inside the Tree nodes. DefaultConstructible.
   *
   */
  template<typename DataType>
  std::vector<Tree<DataType>> make_trees(unsigned int n){
    std::vector<Tree<DataType>> v;
    for(unsigned int i = 0; i < n; ++i){
      v.emplace_back();
    }
    return v;
  }

  /*!
   * \brief Build a vector of tree nodes each encapsulating a given data.
   *
   * Usefull e.g. when k lineages have to be constructed from specific data.
   *
   * \tparam DataType    The type of data inside the Tree nodes.
   *
   */
  template<typename DataType>
  std::vector<Tree<DataType>> make_trees(std::vector<DataType> const& d){
    std::vector<Tree<DataType>> v;
    for(auto const& it : d){
      v.emplace_back(d);
    }
    return v;
  }

  /*!
   * \brief Build a forest of tree nodes each encapsulating a given data. Apparition deme can or not be remembered.
   *
   * Usefull e.g. when k lineages have to be constructed from geographic data
   * with a given NodeCreation policy found in tree_creation_policies.h.
   * The coalescence::policies::record_deme will record the coordinate where data where sampled
   * but DataType in this case should inherit from coalescence::policies::RecordableSpace.
   * The coalescence::policies::copy_data will do nothing else than copying the data into
   * the DataType fields (no further infomrations recorded).
   *
   * \tparam DataType       The type of data inside the Tree nodes.
   * \tparam Space          The type of space locations of the data.
   * \tparam NodeCreation   The creation policy for the nodes.
   *
   */
  template<typename Space, typename DataType, typename NodeCreation>
	Forest<Space, Tree<DataType>> make_trees(std::unordered_map<Space, std::vector<DataType>> const& d) const {
		Forest<Space, Tree<DataType>> forest;
		for(auto const& it1 : data){
			for(auto const& it2 : it1.second){
				forest.insert(it1.first, NodeCreation::make_tree(it1.first, it2));
			}
		}
		return forest;
	}

  /*!
   * \brief Build a forest of tree nodes each encapsulating a given data. Apparition time and apparition deme can or not be recorded.
   *
   * Usefull e.g. when k lineages have to be constructed from geographic data
   * with a given NodeCreation policy found in tree_creation_policies.h.
   * The coalescence::policies::record_deme will record the coordinate where data where sampled
   * but DataType in this case should inherit from coalescence::policies::RecordableSpace.
   * The coalescence::policies::record_time will record the coordinate where data where sampled
   * but DataType in this case should inherit from coalescence::policies::RecordableTime.
   * The coalescence::policies::record_space_and_time will record the coordinate  and time where data where sampled
   * but DataType in this case should inherit both from coalescence::policies::RecordableTime
   * and coalescence::policies::RecordableSpace.
   * The coalescence::policies::copy_data will do nothing else than copying the data into
   * the DataType fields (no further infomrations recorded).
   *
   * \tparam DataType       The type of data inside the Tree nodes.
   * \tparam Space          The type of space locations of the data.
   * \tparam NodeCreation   The creation policy for the nodes.
   *
   */
  template<typename Space, typename Time, typename DataType, typename NodeCreation>
	Forest<Space, Tree<DataType>> make_trees(Time const& t, std::unordered_map<Space, std::vector<DataType>> const& d) const {
		Forest<Space, Tree<DataType>> forest;
		for(auto const& it1 : data){
			for(auto const& it2 : it1.second){
				forest.insert(it1.first, NodeCreation::make_tree(it1.first, t, it2));
			}
		}
		return forest;
	}

} // namespace coalescence

#endif
