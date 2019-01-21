/*
 * AcPlayRecs.cpp
 *
 *  Created on: Jan 15, 2014
 *      Author: Michelle
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

#include "ActionEffectPlayRecs.h"

#define MAX_LINE 256
typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

using namespace MDPLIB;

StateVarList *getStateFromVector(vector< string > vect, StateSpaceVarList *ss, vector<int> stateVarMap);
vector<StateProb> *getStateDistFromVector(vector< string > vect, StateSpaceVarList *ss, vector<int> stateVarMap, vector<bool> stateVarObserved);
int expandStateDist(StateSpaceVarList *ss, vector<float> *varVals, vector<bool> *knownVars, vector<StateProb> *stateDist);
bool checkLegalAction(MDP *mdp, int curStateId, int curActId);
string vectToString(vector< string > vect);

vector<ActionSequence *> *readPlayRecordsFromTable(IOTable table, MDP *mdp, bool enforceTrans, StateVarList *dss, bool verbose) {
	unsigned int numCols = table.getNumCols();
	unsigned int numRows = table.getNumRows();
	unsigned int c = 0, r = 0;
	vector< string > colnames;
	vector< string > rowData;
	string id, lastId, actStr;

	if(numCols == 0 || numRows == 0 || mdp == NULL)
		return NULL;

	StateSpaceVarList *ss = (StateSpaceVarList *)mdp->getStates();
	ActionSet *as = (ActionSet *)mdp->getActions();
	Action *curAct;
	StateVarList *curState = NULL, *nextState = NULL;
	vector<StateProb> *curStateDist = NULL, *nextStateDist = NULL;
	Action *endMissionAct = as->getAction("EndMission");
	bool missionEnded = 0;
	int numStateVars = ss->getNumVars();
	vector <int> stateVarMap;
	vector <bool> foundStateVar(numStateVars, false);
	int varInd;
	bool fullStateInfo = true;

	ActionSequence *pr;
	vector<ActionSequence *> *prl = new vector<ActionSequence *>();
	StateVarList *defStartState = dss;
	if(defStartState == NULL)
		defStartState = (StateVarList *)ss->getState(vector<float>(numStateVars, 0.0));
	curState = defStartState;
	vector<StateProb> *defStartStateDist = new vector<StateProb>(1, StateProb(1.0, defStartState->getId()));
	curStateDist = defStartStateDist;

	lastId = "NO ID";

	// Get the header line.  This will define the state variable names:
	// Expected to be:  StudID Action <var1_name> <var2_name> ....
	colnames = table.getColNames();

	// First two should be StudID and Action, skip these.  State var names start with index 2

	for (c = 2; c < numCols; c++) {
		varInd = ss->getVarInd(colnames[c]);
		if(varInd >= 0) {
			stateVarMap.push_back(varInd);
			foundStateVar[varInd] = true;
		}
		else
			cout << "WARNING: Play record variable " << colnames[c] << " not found in state space.\n";
	}

	if((int)stateVarMap.size() > numStateVars) {
		printf("ERROR: Action Record file has %d state variable columns, while the state space has %d variables. A state variable might be duplicated in the action sequence file.\n", (int)stateVarMap.size(), numStateVars);
		return NULL;
	}

	if((int)stateVarMap.size() < numStateVars) {
		// We will marginalize over the missing state info
		printf("WARNING: Action Record file has %d state variables, while the state space has %d.\n", (int)stateVarMap.size(), numStateVars);
		printf("Marginalizing over possible values for missing state records.\n");
		fflush(stdout);
		fullStateInfo = false;
	} else {
		// Confirm that we have all the vars (no repeats)
		for(int vInd = 0; vInd < numStateVars; vInd++) {
			if(!foundStateVar[vInd]) {
				printf("ERROR: Performance record file has undefined state variables but the right number of columns. A state variable might be duplicated in the action sequence file.\n");
				return NULL;
			}
		}
	}

	// Now start iterating through the data:
	for(r = 0; r < numRows; r++) {
		rowData = table.getRow(r);
		id = rowData[0];
		actStr = rowData[1];

	//	sscanf(str, "%s %s %[^\n]", id, actStr, remStr);

		if(id != lastId) {
			// Must add a EndMission action, if not already included
			if(lastId != "NO ID" && !missionEnded) {
				if(fullStateInfo)
					pr->addStep(endMissionAct, curState);
				else
					pr->addStep(endMissionAct, *curStateDist);
			}

			lastId = id;

			if(verbose) {
				cout << "Processing Performance Record for ID: " << id << "\n";
				cout.flush();
			}

			if(fullStateInfo) {
				curState = defStartState;
				if(actStr == "StartMission") {
					curState = getStateFromVector(rowData, ss, stateVarMap);
					if(curState == NULL || curState->getId() < 0) {
						cout << "WARNING:  Start state not found: " << vectToString(rowData) << "\n";
						return NULL;
					}
				}
				pr = new ActionSequence(id, curState);
			}
			else {
				curStateDist = defStartStateDist;
				pr = new ActionSequence(id, curStateDist);
			}

			prl->push_back(pr);
			missionEnded = 0;
		}

	// Now parse the state variables (if this is not the start mission):

		if(actStr != "StartMission") { // This is NOT the start of a play record

			// Get the next state(s):
			if(fullStateInfo) {
				nextState = getStateFromVector(rowData, ss, stateVarMap);
				if(nextState == NULL || nextState->getId() < 0){
					cout << "WARNING:  State not found: "<< vectToString(rowData) << "\n";
					return NULL;
				}
			} else {
				nextStateDist = getStateDistFromVector(rowData, ss, stateVarMap, foundStateVar);
				if(nextStateDist->size() == 0) {
					cout << "WARNING:  State dist not found: "<< vectToString(rowData) << "\n";
					return NULL;
				}

			}

			curAct = as->getAction(actStr);
			if(curAct == NULL || curAct->getId() < 0) {
				cout << "ERROR:  Action not found: "<< actStr << "\n";
				return NULL;
			}

			// Check that the action can be taken in the curState:
			if(fullStateInfo) {
				if(!checkLegalAction(mdp, curState->getId(), curAct->getId())) {
					cout << "ERROR:  Illegal Action Taken: " << vectToString(rowData) << "\n";
					return NULL;
				}
				if(enforceTrans) {
					// Check that this state transition if legal:
					if(mdp->getTrans()->transitionProb(curAct->getId(), curState->getId(), nextState->getId()) == 0.0) {
						cout << "ERROR:  Illegal State Transition: " << vectToString(rowData) << "\n";
						return NULL;
					}
				}
			} else {
				bool foundPath = false;
				for(unsigned int i = 0; i < curStateDist->size(); i++) {
					if(checkLegalAction(mdp, curStateDist->at(i).getStateId(), curAct->getId())) {
						foundPath = true;
						break;
					}
				}
				if(!foundPath) {
					cout << "ERROR:  Illegal Action Taken: "<< vectToString(rowData) << "\n";
					return NULL;
				}
				// Set the next state distribution probs according to the trans probs:
				// Zero them out first:
				for(unsigned int j =0; j < nextStateDist->size(); j++) {
					nextStateDist->at(j).setProb(0);
				}
				for(unsigned int i = 0; i < curStateDist->size(); i++) {
					for(unsigned int j =0; j < nextStateDist->size(); j++) {
						nextStateDist->at(j).setProb( nextStateDist->at(j).getProb() +
								mdp->getTrans()->transitionProb(curAct->getId(), curStateDist->at(i).getStateId(), nextStateDist->at(j).getStateId()) *
								curStateDist->at(i).getProb());
					}
				}

				// Normalize and confirm some probable next state:
				float totProb = 0.0;
				for(unsigned int j = 0; j < nextStateDist->size(); j++) {
					totProb += nextStateDist->at(j).getProb();
				}
				if(totProb == 0.0) {
					cout << "ERROR:  Illegal State Transition: " << vectToString(rowData) << "\n";
					return NULL;
				}
				for(unsigned int j = 0; j < nextStateDist->size(); j++) {
					nextStateDist->at(j).setProb(nextStateDist->at(j).getProb()/totProb);
				}

			}
			if(fullStateInfo)
				pr->addStep(curAct, nextState);
			else
				pr->addStep(curAct, *nextStateDist);


			if(fullStateInfo)
				curState = nextState;
			else
				curStateDist = nextStateDist;
		}

		if(actStr == "EndMission") {
			missionEnded = 1;
		}

	}

	if(id != "NO ID" && !missionEnded)
		pr->addStep(endMissionAct, curState);


	delete(defStartState);
	return(prl);
}

bool checkLegalAction(MDP *mdp, int curStateId, int curActId) {
	vector<int> aIds;
	bool foundAction = false;
	unsigned int a;

	mdp->getTrans()->possibleActions(aIds, mdp->getActions(), curStateId);
	foundAction = false;
	for(a=0; a< aIds.size() && !foundAction; a++) {
		if(aIds[a] == curActId)
			foundAction = true;
	}

	return(foundAction);
}


StateVarList *getStateFromVector(vector< string > data, StateSpaceVarList *ss, vector<int> stateVarMap) {

	int numStateVars = ss->getNumVars();
	vector <float> stateVarVals(numStateVars, 0.0);

	float varVal;
	unsigned int varInd = 0;
	unsigned int i;

	for (i = 2; i < data.size() && varInd < stateVarMap.size(); i++) {
		sscanf(data[i].c_str(), "%f", &varVal);
		stateVarVals[stateVarMap[varInd++]] = varVal;
	}
	if(varInd < numStateVars) {
		printf("ERROR:  Wrong number of values found. Expecting %d, but got %d\n", numStateVars, varInd);
		return NULL;
	}
	return(ss->getState(stateVarVals));
}

vector<StateProb> *getStateDistFromVector(vector< string > data, StateSpaceVarList *ss, vector<int> stateVarMap, vector<bool> stateVarObserved) {

	vector<StateProb> *stateDist = new vector<StateProb>();

	int numStateVars = ss->getNumVars();
	unsigned int i;
	vector <float> stateVarVals(numStateVars, 0.0);

	float varVal;
	unsigned int varInd = 0;

	for (i = 2; i < data.size() && varInd < stateVarMap.size(); i++) {
		sscanf(data[i].c_str(), "%f", &varVal);
		stateVarVals[stateVarMap[varInd++]] = varVal;
	}
	// If we didn't find the last token at the same time as the last stateVarMap slot, then we have the wrong number of values here:
	if(i < data.size() || varInd != stateVarMap.size()) {
		printf("ERROR:  Wrong number of values found. Expecting %d, but got %d\n", (int)stateVarMap.size(), varInd);
		return NULL;
	}

	int numUnobserved = 0;
	int numVars = ss->getNumVars();
	int vInd;
	for(vInd = 0; vInd < numVars; vInd++)
		if(!stateVarObserved[vInd]) numUnobserved++;


	expandStateDist(ss, &stateVarVals, &stateVarObserved, stateDist);

	// Now loop over all values of the unknown states and distribute the prob equally:
	int s, ns = (int)stateDist->size();
	for(s=0; s < ns; s++) {
		stateDist->at(s).setProb(1/ns);
	}

	return(stateDist);
}

int expandStateDist(StateSpaceVarList *ss, vector<float> *varVals, vector<bool> *knownVars, vector<StateProb> *stateDist) {
	int vInd, unknownVInd = -1;
	for(vInd = 0; vInd < (int)knownVars->size(); vInd++) {
		if(!knownVars->at(vInd)) {
			unknownVInd = vInd;
			break;
		}
	}

	// no unknowns left add the state to the end of stateDist
	if(unknownVInd < 0) {
		int sId = ss->getState(*varVals)->getId();
		if(sId >= 0)
			stateDist->push_back(StateProb(0.0, sId));

		return 0;
	}

	// expand this variable:
	float val, max, step;
	VarDesc *vDesc = ss->getStateVarDesc(unknownVInd);
	if(vDesc != NULL) {
		val = vDesc->getMin();
		max = vDesc->getMax();
		step = vDesc->getStep();
		for(val = vDesc->getMin(); val <= max; val += step) {
			varVals->at(unknownVInd) = val;
			knownVars->at(unknownVInd) = true;
			expandStateDist(ss, varVals, knownVars, stateDist);
			knownVars->at(unknownVInd) = false;
		}

	}
	return unknownVInd;

}

IOTable writePlayRecordsToTable(vector<ActionSequence *> recs, StateSpaceVarList *ss, ActionSet *as, StateVarList *startState, bool writeRewardsOption) {
	int sId, mId, v;
	Action *action;
	StateVarList *state;
	float reward;
	vector< string > vectStr;
	string numStr;
	IOTable retTab;

	int nVar = ss->getNumVars();

	// First create the header line:
	vectStr.push_back("StudID");
	vectStr.push_back("Action");

	if(writeRewardsOption)
		vectStr.push_back("Reward");
	for(v=0; v<nVar; v++) {
		vectStr.push_back(ss->getStateVarName(v));
	}
	retTab.setColNames(vectStr);

	for(sId = 0; sId < (int)recs.size(); sId++) {
		// First start the mission (to record the start state):
		vectStr.clear();
		vectStr.push_back(recs[sId]->getId());
		vectStr.push_back("StartMission");
		if(writeRewardsOption)
			vectStr.push_back(to_string(0));
		for(v=0; v<nVar; v++) {
			vectStr.push_back(to_string(startState->getVarValue(v)));
		}
		retTab.addRow(vectStr);

		for(mId = 0; mId < recs[sId]->getNumSteps(); mId++) {
			vectStr.clear();
			action = recs[sId]->getActionAtStep(mId);
			state = (StateVarList *)(recs[sId]->getStateAtStep(mId));
			if(writeRewardsOption)
				reward = recs[sId]->getRewardAtStep(mId);

			vectStr.push_back(recs[sId]->getId());
			vectStr.push_back(action->getLabel());
			if(writeRewardsOption)
				vectStr.push_back(to_string(reward));

			for(v=0; v<nVar; v++) {
				vectStr.push_back(to_string(state->getVarValue(v)));
			}
			retTab.addRow(vectStr);
		}
	}

	return retTab;
}

string vectToString(vector< string > vect) {
	if(vect.size() == 0)
		return(string());

	string ret = vect[0];
	for(unsigned int i = 1; i< vect.size(); i++) {
		ret.append(" ");
		ret.append(vect[i]);

	}
	return ret;
}

