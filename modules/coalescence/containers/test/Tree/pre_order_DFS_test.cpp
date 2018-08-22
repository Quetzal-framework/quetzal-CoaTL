// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#include "../../Tree.h"

//! [Example]

#include <string>
#include "assert.h"
#include <iostream>

int main(){
	using quetzal::coalescence::Tree;
	using std::string;

	/* Topology :
	 *             a
	 *           /\ \
	 *          b  d e
	 *         /      \\
	 *        c       f g
	 */

	Tree<string> root("a");
	root.add_child("b").add_child("c");
	root.add_child("d");
	auto& e = root.add_child("e");
	e.add_child("f");
	e.add_child("g");

	std::vector<string> expected = {"a","b","c","d","e","f","g"};

	std::vector<string> v;
	auto f = [&v](string s){ v.push_back(s); };
	root.visit_cells_by_pre_order_DFS(f);

	assert(v == expected);
	std::cout << "done" << std::endl;

	return 0;
}

//! [Example]
