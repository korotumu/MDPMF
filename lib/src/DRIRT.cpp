/*
 * DRIRT.cpp
 *
 *  Created on: Dec 4, 2013
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

#include "DRIRT.h"
#include <cfloat>
#define MAX_PVAL (DBL_MAX/100)


namespace MDPLIB {

DR_IRT::DR_IRT(float b)
{
	// quick fix to make the ability non-negative.
	// theta = exp(b);
	theta = b;
	srand (time(NULL));
}

DR_IRT::~DR_IRT() {

}

/* getActionProbsForState
 *
 * Implements a IRT policy: p(a|s) \propto exp(theta * Q(s,a) + delta)
 * though, right now, delta = 0 for all states.
 */

int DR_IRT::getActionProbsForState(std::vector<double> &probs, int sId, MDP *mdp, vector<int> actionIds,
		const vector<double> &stateVals) {
	int a, na = (int)actionIds.size();
	double sum = 0.0, maxVal = -DBL_MAX;

	if((int)probs.size() != na)
		return MDP_ERROR;

	// To keep this from blowing up, we are going to get the action values first and then divide everything by exp(theta * max(actVal))
	// which is the same as subtracting maxVal from each actual value (when exponentiated)
	for(a=0; a < na; a++) {
		probs[a] = mdp->getActionValueForState(sId, actionIds[a], stateVals);
		if(probs[a] > maxVal)
			maxVal = probs[a];
	}
	for(a=0; a < na; a++) {
		probs[a] =exp(theta * (probs[a] - maxVal));
		sum += probs[a];
	}
	// Normalize:
	for(a=0; a < na; a++) {
		probs[a] = probs[a]/sum;
	}

	return MDP_SUCCESS;
}

std::vector<double> DR_IRT::getActionProbsForState(int sId, MDP* mdp, vector<int> actionIds,
		const vector<double> &stateVals) {
	int na = (int)actionIds.size();
	int a;
	vector<double> probs(na, 0.0);
	double sum = 0.0, maxVal = -DBL_MAX;

	// To keep this from blowing up, we are going to get the action values first and then divide everything by exp(theta * max(actVal))
	// which is the same as subtracting maxVal from each actual value (when exponentiated)
	for(a=0; a < na; a++) {
		probs[a] = mdp->getActionValueForState(sId, actionIds[a], stateVals);
		if(probs[a] > maxVal)
			maxVal = probs[a];
	}
	for(a=0; a < na; a++) {
		probs[a] =exp(theta * (probs[a] - maxVal));
		sum += probs[a];
	}
	// Normalize:
	for(a=0; a < na; a++) {
		probs[a] = probs[a]/sum;
	}


	return(probs);
}

std::vector<double> DR_IRT::getActionProbsForState(State* s, MDP* mdp, vector<int> actionIds,
		const vector<double> &stateVals) {
	int na = (int)actionIds.size();
	int a;
	vector<double> probs(na, 0.0);
	double sum = 0.0, maxVal = -DBL_MAX;

	// To keep this from blowing up, we are going to get the action values first and then divide everything by exp(theta * max(actVal))
	// which is the same as subtracting maxVal from each actual value (when exponentiated)
	for(a=0; a < na; a++) {
		probs[a] = mdp->getActionValueForState(s, mdp->getActions()->getAction(actionIds[a]), stateVals);
		if(probs[a] > maxVal)
			maxVal = probs[a];
	}
	for(a=0; a < na; a++) {
		probs[a] =exp(theta * (probs[a] - maxVal));
		sum += probs[a];
	};

	return(probs);
}

Action* DR_IRT::selectActionForState(State* s, MDP* mdp, vector<int> actionIds, vector<double> stateVals) {

	vector<double> probs = getActionProbsForState(s, mdp, actionIds, stateVals);
	int a = 0;
	int na = (int)actionIds.size();
	double cumProb = 0.0;
	double randDraw;

	randDraw = ((double)((rand() % 10000) + 1)/(double)10000);

	while(cumProb < randDraw && a < na) {
		cumProb += probs[a];
		a++;
	}

	return(mdp->getActions()->getAction(actionIds[a-1]));
}


int DR_IRT::selectActionForState(int sId, MDP* mdp, vector<int> actionIds,
		vector<double> stateVals) {

	vector<double> probs = getActionProbsForState(sId, mdp, actionIds, stateVals);
	int a = 0;
	int na = (int)actionIds.size();
	double cumProb = 0.0;
	double randDraw;

	randDraw = ((double)((rand() % 10000) + 1)/(double)10000);

	while(cumProb < randDraw && a < na) {
		cumProb += probs[a];
		a++;
	}

	return(actionIds[a-1]);
}

Action* DR_IRT::selectActionFromProbs(State* s, MDP* mdp,
		vector<int> actionIds, vector<double> actProbs) {
	int a = 0;
	int na = (int)actionIds.size();
	double cumProb = 0.0;
	double randDraw;

	randDraw = ((double)((rand() % 10000) + 1)/(double)10000);

	while(cumProb < randDraw && a < na) {
		cumProb += actProbs[a];
		a++;
	}

	return(mdp->getActions()->getAction(actionIds[a-1]));
}


int DR_IRT::selectActionFromProbs(int sId, MDP* mdp,
		vector<int> actionIds, vector<double> actProbs) {
	int a = 0;
	int na = (int)actionIds.size();
	double cumProb = 0.0;
	double randDraw;

	randDraw = ((double)((rand() % 10000) + 1)/(double)10000);

	while(cumProb < randDraw && a < na) {
		cumProb += actProbs[a];
		a++;
	}

	return(actionIds[a-1]);
}


} /* namespace MDPLIB */


