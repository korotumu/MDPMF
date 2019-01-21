/*
 * MDPIRTModel.h
 *
 *  Created on: Dec 5, 2013
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

#ifndef MDPIRTMODEL_H_
#define MDPIRTMODEL_H_

#include "MDP.h"
#include "HypothesisSpace.h"

#include "DRMax.h"
#include "DRIRT.h"
#include "RewardTable.h"
#include "TransTable.h"
#include "ParamList.h"
#include "ModelSpec.h"
#include <nlopt.hpp>
#include <assert.h>
#include <stdio.h>

// #include <gsl.h>

#define PI 3.14159265359
#define SQRT_2_PI 2.506628

// Types of person estimation:
#define MDPIRT_MLE 0
#define MDPIRT_MAP 1
#define MDPIRT_EAP 2

namespace MDPLIB {

class MDP_IRT_Model {
public:
	enum estErrCalc_t {none, boot};  // How to calculate the estimation errors

	MDP_IRT_Model(MDP *mdp);
	MDP_IRT_Model(vector <MDP *> mdpList);
	MDP_IRT_Model(vector<MDP *> mdpArgs, ParamList *pList);
	MDP_IRT_Model(vector <MDP *> mdpList, ParamList *pList, vector< vector<bool> > dMat);
	virtual ~MDP_IRT_Model();

	ParamList *estimateParameters(vector< vector <ActionSequence *> *> data);
	ParamList *estimateParameters(vector <ActionSequence *> *data);
	ParamList *estimateModelParameters(vector< vector <ActionSequence *> *> data);
	vector<ParamList *> *estimatePersonParametersOld(vector< vector <ActionSequence *> *> data);
	ParamList *estimatePersonParameters(vector< vector <ActionSequence *> *> data);
	double marginalLogLiklihood(vector< vector < ActionSequence *> *> data);

	// Single task interface, maintained for back compatibility:
	ParamList *estimateModelParameters(vector <ActionSequence *> *data);
	vector<ParamList *> *estimatePersonParametersOld(vector <ActionSequence *> *data);


	 MDP *getMdp(int i) const {
		return mdpList[i];
	}

	void setMdp(int i, MDP *mdp) {
		if(i < (int)numTasks)
			this->mdpList[i] = mdp;
		else
			this->mdpList.push_back(mdp);
	}

	float getBetaMu() const {
		return betaMu;
	}

	void setBetaMu(float betaMu) {
		this->betaMu = betaMu;
	}

	float getBetaSigma() const {
		return betaSigma;
	}

	void setBetaSigma(float betaSigma);

	float pointLogLiklihood(float x, int taskId, ActionSequence *data);
	double recordConditionalProbability(Policy *p, ActionSequence *as);
	double recordConditionalLogLikelihood(Policy *p, ActionSequence *as);
	double poRecordConditionalProbability(Policy *p, ActionSequence *as);
	double poRecordConditionalLogLikelihood(Policy *p, ActionSequence *as);
	double recordMarginalProbability(int taskId, ActionSequence *as);
	double logRecordMarginalProbability(int taskId, ActionSequence *as);
	double logOfNormProb(double mu, double sigma, double x);
	double logOfLogNormProb(double mu, double sigma, double x);
	void getLogLikStats(double &ell, double &vll, float x, vector<ActionSequence *> *asList);
	void stateLogLikStats(Policy *pol, State *s, double &e, double &var);
	void stateDistLogLikStats(Policy *pol, vector<StateProb> *sd, double &e, double &var);

	int getNumTasks() const {
		return numTasks;
	}

	ParamList *getParams() const {
		return params;
	}

	void setParams(ParamList *pl) {
		this->params = pl;
		setBetaMu(pl->getParamValue(0));
		setBetaSigma(pl->getParamValue(1));
	}

	float getGlobalOptCrit() const {
		return globalOptCrit;
	}

	void setGlobalOptCrit(float globalOptCrit) {
		this->globalOptCrit = globalOptCrit;
	}

	float getLocalOptCrit() const {
		return localOptCrit;
	}

	void setLocalOptCrit(float localOptCrit) {
		this->localOptCrit = localOptCrit;
	}

	int getNumGlobalIterations() const {
		return numGlobalIterations;
	}

	void setNumGlobalIterations(int numGlobalIterations) {
		this->numGlobalIterations = numGlobalIterations;
	}

	int getNumLocalInterations() const {
		return numLocalInterations;
	}

	void setNumLocalInterations(int numLocalInterations) {
		this->numLocalInterations = numLocalInterations;
	}

	int getPersonEstType() const {
		return personEstType;
	}

	void setPersonEstType(int personEstType) {
		if(personEstType == MDPIRT_MAP || personEstType == MDPIRT_MLE || personEstType == MDPIRT_EAP)
			this->personEstType = personEstType;
	}

	bool isUseLogNormDist() const {
		return useLogNormDist;
	}

	void setUseLogNormDist(bool useLogNormDist) {
		this->useLogNormDist = useLogNormDist;
	}

	bool isUseOptScore() const {
		return useOptScore;
	}

	void setUseOptScore(bool useOptScore) {
		this->useOptScore = useOptScore;
	}

	int getMaxPolicyIterations() const {
		return maxPolicyIterations;
	}

	void setMaxPolicyIterations(int maxPolicyIterations);

	bool isVerbose() const {
		return verbose;
	}

	void setVerbose(bool verbose) {
		this->verbose = verbose;
	}

	int getBetaMuParamIndex() const {
		return betaMuParamIndex;
	}

	void setBetaMuParamIndex(int betaMuParamIndex) {
		this->betaMuParamIndex = betaMuParamIndex;
	}

	int getBetaSigmaParamIndex() const {
		return betaSigmaParamIndex;
	}

	void setBetaSigmaParamIndex(int betaSigmaParamIndex) {
		this->betaSigmaParamIndex = betaSigmaParamIndex;
	}

	bool isTrackLikelihoods() const {
		return trackLikelihoods;
	}

	void setTrackLikelihoods(bool trackLikelihoods) {
		this->trackLikelihoods = trackLikelihoods;
	}

/*
	estErrCalc_t getEstErrCalc() const {
		return estErrCalc;
	}

	void setEstErrCalc(estErrCalc_t estErrCalc) {
		this->estErrCalc = estErrCalc;
	} */

