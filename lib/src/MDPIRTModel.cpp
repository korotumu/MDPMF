/*
 * MDPIRTModel.cpp
 *
 *  Created on: Dec 5, 2013
 *      Author: Michelle LaMar
 *
 *          Copyright (C) 2013, 2014  Michelle M. LaMar

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

#include "MDPIRTModel.h"
#include <limits>
#include <iostream>

int mdpOpt_count;

namespace MDPLIB {

bool writeLikValues = 0;
char mlikValFile[] = "ModLikelihoodMap.txt";
char plikValFile[] = "PersLikelihoodMap.txt";
FILE *likVal_FPTR = NULL;

double getPopLogLiklihood(unsigned int n, double const *x, double *grad, void *my_func_data);
double getPersonLogLiklihood(unsigned int n, double const *x, double *grad, void *input_data);
double getPersonLogPosterior(unsigned int n, double const *x, double *grad, void *input_data);
void vectorRange(vector<int> &vect, int sp, int ep);

// Constructor delegates.  Woot.
MDP_IRT_Model::MDP_IRT_Model(MDP *mdpPtr)
: MDP_IRT_Model(vector<MDP *>(1, mdpPtr), NULL, vector< vector<bool> >(1, vector<bool>())) {}

MDP_IRT_Model::MDP_IRT_Model(vector<MDP *> mdpArgs)
: MDP_IRT_Model(mdpArgs, NULL, vector< vector<bool> >(mdpArgs.size(), vector<bool>())) {}

MDP_IRT_Model::MDP_IRT_Model(vector<MDP *> mdpArgs, ParamList *pList)
: MDP_IRT_Model(mdpArgs, pList, vector< vector<bool> >(mdpArgs.size(), vector<bool>(pList == NULL ? 0 : pList->getLength(), 1))) {}

void vectorRange(vector<int> &vect, int sp, int ep) {
	vect.clear();
	int i;
	for(i = sp; i <= ep; i++) {
		vect.push_back(i);
	}
}

// This is the most general constructor which includes a set of mdp classes, each of which describe a
// separate task, a parameter list, and a design matrix.  The design matrix specifies which parameters apply
// to which tasks.

MDP_IRT_Model::MDP_IRT_Model(vector<MDP *> mdpArgs, ParamList *pList, vector< vector<bool> > dMat)
: irtDR(0.0), maxDR(), designMat(dMat.size(), vector<bool>())
{
	betaParamIndex = -1;
	betaMu = 0;
	betaSigma = 1;

	if(pList == NULL) {
		// We'll set it up ourselves:
		params = new ParamList();
	} else {
		params = pList;
	}

	this->setUpParams();
	numParams = params->getLength();
	assert(numParams >= 2);
	assert(betaParamIndex >=0);

	betaMuParamIndex = params->getHyperParamAIndex(betaParamIndex);
	betaSigmaParamIndex = params->getHyperParamBIndex(betaParamIndex);
	if(betaMuParamIndex < 0 || betaSigmaParamIndex < 0) {  // TODO: Need to deal with only one being defined ..
		// Assuming that beta is fixed ...
		betaMu = params->getParamValue(betaParamIndex);
		betaSigma = 1;
	} else {
		betaMu = params->getParamValue(betaMuParamIndex);
		betaSigma = params->getParamValue(betaSigmaParamIndex);
	}

	numTasks = mdpArgs.size();
	assert(dMat.size() == numTasks);

	unsigned int i, j;
	vector<int> tParamInds;
	vector<int> rParamInds;
//	int tmpPNum;  delete?
	for(i = 0; i < numTasks; i++) {
		mdpList.push_back(mdpArgs[i]);
		irtPolicy.push_back(new Policy(mdpArgs[i], &irtDR));
		if(dMat[i].size() == numParams) {
			designMat[i].assign(dMat[i].begin(), dMat[i].end());
		} else {
			// If it is not set yet, we go ahead and assume that all parameters apply to this task
			if(dMat[i].size() > 0) {
				printf("WARNING: Design matrix mismatch.  For task %d, %d parameters specified and %d needed.  Setting all parameters to true.\n",
						(int)i, (int)dMat[i].size(), (int)numParams);
			}
			designMat[i].assign(numParams, 1);
		}

		// Use the component setting in the ParamList to parsel out the parameters
		tParamInds.clear();
		rParamInds.clear();
		for(j=2; j < numParams; j++) {
			if(designMat[i][j]) {
				if(params->getParamComponent(j) == pc_trans)
					tParamInds.push_back(j);
				if(params->getParamComponent(j) == pc_reward)
					rParamInds.push_back(j);
			}
		}
		numTransParam.push_back(tParamInds.size());
		numRewardParam.push_back(rParamInds.size());
		// TODO: Check that the trans and reward components agree with the param list!

		// Now set the transition and reward parameter lists to the global one:
		mdpArgs[i]->getTrans()->setParamList(params, tParamInds);
		mdpArgs[i]->getReward()->setParamList(params, rParamInds);
	}

	useLogNormDist = true;
	personEstType = MDPIRT_MAP;
	useOptScore = 0;
	numQuadPoints = 9;
	intHalfRange = 3;
	betaSearchRange = 5;
	maxPolicyIterations = 1000;
	verbose = false;
	trackLikelihoods = true;

	// set defaults:
	numGlobalIterations = 2000;
	numLocalInterations = 100;
	globalOptCrit = 0.1;
	localOptCrit = 0.01;

	this->init();
}

// initialize default param list in the case that no param list is passed into the constructor
void MDP_IRT_Model::setUpParams(){
	int hpa_ind, hpb_ind;
	int m_ind, s_ind;

	// Check to see if beta is already in the params list, add it if not
	betaParamIndex = params->getParamIndex("beta");
	if(betaParamIndex < 0) betaParamIndex = params->getParamIndex("Beta");
	if(betaParamIndex < 0) {
		// Giving a broad distribution for starting ...
		hpa_ind = params->addConstant("betaMu_hyperParamA", betaMu - 10);
		hpb_ind = params->addConstant("betaMu_hyperParamB", betaMu + 10);
		m_ind = params->addParam("betaMu", betaMu, pt_estPop, pc_beta, pd_unif, hpa_ind, hpb_ind);
		hpa_ind = params->addConstant("betaSigma_hyperParamA", 0.01);
		hpb_ind = params->addConstant("betaSigma_hyperParamB", 5.0);
		s_ind = params->addParam("betaSigma", betaSigma, pt_estPop, pc_beta, pd_unif, hpa_ind, hpb_ind);
		betaParamIndex = params->addParam("beta", betaMu, pt_estPers, pc_beta, pd_lnorm, m_ind, s_ind);
	}

	params->setParamComponent(betaParamIndex,pc_beta);
	params->setParamComponent(params->getHyperParamAIndex(betaParamIndex), pc_beta);
	params->setParamComponent(params->getHyperParamBIndex(betaParamIndex), pc_beta);


}

void MDP_IRT_Model::init(){

	// If we are using optScores, optimize the IRT policy using the optimal policy (maxDR) to set the state values:
	if(useOptScore) {
		unsigned int i;
		for(i = 0; i < numTasks; i++) {
			if(irtPolicy[i]->optimize() == MDP_ERROR)
				printf("Policy failed to converge for log beta %.4f.\n", ((DR_IRT *)irtPolicy[i]->getDecisionRule())->getLogTheta());
		}
	}

	// Set up for quadrature:
	// TODO: Fix this to use gauss-hermite quadrature
	quadPoints.push_back(-0.9681602395076260898355762);
	quadPoints.push_back(-0.8360311073266357942994298);
	quadPoints.push_back(-0.6133714327005903973087020);
	quadPoints.push_back(-0.3242534234038089290385380);
	quadPoints.push_back(0.0);
	quadPoints.push_back(0.3242534234038089290385380);
	quadPoints.push_back(0.6133714327005903973087020);
	quadPoints.push_back(0.8360311073266357942994298);
	quadPoints.push_back(0.9681602395076260898355762);

	quadWeights.push_back(0.0812743883615744119718922);
	quadWeights.push_back(0.1806481606948574040584720);
	quadWeights.push_back(0.2606106964029354623187429);
	quadWeights.push_back(0.3123470770400028400686304);
	quadWeights.push_back(0.3302393550012597631645251);
	quadWeights.push_back(0.3123470770400028400686304);
	quadWeights.push_back(0.2606106964029354623187429);
	quadWeights.push_back(0.1806481606948574040584720);
	quadWeights.push_back(0.0812743883615744119718922);

	setDNormProbs();
	initIrtPolicies();

	// Add the likelihood result parameters to the paramList
	if(trackLikelihoods) {
		params->addParam("logLikelihood", 0.0, pt_result, pc_diagnostic);
		params->addParam("E_logLikelihood", 0.0, pt_result, pc_diagnostic);
		params->addParam("V_logLikelihood", 0.0, pt_result, pc_diagnostic);
	}
}

MDP_IRT_Model::~MDP_IRT_Model() {

	// TODO Auto-generated destructor stub
}

void MDP_IRT_Model::setMaxPolicyIterations(int maxPolicyIterations) {
	this->maxPolicyIterations = maxPolicyIterations;
	unsigned int i;
	int q;
	for(i = 0; i < numTasks; i++) {
		irtPolicy[i]->setMaxIters(maxPolicyIterations);
	}
	for(q=0; q < numQuadPoints; q++){
		for(i = 0; i < numTasks; i++) {
			quadIrtPolicy[q][i]->setMaxIters(maxPolicyIterations);
		}
	}
}

void MDP_IRT_Model::setDNormProbs() {
	int q;
	dNormProbs.clear();

	for(q=0; q < numQuadPoints; q++){
//		dNormProbs.push_back((1.0/(betaSigma *sqrt(2*PI))) * exp( (-1.0/2.0)*pow((double)quadPoints[q] * (double)intHalfRange / (double)betaSigma, 2)));

		dNormProbs.push_back((1.0/sqrt(2*PI)) * exp( (-1.0/2.0)*pow((double)quadPoints[q] * (double)intHalfRange , 2)));
	}
}


void MDP_IRT_Model::initIrtPolicies() {
	unsigned int i;
	int q;

	quadIrtDR.clear();

	// We copy the already optimized main irtPolicy:
	quadIrtPolicy.assign(numQuadPoints, vector<Policy *>());
	for(q=0; q < numQuadPoints; q++){
		quadIrtDR.push_back(new DR_IRT(0.0));
		quadIrtPolicy[q].clear();
		for(i = 0; i < numTasks; i++) {
			quadIrtPolicy[q].push_back(new Policy(irtPolicy[i]));
		}
	}
}

void MDP_IRT_Model::setBetaSigma(float betaSigma) {
	this->betaSigma = betaSigma;

	int sigInd = params->getParamIndex("betaSigma");
	if(sigInd > 0) {
		params->setParamValue(sigInd, betaSigma);
	}
	// Need to reset the normal probs:
//	setDNormProbs();
}

/***************************************************************************************************
 * estimateModelParameters - Based on the parameter list, estimate the population-level parameters,
 *                           marginalizing over the person-level parameters, and then estimate the
 *                           person parameters.
 */
