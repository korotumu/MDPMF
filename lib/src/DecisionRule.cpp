/*
 * ActionModel.cpp
 *
 *  Created on: Jan 25, 2013
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

#include "DecisionRule.h"

namespace MDPLIB {

MDPLIB::DecisionRule::DecisionRule() {
}

MDPLIB::DecisionRule::~DecisionRule() {
}


 int MDPLIB::DecisionRule::getActionProbsForState(std::vector<double> &probs, int sId,
		MDP* mdp, vector<int> actionIds, const vector<double> &stateVals) {
	vector<double> emptyVect;
	return(MDP_SUCCESS);
}

std::vector<double> MDPLIB::DecisionRule::getActionProbsForState(State* s,
		MDP* mdp, vector<int> actionIds, const vector<double> &stateVals) {
	vector<double> emptyVect;
	return(emptyVect);
}

std::vector<double> MDPLIB::DecisionRule::getActionProbsForState(int sId,
		MDP* mdp, vector<int> actionIds, const vector<double> &stateVals) {
	vector<double> emptyVect;
	return(emptyVect);
}

Action* MDPLIB::DecisionRule::selectActionForState(State* s, MDP* mdp, vector<int> actionIds,
		vector<double> stateVals) {
	return((Action *)0);
}

Action* DecisionRule::selectActionFromProbs(State* s, MDP* mdp,
		vector<int> actionIds, vector<double> actProbs) {
	return((Action *)0);
}

int DecisionRule::selectActionForState(int sId, MDP* mdp, vector<int> actionIds,
		vector<double> stateVals) {
	return -1;
}

int DecisionRule::selectActionFromProbs(int sId, MDP* mdp,
		vector<int> actionIds, vector<double> actProbs) {
	return -1;
}


}/* namespace MDP */
