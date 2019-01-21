/*
 * DRMax.cpp
 *
 *  Created on: Mar 6, 2013
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

#include "DRMax.h"

namespace MDPLIB {

DR_Max::DR_Max() {
	// TODO Auto-generated constructor stub
}

DR_Max::~DR_Max() {
	// TODO Auto-generated constructor stub
}

int DR_Max::getActionProbsForState(std::vector<double> &probs, int sId,
		MDP* mdp, vector<int> actionIds, const vector<double> &stateVals) {
	// The action with the max Q-value is choosen with probability of 1.
	double maxVal = -99999.0;
	double curVal;
	unsigned int a, na = actionIds.size();
	int maxInd = 0;

	if(probs.size() != na)
		return MDP_ERROR;

	if(na == 0)
		return(MDP_SUCCESS);

	for(a=0; a < na; a++) {
		probs[a] = 0.0;
		curVal = mdp->getActionValueForState(sId, actionIds[a], stateVals);
		if(curVal > maxVal) {
			maxVal = curVal;
			maxInd = a;
		}
	}
	probs[maxInd] = 1.0;
	return(MDP_SUCCESS);
}

std::vector<double> DR_Max::getActionProbsForState(int sId, MDP* mdp, vector<int> actionIds,
		const vector<double> &stateVals) {
	// The action with the max Q-value is choosen with probability of 1.
	double maxVal = -99999.0;
	double curVal;
	int a, na = (int)actionIds.size();
	vector<double> probs(na, 0.0);
	int maxInd = 0;

	if(na <= 0)
		return(probs);

	for(a=0; a < na; a++) {
		curVal = mdp->getActionValueForState(sId, actionIds[a], stateVals);
		if(curVal > maxVal) {
			maxVal = curVal;
			maxInd = a;
		}
	}
	probs[maxInd] = 1.0;
	return(probs);
}


std::vector<double> DR_Max::getActionProbsForState(State* s, MDP* mdp, vector<int> actionIds,
		const vector<double> &stateVals) {
	// The action with the max Q-value is choosen with probability of 1.
	double maxVal = -99999.0;
	double curVal;
	vector<double> probs((int)actionIds.size(), 0.0);
	int a, na = (int)actionIds.size(), maxInd = 0;

	mdp->getActions()->startIterator();

	for(a=0; a < na; a++) {
		curVal = mdp->getActionValueForState(s, mdp->getActions()->getAction(actionIds[a]), stateVals);
		if(curVal > maxVal) {
			maxVal = curVal;
			maxInd = a;
		}
	}
	probs[maxInd] = 1.0;
	return(probs);
}

Action* DR_Max::selectActionForState(State* s, MDP* mdp, vector<int> actionIds,
		vector<double> stateVals) {
	unsigned int i;
	vector<double> actProbs = getActionProbsForState(s, mdp, actionIds, stateVals);

	for(i=0; i<actProbs.size(); i++){
		if(actProbs[i] == 1.0) {
			return(mdp->getActions()->getAction(actionIds[i]));
		}
	}
	return((Action *)0);
}


}/* namespace MDPLIB */