ParamList *MDP_IRT_Model::estimateParameters(vector< vector<ActionSequence*> *> data) {
	int numPersParams = 0;
	int numPopParams = 0;
	int i;
	ParamList *estRes = NULL;

	for(i=0; i<params->getLength(); i++) {
		if(params->isParamEstPers(i))
			numPersParams++;
		if(params->isParamEstPop(i))
			numPopParams++;
	}

	if(numPopParams > 0) {
		if(numPersParams > 0) {
			// Do the population estimation using MML
			// For now we can only marginalize over 1 person parameter.
			if(numPersParams > 1) {
				printf("WARNING: Can only run MML over a single person-level parameter.  Second parameter will be considered fixed for population level estimation.\n");
			}
			estRes = estimateModelParameters(data);

		} else {
			// Do a straight MLE estimation of the pop params
			//TODO: figure out how this is different from the MML case.

			estRes = estimateModelParameters(data);
		}
	}

	if(estRes == NULL) {
		printf("ERROR: Failed population parameter estimation.\n");
		return(estRes);
	}

	if(numPersParams > 0) {
		// Estimate the person parameters assuming the population-level parameters are fixed
		estRes = estimatePersonParameters(data);
	}

	return(params);
}


ParamList *MDP_IRT_Model::estimateParameters(vector<ActionSequence*> *data) {
	vector< vector<ActionSequence *> *> dataList;
	int i;
	for(i=0; i < (int)data->size(); i++) {
		dataList.push_back(new vector<ActionSequence *>(1, data->at(i)));
	}
	ParamList *res = estimateParameters(dataList);
	for(i=0; i < (int)data->size(); i++) {
		delete(dataList[i]);
	}

	return(res);
}

