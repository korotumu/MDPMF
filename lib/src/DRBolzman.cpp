/*
 * DRBolzman.cpp
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

#include "DRBolzman.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>

namespace MDPLIB {

DR_Bolzman::DR_Bolzman(float b) {
	if(b < 0)
		beta = 0.0;
	else
		beta = b;

	srand (time(NULL));
}

DR_Bolzman::~DR_Bolzman() {

}

/* getActionProbsForState
 *
 * Implements a bolzman policy: p(a|s) \propto exp(beta * Q(s,a) )
 */

int DR_Bolzman::getActionProbsForState(std::vector<double> &probs, int sId, MDP *mdp, vector<int> actionIds,
		const vector<double> &stateVals) {
	unsigned int a, na = actionIds.size();
	double sum = 0.0;

	if(probs.size() != na)
		return MDP_ERROR;

	for(a=0; a < na; a++) {
		probs[a] = exp(beta * (mdp->getActionValueForState(sId, actionIds[a], stateVals)));
		sum += probs[a];
	}
	// Normalize:
	for(a=0; a < na; a++) {
		probs[a] = probs[a]/sum;
	}

	return MDP_SUCCESS;
}

std::vector<double> DR_Bolzman::getActionProbsForState(int sId, MDP* mdp, vector<int> actionIds,
		const vector<double> &stateVals) {
	unsigned int na = actionIds.size();
	unsigned int a;
	vector<double> probs(na, 0.0);
	double sum = 0.0;

	for(a=0; a < na; a++) {
		probs[a] = exp(beta * (mdp->getActionValueForState(sId, actionIds[a], stateVals)));
		sum += probs[a];
	}
	// Normalize:
	for(a=0; a < na; a++) {
		probs[a] = probs[a]/sum;
	}

	return(probs);
}

std::vector<double> DR_Bolzman::getActionProbsForState(State* s, MDP* mdp, vector<int> actionIds,
		const vector<double> &stateVals) {
	unsigned int na = actionIds.size();
	unsigned int a;
	vector<double> probs(na, 0.0);
	double sum = 0.0;

	for(a=0; a < na; a++) {
		probs[a] = exp(beta * (mdp->getActionValueForState(s, mdp->getActions()->getAction(actionIds[a]), stateVals)));
		sum += probs[a];
	}
	// Normalize:
	for(a=0; a < na; a++) {
		probs[a] = probs[a]/sum;
	}

	return(probs);
}

Action* DR_Bolzman::selectActionForState(State* s, MDP* mdp, vector<int> actionIds, vector<double> stateVals) {

	vector<double> probs = getActionProbsForState(s, mdp, actionIds, stateVals);
	unsigned int a = 0;
	unsigned int na = actionIds.size();
	double cumProb = 0.0;
	double randDraw;

	randDraw = ((double)((rand() % 10000) + 1)/(double)10000);

	while(cumProb < randDraw && a < na) {
		cumProb += probs[a];
		a++;
	}

	return(mdp->getActions()->getAction(actionIds[a-1]));
}

} /* namespace MDPLIB */
