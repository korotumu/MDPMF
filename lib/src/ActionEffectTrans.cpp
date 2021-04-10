/*
 * ActionEffectTrans.cpp
 *
 *  Created on: Jan 27, 2015
 *      Author: Michelle LaMar 
 * 
 * This file is part of the mdpmf application.
 * Copyright 2014-2018  Michelle LaMar
 *
	This application is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this software; if not, see https://www.gnu.org/licenses/.

	mlamar@ets.org
	Educational Testing Service
	660 Rosedale Road
	Princeton, NJ 08541
 */

#include "ActionEffectTrans.h"

namespace MDPLIB {

ActionEffectTrans::ActionEffectTrans(ActionEffectTable aeTab, StateSpaceVarList *ss, ActionSet *as)
	: Transition(ss, as), tempVarValues(ss->getNumVars(),0.0),  stateVarValues(ss->getNumVars(),0.0), actResStates(1, vector< vector<StateProb> >(as->getNumActions(), vector<StateProb>(1))),
	  actEffTab(aeTab)
{
	actions = as;
	statesVL = ss;
	actionsDiffByState = 1;
	actResStatesInitialized = 0;
	startState = NULL;
	numThreads = 1;
	// TODO Should do sanity checks here

	int i, nsv = ss->getNumVars();
	for(i=0; i < nsv; i++) {
		stateVarNames.push_back(ss->getStateVarName(i));
	}
	numVarValues = nsv;

	// Set the parameter references in the action effect expressions:
	// Add the symbol table

	for(i=0; i < nsv; i++) {
		actEffTab.defineAEVar(stateVarNames[i], &(stateVarValues[i]));
	}

	// Init the action results table:
//	initActResStates();
}

ActionEffectTrans::~ActionEffectTrans() {
	// TODO Auto-generated destructor stub
}

// Initializing the states means walking through each one and doing the transition.  Takes a mo, but
// then we never have to do it again:
void ActionEffectTrans::initActResStates() {
	int s, a;
	int ns, na;
	ns = statesVL->getNumStates();
	na = actions->getNumActions();
	actResStates.resize(ns, vector< vector<StateProb> >(na, vector<StateProb>(1)));
	vector<int> actPossResNum(na, 0);
	vector< vector <ActionEffectProb> *> aEffProbList(na, NULL);


	for(a=0; a < na; a++) {
		aEffProbList[a] = actEffTab.getActionEffects(a);
		if(aEffProbList[a] == NULL) {
			cout << "ERROR: Action " << actions->getAction(a)->getLabel() << " returned a null action effect prob vector.\n";
			return;
		}
		actPossResNum[a] = (int)aEffProbList[a]->size();
	}

	// Try parallelization with OpenMP:
//#pragma omp parallel private(s,a,i, stateVarValues, actEffTab)
//		int i, nsv = statesVL->getNumVars();
//		for(i=0; i < nsv; i++) {
//			actEffTab.defineAEVar(stateVarNames[i], &(stateVarValues[i]));
//		}
//    #pragma omp for schedule(static)

	try{


		for(s= 0; s < ns; s++) {
			loadStateValues(s);
			for(a=0; a < na; a++) {
				actResStates[s][a].reserve(actPossResNum[a]);
				this->applyActionEffectOnly(a, s, aEffProbList[a], actResStates[s][a]);
			}
		}
	} catch(mu::Parser::exception_type &e) {
		// TODO Make this a better error handler
		std::cout << "Error parsing expression in ActionEffect File: " + e.GetExpr() << endl;
		std::cout << e.GetMsg() << endl;
	}

	actResStatesInitialized = 1;
}

void ActionEffectTrans::initPartActResStates(int s_start, int s_end, int na) {
	int s, a;
	try{

		for(s= s_start; s < s_end; s++) {

			loadStateValues(s);

			for(a=0; a < na; a++) {
				this->applyActionEffectOnly(a, s, actEffTab.getActionEffects(a), actResStates[s][a]);
			}
		}
	} catch(mu::Parser::exception_type &e) {
		// TODO Make this a better error handler
		std::cout << "Error parsing expression in ActionEffect File: " + e.GetExpr() << endl;
		std::cout << e.GetMsg() << endl;
	}
	return;
}
  
int ActionEffectTrans::setStartState(State *s){
  // Check that this is a valid state TODO

  // Set the start state
  startState = s;
  return MDP_SUCCESS;
}
  
int ActionEffectTrans::setStartState(int sId){
  // Check that this is a valid state
  startState = (State *)statesVL->getState(sId);
  if(startState == NULL)
    return MDP_ERROR;

  return MDP_SUCCESS;
}
  
State *ActionEffectTrans::getStartState(){
  return(startState);
}


float ActionEffectTrans::transitionProb(Action* action, State* s1, State* s2) {
	vector< StateProb> spVect;
	if(actResStatesInitialized) {
		spVect = actResStates[s1->getId()][action->getId()];
	} else {
		spVect = this->applyActionEffect(action, (StateVarList *)s1);
	}

	for(StateProb pv : spVect) {
		if(pv.stateId == s2->getId())
				return(pv.prob);
	}
	return 0.0;

}

float ActionEffectTrans::transitionProb(int aId, int s1Id, int s2Id) {
	vector< StateProb> spVect;
	if(actResStatesInitialized) {
		spVect = actResStates[s1Id][aId];
	} else {
		spVect = this->applyActionEffect(aId, s1Id);
	}

	for(StateProb pv : spVect) {
		if(pv.stateId == s2Id)
				return(pv.prob);
	}
	return 0.0;
}

vector<int>* ActionEffectTrans::nonZeroTransProbs(int aId, int s1Id) {
	vector<int> *retVect = new vector<int>(0);
	vector< StateProb> spVect;
	if(actResStatesInitialized) {
		spVect = actResStates[s1Id][aId];
	} else {
		spVect = this->applyActionEffect(aId, s1Id);
	}

	for(StateProb pv : spVect) {
		retVect->push_back(pv.stateId);
	}

	return(retVect);
}

int ActionEffectTrans::nonZeroTransProbs(vector<int>& retVect, int aId, int s1Id) {
	retVect.clear();
	vector< StateProb> spVect;
	if(actResStatesInitialized) {
		spVect = actResStates[s1Id][aId];
	} else {
		spVect = this->applyActionEffect(aId, s1Id);
	}

	for(StateProb pv : spVect) {
		retVect.push_back(pv.stateId);
	}

	return MDP_SUCCESS;
}

// If the actions take any variable out of range they are not possible.
int ActionEffectTrans::possibleActions(vector<int>& retVect, ActionSet* as, int s1Id) {
	retVect.clear();

	int aId, numActs = as->getNumActions();

	if(actResStatesInitialized) {
		for(aId = 0; aId < numActs; aId++) {
			if(actResStates[s1Id][aId].size() > 0)
				retVect.push_back(aId);
		}
	} else {
		vector< StateProb> spVect;
		for(aId = 0; aId < numActs; aId++) {
			this->applyActionEffect(aId, s1Id, spVect);
			if(spVect.size() >= 0)
				retVect.push_back(aId);
		}
	}

	return MDP_SUCCESS;
}

vector<Action*> ActionEffectTrans::possibleActions(int sId) {
	vector<Action *>retVect;
	vector<int> intVect;

	this->possibleActions(intVect, actions, sId);

	for(int aId : intVect) {
		retVect.push_back(actions->getAction(aId));
	}

	return(retVect);
}


int ActionEffectTrans::selectNextState(int aId, int s1Id) {
	vector< StateProb> spVect;
	if(actResStatesInitialized) {
		spVect = actResStates[s1Id][aId];
	} else {
		this->applyActionEffect(aId, s1Id, spVect);
	}

	float randProb = ((float)((rand() % 10000) + 1)/(float)10000);
	float currProb = 0;

	for(StateProb pv : spVect) {
		currProb += pv.prob;
		if(currProb >= randProb)
			return(pv.stateId);
	}

	return(-1); // Should never get here, but hey, it doesn't cost anything to be sure ...
}

State* ActionEffectTrans::selectNextState(Action* action, State* s1) {
	int s2Id = this->selectNextState(action->getId(), s1->getId());

	return(states->getState(s2Id));
}

vector<StateProb> ActionEffectTrans::applyActionEffect(Action* a, StateVarList* s1) {

	return(this->applyActionEffect(a->getId(), s1->getId()));
}

vector<StateProb> ActionEffectTrans::applyActionEffect(int aId, StateVarList* s1) {
	return(this->applyActionEffect(aId, s1->getId()));
}


vector<StateProb> ActionEffectTrans::applyActionEffect(int aId, int sId) {
	vector <StateProb> retVect;
	applyActionEffect(aId,sId,retVect);
	return(retVect);
}

int ActionEffectTrans::applyActionEffect(int aId, int sId, vector<StateProb> &retVect) {
	int ret = 0;

	if(aId < 0 || aId > (int)actEffTab.getNumActions())
		return MDP_ERROR;

	// This puts the current state var values in the place where muParser's symbol table points
	loadStateValues(sId);
	try{
		ret = applyActionEffectOnly(aId,sId, actEffTab.getActionEffects(aId), retVect);
	} catch(mu::Parser::exception_type &e) {
		// TODO Make this a better error handler
		std::cout << "Error parsing expression in ActionEffect File: " + e.GetExpr() << endl;
		std::cout << e.GetMsg() << endl;
	}
	return(ret);
}


int ActionEffectTrans::applyActionEffectOnly(int aId, int sId, vector <ActionEffectProb> *aeProbVect, vector<StateProb> &retVect) {

	unsigned int i, retCnt = 0;
	int newState;
	float prob = 0.0;
	float totProb = 0.0;
	bool foundState = false;
	retVect.clear();

	for(ActionEffectProb aep : *aeProbVect){

		for(i = 0; i < numVarValues; i++) {
			if(aep.isActionVarEffectNullOp(i))  // The null op case, no need to evaluate anything
				tempVarValues[i] = (float)stateVarValues[i];
			else
				tempVarValues[i] = aep.getActionVarEffect(i).Eval();
		}
		// and the prob if it is an expression:
		if(aep.prob < 0 )
			prob = aep.probEq.Eval();
		else
			prob = aep.prob;

		newState = statesVL->getStateId(tempVarValues);

		if(newState >= 0) // Keep track of in-bounds transitions
			retCnt++;
		else // If out of bounds, make this a null-op:
			newState = sId;

		if(prob < 0) prob = 0;
		totProb += prob;

		// If we've already seen this state, just update the probability:
		foundState = false;
		for(i=0; i < retVect.size(); i++) {
			if(retVect[i].stateId == newState) {
				foundState = true;
				retVect[i].prob += prob;
			}
		}
		if(!foundState)
			retVect.push_back(StateProb(prob, newState));
	}

	// No real results,  return an empty vector
	if(retCnt == 0) {
		retVect.clear();
		return(MDP_SUCCESS);
	}

	// Normalize the probabilities if needed
	if(totProb != 1.0) {
		for(i = 0; i < retVect.size(); i++) {
			retVect[i].prob /= totProb;
		}
	}

	return(MDP_SUCCESS);
}

void ActionEffectTrans::loadStateValues(int sId) {

	for(int i = 0; i < numVarValues; i++) {
		stateVarValues[i] = statesVL->getStateVarValue(sId, i);
	}
}

} /* namespace MDPLIB */