ParamList *MDP_IRT_Model::estimateModelParameters(vector<ActionSequence*> *data) {
	vector< vector<ActionSequence *> *> dataList;
	int i;
	for(i=0; i < (int)data->size(); i++) {
		dataList.push_back(new vector<ActionSequence *>(1, data->at(i)));
	}
	ParamList *res = estimateModelParameters(dataList);
	for(i=0; i < (int)data->size(); i++) {
		delete(dataList[i]);
	}

	return(res);
}


ParamList *MDP_IRT_Model::estimateModelParameters(vector< vector<ActionSequence*> *> data) {
	int numEstParams = 0;
	int i,j, llInd;
	vector<double> lb;
	vector<double> ub;
	std::vector<double> *xPtr = new vector<double>();
	double maxf = 0;
	nlopt::result result;

	for(i=0; i<params->getLength(); i++) {
		if(params->isParamEstPop(i))
			numEstParams++;
	}
	if(numEstParams == 0)  // Nothing to do here.
		return(params);

	if(writeLikValues) {
		likVal_FPTR = fopen(mlikValFile, "w");
	}

	mdpirt_opt_data mdpOptDat;

	// Set up the MDP elements in the optimization data structure:

	mdpOptDat.params = params;
	mdpOptDat.actSeqList = &data;
	mdpOptDat.miModel = this;

	//	mdpOptDat.useRec =  new vector<int>(playRecs->size(), 1);

	// First run a global optimizer:
	try{
		mdpOpt_count = 0;
		nlopt::opt optg(nlopt::GN_ISRES, numEstParams);
		//	nlopt::opt opt(nlopt::GN_DIRECT_L, modParams->getLength());

		optg.set_max_objective(getPopLogLiklihood, (void *)&mdpOptDat);

		// Set the constraints according to the paramList:
		// Note, for some optimizers these need to be reasonable ranges (not -inf to inf)

		lb.clear();
		ub.clear();

		for(i=0; i< params->getLength(); i++) {
			if(params->isParamEstPop(i)) {
				lb.push_back(params->getMinValue(i));
				ub.push_back(params->getMaxValue(i));
			}
		}

		optg.set_lower_bounds(lb);
		optg.set_upper_bounds(ub);


		optg.set_maxeval(numGlobalIterations);
		optg.set_xtol_abs(globalOptCrit);

		// Set the starting values to whatever the params are currently:
		xPtr->clear();
		for(i=0; i < params->getLength(); i++) {
			if(params->isParamEstPop(i)) {
				xPtr->push_back(params->getParamValue(i));
			}
		}

		result = optg.optimize(*xPtr, maxf);
	} catch(std::exception &e) {
		std::cout << "ERROR running global optimization in estimateModelParameters\n" ;
		std::cout << e.what() << "\n";
		return(NULL);
	}

	// Set results in ParamList and return

	printf("The global optimzation ran for %d iterations.\n", mdpOpt_count);

	for(i=0, j = 0; i < params->getLength(); i++) {
		if(params->isParamEstPop(i)) {
			printf("Parameter %s estimated as: %.4f\n", params->getParamLabel(i)->c_str(), xPtr->at(j));
			params->setParamValue(i,  xPtr->at(j++));
		}
	}
	printf("Max LL was: %f\n", maxf);

	// Then use these results to start a local optimizer:
	mdpOpt_count = 0;
	//	nlopt::opt optl(nlopt::LN_BOBYQA, numEstParams);
	try{
		nlopt::opt optl(nlopt::LN_COBYLA, numEstParams);

		optl.set_max_objective(getPopLogLiklihood, (void *)&mdpOptDat);

		optl.set_lower_bounds(lb);
		optl.set_upper_bounds(ub);

		optl.set_maxeval(numLocalInterations);
		optl.set_xtol_abs(localOptCrit);

		// Set the starting values to whatever the params are currently:
		for(i=0, j = 0; i < params->getLength(); i++) {
			if(params->isParamEstPop(i)) {
				xPtr->at(j++) = params->getParamValue(i);
			}
		}

		result = optl.optimize(*xPtr, maxf);
	} catch(std::exception &e) {
		std::cout << "ERROR running local optimization in estimateModelParameters\n" ;
		std::cout << e.what() << "\n";
		return(NULL);
	}
	// Set results in ParamList and return

	printf("The local optimization converged after %d iterations.\n", mdpOpt_count);
	printf("Max LL was: %f\n", maxf);

	for(i=0, j=0; i < params->getLength(); i++) {
		if(params->isParamEstPop(i)) {
			params->setParamValue(i, xPtr->at(j));
			printf("ModParam %s estimated as: %.4f\n", params->getParamLabel(i)->c_str(), xPtr->at(j));

			//TODO: Add confidence interval estimation (or std error)
			j++;
		}
	}

	// Add likelihood as a parameter for reporting if it is not already included:
	llInd = params->getParamIndex("pop_logLikelihood");
	if(llInd < 0) {
		params->addParam("pop_logLikelihood", maxf, pt_fixed, pc_none);
		llInd = params->getParamIndex("pop_logLikelihood");
	} else {
		params->setParamValue(llInd, maxf);
	}

	if(writeLikValues) {
		fclose(likVal_FPTR);
	}
	delete(xPtr);
	return(params);
}

