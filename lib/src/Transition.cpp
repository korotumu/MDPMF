/*
 * Transition.cpp
 *
 *  Created on: Jan 8, 2013
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

#include "Transition.h"
#include <stdlib.h>


namespace MDPLIB {

Transition::Transition(StateSpace *ss, ActionSet *as) {
	states = ss;
	actions = as;
	actionsDiffByState = 0;
	pList = NULL;
}

Transition::~Transition() {
	// TODO Auto-generated destructor stub
}

float Transition::transitionProb(Action* action, State* s1, State* s2) {
	// This is a stub.  The real functionality will be implemented in
	// problem specific subclasses of Transition.
	return (float)0.0;
}


float Transition::transitionProb(int a, int s1, int s2) {
	// This is a stub.  The real functionality will be implemented in
	// problem specific subclasses of Transition.
	return (float)0.0;
}
vector<int>* Transition::nonZeroTransProbs(int aId, int s1Id) {
	vector<int> *retVect = new vector<int>();
	nonZeroTransProbs(*retVect, aId, s1Id);
	return(retVect);
}

int MDPLIB::Transition::nonZeroTransProbs(vector<int> &retVect, int aId, int s1Id) {
	// This is a stub.  Yada yada.
	return(-1);
}

int MDPLIB::Transition::selectNextState(int aId, int s1Id){
	vector<int> posStates(0);

	if(nonZeroTransProbs(posStates, aId, s1Id) != MDP_SUCCESS || posStates.size() < 1)
		return -1;

	int i = 0;
	double cumProb = 0.0;
	double randDraw;

	randDraw = ((double)((rand() % 10000) + 1)/(double)10000);

	while(cumProb < randDraw && i < (int)posStates.size()) {
		cumProb += transitionProb(aId, s1Id, posStates[i]);
		i++;
	}

	return(posStates[i-1]);
}

State *MDPLIB::Transition::selectNextState(Action *action, State *s1){

	int sId = selectNextState(action->getId(), s1->getId());
	if(sId < 0)
		return(NULL);
	else
		return (states->getState(sId));
}

ParamList *MDPLIB::Transition::getParamList() {
	return((ParamList *)(0));
}

void MDPLIB::Transition::setParamList(ParamList* params, vector<int> pIndex) {
	pList = params;
}

int MDPLIB::Transition::getNumParams() {
	return(0);
}

int MDPLIB::Transition::possibleActions(vector<int>& retVect, ActionSet* as,
		int s1Id) {
	int i, na = as->getNumActions();
	retVect.clear();

	for(i=0; i < na; i++){
		retVect.push_back(i);
	}
	return(MDP_SUCCESS);
}

vector<Action*> MDPLIB::Transition::possibleActions(int sId) {
	// Defaults is to use all actions:
	return(actions->getActionList());
}

void MDPLIB::Transition::paramsUpdated() {

}

}/* namespace MDP */

