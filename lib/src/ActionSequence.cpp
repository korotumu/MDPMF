/*
 * ActionSequence.cpp
 *
 *  Created on: Mar 7, 2013
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

#include "ActionSequence.h"
#include "GlobalDefs.h"

namespace MDPLIB {

ActionSequence::ActionSequence() {
	statesObserved = so_fully;
	rewardsObserved = 1;
	actProbsObserved = 1;
	numSteps = 0;
	startState = (State *)0;
	startStateDist = NULL;
}


ActionSequence::ActionSequence(State* ss) {
	statesObserved = so_fully;
	rewardsObserved = 1;
	actProbsObserved = 1;
	numSteps = 0;
	startState = ss;
	startStateDist = NULL;
}

ActionSequence::ActionSequence(string id) {
	this->id = id;
	statesObserved = so_fully;
	rewardsObserved = 1;
	actProbsObserved = 1;
	numSteps = 0;
	startState = (State *)0;
	startStateDist = NULL;
}

ActionSequence::ActionSequence(string id, State* ss) {
	this->id = id;
	statesObserved = so_fully;
	rewardsObserved = 1;
	actProbsObserved = 1;
	numSteps = 0;
	startState = ss;
	startStateDist = NULL;
}

ActionSequence::ActionSequence(string id, vector<StateProb> *ssd) {
	this->id = id;
	statesObserved = so_partial;
	rewardsObserved = 1;
	actProbsObserved = 1;
	numSteps = 0;
	startState = NULL;
	startStateDist = ssd;
}

ActionSequence::~ActionSequence() {
}

void ActionSequence::setStartState(State* s) {
	startState = s;
}

State* ActionSequence::getStartState() const {
	return startState;
}


void ActionSequence::setStartStateDist(vector<StateProb>* ssd) {
	startStateDist = ssd;
}

vector<StateProb>* ActionSequence::getStartStateDist() const {
	return startStateDist;
}


int ActionSequence::getNumSteps() const {
	return numSteps;
}

void ActionSequence::addStep(Action* a, State* s, float r, vector<int> acts, vector<double> probs) {
	actionSeq.push_back(a);
	if(statesObserved == so_fully)
		stateSeq.push_back(s);
	if(statesObserved == so_partial)
		stateDistSeq.push_back(vector<StateProb>(1, StateProb(1.0, s->getId())));
	if(rewardsObserved)
		rewardSeq.push_back(r);
	if(actProbsObserved) {
		possActionSeq.push_back(acts);
		actionProbSeq.push_back(probs);
	}
	numSteps++;

}

void ActionSequence::addStep(Action* a, State* s, float r) {
	actionSeq.push_back(a);
	if(statesObserved == so_fully)
		stateSeq.push_back(s);
	if(statesObserved == so_partial)
		stateDistSeq.push_back(vector<StateProb>(1, StateProb(1.0, s->getId())));

	if(rewardsObserved)
		rewardSeq.push_back(r);

	actProbsObserved = 0;
	numSteps++;

}

void ActionSequence::addStep(Action* a, State* s) {
	if(statesObserved == so_fully)
		stateSeq.push_back(s);
	if(statesObserved == so_partial)
		stateDistSeq.push_back(vector<StateProb>(1, StateProb(1.0, s->getId())));
	actionSeq.push_back(a);
	numSteps++;

	rewardsObserved = 0;
	actProbsObserved = 0;
}

void ActionSequence::addStep(Action* a) {
	actionSeq.push_back(a);
	numSteps++;

	statesObserved = so_none;
	rewardsObserved = 0;
	actProbsObserved = 0;
}

void ActionSequence::addStep(Action* a, vector<StateProb> stateDist) {
	if(statesObserved == so_fully && numSteps > 0 && stateDistSeq.size() == 0) {
		// Convert all prev states to state dist:
		for(int s = 0; s < numSteps; s++) {
			stateDistSeq.push_back(vector<StateProb>(1, StateProb(1.0, stateSeq[s]->getId())));
		}
		statesObserved = so_partial;
	}
	if(statesObserved == so_partial)
		stateDistSeq.push_back(stateDist);
	actionSeq.push_back(a);
	numSteps++;

	rewardsObserved = 0;
	actProbsObserved = 0;
}

vector<StateProb> *ActionSequence::getStateProbsAtStep(int step) {
	if(statesObserved == so_partial && step < numSteps && step >= 0)
		return(&stateDistSeq[step]);
	else
		return(new vector<StateProb>());
}

State* ActionSequence::getStateAtStep(int step) {
	if(statesObserved == so_fully && step < numSteps && step >= 0)
		return(stateSeq[step]);
	else
		return((State *)0);
}

Action* ActionSequence::getActionAtStep(int step) {
	if(step < numSteps)
		return(actionSeq[step]);
	else
		return((Action *)0);
}

float ActionSequence::getRewardAtStep(int step) {
	if(rewardsObserved && step < numSteps)
		return(rewardSeq[step]);
	else
		return(VALUE_ERROR);
}

vector<int>* ActionSequence::getPossActionsAtStep(int step) {
	if(actProbsObserved && step < numSteps)
		return(&(possActionSeq[step]));
	else
		return(NULL);

}

vector<double>* ActionSequence::getActionProbsAtStep(int step) {
	if(actProbsObserved && step < numSteps)
		return(&(actionProbSeq[step]));
	else
		return(NULL);
}

} /* namespace MDPLIB */


