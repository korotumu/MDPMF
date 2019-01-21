/*
 * StateGrid.h
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

#ifndef STATEGRID_H_
#define STATEGRID_H_

#include "State.h"

namespace MDPLIB {

typedef vector<vector<char> > charGrid;

class StateGrid: public MDPLIB::State {
public:
	StateGrid(int i, int numOpts, vector<vector<char> > g);
	virtual ~StateGrid();
	 charGrid*getGrid();

	int getNumOpts() const {
		return numOpts;
	}

private:
	int numOpts;
	charGrid grid;
//	int maxHistory;
//	int numHistory;
//	vector<int> *history;
};

} /* namespace MDPLIB */

#endif /* STATEGRID_H_ */
