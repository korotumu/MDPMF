/*
 * StateGrid.cpp
 *
 *  Created on: Jan 8, 2014
 *      Author: Michelle LaMar
 *
 *      Copyright (C)  2014  Michelle M. LaMar

This library is free software; you can redistribute it and/or  
modify it under the terms of the GNU Lesser General Public 
License as published by the Free Software Foundation; either 
version 2.1 of the License, or (at your option) any later version. 

This library is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
Lesser General Public License for more details. 

You should have received a copy of the GNU Lesser General Public 
License along with this library; if not, see https://www.gnu.org/licenses/. 

mlamar@ets.org 
Educational Testing Service 
660 Rosedale Road 
Princeton, NJ 08541
 */

#include "StateGrid.h"

namespace MDPLIB {

StateGrid::StateGrid(int i, int no, vector<vector<char> > g)
	: State(i), grid(g.size(), vector<char>(g[0].size(),0))
{
	numOpts = no;
	unsigned int x,y;
//	maxHistory = 0;
//	numHistory = 0;
//	history = NULL;

	for(x=0; x < g.size(); x++) {
		for(y=0; y < g[0].size(); y++) {
			grid[x][y] = g[x][y];
		}
	}
}

StateGrid::~StateGrid() {
	// TODO Auto-generated destructor stub
}

vector< vector<char> > *StateGrid::getGrid() {
	return(&grid);
}

} /* namespace MDPLIB */
