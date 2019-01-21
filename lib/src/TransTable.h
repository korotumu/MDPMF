/*
 * TransArray.h
 *
 *  Created on: Feb 18, 2013
 *      Author: Michelle LaMar
 *
 *      Copyright (C) 2013, 2014  Michelle M. LaMar

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

#ifndef TRANSARRAY_H_
#define TRANSARRAY_H_

#include "Transition.h"
#include "GlobalDefs.h"
#include <vector>

namespace MDPLIB {

/*! TransArray
 * \brief Discrete transition function using a 3 dimensional array lookup.
 */
class TransTable: public MDPLIB::Transition {
public:
	TransTable(StateSpace *ss, ActionSet *as);
	TransTable(const TransTable& origTT);
	float transitionProb(Action* action, State* s1, State* s2);
	float transitionProb(int aId, int s1Id, int s2Id);
	int nonZeroTransProbs(vector<int> &retVect, int aId, int s1Id);

	int selectNextState(int aId, int s1Id);
	State *selectNextState(Action *action, State *s1);
	int setActionProbs(State *s1, State *s2, vector<float> probs);
	int setResultStateProbs(State *s1, Action *a, vector<float> probs);
private:
	int numActions;
	int numStates;
	vector< vector< vector<float> > > probTable;
};

} /* namespace MDP */
#endif /* TRANSARRAY_H_ */
