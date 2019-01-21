/*
 * StateSpaceGrid.h
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

#ifndef STATESPACEGRID_H_
#define STATESPACEGRID_H_

#include "StateSpace.h"
#include "StateGrid.h"
#include <map>

namespace MDPLIB {

struct GridCompare {
	bool operator() (const charGrid& g1, const charGrid& g2) {
		int x,y;
		int gridWidth = g1.size();
		int gridHeight = g1[0].size();

		for(x=0; x < gridWidth; x++) {
			for(y=0; y < gridHeight; y++) {
				if(g1[x][y] < g2[x][y])
					return 1;
				else if(g1[x][y] > g2[x][y])
					return 0;
			}
		}
		return 0;
	}
};

class StateSpaceGrid: public MDPLIB::StateSpace {
public:
	StateSpaceGrid(int no, int w, int h);
	StateSpaceGrid(StateGrid* start);
	virtual ~StateSpaceGrid();

	int addState(vector< vector<char> > grid);
	StateGrid *getState(vector< vector<char> > grid);

	bool gridLess(vector<vector <char> > sg1, vector<vector <char> > sg2);
	bool gridStateLess(StateGrid *sg1, StateGrid *sg2);

	int getGridHeight() const {
		return gridHeight;
	}

	void setGridHeight(int gridHeight) {
		this->gridHeight = gridHeight;
	}

	int getGridWidth() const {
		return gridWidth;
	}

	void setGridWidth(int gridWidth) {
		this->gridWidth = gridWidth;
	}

	int getNumOpt() const {
		return numOpt;
	}

	void setNumOpt(int numOpt) {
		this->numOpt = numOpt;
	}

private:
	int gridWidth;
	int gridHeight;
	int numOpt;
	map<vector<vector <char> >, int, GridCompare> gridMap;
};


// For the map compare function:
//bool(*cmpfn_pt)(vector<vector <char> >,vector<vector <char> >) = StateSpaceGrid::gridLess;

} /* namespace MDPLIB */



#endif /* STATESPACEGRID_H_ */
