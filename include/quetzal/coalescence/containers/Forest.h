// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

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

namespace quetzal
{
	namespace coalescence
	{
		/*!
		* \ingroup coal_containers
		* \brief Collection of geo-localized coalescents.
		* \tparam Position Type of the geographic coordinate of the associated tree.
		* \tparam Tree Type of the coalescence tree.
		*/
		template<typename Position, typename Tree>
		class Forest
		{
		private:
			using SelfType = Forest<Position, Tree>;
			using data_type = std::unordered_multimap<Position, Tree>;
			using size_type = typename data_type::size_type;
			data_type m_data;
		public:
			using position_type = Position;
			using iterator = typename data_type::iterator;
			using const_iterator = typename data_type::const_iterator;
			/**
			* \brief Default constructor
			* \remark Position and Tree must be default constructible
			*/
			Forest(): m_data() {}
			/**
			* \brief Copy constructor
			* \param other the Forest to be constructed from
			* \remark Position and Tree must be copy constructible.
			*/
			Forest(const Forest<Position, Tree>& other):
			m_data(other.m_data)
			{}
			/**
			* \brief Move constructor
			* \param other the Forest to be constructed from
			* \remark Position and Tree must be copy constructible.
			*/
			Forest(Forest<Position, Tree>&& other) noexcept :
			m_data(std::move(other.m_data))
			{}
			/**
			* \brief Copy assignment operator
			* \remark Position and Tree must be copy constructible and move assignable.
			*/
			Forest<Position, Tree>& operator=(const Forest<Position, Tree>& other)
			{
				Forest<Position, Tree> otherCopy(other);
				*this = std::move(otherCopy);
				return *this;
			}
			/**
			* \brief Move assignment operator
			* \remark CellT must be move assignable.
			*/
			Forest<Position, Tree>& operator=(Forest<Position, Tree>&& other) noexcept
			{
				m_data = std::move(other.m_data);
				return *this;
			}
			/**
			* \brief returns iterator to the begin
			*/
			iterator begin()
			{
				return m_data.begin();
			}

			/**
			* \brief returns iterator to the end
			*/
			iterator end()
			{
				return m_data.end();
			}

			/**
			* \brief returns iterator to the begin
			*/
			const_iterator begin() const
			{
				return m_data.begin();
			}

			/**
			* \brief returns iterator to the begin
			*/
			const_iterator end() const
			{
				return m_data.end();
			}

			/**
			* \brief returns iterator to the begin
			*/
			const_iterator cbegin() const
			{
				return m_data.cbegin();
			}

			/**
			* \brief returns iterator to the begin
			*/
			const_iterator cend() const
			{
				return m_data.cend();
			}
			/**
			* \brief number of trees in the forest
			*/
			unsigned int nb_trees() const
			{
				return m_data.size();
			}
			/**
			* \brief number of trees in the forest at a given position
			*/
			unsigned int nb_trees(Position const& position) const
			{
				return m_data.count(position);
			}
			/**
			* \brief non-modifying access to trees in the forest at a given position
			* \return a range [first, last) given by a pair of const iterators. The second member
			of the pair is the past-the-end iterator
			*/
			std::pair<const_iterator,const_iterator> trees_at_same_position (const Position& position) const
			{
				assert(m_data.count(position) > 0 && "no trees at position" );
				return m_data.equal_range(position);
			}

			/**
			* \brief access to trees in the forest at a given position
			* \return a range [first, last) given by a pair of iterators. The second member
			of the pair is the past-the-end iterator
			*/
			std::pair<iterator,iterator> trees_at_same_position (const Position& position)
			{
				assert(m_data.count(position) > 0 && "no trees at position" );
				return m_data.equal_range(position);
			}

			/**
			* \brief insert a new tree at a given position
			* \return an iterator to the inserted tree
			* \remark Position and Tree should be copy constructible.
			*/
			iterator insert(Position const& position, Tree const& tree)
			{
				return m_data.insert( std::pair<Position, Tree>(position, tree));
			}
			/**
			* \brief insert a new tree at a given position
			* \return an iterator to the inserted tree
			* \remark Position and Tree should be move constructible.
			*/
			iterator insert(Position const& position, Tree&& tree) noexcept
			{
				return m_data.insert( std::pair<Position, Tree>(position, std::move(tree)));
			}
			/**
			* \brief insert a vector of trees at a given position
			* \return a reference to the Forest
			* \remark Position and Tree should be copy constructible.
			*/
			Forest<Position, Tree>& insert(Position const& position, std::vector<Tree> const& trees)
			{
				for(auto const& it : trees)
				{
					insert(position, it);
				}
				return *this;
			}
			/**
			* \brief insert a vector of trees at a given position
			* \return a reference to the Forest
			* \remark Position should be copy constructible.
			*/
			Forest<Position, Tree>& insert(Position const& position, std::vector<Tree>&& trees) noexcept
			{
				for(auto&& it : trees)
				{
					insert(position, std::move(it));
				}
				return *this;
			}
			/**
			* \brief erase a position from the Forest.
			* \return the number of deleted trees.
			*/
			size_type erase(Position const& x)
			{
				return m_data.erase(x);
			}
			/**
			* \brief remove elements from the Forest.
			* \return the number of deleted trees.
			*/
			void erase(iterator first, iterator last)
			{
				m_data.erase(first,last);
			}
			/**
			* \brief positions in the forest
			* \return a set of coordinates giving the spatial occupancy of the forest.
			*/
			std::set<Position> positions() const
			{
				std::set<Position> s;
				for(auto const& it : m_data)
				{
					s.insert(it.first);
				}
				return s;
			}
			/**
			* \brief copies all trees in the forest
			* \return a vector of trees.
			*/
			std::vector<Tree> get_all_trees() const
			{
				std::vector<Tree> trees;
				trees.reserve(this->nb_trees());
				for(auto & it : *this)
				{
					trees.push_back(it.second);
				}
				return trees;
			}
		}; // class Forest
	} // namespace coalescence
} // namespace quetzal
#endif
