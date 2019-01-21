/*
 * StateSpaceGrid.cpp
 *
 *  Created on: Jan 8, 2014
 *      Author: Michelle LaMar
 *
 *      Copyright (C) 2014  Michelle M. LaMar

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

#include "StateSpaceGrid.h"

namespace MDPLIB {

StateSpaceGrid::StateSpaceGrid(int no, int w, int h)
{
	numOpt = no;
	gridWidth = w;
	gridHeight = h;
}


StateSpaceGrid::StateSpaceGrid(StateGrid *start)
{
	// The start state determines the grid parameters.  All other states must comply.
	numOpt = start->getNumOpts();
	gridWidth = (int)start->getGrid()->size();
	gridHeight = (int)start->getGrid()->at(0).size();

	stateList.push_back(start);
	start->setId(0);
	gridMap[*(start->getGrid())] = 0;
	numStates = 1;
}
StateSpaceGrid::~StateSpaceGrid() {
	// TODO Auto-generated destructor stub
}

int MDPLIB::StateSpaceGrid::addState(vector<vector<char> > grid) {
	if(grid.size() != (unsigned int)gridWidth|| grid[0].size() != (unsigned int)gridHeight)
		return MDP_ERROR;

	StateGrid *newState = new StateGrid(numStates, numOpt, grid);
	stateList.push_back(newState);
	numStates++;

	// Add to set for fast lookup:
	gridMap[*(newState->getGrid())] = newState->getId();

	return newState->getId();
}

StateGrid* MDPLIB::StateSpaceGrid::getState(vector<vector<char> > grid) {
	int sgId = gridMap[grid];

	if(sgId > 0 && sgId < numStates)
		return((StateGrid *)stateList[sgId]);
	else
		return((StateGrid *)0);
}
/*
bool MDPLIB::StateSpaceGrid::gridStateLess(StateGrid* sg1, StateGrid* sg2) {

	vector<vector <char> > *g1 = sg1->getGrid();
	vector<vector <char> > *g2 = sg2->getGrid();

	return(gridLess((*g1),(*g2)));
}
*/


} /* namespace MDPLIB */