/*
 * estimatePersonParameters
 *
 * Using current model parameters, for each play record, estimate the person ability
 * parameter using maximum likelihood.  This function may be expanded in the future to
 * include additional person parameters.
 */

vector<ParamList *> *MDP_IRT_Model::estimatePersonParametersOld(vector<vector <ActionSequence *> *> data) {
	vector<ParamList *> *resultLists = new vector<ParamList *>();

	// Call the new interface
	MDP_IRT_Model::estimatePersonParameters(data);

	// Now repackage in the form of the old interface
	int ll_pInd = params->getParamIndex("logLikelihood");
	int ell_pInd = params->getParamIndex("E_logLikelihood");
	int vll_pInd = params->getParamIndex("V_logLikelihood");

	if(writeLikValues) {
		likVal_FPTR = fopen(plikValFile, "a");
	}

	// Set up results param lists:
	for(int i=0; i<params->getLength(); i++) {
		if(params->isParamEstPers(i)) {
			resultLists->push_back(params->getParamSubList(i));
		}
	}

	if(trackLikelihoods) {
		resultLists->push_back(params->getParamSubList(ll_pInd));
		resultLists->push_back(params->getParamSubList(ell_pInd));
		resultLists->push_back(params->getParamSubList(vll_pInd));
	}
	return(resultLists);
}

vector<ParamList *> *MDP_IRT_Model::estimatePersonParametersOld(vector<ActionSequence*>* data) {
	vector< vector<ActionSequence *> *> dataList;
	int i;
	for(i=0; i < (int)data->size(); i++) {
		dataList.push_back(new vector<ActionSequence *>(1, data->at(i)));
	}
	vector<ParamList *> *res = estimatePersonParametersOld(dataList);
	for(i=0; i < (int)data->size(); i++) {
		delete(dataList[i]);
	}
	return(res);
}

/* For the multi-task version, the outer vector is organized by person, with the inner vector being
 * the tasks.  If a person has not performed a particular task, NULL should be
 * inserted in that slot.  Thus the array is PxT where P is number of persons and T is number of tasks.
 * For each person we also calculate the ll(\hat(beta)) along with ll(\infty), ll(0), E(ll(\hat(beta)) and V(ll(\hat(beta))
 */
