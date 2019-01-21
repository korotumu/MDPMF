/*
 * StateSpaceVarList.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: Michelle LaMar
 *
 *      Copyright (C)  2014  Michelle M. LaMar

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

#include "StateSpaceVarList.h"

namespace MDPLIB {

StateSpaceVarList::StateSpaceVarList()
{
	numVars = 0;
	numStates = 0;
	pruneToReachable = false;
	numReachable = 0;
}

StateSpaceVarList::~StateSpaceVarList() {
	int i;
	for(i=1; i<numVars; i++){
		delete(varDescList[i]);
	}
	numStates = 0;
}

int StateSpaceVarList::setReachableStates(vector<int> reachableIDs) {
	unsigned int i, nrs = reachableIDs.size();

	if((int)nrs >= numStates)
		return MDP_ERROR;

	origID2reachableID.clear();
	origID2reachableID.assign(numStates, -1);

	// Note that the reachable ID list redefines the state ids.  We no longer
	// guarentee any relationship between ID and the state space.
	for(i=0; i<nrs; i++){
		if(reachableIDs[i] >= numStates)
			return MDP_ERROR;
		else {
			reachableStates.push_back(reachableIDs[i]);
			origID2reachableID[reachableIDs[i]] = i;
		}
	}

	numReachable = nrs;
	pruneToReachable = true;

	return MDP_SUCCESS;
}

int StateSpaceVarList::addDiscreteVar(string name, int min, int max) {
	// extra whitespace in a name causes a crash
	boost::trim_right(name);
	VarDesc *newVar;
	newVar = new VarDesc(name, DISCRETE, (float)min, (float)max);
	varDescList.push_back(newVar);
	varLabels.push_back(name);
	varType.push_back(DISCRETE);
	varNumValues.push_back(max - min + 1);
	varMinValues.push_back((float)min);
	varMaxValues.push_back((float)max);
	varStepSize.push_back((float)1);
	varChunkSize.push_back(1);
	curVarIndex.push_back(0);
	numVars++;
	if(numStates > 0 )
		numStates *= varNumValues[numVars-1];
	else
		numStates = varNumValues[numVars-1];

	recalcChunks();
	pruneToReachable = false;
	return MDP_SUCCESS;
}

int StateSpaceVarList::addContinuousVar(string name, float min, float max, float step) {
	boost::trim_right(name);
	VarDesc *newVar;
	newVar = new VarDesc(name, CONTINUOUS, min, max, step);
	varDescList.push_back(newVar);
	varLabels.push_back(name);
	varType.push_back(CONTINUOUS);
	int nv = (int)(floor((max - min)/step) + 1.0);
	varNumValues.push_back(nv);
	varMinValues.push_back(min);
	varMaxValues.push_back(max);
	varStepSize.push_back(step);
	varChunkSize.push_back(1);
	curVarIndex.push_back(0);
	numVars++;

	if(numStates > 0 )
			numStates *= varNumValues[numVars-1];
		else
			numStates = varNumValues[numVars-1];

	recalcChunks();
	pruneToReachable = false;
	return MDP_SUCCESS;
}

int StateSpaceVarList::addCategoryVar(string name) {
	boost::trim_right(name);
	VarDesc *newVar;
	newVar = new VarDesc(name, CATEGORY, (float)0.0, (float)0.0);
	varDescList.push_back(newVar);
	varLabels.push_back(name);
	varType.push_back(CATEGORY);
	varNumValues.push_back(0);
	varMinValues.push_back((float)0);
	varMaxValues.push_back((float)0);
	varStepSize.push_back((float)1);
	varChunkSize.push_back(1);
	curVarIndex.push_back(0);
	numVars++;

	recalcChunks();
	pruneToReachable = false;
	return MDP_SUCCESS;
}

int StateSpaceVarList::addCategoryToVar(string name, string catLabel) {

	// TODO Implement addCategoryToVar
	return 0;
}

State *StateSpaceVarList::startIterator()
{
	curStateIndex = 0;

	return getState(curStateIndex);
}

State *StateSpaceVarList::nextState()
{
	curStateIndex++;
	if((pruneToReachable && curStateIndex > numReachable) || curStateIndex > numStates)
		return new StateVarList(END_OF_STATES);

	return getState(curStateIndex);
}

State *MDPLIB::StateSpaceVarList::randState() {
	return ((State *) (0));
}

int StateSpaceVarList::getNumStates() {
	if(pruneToReachable)
		return(numReachable);

	return numStates;
}

StateVarList *StateSpaceVarList::getState(int id) {
	if(id >= numStates || (pruneToReachable && id > numReachable))
			return (StateVarList *)0;

	int origID;

	if(pruneToReachable)
		origID = reachableStates[id];
	else
		origID = id;

	vector<int> varInd(numVars);
	vector<float> data(numVars);
	int i, cs = 1;

	for(i = numVars-1; i>=0; i--) {
		varInd[i] = (origID/cs) % varNumValues[i];
		cs *= varNumValues[i];
	}

	for(i = 0; i < numVars; i++) {
		data[i] = (float)(varMinValues[i]) + (float)(varInd[i]) * varStepSize[i];
	}

	return new StateVarList(id, &varDescList, data);
}


StateVarList *StateSpaceVarList::getState(vector<double> ddata) {
	vector<float> fdata;

	for(unsigned int i = 0; i < ddata.size(); i++)
		fdata.push_back((float)ddata[i]);

	return(getState(fdata));
}

int StateSpaceVarList::getStateId(vector<double> ddata){
	vector<float> fdata;

	for(unsigned int i=0; i < ddata.size(); i++)
		fdata.push_back((float)ddata[i]);

	return(getStateId(fdata));
}

StateVarList *StateSpaceVarList::getState(vector<float> data) {
	int id = getStateId(data);

	return new StateVarList(id, &varDescList, data);
}

int StateSpaceVarList::getStateId(vector<float> data){
	int id = 0;
	vector<int> varInd(numVars);
	int i, cs = 1;

	if((int)data.size() != numVars)
		return(NO_SUCH_STATE);

	for(i = numVars-1; i>=0; i--) {
		if(data[i] < varMinValues[i] || data[i] > varMaxValues[i])
			return(NO_SUCH_STATE);
		id += ( (int)floor((data[i] - varMinValues[i]) / varStepSize[i]) ) * cs;
		cs *= varNumValues[i];
	}

	if(pruneToReachable)
		return(origID2reachableID[id]);
	else
		return id;
}

int StateSpaceVarList::getVarInd(string name) {
	unsigned int v;
	for(v=0; v < varDescList.size(); v++) {
		if(name.compare(varDescList[v]->getName()) == 0)
			return(v);
	}
	return -1;
}

void StateSpaceVarList::recalcChunks() {
	int i, cs = 1;

	for(i = numVars-1; i>=0; i--) {
		varChunkSize[i] = cs;
		cs *= varNumValues[i];
	}
}
string StateSpaceVarList::getStateVarName(int vId) {
	if(vId >= 0 && vId < numVars)
		return(varLabels[vId]);
	else
		return(string(""));
}


VarDesc* StateSpaceVarList::getStateVarDesc(int vInd) {
	if(vInd >= 0 && vInd < numVars)
		return(varDescList[vInd]);
	else
		return(NULL);
}

double StateSpaceVarList::getStateVarValue(int sId, int vId) {
//	if(sId >= numStates || vId >= numVars)
//			return VALUE_ERROR;
	if(pruneToReachable)
		return(varMinValues[vId] +
			(float)((reachableStates[sId]/varChunkSize[vId]) % varNumValues[vId]) * varStepSize[vId]);
	else
		return(varMinValues[vId] +
			(float)((sId/varChunkSize[vId]) % varNumValues[vId]) * varStepSize[vId]);
}

// Returns 1 if they are the same, 0 otherwise.
int StateSpaceVarList::compareStateVarValue(int vId, int s1Id, int s2Id) {

	if(pruneToReachable)
		return( (reachableStates[s1Id]/varChunkSize[vId]) % varNumValues[vId] == (reachableStates[s2Id]/varChunkSize[vId]) % varNumValues[vId]);
	else
		return( (s1Id/varChunkSize[vId]) % varNumValues[vId] == (s2Id/varChunkSize[vId]) % varNumValues[vId]);
}


int StateSpaceVarList::getNumVars() {
	return numVars;
}


} /* namespace MDPLIB */

