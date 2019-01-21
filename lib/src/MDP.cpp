/*
 * MDP.cpp
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

#include "MDP.h"

namespace MDPLIB {

MDP::MDP() : tmpStateList(MIN_TMPLIST_LEN, 0) {
	states = NULL;
	actions = NULL;
	trans = NULL;
	reward = NULL;
	discount = 1;
	stopActionInd = -1;
}

MDP::MDP(StateSpace *ss, ActionSet *as, Transition *t, Reward *r, double disc)
	: tmpStateList(MIN_TMPLIST_LEN, 0)
{
	states = ss;
	actions = as;
	trans = t;
	reward = r;
	discount = disc;
	stopActionInd = actions->getStopAction();
}

MDP::~MDP() {

}
ActionSet *MDP::getActions() const
{
	return actions;
}

double MDP::getDiscount() const
{
	return discount;
}

Reward *MDP::getReward() const
{
	return reward;
}

StateSpace *MDP::getStates() const
{
	return states;
}

Transition *MDP::getTrans() const
{
	return trans;
}

void MDP::setActions(ActionSet *actions)
{
	this->actions = actions;
}

void MDP::setDiscount(double discount)
{
	this->discount = discount;
}

void MDP::setReward(Reward *reward)
{
	this->reward = reward;
}

void MDP::setStates(StateSpace *states)
{
	this->states = states;
}

void MDP::setTrans(Transition *trans)
{
	this->trans = trans;
}

void MDP::setStopAction(int saInd) {
	stopActionInd = saInd;
}

double MDP::getActionValueForState(const int &s1Id, const int &aId, const vector<double> &stateVals)
{
	unsigned int i;
	bool useFuture = 1;
	double val = 0;

	// A stop action should still get a reward for the state in which play was stopped, but there is no future
	if(stopActionInd >= 0 && aId == stopActionInd)
		useFuture = 0;

//	vector<int> s2Range;
	trans->nonZeroTransProbs(tmpStateList, aId, s1Id);
	for(i=0; i < tmpStateList.size(); i++) {
		val += trans->transitionProb(aId, s1Id, tmpStateList[i]) *  (reward->rewardValue(aId, s1Id, tmpStateList[i]) + useFuture * discount * stateVals[tmpStateList[i]]);
	}
	return(val);
}

double MDP::getActionValueForState(State* s1, Action* action, const vector<double> &stateVals)
{
	return(getActionValueForState(s1->getId(), action->getId(), stateVals));
	/*
	int i;
	double val = 0;
	State *s2;

	//if(action->index == STOP_ACTION)
	//	return(0);

	for(s2 = states->startIterator(), i=0; s2->getId() != END_OF_STATES; i++, s2 = states->nextState()){
		val += trans->transitionProb(action, s1, s2) *  (reward->rewardValue(action, s1, s2) + discount * stateVals[i]);
	}
	return(val);
	*/
}

void MDP::paramsUpdated() {
	trans->paramsUpdated();
	reward->paramsUpdated();
}

}/* namespace MDP */