ParamList *MDP_IRT_Model::estimatePersonParameters(vector<vector <ActionSequence *> *> data) {

	mdpirt_opt_data mdpOptDat;
	//	vector<ParamList *> *resultLists = new vector<ParamList *>();
	int numPersParams = 0;
	vector<int> persParamIndex;
	ParamList *pslPtr = NULL;
	int ll_pInd = params->getParamIndex("logLikelihood");
	int ell_pInd = params->getParamIndex("E_logLikelihood");
	int vll_pInd = params->getParamIndex("V_logLikelihood");

	unsigned int i;
	int p, asNum = (int)data.size();
	string pId;
	vector< vector<ActionSequence *> *> tmpData;
	std::vector<double> *xPtr = new vector<double>();
	double maxf;

	double  E_ll, V_ll;

	if(writeLikValues) {
		likVal_FPTR = fopen(plikValFile, "a");
	}

	// Set up results param lists:
	for(i=0; (int)i<params->getLength(); i++) {
		if(params->isParamEstPers(i)) {
			numPersParams++;
			persParamIndex.push_back(i);
			pslPtr = params->getParamSubList(i);
			if(pslPtr == NULL) {
				pslPtr = new ParamList();
				params->setParamSubList(i, pslPtr);
			}
			// Model parameters might have changed, so reset the min/max from the distribution parameters:
			params->updateParamRange(i);
		}
	}
	if(trackLikelihoods) {
		if(ll_pInd > 0 && params->getParamSubList(ll_pInd) == NULL) params->setParamSubList(ll_pInd, new ParamList());
		if(ell_pInd > 0 && params->getParamSubList(ell_pInd) == NULL) params->setParamSubList(ell_pInd, new ParamList());
		if(vll_pInd > 0 && params->getParamSubList(vll_pInd) == NULL) params->setParamSubList(vll_pInd, new ParamList());
	}

	// Set up the MDP elements in the optimization data structure:

	mdpOptDat.params = params;
	vector<double> lb;
	vector<double> ub;

	for(p=0; p < asNum; p++) {
		// Get the person id, and make sure we have a record:
		for(i=0; i < data[p]->size(); i++) {
			if(data[p]->at(i) != NULL) {
				pId = data[p]->at(i)->getId();
				break;
			}
		}
		if(i == data[p]->size())  // Nothing to see here folks!
			continue;

		if(verbose)
			cout << "Estimating person param for data record " << p << ".\n";

		tmpData.clear();
		lb.clear();
		ub.clear();
		tmpData.push_back(data[p]);
		mdpOptDat.actSeqList = &tmpData;
		mdpOptDat.miModel = this;

		//	mdpOptDat.useRec =  new vector<int>(playRecs->size(), 1);

		// Run the optimizer
		try{
			mdpOpt_count = 0;
			nlopt::opt opt(nlopt::LN_COBYLA, numPersParams);
			//	nlopt::opt opt(nlopt::GN_DIRECT_L, persParams->getLength());
			//	nlopt::opt opt(nlopt::LN_BOBYQA, persParams->getLength());

			if(personEstType == MDPIRT_MLE)
				opt.set_max_objective(getPersonLogLiklihood, (void *)&mdpOptDat);
			else // Will add EAP possibility later
				opt.set_max_objective(getPersonLogPosterior, (void *)&mdpOptDat);

			// Set the constraints according to the paramList:
			// Note, for some optimizers these need to be reasonable ranges (not -inf to inf)


			for(i=0; i<persParamIndex.size(); i++) {
				lb.push_back(params->getMinValue(persParamIndex[i]));
				ub.push_back(params->getMaxValue(persParamIndex[i]));
			}

			opt.set_lower_bounds(lb);
			opt.set_upper_bounds(ub);

			opt.set_maxeval(500);
			opt.set_xtol_abs(0.005);
			opt.set_ftol_abs(0.0001);

			// Set the starting values to mean beta:
			xPtr->clear();

			for(i=0; i<persParamIndex.size(); i++) {
				xPtr->push_back(params->getParamValue(persParamIndex[i]));
			}
			assert(xPtr->size() == 1);  // When we add other person-level parameters, remove this.

			nlopt::result result = opt.optimize(*xPtr, maxf);
		} catch(std::exception &e) {
			std::cout << "ERROR running optimization in personModelParameters for " << pId <<"\n" ;
			std::cout << e.what() << "\n";
			return(NULL);
		}

		for(i=0; i<persParamIndex.size(); i++) {
			params->getParamSubList(persParamIndex[i])->addParam(&pId, xPtr->at(i), pt_result, params->getParamComponent(persParamIndex[i]));
		}
		if(trackLikelihoods && ll_pInd > 0)
			params->getParamSubList(ll_pInd)->addParam(&pId, maxf, pt_result, pc_diagnostic);

		// Get remaining ll stats to calculate fit values:
		if(trackLikelihoods) {
			getLogLikStats(E_ll, V_ll, xPtr->at(0), data[p]);
			if(ell_pInd > 0 ) params->getParamSubList(ell_pInd)->addParam(&pId, E_ll, pt_result, pc_diagnostic);
			if(vll_pInd > 0 ) params->getParamSubList(vll_pInd)->addParam(&pId, V_ll, pt_result, pc_diagnostic);
		}

		if(verbose)
			cout << "Estimate for " << p << ", ability est: " << xPtr->at(0) << " log-likelihood: " << maxf << ".\n";

	}

	if(writeLikValues) {
		fclose(likVal_FPTR);
	}

	delete(xPtr);
	return(params);

}

/* This is the objective function for the model parameter optimizer.  This function is called by the
 * optimizer.
 *
 * n is the number of parameters being optimized.
 * x is a list of the parameter values being optimized (for this iteration).
 * grad would be the gradient of the function, if we had it.
 * input_data: All data and structures needed to generate the logLiklihood from the data.
 */
double getPopLogLiklihood(unsigned int n, double const *x, double *grad, void *input_data) {
	mdpirt_opt_data *dat = (mdpirt_opt_data *)input_data;
	int i, j;

	// Set all the parameters:
	for(i=0, j = 0; (int)i < dat->params->getLength(); i++) {
		if(dat->params->isParamEstPop(i)) {
			if(i == dat->miModel->getBetaMuParamIndex())
				dat->miModel->setBetaMu(x[j]);
			if(i == dat->miModel->getBetaSigmaParamIndex())
				dat->miModel->setBetaSigma(x[j]);

			dat->params->setParamValue(i, x[j++]);
		}
	}
	assert(j == (int)n);

	// Notify all the MDPs' components that the parameters have been updated, just in case they care:
	for(i=0; (int)i<dat->miModel->getNumTasks(); i++) {
		dat->miModel->getMdp(i)->paramsUpdated();
	}
/*
	// Set the transition and reward params
	for(p = 2, i = 0; p < n && i < dat->miModel->getNumTransParam(); p++, i++) {
		dat->miModel->getMdp()->getTrans()->getParamList()->setParamValue(i, x[p]);
	}
	vector <double> rewPVals(dat->miModel->getNumRewardParam(), 0.0);
	for(p = 2 + dat->miModel->getNumTransParam(), i = 0; p < n && i < dat->miModel->getNumRewardParam(); p++, i++) {
		rewPVals[i] = x[p];
//		dat->miModel->getMdp()->getReward()->getParamList()->setParamValue(i, x[p]);
	}
	dat->miModel->getMdp()->getReward()->setParams(rewPVals);
	*/
	mdpOpt_count++;


	if(writeLikValues && likVal_FPTR != NULL) {
		double likVal = dat->miModel->marginalLogLiklihood(*(dat->actSeqList));
		fprintf(likVal_FPTR, "%.5f %.5f %.5f\n", x[0], x[1], likVal);
		return(likVal);
	} else
		return(dat->miModel->marginalLogLiklihood(*(dat->actSeqList)));
}


