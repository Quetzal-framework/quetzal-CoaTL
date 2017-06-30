// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __FOREST_H_INCLUDED__
#define __FOREST_H_INCLUDED__

#include <unordered_map>
#include <set>
#include <vector>
#include "assert.h"

namespace quetzal{
namespace coalescence{

/*!
 * \ingroup coal_containers
 * \brief Defines a class for implementating a collection of geo-localized coalescents.
 * \tparam Position Type of the geographic coordinate of the associated tree.
 * \tparam Tree Type of the coalescence tree.
 */
template<typename Position, typename Tree>
class Forest{

private:

using SelfType = Forest<Position, Tree>;
using data_type = std::unordered_multimap<Position, Tree>;
using size_type = typename data_type::size_type;
data_type m_data;

public:
	using position_type = Position;
	using iterator = typename data_type::iterator;
	using const_iterator = typename data_type::const_iterator;

	//! \remark Tree must be default constructible
	Forest();

	//! \remark Tree must be copy constructible.
	Forest(const SelfType& other);

	//! \remark Tree must be move constructible.
	Forest(SelfType&& other) noexcept;

	//! \remark Tree must be copy constructible and move assignable.
	SelfType& operator=(const SelfType& other);

	//! \remark Tree must be move assignable.
	SelfType& operator=(SelfType&& other) noexcept;

	//! \remark Return iterator to beginning
	iterator begin();

	//! \remark Return iterator to end
	iterator end();

	//! \remark Return iterator to beginning
	const_iterator begin() const;

	//! \remark Return iterator to end
	const_iterator end() const;

	//! \remark Return const_iterator to beginning
	const_iterator cbegin() const;

	//! \remark Return const_iterator to end
	const_iterator cend() const;

	//! \remark Return the number of trees in the forest
	unsigned int nb_trees() const;

	//! \remark Return the number of trees in the forest at a given position
	unsigned int nb_trees(Position const& position);

	std::pair<const_iterator,const_iterator> trees_at_same_position (const Position& position) const;

	std::pair<iterator,iterator> trees_at_same_position (const Position& position);

	//! \remark Tree must be copy constructible.
	iterator insert(Position const& position, Tree const& tree);

	//! \remark Tree must be move constructible.
	iterator insert(Position const& position, Tree&& tree) noexcept;

	//! \remark Tree must be copy constructible.
	SelfType& insert(Position const& position, std::vector<Tree> const& trees);

	//! \remark Tree must be move constructible.
	SelfType& insert(Position const& position, std::vector<Tree>&& trees) noexcept;

	//! \remark return the number of deleted trees
	size_type erase(Position const& x);

	std::set<Position> positions() const;

};

template<typename Position, typename Tree>
Forest<Position, Tree>::Forest() :
	m_data()
{}

template<typename Position, typename Tree>
Forest<Position, Tree>::Forest(const SelfType& other) :
	m_data(other.m_data)
{}

template<typename Position, typename Tree>
Forest<Position, Tree>::Forest(SelfType&& other) noexcept :
	m_data(std::move(other.m_data))
{}

template<typename Position, typename Tree>
Forest<Position, Tree>& Forest<Position, Tree>::operator=(const SelfType& other)
{
	Forest<Position, Tree> otherCopy(other);
	*this = std::move(otherCopy);
	return *this;
}

template<typename Position, typename Tree>
Forest<Position, Tree>& Forest<Position, Tree>::operator=(SelfType&& other) noexcept
{
	m_data = std::move(other.m_data);
	return *this;
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::iterator Forest<Position, Tree>::begin(){
	return m_data.begin();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::iterator Forest<Position, Tree>::end(){
	return m_data.end();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::const_iterator Forest<Position, Tree>::begin() const {
	return m_data.begin();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::const_iterator Forest<Position, Tree>::end() const {
	return m_data.end();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::const_iterator Forest<Position, Tree>::cbegin() const {
	return m_data.cbegin();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::const_iterator Forest<Position, Tree>::cend() const {
	return m_data.cend();
}

template<typename Position, typename Tree>
unsigned int Forest<Position, Tree>::nb_trees() const {
	return m_data.size();
}

template<typename Position, typename Tree>
unsigned int Forest<Position, Tree>::nb_trees(Position const& position){
	return m_data.count(position);
}

template<typename Position, typename Tree>
std::pair<typename Forest<Position, Tree>::const_iterator, typename Forest<Position, Tree>::const_iterator> Forest<Position, Tree>::trees_at_same_position (const Position& position) const{
	assert(m_data.count(position) > 0 && "no trees at position" );
	return m_data.equal_range(position);
};

template<typename Position, typename Tree>
std::pair<typename Forest<Position, Tree>::iterator, typename Forest<Position, Tree>::iterator> Forest<Position, Tree>::trees_at_same_position (const Position& position){
	assert(m_data.count(position) > 0 && "no trees at position" );
	return m_data.equal_range(position);
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::iterator Forest<Position, Tree>::insert(Position const& position, Tree const& tree){
	return m_data.insert( std::pair<Position, Tree>(position, tree));
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::iterator Forest<Position, Tree>::insert(Position const& position, Tree&& tree) noexcept {
	return m_data.insert( std::pair<Position, Tree>(position, std::move(tree)));
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::size_type Forest<Position, Tree>::erase(Position const& x){
	return m_data.erase(x);
}

template<typename Position, typename Tree>
Forest<Position, Tree>& Forest<Position, Tree>::insert(Position const& position, std::vector<Tree> const& trees){
	for(auto const& it : trees){
		insert(position, it);
	}
	return *this;
}

template<typename Position, typename Tree>
Forest<Position, Tree>& Forest<Position, Tree>::insert(Position const& position, std::vector<Tree>&& trees) noexcept{
	for(auto&& it : trees){
		insert(position, std::move(it));
	}
	return *this;
}

template<typename Position, typename Tree>
std::set<Position> Forest<Position, Tree>::positions() const {
	std::set<Position> s;
	for(auto const& it : m_data){
		s.insert(it.first);
	}
	return s;
}

} // namespace coalescence
} // namespace quetzal

#endif
