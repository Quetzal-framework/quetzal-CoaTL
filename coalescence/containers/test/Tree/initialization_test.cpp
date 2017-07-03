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

#include "assert.h"
#include <iostream>

int main(){

	using quetzal::coalescence::Tree;
	using std::string;

	// initialization by default constructor
	Tree<int> a;
	Tree<int> b;
	Tree<int> c;

	// assignment by copy assignment operator
	a = b;

	// assignment by move assignment operator
	c = std::move(b); // b should not be used anymore !

	int cell = 12;

	// initialization copying cell
	Tree<int> d(cell);

	// initialization moving cell
	Tree<int> e(std::move(cell)); // cell should not be used anymore !

	std::cout << "done" << std::endl;
	return 0;
}

//! [Example]