/* This is the objective function for the person parameter optimizer when using MLE estimation.
 * This function is called by the optimizer.
 *
 * n is the number of parameters being optimized.
 * x is a list of the parameter values being optimized (for this iteration).
 * grad would be the gradient of the function, if we had it.
 * input_data: All data and structures needed to generate the logLiklihood from the data.
 */
double getPersonLogLiklihood(unsigned int n, double const *x, double *grad, void *input_data) {
	mdpirt_opt_data *dat = (mdpirt_opt_data *)input_data;
	double logPost = 0;
	unsigned int t;

	// Our only parameter should be the person beta
//	assert((int)n == dat->params->getLength());
//	assert(n == 1);

	// This is per person, so we have a vector of one element that is a vector of task action sequences in the data:
	mdpOpt_count++;
	for(t=0; (int)t < dat->miModel->getNumTasks(); t++) {
		if(dat->actSeqList->at(0)->at(t) != NULL)
			logPost += dat->miModel->pointLogLiklihood(x[0], t, dat->actSeqList->at(0)->at(t)) ;
	}

	if(writeLikValues && likVal_FPTR != NULL) {
		fprintf(likVal_FPTR, "%s %.5f %.5f\n", dat->actSeqList->at(0)->at(0)->getId().c_str(), x[0], logPost);
	}

	return(logPost);
}

/* This is the objective function for the person parameter optimizer when using MLE.
 * This function is called by the optimizer.
 *
 * n is the number of parameters being optimized.
 * x is a list of the parameter values being optimized (for this iteration).
 * grad would be the gradient of the function, if we had it.
 * input_data: All data and structures needed to generate the logLiklihood from the data.
 */
double getPersonLogPosterior(unsigned int n, double const *x, double *grad, void *input_data) {
	mdpirt_opt_data *dat = (mdpirt_opt_data *)input_data;
	double logPost = 0;
	unsigned int t;

	// Our only parameter should be the person beta
//	assert((int)n == dat->params->getLength());
//	assert(n == 1);

	// This is per person, so we have a vector of one element that is a vector of task action sequences in the data:
	mdpOpt_count++;
	for(t=0; (int)t < dat->miModel->getNumTasks(); t++) {
		if(dat->actSeqList->at(0)->at(t) != NULL)
			logPost += dat->miModel->pointLogLiklihood(x[0], t, dat->actSeqList->at(0)->at(t)) ;
	}

	if(dat->miModel->isUseLogNormDist())
		logPost += dat->miModel->logOfLogNormProb((double)(dat->miModel->getBetaMu()), (double)(dat->miModel->getBetaSigma()), x[0]);
	else
		logPost += dat->miModel->logOfNormProb((double)(dat->miModel->getBetaMu()), (double)(dat->miModel->getBetaSigma()), x[0]);

	if(writeLikValues && likVal_FPTR != NULL) {
		fprintf(likVal_FPTR, "%s %.5f %.5f\n", dat->actSeqList->at(0)->at(0)->getId().c_str(), x[0], logPost);
	}

	return(logPost);
}

/*
 * This function implements Gaussian Quadrature to get a marginal likelihood over the ability distribution.
 * The ability distribution is assumed to be N(betaMu, betaSigma^2)
 *
 * So that we only need to update the action probabilities once per quadrature point, we sum the log likelihood
 * for all the records at a single quad point before going on to the next.
 *
 * TODO: Note for speed, we should probably use the dynamic probability updater once we have ensured that this method works.
 *
 * Right now this is only a poor approximation -- intended as a place holder for a better implementation.
 */
double MDP_IRT_Model::marginalLogLiklihood(vector< vector <ActionSequence *> *> data) {
	int j, q;
	unsigned int t;
	double logLik = 0.0;

	// Set the IRT policies for each quad point and each task:
	for(q=0; q < numQuadPoints; q++) {
		for(t=0; t < numTasks; t++) {
			if(useLogNormDist)
				quadIrtDR[q]->setLogTheta(betaMu + intHalfRange * betaSigma * quadPoints[q]);
			else
				quadIrtDR[q]->setTheta(betaMu + intHalfRange * betaSigma * quadPoints[q]);
			quadIrtPolicy[q][t]->setDecisionRule(quadIrtDR[q]);
			// If we are not using the optScores, we need to recalc the scores for this beta:
			if(!useOptScore) {
				quadIrtPolicy[q][t]->resetValues();
				quadIrtPolicy[q][t]->setOptimized(0);
				if(quadIrtPolicy[q][t]->optimize() == MDP_ERROR) {
					printf("Beta: %.4f\n", betaMu + intHalfRange * betaSigma * quadPoints[q]);
					quadIrtPolicy[q][t]->printModelParams();
				}
			}
		}

		/* Reset the action probabilities */
		//		quadIrtPolicy[q]->updateActionProbs();   dynamically updated
	}

	/* Loop over all action sequences */
	for(j=0; j < (int)data.size(); j++) {
		for(t=0; t < numTasks; t++) {
			if(data[j]->at(t) != NULL)
				logLik += logRecordMarginalProbability(t, data[j]->at(t));
		}
	}

	return(logLik);
}

