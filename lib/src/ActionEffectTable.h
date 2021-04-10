/*
 * ActionEffectTable.h
 *
 *  Created on: Jan 26, 2015
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

#ifndef ACTIONEFFECTTABLE_H_
#define ACTIONEFFECTTABLE_H_

#include <vector>
#include <string>
#include <boost/tokenizer.hpp>
#include <muParser.h>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "VarDesc.h"
#include "ActionEffectProb.h"
#include "StateSpaceVarList.h"
#include "IOTable.h"

#define AT_ERROR_VALUE 99
#define AT_MAX_LINE 5120

// TODO: Need to resolve:
// How to specify start play string
// end play string,
// Whether reset (restart) is a choice
// Whether stopping is a choice

namespace MDPLIB {

class ActionEffectTable {
public:
	ActionEffectTable();
	ActionEffectTable(const MDPLIB::ActionEffectTable& aet);
	virtual ~ActionEffectTable();

	// Loading the table from a string table format
	int readFromIOTable(IOTable iot);

	// Lookup function -- returns a parser loaded with the appropriate expression:
	vector<ActionEffectProb> *getActionEffects(int actionIndex);

	// Define the variables used in the expressions:
	void defineAEVar(string varName, double *valPtr);

	// Normalize action probabilities if they don't already sum to 1
	void normalizeActionProbabilities();

	// Check to see if other AET has same action set, state space, and initial state as this one
	bool isCompatibleAET(ActionEffectTable *aet);

	// Getters and Setters:
	const vector<string>& getActionList() const {
		return actionList;
	}

	void setActionList(const vector<string>& actionList) {
		this->actionList = actionList;
	}

	const vector<VarDesc*>& getStateVarList() const {
		return stateVarList;
	}

	void setStateVarList(const vector<VarDesc *>& stateVarList) {
		this->stateVarList = stateVarList;
		numStateVars = stateVarList.size();
	}

	unsigned int getNumActions() const {
		return numActions;
	}

	unsigned int getNumStateVars() const {
		return numStateVars;
	}

	const vector<float>& getStartVarVals() const {
		return startVarVals;
	}

	void setStartVarVals(const vector<float>& startVarVals) {
		this->startVarVals = startVarVals;
	}


private:
	unsigned int numActions;
	unsigned int numStateVars;
	vector <string> actionList;
	vector <VarDesc *> stateVarList;
	vector <float> startVarVals;
	vector< vector <ActionEffectProb> > *effectTable;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    	ar & numActions;
    	ar & numStateVars;
    	ar & actionList;
    	ar & stateVarList;
    	ar & startVarVals;
    	ar & effectTable;
    }

};


}
#endif /* ACTIONEFFECTTABLE_H_ */
