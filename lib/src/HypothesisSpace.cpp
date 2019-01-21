/*
 * HypothesisSpace.cpp
 *
 *  Created on: Mar 7, 2013
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

#include "HypothesisSpace.h"

namespace MDPLIB {

HypothesisSpace::HypothesisSpace() {
	numHyp = 0;
}

HypothesisSpace::HypothesisSpace(vector<Policy> hlist, vector<float> plist) :
		hypotheses(hlist), priorProbs(plist), posteriorProbs(plist)
{
	if(hlist.size() != plist.size()) {
		numHyp = min(hlist.size(), plist.size());
	} else {
		numHyp = hlist.size();
	}
}

HypothesisSpace::~HypothesisSpace() {
	// TODO Auto-generated destructor stub
}


int HypothesisSpace::getNumHypotheses() {
	return(numHyp);
}

/* Do I want these functions?
void HypothesisSpace::setHypotheses(vector<Policy> hlist) {
}

void HypothesisSpace::setProbabilities(vector<float> plist) {
}
*/

void HypothesisSpace::addHypothesis(Policy hyp, float prob) {
	hypotheses.push_back(hyp);
	priorProbs.push_back(prob);
	posteriorProbs.push_back(prob);
	numHyp++;
}

void HypothesisSpace::normalizePriors() {
	float sum = 0.0;
	int i;

	for(i=0; i<numHyp; i++){
		sum += priorProbs[i];
	}
	for(i=0; i<numHyp; i++){
		priorProbs[i] /= sum;
	}
}

Policy* HypothesisSpace::getHypothesis(int ind) {
	if(ind >= numHyp)
		return((Policy *)0);
	return(&hypotheses[ind]);
}

float HypothesisSpace::getPriorProb(int ind) {
	if(ind >= numHyp)
		return(PROB_ERR);
	return(priorProbs[ind]);
}


float HypothesisSpace::getPosteriorProb(int ind) {
	if(ind >= numHyp)
		return(PROB_ERR);
	return(posteriorProbs[ind]);
}


int HypothesisSpace::setUniformPriors() {
	int i;
	for(i=0; i<numHyp; i++)
		priorProbs[i] = 1/numHyp;
	return MDP_SUCCESS;
}

int HypothesisSpace::setPosteriorsToPriors() {
	int i;
	if((int)posteriorProbs.size() != numHyp)
		return MDP_ERROR;

	for(i=0; i<numHyp; i++)
		priorProbs[i] = posteriorProbs[i];
	return MDP_SUCCESS;
}


int HypothesisSpace::setPosteriorsForActionSeq(ActionSequence* as) {
	double sumVal = 0.0;
	int i;
	double maxLogVal = -100000;

	for(i=0; i<numHyp; i++){
		// Need to run the optimization to set the values for this hypothesis:
		hypotheses[i].optimize();

		posteriorProbs[i] = getLogLiklihoodForActionSeq(as, i);
		//curVal = alpha * getPlayValueInWorld(pr, ws->list[i], ws->worldPolicy[i]) + log(ws->priors[i]);
		if(posteriorProbs[i] > maxLogVal)
			maxLogVal = posteriorProbs[i];
	}

	// Subtract off the max val as a constant (to keep the probabilities from going to zero), and calculate the sum:
	for(i=0; i < numHyp; i++) {
		posteriorProbs[i] = exp(posteriorProbs[i] - maxLogVal) * priorProbs[i];
		sumVal += posteriorProbs[i];
	}

	// Normalize probabilities by the sum:
	for(i=0; i < numHyp; i++) {
		posteriorProbs[i] = posteriorProbs[i]/sumVal;
	}
	return MDP_SUCCESS;
}

// Note this algorithm assumes observed states in the action sequence
double HypothesisSpace::getLogLiklihoodForActionSeq(ActionSequence* as, int hypInd) {
	double value = 0, sVal;
	int step;

	if(as->getNumSteps() == 0)
		return 0.0;

	/* First step is from the startState */
	value = log(hypotheses[hypInd].getActionProbsForState(as->getStartState())[as->getActionAtStep(0)->getId()]);

	for(step = 1; step < as->getNumSteps(); step++) {
	//	sVal = log((policy->actionProbsByState[pr->stateSeq[step]->index])->prob[pr->actionSeq[step]->index] *
	//			world->transFunc(world->context, pr->actionSeq[step], pr->stateSeq[step], pr->stateSeq[step+1]));

		sVal = log(hypotheses[hypInd].getActionProbsForState(as->getStateAtStep(step-1))[as->getActionAtStep(step)->getId()]
		                                                                                 + 0.000000001);
	//	sVal = log((policy->actionProbsByState[pr->stateSeq[step]->index])->prob[pr->actionSeq[step]->index]) ;
		value += sVal;
	}
	return(value);
}

vector< vector<double> > HypothesisSpace::getActionProbsForActionSeq(ActionSequence *as) {
	vector< vector<double> > result(as->getNumSteps(), vector<double>(numHyp, 0.0));
	int step, hypInd;

	if(as->getNumSteps() == 0)
		return result;

	/* First step is from the startState */
	for(hypInd=0; hypInd < numHyp; hypInd++) {
		result[0][hypInd] = hypotheses[hypInd].getActionProbsForState(as->getStartState())[as->getActionAtStep(0)->getId()];

		for(step = 1; step < as->getNumSteps(); step++) {
			result[step][hypInd] = hypotheses[hypInd].getActionProbsForState(as->getStateAtStep(step-1))[as->getActionAtStep(step)->getId()];
		}
	}
	return(result);

}

}/* namespace MDPLIB */