/*
 * The log likelihood of one or more data sequences for a given value of beta.
 */
float MDP_IRT_Model::pointLogLiklihood(float x, int taskId, ActionSequence *as) {
	if(as->getNumSteps() == 0) return(0);

	/* Set the beta value for the IRT_DR */
	if(useLogNormDist)
		irtDR.setLogTheta(x);
	else
		irtDR.setTheta(x);

	irtPolicy[taskId]->setDecisionRule(&irtDR);
	// If we are not using the optScores, we need to recalc the scores for this beta:
	if(!useOptScore) {
		irtPolicy[taskId]->setOptimized(0);
		if(irtPolicy[taskId]->optimize() == MDP_ERROR) {
			printf("Policy failed to converge for log beta %.4f.\n", x);
			irtPolicy[taskId]->setOptimized(1);
		}
	}

	if(as->getStatesObserved() == so_partial)
		return(poRecordConditionalLogLikelihood(irtPolicy[taskId], as));
	else
		return(recordConditionalLogLikelihood(irtPolicy[taskId], as));

}

void MDP_IRT_Model::getLogLikStats(double &ell, double &vll, float x, vector<ActionSequence *> *asList) {
	ell = vll = 0.0;
	ActionSequence *as;
	if(asList->size() == 0) return;
	double sell, svll;

	/* Set the beta value for the IRT_DR */
	if(useLogNormDist)
		irtDR.setLogTheta(x);
	else
		irtDR.setTheta(x);

	// Iterate over tasks:
	int t, nt = getNumTasks();
	for(t=0; t < nt; t++) {
		as = asList->at(t);

		if(as == NULL || as->getNumSteps() == 0)
			continue;

		irtPolicy[t]->setDecisionRule(&irtDR);
		// If we are not using the optScores, we need to recalc the scores for this beta:
		if(!useOptScore) {
			irtPolicy[t]->setOptimized(0);
			irtPolicy[t]->optimize();
		}

		int s, ns = as->getNumSteps() - 1; // Note the last state does not count because no action is taken from it.

		/* First step is from the startState */
		if(as->getStatesObserved() == so_fully)
			stateLogLikStats(irtPolicy[t], as->getStartState(), sell, svll);
		else
			stateDistLogLikStats(irtPolicy[t], as->getStartStateDist(), sell, svll);
		ell += sell;
		vll += svll;

		// Iterate over remaining states
		for(s = 0; s < ns; s++) {

			if(as->getStatesObserved() == so_fully)
				stateLogLikStats(irtPolicy[t], as->getStateAtStep(s), sell, svll);
			else
				stateDistLogLikStats(irtPolicy[t], as->getStateProbsAtStep(s), sell, svll);

			ell += sell;
			vll += svll;
		}
	}

	return;
}

// Returns passed through argument references
void MDP_IRT_Model::stateLogLikStats(Policy *pol, State *s, double &e, double &var) {
	// Note the policy must already be optimized!
	vector<double> aProbs = pol->getActionProbsForState(s);
	unsigned int a, na = aProbs.size();
	double esll = 0;
	e = var = 0;

	for(a=0; a < na; a++) {
		if(aProbs[a] > 0) {
			e += aProbs[a] * log(aProbs[a]);
			esll += aProbs[a] * pow(log(aProbs[a]),2);
		}
	}
	var = esll - pow(e,2);
	return;
}

// Returns passed through argument references
// TODO: Check the math on this one.
void MDP_IRT_Model::stateDistLogLikStats(Policy *pol, vector<StateProb> *sd, double &e, double &var) {
	// Note the policy must already be optimized!
	vector<double> aProbs;
	unsigned int i, a, na;
	double esll = 0, tp = 0;
	e = var = 0;

	for(i=0; i < sd->size(); i++) {
		aProbs = pol->getActionProbsForState(sd->at(i).getStateId());
		na = aProbs.size();

		for(a=0; a < na; a++) {
			if(aProbs[a] > 0) {
				tp = aProbs[a] * sd->at(i).getProb();
				if(tp > 0) {
					e += tp * log(tp);
					esll += tp * pow(log(tp),2);
				}
			}
		}
	}
	var = esll - pow(e,2);
	return;
}


/* recordConditionalProbability
 * Returns the joint probability of the action sequence according to the policy.
 * It is assumed that the policy is already optimized.
 */
double MDP_IRT_Model::recordConditionalProbability(Policy *pol, ActionSequence *as) {
	if(as->getNumSteps() == 0) return(1.0);

	double value, sVal;
	int step;

	/* First step is from the startState */
	value = pol->getActionProbForState(as->getStartState(), as->getActionAtStep(0));

	for(step = 1; step < as->getNumSteps(); step++) {

		sVal = pol->getActionProbForState(as->getStateAtStep(step-1), as->getActionAtStep(step))
		                                                                        + 0.000000001;
		value *= sVal;
	}
	return(value);
}

/* recordConditionalLogLikelihood
 * Returns the *log* of the joint probability of the action sequence according to the policy.
 * It is assumed that the policy is already optimized.
 */
double MDP_IRT_Model::recordConditionalLogLikelihood(Policy *pol, ActionSequence *as) {
	if(as->getNumSteps() == 0) return(1.0);

	double value, sVal;
	int step;

	/* First step is from the startState */
	value = log(pol->getActionProbForState(as->getStartState(), as->getActionAtStep(0)));

	for(step = 1; step < as->getNumSteps(); step++) {

		sVal = log(pol->getActionProbForState(as->getStateAtStep(step-1), as->getActionAtStep(step)));
		value += sVal;
	}
	return(value);
}

