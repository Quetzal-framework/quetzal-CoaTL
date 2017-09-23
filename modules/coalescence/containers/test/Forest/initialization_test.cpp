// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/


#include "../../Forest.h"

//! [Example]

#include <iostream>

int main(){

	using position_type = int;
	using tree_type =int;
	using forest_type = quetzal::coalescence::Forest<position_type, tree_type>;

	// initialization by default constructor
	forest_type a;

	// initialization by copy constructor
	forest_type b(a);

	// initialization by move constructor
	forest_type c(std::move(b)); // b should not be used again

	// Also initialization by copy constructor
	forest_type d = a;

	// assignment by copy assignment operator
	d = c;

	// assignment by move assignment operator
	d = std::move(c);

	std::cout << "done" << std::endl;
	
	return 0;
}

//! [Example]
