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
	using quetzal::coalescence::Tree;
	using std::string;

	/* Topology :
	 *            a
	 *           / \
	 *          b   c
	 */

	Tree<string> a("a");

	assert( ! a.has_parent() );
	assert( ! a.has_children() );

	auto& b = a.add_child("b");

	assert( b.has_parent() );
	assert( a.has_children() );

	auto& c = a.add_child("c");

	assert(c.cell() == "c");

	c.cell() = "d";

	assert(c.cell() != "c");

	std::cout << "done" << std::endl;
	
	return 0;
}

//! [Example]
