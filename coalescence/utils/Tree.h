// Copyright 2016 Arnaud Becheler <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <vector>

namespace quetzal{
namespace coalescence{
namespace utils {

/*!
 * \brief Defines a class for implementating a N-ary tree with each node containing a data field.
 * \tparam CellT Cell type, ie the value of the node. The requirements that are imposed on CellT
 *               depend on the actual operations performed on Tree<CellT>.
 */

// Inspired from code publicated by Pyramidev here : http://www.developpez.net/forums/d1623113/c-cpp/cpp/debuter/loi-demeter/#post8894717

template<class CellT>
class Tree
{
private:
	typedef Tree<CellT> SelfType;

	SelfType*             m_parent;
	CellT                 m_cell;
	std::vector<SelfType> m_children;

public:

	//! \remark CellT must be default constructible
	Tree();

	//! \remark CellT must be copy constructible.
	Tree(const CellT& cell);

	//! \remark CellT must be move constructible.
	Tree(CellT&& cell) noexcept;

	//! \remark CellT must be copy constructible.
	//! \todo enable copy construction if needed, for now delete it for efficiency sake
	Tree(const SelfType& other) = delete;

	//! \remark CellT must be move constructible.
	Tree(SelfType&& other) noexcept;

	//! \remark CellT must be move constructible.
	Tree(CellT&& cell, std::vector<SelfType>&& children) noexcept;

	//! \remark CellT must be copy constructible and move assignable.
	//! \todo enable copy assignment if needed, for now delete it for efficiency sake
	SelfType& operator=(const SelfType& other) = delete;

	//! \remark CellT must be move assignable.
	SelfType& operator=(SelfType&& other) noexcept;

 	//! \remark Read-only data access
 	const CellT& cell() const;

 	//! \remark Read-and-write data access
 	CellT& cell();

 	//! \remark Returns reference on the copied subtree
 	Tree<CellT>& add_child(const SelfType& subtree);

 	//! \remark Returns reference on the moved subtree
 	Tree<CellT>& add_child(SelfType&& subtree) noexcept;

 	//! \remark Returns reference on the emplaced new child with moved data
 	Tree<CellT>& add_child(CellT&& cell) noexcept;

 	//! Checks whether the tree has a parent
 	bool has_parent() const;

 	//! Checks whether the tree has one or more child
 	bool has_children() const;

 	//! \remark Treament must be callable with CellT argument
 	template<typename Treatment>
	void pre_order_DFS(Treatment const& fun) const ;

 	//! \remark Treament must be callable with CellT argument
	template<typename Treatment>
	void access_leaves_by_DFS(Treatment const& fun) const ;
};

template<typename CellT>
Tree<CellT>::Tree() :
	m_parent(nullptr), m_cell(), m_children()
{}

template<typename CellT>
Tree<CellT>::Tree(const CellT& cell) :
	m_parent(nullptr), m_cell(cell), m_children()
{}

template<typename CellT>
Tree<CellT>::Tree(CellT&& cell) noexcept :
	m_parent(nullptr), m_cell(std::move(cell)), m_children()
{}

/*
 * \remark implementation of deleted Copy constructor
template<class CellT>
Tree<CellT>::Tree(const SelfType& other) :
	m_parent(nullptr), m_cell(other.m_cell), m_children(other.m_children)
{
	for(Tree<CellT>& child : m_children)
		child.m_parent = this;
}
 */

template<class CellT>
Tree<CellT>::Tree(SelfType&& other) noexcept :
	m_parent(nullptr), m_cell(std::move(other.m_cell)), m_children(std::move(other.m_children))
{
	for(Tree<CellT>& child : m_children)
		child.m_parent = this;
}

template<class CellT>
Tree<CellT>::Tree(CellT&& cell, std::vector<SelfType>&& children) noexcept :
	m_parent(nullptr), m_cell(std::move(cell)), m_children(std::move(children))
{
	for(Tree<CellT>& child : m_children)
		child.m_parent = this;
}

/*
 * \remark implementation of deleted copy assignment
template<class CellT>
Tree<CellT>& Tree<CellT>::operator=(const SelfType& other)
{
	Tree<CellT> otherCopy(other);
	*this = std::move(otherCopy);
	return *this;
}
 */

template<class CellT>
Tree<CellT>& Tree<CellT>::operator=(SelfType&& other) noexcept
{
	m_cell     = std::move(other.m_cell);
	m_children = std::move(other.m_children);
	for(Tree<CellT>& child : m_children)
		child.m_parent = this;
	return *this;
}

template<class CellT>
const CellT& Tree<CellT>::cell() const
{
	return m_cell;
}

template<class CellT>
CellT& Tree<CellT>::cell()
{
 	return m_cell;
}

template<class CellT>
Tree<CellT>& Tree<CellT>::add_child(const SelfType& subtree)
{
	Tree<CellT> otherCopy(subtree);
	otherCopy.m_parent = this;
	m_children.push_back(std::move(otherCopy));
	return *this;
}

template<class CellT>
Tree<CellT>& Tree<CellT>::add_child(SelfType&& subtree) noexcept
{
	subtree.m_parent = this;
	m_children.push_back(std::move(subtree));
	return *this;
}

template<class CellT>
Tree<CellT>& Tree<CellT>::add_child(CellT&& cell) noexcept{
	Tree<CellT> subtree(std::move(cell));
	subtree.m_parent = this;
	m_children.push_back(std::move(subtree));
	return m_children.back();
}


template<class CellT>
bool Tree<CellT>::has_parent() const {
	return !m_parent;
}

template<class CellT>
bool Tree<CellT>::has_children() const {
	return !m_children.empty();
}

template<class CellT>
template<class Treatment>
void Tree<CellT>::pre_order_DFS(Treatment const& fun) const {
	fun(this->cell());
	for(auto const& child : this->m_children){
		child.pre_order_DFS(fun);
	}
}

template<class CellT>
template<class Treatment>
void Tree<CellT>::access_leaves_by_DFS(Treatment const& fun) const {
	if(has_children()){
		for(auto const& child : this->m_children){
			child.access_leaves_by_DFS(fun);
		}
	}else{
		fun(this->cell());
	}
}

} // namespace utils
} // namespace coalescence
} // namespace quetzal

#endif
