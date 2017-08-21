// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o Coords_test Coords_test.cpp -std=c++1y -Wall

#include <assert.h>

#include "GeographicCoordinates.h"

int main()
{
	using Coords = quetzal::geography::GeographicCoordinates;
	/**********************
	/ Construction
	**********************/
	Coords c1(0. , 1.);
	Coords c2(0. , 2.);
	Coords c3(-1. , 0.);
	Coords c4(0. , 1.);
	Coords c5(-1. , 5.);


	/**********************
	/ Getters
	**********************/
	assert(c1.lat() == 0.);
	assert(c1.lon() == 1.);

	/**********************
	/ Comparison operators
	**********************/
	assert(c1 == c4);
	assert( !(c1 == c2));

	assert(c1 < c2);
	assert(c1 < c2);
	assert(c1 < c2);
	assert(c1 < c2);
	assert( !(c1 < c3));
	assert( c5 < c1);

	/**********************
	/ Distance computation
	**********************/
	Coords paris(48.856614, 2.3522219000000177);
	Coords moscow(55.7522200, 37.6155600 );

	Coords::km computed = paris.distance_to(moscow);
	Coords::km expected = 2486.22;

	Coords::km EPSILON = 1.; // a 1-km error is acceptable.
	Coords::km diff = expected - computed;
	assert((diff < EPSILON) && (-diff < EPSILON));

	Coords a(44.13, 0.35);
	Coords b(50.3333, -3.33333);
	a.distance_to(b);

    return 0;
}