/* poRecordConditionalProbability
 * Returns the joint probability of the partially observed action sequence according to the policy.
 * It is assumed that the policy is already optimized.
 */
double MDP_IRT_Model::poRecordConditionalProbability(Policy *pol, ActionSequence *as) {
	if(as->getNumSteps() == 0) return(1.0);
	if(as->getStatesObserved() == so_fully)
			return(recordConditionalProbability(pol, as));

	double value = 0.0, sVal;
	int step;
	unsigned int i, nStates;
	vector<StateProb> *sDist;

	// First step is from the startState
	sDist = as->getStartStateDist();
	for(i = 0, nStates = sDist->size(); i < nStates; i++) {
		value += pol->getActionProbForState(sDist->at(i).getStateId(), as->getActionAtStep(0)->getId()) * sDist->at(i).getProb();
	}

	// Now all the other actions:
	for(step = 1; step < as->getNumSteps(); step++) {
		sDist = as->getStateProbsAtStep(step-1);
		for(i = 0, nStates = sDist->size(), sVal = 0.0; i < nStates; i++) {
			sVal += pol->getActionProbForState(sDist->at(i).getStateId(), as->getActionAtStep(step)->getId()) * sDist->at(i).getProb()
				                                                                        + 0.000000001;
		}
		value *= sVal;
	}
	return(value);
}

/* poRecordConditionalLogLikelihood
 * Returns the *log* of the joint probability of the partially observed action sequence according to the policy.
 * It is assumed that the policy is already optimized.
 */
double MDP_IRT_Model::poRecordConditionalLogLikelihood(Policy *pol, ActionSequence *as) {
	if(as->getNumSteps() == 0) return(1.0);
	if(as->getStatesObserved() == so_fully)
			return(recordConditionalLogLikelihood(pol, as));

	double value = 0.0, sVal;
	int step;
	unsigned int i, nStates;
	vector<StateProb> *sDist;

	// First step is from the startState
	sDist = as->getStartStateDist();
	for(i = 0, nStates = sDist->size(), sVal = 0.0; i < nStates; i++) {
		sVal += pol->getActionProbForState(sDist->at(i).getStateId(), as->getActionAtStep(0)->getId()) * sDist->at(i).getProb();
	}
	value = log(sVal);

	for(step = 1; step < as->getNumSteps(); step++) {
		sDist = as->getStateProbsAtStep(step-1);
		for(i = 0, nStates = sDist->size(), sVal = 0.0; i < nStates; i++) {
			sVal += pol->getActionProbForState(sDist->at(i).getStateId(), as->getActionAtStep(step)->getId()) * sDist->at(i).getProb()
				                                                                        + 0.000000001;
		}
		value += log(sVal);
	}
	return(value);
}
/*
double MDP_IRT_Model::recordStateMarginalProbability(Policy *pol, ActionSequence *as) {
	if(as->getNumSteps() == 0) return(1.0);

	double value, sVal;
	int step;
	vector<double> sProb;

	// First step is from the startState
	value = pol->getActionProbForState(as->getStartState(), as->getActionAtStep(0));

	for(step = 1; step < as->getNumSteps(); step++) {

		sVal = pol->getActionProbForState(as->getStateAtStep(step-1), as->getActionAtStep(step))
		                                                                        + 0.000000001;
		value *= sVal;
	}
	return(value);
}
*/


double MDP_IRT_Model::logRecordMarginalProbability(int taskId, ActionSequence *as) {
	double value = 0.0;
	int q;

	// For each quad point, get the conditional probability, scaled and shifted, and multiply by the weight.
	for(q = 0; q < numQuadPoints; q++) {
		if(as->getStatesObserved() == so_partial)
			value += poRecordConditionalProbability( quadIrtPolicy[q][taskId], as) *
			dNormProbs[q] * quadWeights[q];
		else
			value += recordConditionalProbability( quadIrtPolicy[q][taskId], as) *
			dNormProbs[q] * quadWeights[q];
	}

	if(value < std::numeric_limits<double>::denorm_min())
		value = std::numeric_limits<double>::denorm_min();

	return(log(value) + log(intHalfRange));
}

double MDP_IRT_Model::recordMarginalProbability(int taskId, ActionSequence *as) {
	double value = 0.0;
	int q;

	// For each quad point, get the conditional probability, scaled and shifted, and multiply by the weight.
	for(q = 0; q < numQuadPoints; q++) {
		if(as->getStatesObserved() == so_partial)
			value += poRecordConditionalProbability( quadIrtPolicy[q][taskId], as) *
			dNormProbs[q] * quadWeights[q];
		else
			value += recordConditionalProbability( quadIrtPolicy[q][taskId], as) *
			dNormProbs[q] * quadWeights[q];
	}
	return(value * intHalfRange);
}

// TODO: once fully tested and debugged, remove the variable.
double MDP_IRT_Model::logOfNormProb(double mu, double sigma, double x) {
	double lnp = log( (1/(sigma * sqrt(2 * PI))) * (exp((-1.0/(2.0 * pow(sigma,2) )) * pow(mu - x, 2))) ) ;
	return(lnp);
}

double MDP_IRT_Model::logOfLogNormProb(double mu, double sigma, double x) {
	// The log normal part is the -x at the end, this is the + log(1/exp(x))
	double lnp = log( (1/(sigma * SQRT_2_PI))) + (-1.0/(2.0 * pow(sigma,2) )) * pow(mu - x, 2) - x;
	return(lnp);
}


} /* namespace MDPLIB */
