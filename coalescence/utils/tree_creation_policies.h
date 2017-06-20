// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr> Florence Jornod <florence@jornod.com>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __TREE_CREATION_POLICIES_H_INCLUDED__
#define __TREE_CREATION_POLICIES_H_INCLUDED__

#include "utils/Tree.h"
#include <type_traits>

namespace coalescence {
namespace policies{

/*!
 * \brief Defines various policies classes to which the specific details of tree creation during coalescence process are delegated
 *
 * When spatial coalescence algorithms are about to create a new coalescent node (when creating the leaves
 * from genetic data or when coalescence events happen), local informations can be registered in the internal tree
 * state for further retrieval, like the apparition time or the deme where the coalescence happened. According to the chosen
 * policy, time, space or both can be recorded. This allows to speed up algorithm execution when further retrieval of coalescence
 * time or space distributions are not needed. Tree internal data (Tree::CellT) has to inherit from corresponding base classes.
 *
 */

//! \remark Tree::CellT has to inherit from RecordableTime so record_time policy can be used
template<typename Time>
class RecordableTime {
	public:
		using time_type = Time;
		const time_type & apparition_time() const {return m_apparition_time;}
		void set_apparition_time(Time const& t) { m_apparition_time = t; }
	private:
		time_type m_apparition_time;
};

//! \remark Tree::CellT has to inherit from RecordableSpace so record_deme policy can be used
template<typename Space>
class RecordableSpace {
	public:
		using coord_type = Space;
		const coord_type & apparition_deme() const {return m_apparition_deme;}
		void set_apparition_deme(coord_type const& x) { m_apparition_deme = x; }
	private:
		coord_type m_apparition_deme;
};


//! \remark Tree:CellT has to be copy constructible
struct copy_data{

	template<Data>
	static auto make_tree(Data const& data){
		return coalescence::utils::Tree<Data>(data);
	}

	//! \remark Used for time-explicit algorithms where record_time policy could be used instead
	template<typename Time, typename Data>
	static auto make_tree(Time const& t, Data const& data){
		return coalescence::utils::Tree<Data>(data);
	}

	//! \remark Used for space and time - explicit algorithm where record_time policy can be used instead
	template<typename Space, typename Time, typename Data>
	static auto make_tree(Space const& x, Time const& t, Data const& data){
		return coalescence::utils::Tree<Data>(data);
	}

};

//! \remark Tree::CellT has to inherit from RecordableTime
struct record_time{

	template<typename Time, typename Data>
	static auto make_tree(Space const& x, Time const& t, Data const& data){
		static_assert(std::is_base_of<RecordableTime<Time>, Data>::value, "Data should inherit from RecordableTime");
		coalescence::utils::Tree<Data> tree(data);
		tree.cell().set_apparition_time(t);
		return tree;
	}

	//! \remark Used for space-explicit algorithms where record_deme policy could be used instead
	template<typename Space, typename Time, typename Data>
	static auto make_tree(Space const& x, Time const& t, Data const& data){
		static_assert(std::is_base_of<RecordableTime<Time>, Data>::value, "Data should inherit from RecordableTime");
		coalescence::utils::Tree<Data> tree(data);
		tree.cell().set_apparition_time(t);
		return tree;
	}


};

//! \remark Tree::CellT has to inherit from RecordableDeme
struct record_deme{

	template<typename Space, typename Data>
	static auto make_tree(Space const& x, Data const& data) {
		static_assert(std::is_base_of<RecordableSpace<Space>, Data>::value, "Data should inherit from RecordableSpace");
		coalescence::utils::Tree<Data> tree(data);
		tree.cell().set_apparition_deme(x);
		return tree;
	}

	template<typename Space, typename Time,typename Data>
	static auto make_tree(Space const& x, Time const& t, Data const& data) {
		static_assert(std::is_base_of<RecordableSpace<Space>, Data>::value, "Data should inherit from RecordableSpace");
		coalescence::utils::Tree<Data> tree(data);
		tree.cell().set_apparition_deme(x);
		return tree;
	}

};

// \remark Tree::CellT has to inherit from both Recordable Time and RecordableDeme
struct record_time_and_deme{

	template<typename Space, typename Time, typename Data>
	static auto make_tree(Space const& x, Time const& t, Data const& data) {
		static_assert(std::is_base_of<RecordableTime<Time>, Data>::value &&
			  			std::is_base_of<RecordableSpace<Space>, Data>::value,
						"Data should inherit from RecordableTime and RecordableSpace");
		coalescence::utils::Tree<Data> tree(data);
		tree.cell().set_apparition_time(t);
		tree.cell().set_apparition_deme(x);
		return tree;
	}
};

} // namespace policies
} // namespace coalescence

#endif
