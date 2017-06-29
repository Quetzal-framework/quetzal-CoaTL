// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

/**
 * \file Tree_test.cpp
 * \brief Program for unit testing
 * \author Arnaud Becheler <Arnaud.Becheler@egce.cnrs-gif.fr>
 *
 * Test program for interface of Tree class using basic parameters type for illustration.
 * Compiles with g++ -o test Tree_test.cpp -std=c++14 -Wall
 *
 */

#include "Tree.h"

#include <string>
#include "assert.h"
#include <iostream>
int main(){

	using quetzal::coalescence::Tree;
	using std::string;

	/**
	 * Initialization
	 */

	// initialization by default constructor
	Tree<int> first;
	Tree<int> second;
	// assignment by move assignment operator
	first = std::move(second);

	// initialization copying cell
	int a = 3;
	Tree<int> A(a);

	// initialization moving cell
	Tree<int> B(std::move(a));


	/**
	 * Topology edition
	 */

	/* Expected topology :
	 *            a
	 *           /\ \
	 *          b  d e
	 *         /      \\
	 *        c       f g
	 */

	// From root to leaves construction

	Tree<string> root;
	assert(root.cell() == "");

	root.cell() = "a";
	assert(root.cell() == "a");

	root.add_child("b").add_child("c");
	const auto & d = root.add_child("d");
	assert(d.has_parent());

	// From leaves to root construction
	Tree<string> f("f");
	Tree<string> g("g");

	std::vector<Tree<string>> children;

	children.push_back(std::move(f));
	children.push_back(std::move(g));

	Tree<string> e("e", std::move(children));

	// Rebranching subtree
	root.add_child(std::move(e));

	/*
	 * Using traversals
	 */

	// Depth First Search Algorithm
	std::vector<string> expected_data = {"a","b","c","d","e","f","g"} ;
	std::vector<string> v;

	auto treatment = [&v](string s){ v.push_back(s); };

	root.pre_order_DFS(treatment);

	assert(v == expected_data);
	v.clear();

	// Access leaves by DFS
	std::vector<string> expected_children_data = {"c","d","f","g"};

	root.access_leaves_by_DFS(treatment);

	assert(v == expected_children_data);

	return 0;
}
