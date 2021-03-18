/*
 * ActionEffectTable.cpp
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

#include "ActionEffectTable.h"
#include <regex>

typedef boost::tokenizer<boost::escaped_list_separator<char> > csv_tokenizer;

void stringToFloatVect(vector<string> sv, vector<float> *fv, unsigned int index);


MDPLIB::ActionEffectTable::ActionEffectTable()
{
	numActions = 0;
	numStateVars = 0;
	effectTable = NULL;
//	errExpr.SetExpr("ERROR");
}

// This constructor copies an existing AET
MDPLIB::ActionEffectTable::ActionEffectTable(const MDPLIB::ActionEffectTable& aet) : actionList(aet.actionList), stateVarList(aet.stateVarList), startVarVals(aet.startVarVals)
{
	numActions = aet.numActions;
	numStateVars = aet.numStateVars;
	effectTable = new vector< vector <ActionEffectProb> >(*(aet.effectTable));
}

MDPLIB::ActionEffectTable::~ActionEffectTable() {
	unsigned int v;
	for(v=0; v< numStateVars; v++) {
		delete(stateVarList[v]);
	}
}

/*
 * Note that the action effect table is expected to have state vars in the columns
 * and actions in the rows.  Both rows and columns should have headers.  All entries
 * must have a value.
 */
int MDPLIB::ActionEffectTable::readFromIOTable(IOTable iot) {
	string lastAct;
	vector <string> tmpStrings;
	vector <string> varNames;
	vector <string> expr_strings;
	vector <string> action_strings;
	vector <bool> isNullOp;
	regex hasAlpha("[a-zA-Z]");
	vector <float> varMin;
	vector <float> varMax;
	float prob;
	string tmp_str, expr_str, pExpr_str;
	int cInd = 0, aInd = 0, rowNum = 0;
	unsigned int v, r, numRows = iot.getNumRows();

	varNames = iot.getColNames();
	if(varNames.size() < 3 || numRows < 4) {
		return MDP_ERROR;
	}

	// Remove the first two columns (action and prob)
	varNames.erase(varNames.begin());
	varNames.erase(varNames.begin());
	numStateVars = varNames.size();

	// The first three lines should define the variable space and the start condition: varMin, varMax, startVal
	stringToFloatVect(iot.getRow(rowNum++), &varMin, 2);
	stringToFloatVect(iot.getRow(rowNum++), &varMax, 2);
	stringToFloatVect(iot.getRow(rowNum++), &startVarVals,2);

	stateVarList.clear();
	for(v=0; v < numStateVars; v++) {
		stateVarList.push_back(new VarDesc(varNames[v], DISCRETE, varMin[v], varMax[v]));
	}

	lastAct = "";
	action_strings = iot.getCol(0);
	for(v = 3; v <action_strings.size(); v++) {
		if(action_strings[v] != lastAct){
			actionList.push_back(action_strings[v]);
			lastAct = action_strings[v];
		}
	}
	numActions = actionList.size();

	// Allocate the table:
	effectTable = new vector<vector <ActionEffectProb>  >(numActions,  vector<ActionEffectProb>());
	bool nullOpFlag = false;

	lastAct = "";
	aInd = -1;
	for(r = 3; r < numRows; r++) {
		tmpStrings = iot.getRow(r);
		if(tmpStrings[0] != lastAct) {
			lastAct = tmpStrings[0];
			aInd++;
		}

		tmpStrings.erase(tmpStrings.begin());

		cInd = -1; // to allow for probability column
		for (const auto &t : tmpStrings) {
			// The first token should be the probability
			if(cInd < 0) {
				// if it is purely numeric, convert to a float, otherwise save the expression
				if(!(std::regex_search(t, hasAlpha)))
					sscanf(t.c_str(), "%f", &prob);
				else {
					pExpr_str = t.c_str();
					prob = -1.0;
				}
				expr_strings.clear();
				isNullOp.clear();

			} else {
				if(cInd < (int)numStateVars) {
					nullOpFlag = false;
					if(t.find("::") == 0) {
						// This is an absolute, not relative setting
						expr_str = t.substr(2);
					} else {
						if(strcmp(t.c_str(),"0") == 0) // The expression is to not change the var
							nullOpFlag = true;
						expr_str = varNames[cInd] + " + (" + t + ")";
					}
					expr_strings.push_back(expr_str);
					isNullOp.push_back(nullOpFlag);
				}
			}
			cInd++;
		}
		assert(expr_strings.size() == numStateVars);
		if(prob >=0)
			(*effectTable)[aInd].push_back(ActionEffectProb(prob, expr_strings, isNullOp));
		else
			(*effectTable)[aInd].push_back(ActionEffectProb(pExpr_str, expr_strings, isNullOp));
	}

	return MDP_SUCCESS;

}

