/*
 * Transition.h
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

#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "Action.h"
#include "ActionSet.h"
#include "State.h"
#include "StateSpace.h"
#include "ParamList.h"
#include "GlobalDefs.h"

namespace MDPLIB {

class Transition {
public:
	Transition() {
		states = NULL;
		actions = NULL;
		actionsDiffByState = false;
		pList = NULL;
	}
	Transition(StateSpace *states, ActionSet *actions);
	virtual ~Transition();
	virtual float transitionProb(Action *action, State *s1, State *s2);
	virtual float transitionProb(int aId, int s1Id, int s2Id);
	virtual vector<int> *nonZeroTransProbs(int aId, int s1Ide);
//	virtual vector<int> *nonZeroTransProbs(int aId, int s1Id, bool &needToFree);  Possible new implementation for optimizing memory issues
	virtual int nonZeroTransProbs(vector<int> &retVect, int aId, int s1Id);
	virtual int possibleActions(vector<int> &retVect, ActionSet *as, int s1Id);
	virtual vector<Action *> possibleActions(int sId);
	virtual int selectNextState(int aId, int s1Id);
	virtual State *selectNextState(Action *action, State *s1);
	virtual ParamList *getParamList();
	virtual void setParamList(ParamList *params, vector<int> pIndex);
	virtual int getNumParams();
	virtual void paramsUpdated();

	bool areActionsDiffByState() const {
		return actionsDiffByState;
	}

	void setActionsDiffByState(bool actionsDiffByState) {
		this->actionsDiffByState = actionsDiffByState;
	}

protected:
	StateSpace *states;
	ActionSet *actions;
	bool actionsDiffByState;
	ParamList *pList;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    	ar &  states;
    	ar &  actions;
    	ar &  actionsDiffByState;
        ar & pList;
    }
};

} /* namespace MDP */
#endif /* TRANSITION_H_ */