private:
	float betaMu;
	float betaSigma;
	int betaParamIndex;
	int betaMuParamIndex;
	int betaSigmaParamIndex;
	unsigned int numTasks;
	unsigned int numParams;
	ParamList *params;
	vector<int> numTransParam;
	vector<int> numRewardParam;
	vector <MDP *> mdpList;
	DR_IRT irtDR;
	DR_Max maxDR;
	vector< vector<bool> > designMat;
	vector<Policy *> irtPolicy;
	int numQuadPoints;
	float intHalfRange;  // Half of the range over which the integral will be evaluated, in sd units
	float betaSearchRange; // Half of the range over which we will optimize beta
	vector<float> quadPoints;
	vector<float> quadWeights;
	vector<float> dNormProbs;
	vector<DR_IRT *> quadIrtDR;
	vector< vector<Policy *> > quadIrtPolicy;

	// Configuration variables
	int personEstType;
	bool useOptScore;
	int numGlobalIterations;
	int numLocalInterations;
	float globalOptCrit;
	float localOptCrit;
	bool useLogNormDist;
	int maxPolicyIterations;
	bool verbose;
	bool trackLikelihoods;
//	estErrCalc_t estErrCalc;

	void init(void);
	void setDNormProbs(void);
	void initIrtPolicies(void);
	void setUpParams(void);
};

} /* namespace MDPLIB */



// For nlopt


typedef struct {
    double a, b;
} mdpirt_constraint_data;

typedef struct {
	MDPLIB::StateSpace *states;
	MDPLIB::ActionSet *actions;
	MDPLIB::ParamList *params;
	MDPLIB::MDP_IRT_Model *miModel;
	double discount;
	int stopInd;
	vector< vector<MDPLIB::ActionSequence *> *> *actSeqList;
	vector<int> *useRec;
} mdpirt_opt_data;


#endif /* MDPIRTMODEL_H_ */
