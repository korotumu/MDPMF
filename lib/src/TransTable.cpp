/*
 * TransTable.cpp
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

#include "TransTable.h"

namespace MDPLIB {


MDPLIB::TransTable::TransTable(StateSpace *ss, ActionSet *as) :
		Transition(ss, as), probTable(ss->getNumStates(), vector< vector<float> >(ss->getNumStates(), vector<float>(as->getNumActions(), 0.0)))
{
	numActions = ss->getNumStates();
	numStates = as->getNumActions();
}

MDPLIB::TransTable::TransTable(const TransTable& origTT) :
		Transition(origTT.states, origTT.actions), probTable(origTT.numStates, vector< vector<float> >(origTT.numStates))
{
	numActions = origTT.numActions;
	numStates = origTT.numStates;

	int s1, s2;
	for(s1 = 0; s1 < numStates; s1++) {
		for(s2 = 0; s2 < numStates; s2++) {
			probTable[s1][s2] = vector<float>(origTT.probTable[s1][s2]);
		}
	}
}

float MDPLIB::TransTable::transitionProb(Action* action, State* s1, State* s2) {
	if(s1->getId() >= numStates || s2->getId() >= numStates || action->getId() >= numActions)
		return PROB_ERR;
	return probTable[s1->getId()][s2->getId()][action->getId()];
}

float MDPLIB::TransTable::transitionProb(int aId, int s1Id, int s2Id) {
	if(s1Id >= numStates || s2Id >= numStates || aId >= numActions)
		return PROB_ERR;
	return probTable[s1Id][s2Id][aId];
}


int MDPLIB::TransTable::nonZeroTransProbs(vector<int> &retVect, int aId, int s1Id) {
	int s2Id;

	retVect.clear();
	for(s2Id = 0; s2Id < numStates; s2Id++) {
		if(probTable[s1Id][s2Id][aId] > 0)
			retVect.push_back(s2Id);
	}
	return(MDP_SUCCESS);
}

// TODO: Implement these two methods:  (why?)

int MDPLIB::TransTable::selectNextState(int aId, int s1Id){
	return -1;
}

State *MDPLIB::TransTable::selectNextState(Action *action, State *s1){
	return (State *)0;
}

int MDPLIB::TransTable::setActionProbs(State* s1, State* s2, vector<float> probs) {
	int a;
	if((int)probs.size() != numActions)
		return MDP_ERROR;

	for(a = 0; a < numActions; a++)
		probTable[s1->getId()][s2->getId()][a] = probs[a];
	return MDP_SUCCESS;
}

int TransTable::setResultStateProbs(State* s1, Action* a, vector<float> probs) {
	int s2;
	if((int)probs.size() != numStates)
		return MDP_ERROR;

	// TODO: Should confirm that these add to 1 or are all 0.
	for(s2=0; s2 < numStates; s2++) {
		probTable[s1->getId()][s2][a->getId()] = probs[s2];
	}
	return MDP_SUCCESS;
}


}


 /* namespace MDP */
