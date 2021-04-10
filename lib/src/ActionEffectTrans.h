/*
 * ActionEffectTrans.h
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

#ifndef ACTIONEFFECTTRANS_H_
#define ACTIONEFFECTTRANS_H_

#include "Transition.h"
#include "ActionEffectTable.h"
#include "StateVarList.h"
#include <thread>
//#include <omp.h>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

namespace MDPLIB {

class ActionEffectTrans: public Transition {
public:
	ActionEffectTrans() {
		statesVL = NULL;
	}
	ActionEffectTrans(ActionEffectTable aeTab, StateSpaceVarList *ss, ActionSet *as);
	virtual ~ActionEffectTrans();

	virtual float transitionProb(Action *action, State *s1, State *s2);
	virtual float transitionProb(int aId, int s1Id, int s2Id);

	virtual vector<int> *nonZeroTransProbs(int aId, int s1Id);
	virtual int nonZeroTransProbs(vector<int> &retVect, int aId, int s1Id);

	virtual int possibleActions(vector<int> &retVect, ActionSet *as, int s1Id);
	virtual vector<Action *> possibleActions(int sId);

	virtual int selectNextState(int aId, int s1Id);
	virtual State *selectNextState(Action *action, State *s1);

        int setStartState(State *s);
        int setStartState(int sId);
        State *getStartState();

	void initActResStates();
	void initPartActResStates(int s_start, int s_end, int na);

	int getNumThreads() const {
		return numThreads;
	}

	void setNumThreads(int numThreads) {
		this->numThreads = numThreads;
	}

	// Do we need to override these?  Probably not.
	// ParamList *getParamList();
	// void setParamList(ParamList *params, vector<int> pIndex);
	// int getNumParams();
	// void paramsUpdated();
protected:
	ActionEffectTable actEffTab;
	StateSpaceVarList *statesVL;
        State *startState;
	vector< vector< vector<StateProb> > > actResStates;
	bool actResStatesInitialized;
	int numThreads;

	vector<string> stateVarNames;
	vector<double> stateVarValues;
	int numVarValues;
	vector<float> tempVarValues;

	vector<StateProb> applyActionEffect(Action *a, StateVarList *s1);
	vector<StateProb> applyActionEffect(int aId, int sId);
	vector<StateProb> applyActionEffect(int aId, StateVarList *s1);

	int applyActionEffect(int aId, int sId, vector<StateProb> &resVect);
	int applyActionEffectOnly(int aId, int sId, vector <ActionEffectProb> *aepVect, vector<StateProb> &resVect);

	void loadStateValues(int sId);

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<MDPLIB::Transition>(*this);
        ar & states;

//        ar & actEffTab;
        ar & statesVL;
        ar & actResStates;
        ar & actResStatesInitialized;
        ar & numThreads;

        ar & stateVarNames;
        ar & stateVarValues;
        ar & numVarValues;
        ar & tempVarValues;
    }
};

} /* namespace MDPLIB */

#endif /* ACTIONEFFECTTRANS_H_ */