void MDPLIB::ActionEffectTable::defineAEVar(string varName, double *valPtr) {
	unsigned int a, v, p;

	for(a = 0; a < numActions; a++) {
		for(p = 0; p < (*effectTable)[a].size(); p++) {
			for(v = 0; v < numStateVars; v++) {
				(*effectTable)[a][p].stateEqVect[v].DefineVar(varName, valPtr);
			}
			(*effectTable)[a][p].probEq.DefineVar(varName, valPtr);
		}
	}
}

vector<MDPLIB::ActionEffectProb> *MDPLIB::ActionEffectTable::getActionEffects(int actionIndex) {

	if(actionIndex >= (int)numActions)
		throw(AT_ERROR_VALUE);
	else
		return(&( effectTable->at(actionIndex) ) );
}

void MDPLIB::ActionEffectTable::normalizeActionProbabilities() {
	for (unsigned int i = 0; i < numActions; i++) {
		float prob_sum = 0;
		vector<ActionEffectProb> aepVector = (*effectTable)[i];
		for (unsigned int j = 0; j < aepVector.size(); j++) {
			prob_sum += aepVector[j].prob;
		}
		if (prob_sum != 1.0) {
			cerr<<"Normalizing probabilities for "<<actionList[i]<<"..."<<endl;
			for (unsigned int j = 0; j < aepVector.size(); j++) {
				ActionEffectProb normalizedAEP = aepVector[j];
				normalizedAEP.prob = normalizedAEP.prob / prob_sum;
				aepVector[j] = normalizedAEP;
			}
		}
	}
}

bool MDPLIB::ActionEffectTable::isCompatibleAET(ActionEffectTable* aet) {
	// Check the state space
	if(numStateVars != aet->getNumStateVars()) {
		cout << "StateVar Num mismatch. \n";
		return false;
	}

	vector<VarDesc*> varList = aet->getStateVarList();
	if(varList.size() != stateVarList.size()){
		cout << "StateVar List Num mismatch. \n";
		return false;
	}
	for(unsigned int i = 0; i < varList.size(); i++) {
		if(!(varList[i]->getName() == stateVarList[i]->getName())) {
			cout << "StateVar " << varList[i]->getName() << " not equal to: " << stateVarList[i]->getName() << "\n";
			return false;
		}
		if(varList[i]->getType() != stateVarList[i]->getType()) {
			cout << "StateVar " << varList[i]->getName() << " type mismatch.\n";
			return false;
		}
		if(varList[i]->getMax() != stateVarList[i]->getMax()) {
			cout << "StateVar " << varList[i]->getName() << " max mismatch.\n";
			return false;
		}
		if(varList[i]->getMin() != stateVarList[i]->getMin()) {
			cout << "StateVar " << varList[i]->getName() << " min mismatch.\n";
			return false;
		}
		if(varList[i]->getStep() != stateVarList[i]->getStep()) {
			cout << "StateVar " << varList[i]->getName() << " step mismatch.\n";
			return false;
		}
	}
	// Check the action set
	if(numActions != aet->getNumActions()){
		cout << "Action List Num mismatch. \n";
		return false;
	}
	vector<string> actions = aet->getActionList();
	for(unsigned int i = 0; i < actionList.size(); i++) {
		if(!(actions[i] == actionList[i])) {
			cout << "Action " << actions[i] << " not equal to: " << actionList[i] << "\n";
			return false;
		}
	}

	// Check the initial state
	vector<float> sVals = aet->getStartVarVals();
	for(unsigned int i = 0; i < startVarVals.size(); i++) {
		if(sVals[i] != startVarVals[i]){
			cout << "StateVar " << varList[i]->getName() << " initial value mismatch.\n";
			return false;
		}
	}
	return true;
}

void stringToFloatVect(vector<string> sv, vector<float> *fv, unsigned int index) {
	float f;
	fv->clear();
	unsigned int i, n = sv.size();
	for(i = index; i < n; i++) {
		sscanf(sv[i].c_str(), "%f", &f);
		fv->push_back(f);
	}
}
