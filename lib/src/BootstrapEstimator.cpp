/*
 * BootstrapEstimator.cpp
 *
 *  Created on: Jun 12, 2017
 *      Author: Michelle LaMar 
 * 
 *      Copyright (C) 2017, 2018  Michelle M. LaMar

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

#include "BootstrapEstimator.h"

#define MAXLINE 128

namespace MDPLIB {


BootstrapEstimator::BootstrapEstimator(MDP_IRT_Model *mdpm) {
	mdpMod = mdpm;
	ciSpan = 0.95;
	minIterations = 500;
	maxIterations = 1000;
	changeCrit = 0.001;
	numUnderCrit = 20;
}

BootstrapEstimator::BootstrapEstimator(MDP_IRT_Model *mdpm, int min, int max, double crit, int niCrit, double cis) {
	mdpMod = mdpm;
	ciSpan = cis;
	minIterations = min;
	maxIterations = max;
	changeCrit = crit;
	numUnderCrit = niCrit;
}

int BootstrapEstimator::persListEstBootstrap(int pInd, vector< vector <ActionSequence *> *> data) {

	Param *p;
	ParamList *pl, *individPl;

	// Sanity checks and get the original parameter list specified:
	if(pInd < 0 || pInd >= mdpMod->getParams()->getLength()) return(MDP_ERROR);

	if(!mdpMod->getParams()->isParamEstPers(pInd) ) return(MDP_ERROR);
	if(mdpMod->getParams()->getParamComponent(pInd) != pc_beta ) return(MDP_ERROR);

	pl = mdpMod->getParams()->getParamSubList(pInd);

	if(pl == NULL || pl->getLength() != (int)data.size()) return(MDP_ERROR);

	// Turn off the results updating in the estimation engine and swap out the individual param list so as to not overwrite it.
	bool oldTL = mdpMod->isTrackLikelihoods();
	mdpMod->setTrackLikelihoods(false);


	// For each person, do the bootstrap estimation and update the paramList:
	unsigned int i;
	for(i=0; i < data.size(); i++) {
		individPl = new ParamList();
		mdpMod->getParams()->setParamSubList(pInd, individPl);
		p = pl->getParam(i);
		p = persEstBootstrap(p, data[i]);
		pl->setParam(i, *p);
		delete(p);
		delete(individPl);
	}

	mdpMod->getParams()->setParamSubList(pInd, pl);
	mdpMod->setTrackLikelihoods(oldTL);

	return(MDP_SUCCESS);
}


int BootstrapEstimator::persListEstBootstrap(ParamList* pl, vector< vector <ActionSequence *> *> data) {

	Param *p;

	if(pl->getLength() != (int)data.size()) {
		return(MDP_ERROR);
	}

	// For each person, do the bootstrap estimation and update the paramList:
	unsigned int i;
	for(i=0; i < data.size(); i++) {
		p = pl->getParam(i);
		p = persEstBootstrap(p, data[i]);
		pl->setParam(i, *p);
		delete(p);
	}

	return(MDP_SUCCESS);
}


int BootstrapEstimator::persListEstBootstrap(int pInd, vector<ActionSequence*> *data) {
	// Package our single task in a list for the generalized interface:
	vector< vector<ActionSequence *> *> dataList;
	unsigned int i;
	for(i=0; i < data->size(); i++) {
		dataList.push_back(new vector<ActionSequence *>(1, data->at(i)));
	}
	int res = persListEstBootstrap(pInd, dataList);

	for(i=0; i < data->size(); i++) {
		delete(dataList[i]);
	}

	return(res);
}

int BootstrapEstimator::persListEstBootstrap(ParamList* pl, vector<ActionSequence*> *data) {
	// Package our single task in a list for the generalized interface:
	vector< vector<ActionSequence *> *> dataList;
	unsigned int i;
	for(i=0; i < data->size(); i++) {
		dataList.push_back(new vector<ActionSequence *>(1, data->at(i)));
	}
	int res = persListEstBootstrap(pl, dataList);

	for(i=0; i < data->size(); i++) {
		delete(dataList[i]);
	}

	return(res);
}

Param* BootstrapEstimator::persEstBootstrap(Param *param, vector<ActionSequence *> *data) {
	unsigned int i, t, j, uci, numTasks, recLen;
	double curSE, prevSE = 2000.0;
	State *start;
	ActionSequence *as;
	vector<ActionSequence *> *simData = new vector<ActionSequence *>();  // A list of the simulated performance in the tasks.  Vector of one if only one task.
	vector< vector<ActionSequence *> *> dataList;  // This will be a list of one here because we are estimating one person at a time
	vector<SimEngine *> simEngList;
	Policy *newPol;
	vector<ParamList *> *estRes;
	ParamList *betaParams;
	double betaEst;
	char sid[MAXLINE];
	float totProb, randDraw;

//	std::size_t cacheSize = maxIterations/5;
	int betaInd = mdpMod->getParams()->getParamIndex("beta");
	if(betaInd < 0) betaInd = mdpMod->getParams()->getParamIndex("Beta");
	if(betaInd < 0) return NULL;

	accumulator_set<double, features<tag::mean, tag::variance> > estAccVar;
//	accumulator_set<double, stats<tag::tail_quantile<right> > > estAccTailR( tag::tail<right>::cache_size = cacheSize );
//	accumulator_set<double, stats<tag::tail_quantile<left> > > estAccTailL( tag::tail<left>::cache_size = cacheSize );

	// In case this function has been called before:
	seList.clear();
	estList.clear();

	// Set the sim engine to get the most up-to-date values
	numTasks = mdpMod->getNumTasks();
	if(data->size() != numTasks) {
		// Raise an error.  If only we had error handling.
		return(param);
	}

	for(t=0; t< numTasks; t++) {
		newPol = new Policy(mdpMod->getMdp(t), new DR_IRT(exp(param->value)));
		newPol->optimize();
		simEngList.push_back(new SimEngine(newPol, mdpMod->getMdp(t)->getTrans()));
	}

	uci = 0;  // This is the number of iterations that the change in se is under the criterion
	for(i=0; (int)i<maxIterations; i++) {
		// Simulate a data set
		recLen = 0;
		simData->clear();
		dataList.clear();
		for(t=0; t< numTasks; t++) {
			if(data->at(t)->getStatesObserved() == so_fully) {
				start = data->at(t)->getStartState();
			} else {
				// Randomly select from the start states:
				vector<StateProb> *startDist = data->at(t)->getStartStateDist();
				if(startDist == NULL)
					continue;  // Should we throw an error?
				randDraw = ((float)((rand() % 10000) + 1)/(float)10000);
				totProb = 0.0;
				for(j = 0; j < startDist->size(); j++) {
					totProb += startDist->at(j).getProb();
					if(randDraw <= totProb)
						break;
				}
				if(j == startDist->size()) j--;
				start = mdpMod->getMdp(t)->getStates()->getState(startDist->at(j).getStateId());
			}
			sprintf(sid, "bs_%s_%d", data->at(t)->getId().c_str(), i);
			as = simEngList[t]->simActions(string(sid), start);
			simData->push_back(as);
			recLen += as->getNumSteps();
		}
		recLenList.push_back(recLen);
		dataList.push_back(simData);

		// Clear any previous estimates:
		mdpMod->getParams()->setParamSubList(betaInd, NULL);

		// Estimate beta
		estRes = mdpMod->estimatePersonParametersOld(dataList);
		betaParams = estRes->at(0);
		assert(betaParams->getLength() == 1);
		betaEst = betaParams->getParamValue(0);
		estList.push_back(betaEst);
		estAccVar(betaEst);
		delete(mdpMod->getParams()->getParamSubList(betaInd));
//		estAccTailR(betaEst);
//		estAccTailL(betaEst);
		if((int)i > minIterations) {
			curSE = sqrt(variance(estAccVar));
			seList.push_back(curSE);
			if(abs(curSE - prevSE) < changeCrit)
				uci++;
			else
				uci = 0;
		}
		prevSE = curSE;
		if((int)uci > numUnderCrit)
			break;
	}

	// Now set all the estimate distribution vars
	param->setStdErrEst(sqrt(variance(estAccVar)));
//	param->setConfInterval(quantile(estAccTailL, quantile_probability = (1-ciSpan)/2 ),
//			quantile(estAccTailR, quantile_probability = ciSpan + (1-ciSpan)/2));
	std::sort(estList.begin(), estList.end());
	int lowInd = ceil(estList.size() * ((1-ciSpan)/2));
	int highInd = floor(estList.size() * (ciSpan + (1-ciSpan)/2));
	param->setConfInterval(estList[lowInd], estList[highInd]);

	return(param);

}


} /* namespace MDPLIB */
