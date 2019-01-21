/*
 * SimEngine.cpp
 *
 *  Created on: Jun 11, 2017
 *      Author: Michelle LaMar 
 * 
 *      Copyright (C) 2017, 2018  Michelle M. LaMar

This library is free software; you can redistribute it and/or  
modify it under the terms of the GNU Lesser General Public 
License as published by the Free Software Foundation; either 
version 2.1 of the License, or (at your option) any later version. 

This library is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
Lesser General Public License for more details. 

You should have received a copy of the GNU Lesser General Public 
License along with this library; if not, see website. 

mlamar@ets.org 
Educational Testing Service 
660 Rosedale Road 
Princeton, NJ 08541
 */

#include "SimEngine.h"

namespace MDPLIB {

SimEngine::SimEngine() {
	simPol = NULL;
	envTrans = NULL;
	record_action_choices = false;
	maxSimRec = 10000;
}

SimEngine::SimEngine(Policy *sp, Transition *et) {
	simPol = sp;
	envTrans = et;
	record_action_choices = false;
	maxSimRec = 10000;
}

SimEngine::SimEngine(Policy *sp) {
	simPol = sp;
	envTrans = sp->getMDP()->getTrans();
	record_action_choices = false;
	maxSimRec = 10000;
}

ActionSequence *SimEngine::simActions(string label, State *start) {
	if(simPol == NULL || envTrans == NULL || start == NULL)
		return(NULL);

	ActionSequence *pr = new ActionSequence(label, start);
	bool end = false;
	State *curState, *nextState;
	Action *action;
	MDP *mdp = simPol->getMDP();


	float scoreVal;
	int endActId = mdp->getStopActionInd();

	vector<int> aList;
	vector<double> pList;

	curState = start;

	while(!end && pr->getNumSteps() < maxSimRec) {
		// Get action for state:
		action = simPol->chooseActionForState(curState);
		nextState = envTrans->selectNextState(action,curState);
		if(nextState->getId() < 0){
			cout << "WARNING: Sim returned impossible state for action.\n";
			nextState = curState;
		}
		scoreVal = mdp->getReward()->rewardValue(action, curState, nextState);
		// For analysis, we record the action id's and probabilities for each choice
		if(record_action_choices) {
			mdp->getTrans()->possibleActions(aList, mdp->getActions(), curState->getId());
			pList = simPol->getActionProbsForState(curState);
			pr->addStep(action, nextState, scoreVal, aList, pList);
		} else {
			pr->addStep(action, nextState, scoreVal);
		}

		if(action->getId() == endActId) {
			end = 1;
		} else {
			//	if(curState != startState)
			//		delete(curState);   * Can't delete the states because they are being passed to the playRecord object.
			curState = nextState;
		}
	}

	return(pr);
}

} /* namespace MDPLIB */
