/*
 * ActionSequence.h
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

#ifndef ACTIONSEQUENCE_H_
#define ACTIONSEQUENCE_H_

#include "MDP.h"
#include "StateProb.h"
#include <vector>

namespace MDPLIB {

enum statesObserved_t {so_fully, so_partial, so_none};

class ActionSequence {
public:
	ActionSequence();
	ActionSequence(State *ss);
	ActionSequence(string id);
	ActionSequence(string id, State *ss);
	ActionSequence(string id, vector<StateProb> *ssd);
	virtual ~ActionSequence();

	void setStartState(State *s);
	void setStartStateDist(vector<StateProb> *ssd);
	void addStep(Action *a, State *s, float r, vector<int> acts, vector<double> probs);
	void addStep(Action *a, State *s, float r);
	void addStep(Action *a, State *s);
	void addStep(Action *a, vector<StateProb> stateDist);
	void addStep(Action *a);
	State *getStateAtStep(int step);
//	int getStateIDAtStep(int step);
	vector<StateProb> *getStateProbsAtStep(int step);
	Action *getActionAtStep(int step);
	float getRewardAtStep(int step);
	vector<int> *getPossActionsAtStep(int step);
	vector<double> *getActionProbsAtStep(int step);
	State *getStartState() const;
	vector<StateProb>  *getStartStateDist() const;
	int getNumSteps() const;

	string getId() const {
		return id;
	}

	void setId(string id) {
		this->id = id;
	}

	statesObserved_t getStatesObserved() const {
		return statesObserved;
	}

private:
	string id;
	int numSteps;
	State *startState;
	vector<StateProb> *startStateDist;
	vector<State *> stateSeq;
	vector<Action *> actionSeq;
	vector<float> rewardSeq;
	vector< vector<int> > possActionSeq;
	vector< vector<double> > actionProbSeq;
	vector< vector<StateProb> > stateDistSeq;
	statesObserved_t statesObserved;
	bool rewardsObserved;
	bool actProbsObserved;
};

} /* namespace MDPLIB */
#endif /* ACTIONSEQUENCE_H_ */
