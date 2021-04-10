/*
 * ActionEffectReward.cpp
 *
 *  Created on: Dec 4, 2015
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

#include "ActionEffectReward.h"

namespace MDPLIB {

ActionEffectReward::ActionEffectReward(StateSpaceVarList *ss, ActionSet *as)
	: condMap(ss->getNumStates(), vector<bool>()), actionMap(as->getNumActions(), vector<bool>())
{
	params = new ParamList();
	actions = as;
	states = ss;
	numRParams = 0;
}

int ActionEffectReward::initFromIOTable(IOTable iot) {
	int i, v;
	int ret = loadRewardsFromIOTable(iot);

	estParams.clear();
	pListMap.clear();
	paramMin.clear();
	paramMax.clear();
	condExpr.clear();    // TODO: This is a potential memory leak.  We should free all memory existing here
	stateVarNames.clear();
	stateVarValues.clear();

	if(ret != MDP_SUCCESS) {
		// Abort.
		return MDP_ERROR;
	}

	int nsv = states->getNumVars();
	for(i=0; i < nsv; i++) {
		stateVarNames.push_back(states->getStateVarName(i));
		stateVarValues.push_back(0.0);
	}

	try {
		for(i=0; i< (int)numRParams; i++) {
			// Create the conditional expression
			condExpr.push_back(new mu::Parser());

			// Add the symbol table
			for(v=0; v < nsv; v++) {
			  condExpr[i]->DefineVar(stateVarNames[v], &(stateVarValues[v]));
			}
			condExpr[i]->SetExpr(condExprStr[i]);

			// Set the param estimation vectors:
			estParams.push_back(0);
			pListMap.push_back(-1);
			paramMin.push_back(currValues[i]);
			paramMax.push_back(currValues[i]);
		}
	} catch(mu::Parser::exception_type &e) {
		// TODO Make this a better error handler
		std::cout << "Error parsing expression in Reward File: " + e.GetExpr() << endl;
		std::cout << e.GetMsg() << endl;
		return MDP_ERROR;
	}

	resetParamIndicies();

	// Set all the states and action booleans for quick lookup later:
	setCondMap();
	return(MDP_SUCCESS);
}

ActionEffectReward::~ActionEffectReward() {
	delete(params);
}

void ActionEffectReward::setCondMap() {
	// For each state, create a vector of booleans that record whether this state meets the
	// reward parameter condition:

	int s, ns = states->getNumStates();
	double cVal = -1;

	for(s = 0; s < ns; s++) {
		condMap[s].clear();
		loadStateValues(s);
		for(auto c : condExpr) {
			try {
				cVal = c->Eval();
			} catch (mu::Parser::exception_type &e) {
				std::cout << "Error parsing expression in Reward File: " << e.GetExpr() << endl;
				std::cout << e.GetMsg() << endl;
				cVal = 0;
			}
			condMap[s].push_back(cVal != 0);
		}
	}

	int a, na = actions->getNumActions();
	for(a = 0; a < na; a++) {
		actionMap[a].clear();
		for(auto ra : actionIds) {
			if(ra == SVR_WILDCARD || ra == a)
				actionMap[a].push_back(true);
			else
				actionMap[a].push_back(false);
		}
	}
	return;
}

void ActionEffectReward::loadStateValues(int sId) {
	int nsv = states->getNumVars();
	for(int i = 0; i < nsv; i++) {
		stateVarValues[i] = (float)(states->getStateVarValue(sId, i));
	}
}

void ActionEffectReward::resetParamIndicies() {

	pListMap.clear();
	for(auto rpname : paramNames) {
		pListMap.push_back(params->getParamIndex(&rpname));
	}
}

float ActionEffectReward::rewardValue(Action* action, State* s1, State* s2) {
	return(rewardValue(action->getId(), s1->getId(), s2->getId()));
}

float ActionEffectReward::rewardValue(int aId, int s1Id, int s2Id) {

	// Dot product of the state CondMap vector with the action actionMap vector and the current reward values
	unsigned int i;
	float ret = 0;
	for(i=0; i < numRParams; i++) {
		ret += actionMap[aId][i] * condMap[s2Id][i] * currValues[i];
	}
	return(ret);

}

// See if dotProd is immplmented at lower level.  In the meantime:
float ActionEffectReward::dotProd(vector<bool> b1, vector<bool> b2, vector<float> f) {
	int i, N = b1.size();
	float ret = 0;
	for(i=0; i < N; i++) {
		ret += b1[i] * b2[i] * f[i];
	}
	return(ret);
}

ParamList* ActionEffectReward::getParamList() {
	return(params);
}

void ActionEffectReward::setParamList(ParamList* newParams, vector<int> pIndex) {
	delete(params);
	params = newParams;

	resetParamIndicies();
	return;
}

void ActionEffectReward::setParams(vector<double> values) {
	unsigned int i, pn = params->getLength();

	// This is called in two different modes.  If all parameter values are provided, then
	// we set both the parameter values and the fixed values.
	// If only the number of parameters are passed in, we set only the parameters.
	if(values.size() < numRParams) {
		// These should be just the parameter list
		if(pn != values.size()) {
			printf("Warning: %d values passed to ActionEffectReward::setParams (%d params)\n", (int)(values.size()), pn);
			return;
		}

		for(i=0; i < pn; i++) {
			params->setParamValue(i, values[i]);
		}
		return;
	}

	// Set the estimated parameters as parameters, and the remaining as fixed values
	for(i = 0; i < numRParams; i++) {
		if(estParams[i])
			params->setParamValue(pListMap[i], values[i]);
		currValues[i] = values[i];
	}

	return;
}

int ActionEffectReward::setEstParams(vector<bool> ep) {
	if(ep.size() != numRParams)
		return MDP_ERROR;

	unsigned int i, pi;
	for(i = 0; i < numRParams; i++) {

		pi = params->getParamIndex(&(paramNames[i]));
		if(ep[i] && pi < 0) {
			// Need to add it
			params->addParam(paramNames[i].c_str(), currValues[i], paramMin[i], paramMax[i]);
		} else if(!ep[i] && pi >= 0) {
			// Need to remove parameter
			params->removeParam(pi);
		}
		estParams[i] = ep[i];
	}
	resetParamIndicies();
	return(MDP_SUCCESS);
}

int ActionEffectReward::loadRewardsFromIOTable(IOTable iot){
	// char line[R_MAX_LINE], name[R_MAX_LINE], actstr[R_MAX_LINE], condstr[R_MAX_LINE];

	string name, actstr, condstr;
	float tmpVal;
	string tmp_str;
	int  aId;

	// No checks for proper formatting yet - will work on it
	if(iot.getNumCols() != 4){
		cout << "ERROR: Misformatted Action Effect Reward table";
		return MDP_ERROR;
	}

	// Reset everything for the case that we are re-initializing:
	numRParams = 0;
	actionIds.clear();
	defaultValues.clear();
	currValues.clear();
	condExprStr.clear();
	paramNames.clear();

	for (unsigned int i = 0; i < iot.getNumRows(); i++) {
		vector <string> rowData = iot.getRow(i);
		name = rowData[0];
		tmpVal = std::stof(rowData[1]);
		actstr = rowData[2];
		condstr = rowData[3];	

		paramNames.push_back(name);
		if(strcmp(actstr.c_str(),"*") == 0) {
			// Any action will do:
			aId = SVR_WILDCARD;
		} else {
			aId = actions->getAction(actstr)->getId();
			if(aId < 0) {
				// Action not found
				cout << "In loadRewardsFromIOTable: Action " << string(actstr) << "not found.";
				return MDP_ERROR;
			}
		}
		actionIds.push_back(aId);
		defaultValues.push_back(tmpVal);
		currValues.push_back(tmpVal);
		condExprStr.push_back(condstr);
		numRParams++;
	}

	return MDP_SUCCESS;
}


} /* namespace MDPLIB */


