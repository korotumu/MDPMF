/*
 * ParamList.cpp
 *
 *  Created on: Dec 5, 2013
 *      Author: Michelle LaMar
 *
 * 	Revised on: Apr 18, 2017
 *		Author: Rutuja Ubale added functions to class ParamList
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

#include "ParamList.h"

namespace MDPLIB {

ParamList::ParamList() {
	length = 0;
}

ParamList::~ParamList() {
}

// TODO: add parameter constraints
int ParamList::addParams(ParamList* pl) {
	if(pl == NULL)
		return(MDP_SUCCESS);

	int i, np = pl->getLength();
	for(i = 0; i < np; i++) {
		this->addParam(&pl->labels[i], pl->values[i], pl->types[i], pl->modelComponents[i], pl->distributions[i], pl->hypParamA_index[i], pl->hypParamB_index[i], pl->stdErrEst[i], pl->confInterval[i][0], pl->confInterval[i][1]);

		minVals[length-1] = pl->getMinValue(i);
		maxVals[length-1]=  pl->getMaxValue(i);
	}
	return(MDP_SUCCESS);
}


int ParamList::addParam(const char lab[], double val, paramType_t pt, paramComp_t pc){
	string *slab = new string(lab);
	int ret = addParam(slab, val, pt, pc);
	delete(slab);
	return(ret);
}


int ParamList::addParam(string* lab, double val, paramType_t pt, paramComp_t pc) {
	double ciLow = NEG_INF, ciHigh = POS_INF, se = POS_INF;
	if(pt == pt_fixed) {
		ciLow = val;
		ciHigh = val;
		se = 0;
	}

	return(addParam(lab, val, pt, pc, pd_error, BAD_REF, BAD_REF, se, ciLow, ciHigh));
}


int ParamList::addParam(string *lab, double val, double min, double max) {

	// Going to make this a uniform dist.  Create the constant hyper params first
	string *pLab = new string(*lab + "_hypParamA");
	addConstant(pLab, min);
	pLab->assign(*lab + "_hypParamB");
	addConstant(pLab, max);
	delete(pLab);

	int ret =addParam(lab, val, pt_estPop, pc_error, pd_unif, length - 2, length - 1, POS_INF, NEG_INF, POS_INF);
	return(ret);
}

int ParamList::addParam(const char lab[], double val, double min, double max) {
	string *slab = new string(lab);
	int ret = addParam(slab, val, min, max);
	delete(slab);
	return(ret);
}

int ParamList::addParam(const char lab[], double val, paramType_t type, paramComp_t pc, paramDist_t dist, int hpA, int hpB) {
	string *slab = new string(lab);
	int ret = addParam(slab, val, type, pc, dist, hpA, hpB);
	delete(slab);
	return(ret);
}

int ParamList::addParam(string *lab, double val, paramType_t type, paramComp_t pc, paramDist_t dist, int hpA, int hpB){
	return(addParam(lab, val, type, pc, dist, hpA, hpB, 0.0, NEG_INF, POS_INF));
}


int ParamList::addParam(string *lab, double val, paramType_t type, paramComp_t pc, paramDist_t distType, int hpA, int hpB, double se, double ciLow, double ciHigh){
	return(addParam( *lab,  val,  type, pc,  distType,  hpA,  hpB,  se,  ciLow,  ciHigh));
}

// This is the master addParam function.  All others eventually call this one:
int ParamList::addParam(string lab, double val, paramType_t type, paramComp_t pc, paramDist_t distType, int hpA, int hpB, double se, double ciLow, double ciHigh){

	// Deal with the distribution hyperparameters first so if there is a problem we don't have to roll back
	if(distType == pd_constant || distType == pd_error) {
		hypParamA_index.push_back(-1);
		hypParamB_index.push_back(-1);
		minVals.push_back(val);
		maxVals.push_back(val);
	} else {
		if(hpA >= length || hpB >= length) {
			// TODO: Need to figure out how to handle these errors.
			return(-1);
		}
		// These are the understood distributions.  Add here if you introduce a new one:
		if(distType == pd_unif || distType == pd_norm || distType == pd_lnorm) {
			// Need two hyper parameters for these distributions
			if(hpA < 0 || hpB < 0 )
				return(-1);
			hypParamA_index.push_back(hpA);
			hypParamB_index.push_back(hpB);

			if(distType == pd_unif) {
				minVals.push_back(this->getParamValue(hpA));
				maxVals.push_back(this->getParamValue(hpB));
			} else {
				minVals.push_back(this->getParamValue(hpA) - (5 * this->getParamValue(hpB)));
				maxVals.push_back(this->getParamValue(hpA) + (5 * this->getParamValue(hpB)));
			}
		} else
			return(-1);
	}

	string newStr(lab);
	labels.push_back(newStr);
	values.push_back(val);
	types.push_back(type);
	modelComponents.push_back(pc);
	distributions.push_back(distType);

	stdErrEst.push_back(se);
	confInterval.push_back(vector<double>());
	confInterval[length].push_back(ciLow);
	confInterval[length].push_back(ciHigh);



	// This is a place holder for person-specific or task-specific parameter values
	paramSubList.push_back(NULL);

	length++;
	return(length-1);
}

int ParamList::addConstant(const char lab[], double val) {
	string *newLab = new string(lab);
	int ret = addParam(newLab, val, pt_fixed, pc_none, pd_constant, NA_REF, NA_REF, 0, val, val);
	delete(newLab);
	return(ret);
}

int ParamList::addConstant(string* lab, double val) {
	return(addParam(lab, val, pt_fixed, pc_none, pd_constant, NA_REF, NA_REF, 0, val, val));
}


int ParamList::addParamDistribution(int i, string distStr) {
	if(i >= length)
		return MDP_ERROR;

	paramDist_t pd = pd_error;
	int hpA_ind = BAD_REF, hpB_ind = BAD_REF;
	char distType_str[16], hpA_str[16], hpB_str[16];
	char *endPtr = NULL;
	double a, b, min = 0, max = 0;
	int len, c;

	// TODO deal more gracefully with spaces.  Like trim or something.

	if(sscanf(distStr.c_str(), "%[^(),](%[^(),],%[^(),])", distType_str, hpA_str,hpB_str) == 3) { // These are the distributions with 2 hyper parameters
		len = strlen(distType_str);
		for(c=0; c<len; c++) {
			distType_str[c] = tolower(distType_str[c]);
		}
		if(strcmp(distType_str, "unif") ==0 )
			pd = pd_unif;
		if(strcmp(distType_str, "n") == 0 || strcmp(distType_str, "norm") == 0)
			pd = pd_norm;
		if(strcmp(distType_str, "ln") == 0 || strcmp(distType_str, "lnorm") == 0  || strcmp(distType_str, "lognorm") == 0 || strcmp(distType_str, "logn") == 0)
			pd = pd_lnorm;

		// Check to see if the hyperparameters are numeric constants or parameter names.  Then do the right thing.
		a = strtod(hpA_str, &endPtr);
		if(a == 0 && (!endPtr || endPtr == (char *)hpA_str)) {
			// Not a number.  Find the parameter being referenced
			hpA_ind = this->getParamIndex(hpA_str);

		} else {
			// A numeric constant.  Create a new parameter for this
			hpA_ind = length;
			string *pLab = new string(labels[i] + "_hypParamA");
			this->addConstant(pLab, a);
			delete(pLab);
		}

		b = strtod(hpB_str, &endPtr);
		if(b == 0 && (!endPtr || endPtr == (char *)hpB_str)) {
			// Not a number.  Find the parameter being referenced
			hpB_ind = this->getParamIndex(hpB_str);

		} else {
			// A numeric constant.  Create a new parameter for this
			hpB_ind = length;
			string *pLab = new string(labels[i] + "_hypParamB");
			this->addConstant(pLab, b);
			delete(pLab);
		}

		if(pd == pd_error || hpA_ind < 0 || hpB_ind < 0) {
			return MDP_ERROR;
		}

	} else { // Zero parameters for the constant distribution:
		if(strcmp(distStr.c_str(), "const") == 0 || strcmp(distStr.c_str(), "constant") == 0) {
			pd = pd_constant;
			hpA_ind = NA_REF;
			hpB_ind = NA_REF;
		} else {
			// Don't know what the heck this is
			return MDP_ERROR;
		}
	}

	// Set the range:
	if(pd == pd_unif) {
		min = this->getParamValue(hpA_ind);
		max = this->getParamValue(hpB_ind);
	} else if(pd == pd_constant){
		min = this->getParamValue(i);
		max = this->getParamValue(i);
	} else {         //This covers norm and lnorm
		min = this->getParamValue(hpA_ind) - (5 * this->getParamValue(hpB_ind));
		max = this->getParamValue(hpA_ind) + (5 * this->getParamValue(hpB_ind));
	}

	// Now set everything in the actual parameter i:
	distributions[i] = pd;
	hypParamA_index[i] = hpA_ind;
	hypParamB_index[i] = hpB_ind;
	minVals[i] = min;
	maxVals[i] = max;

	return MDP_SUCCESS;
}

int ParamList::updateParamRange(int i) {

	if(i >= length || i < 0)
		return MDP_ERROR;

	if(distributions[i] == pd_constant){
		minVals[i] = values[i];
		maxVals[i] = values[i];
	} else {
		// Need the hyperparameters:
		if(hypParamA_index[i] < 0 || hypParamB_index[i] < 0) {
			return MDP_ERROR;
		}
		if(distributions[i] == pd_unif) {
			minVals[i] = this->getParamValue(hypParamA_index[i]);
			maxVals[i] = this->getParamValue(hypParamB_index[i]);
		} else  {         //This covers norm and lnorm
			minVals[i] = this->getParamValue(hypParamA_index[i]) - (5 * this->getParamValue(hypParamB_index[i]));
			maxVals[i] = this->getParamValue(hypParamA_index[i]) + (5 * this->getParamValue(hypParamB_index[i]));
		}
	}
	return MDP_SUCCESS;
}


Param* ParamList::getHyperParamA(int i) {
	if(i < length && hypParamA_index[i] >= 0)
		return(getParam(hypParamA_index[i]));
	else
		return(NULL);
}

Param* ParamList::getHyperParamB(int i) {
	if(i < length && hypParamB_index[i] >= 0)
		return(getParam(hypParamB_index[i]));
	else
		return(NULL);
}

int ParamList::getHyperParamAIndex(int i) {
	if(i < length && hypParamA_index[i] >= 0)
		return(hypParamA_index[i]);
	else
		return(-1);
}

int ParamList::getHyperParamBIndex(int i) {
	if(i < length && hypParamB_index[i] >= 0)
		return(hypParamB_index[i]);
	else
		return(-1);
}

double ParamList::getHyperParamAValue(int i) {
	if(i < length && hypParamA_index[i] >= 0)
		return(getParamValue(hypParamA_index[i]));
	else
		return(VALUE_ERROR);
}

double ParamList::getHyperParamBValue(int i) {
	if(i < length && hypParamB_index[i] >= 0)
		return(getParamValue(hypParamB_index[i]));
	else
		return(VALUE_ERROR);
}

int ParamList::setParamRange(int i, double min, double max) {
	if(i < 0 || i > length - 1 || min > max)
		return(-1);

	minVals[i] = min;
	maxVals[i] = max;

	if(values[i] > max)
		values[i] = max;
	if(values[i] < min)
		values[i] = min;

	return(i);
}

int ParamList::removeParam(int i) {
	if(i >= length)
		return MDP_ERROR;

	labels.erase(labels.begin() + i);
	values.erase(values.begin() + i);
	types.erase(types.begin() + i);
	minVals.erase(minVals.begin() + i);
	maxVals.erase(maxVals.begin() + i);
	distributions.erase(distributions.begin() + i);
	hypParamA_index.erase(hypParamA_index.begin() + i);
	hypParamB_index.erase(hypParamB_index.begin() + i);
	confInterval.erase(confInterval.begin() + i);
	stdErrEst.erase(stdErrEst.begin() + i);

	length--;
	return MDP_SUCCESS;
}

int ParamList::getLength() {
	return(length);
}

int ParamList::getParamIndex(string *lab) {
	int i;
	for(i=0; i< length; i++) {
		if(lab->compare(labels[i]) == 0)
			return i;
	}
	return -1;
}

int ParamList::getParamIndex(const char lab[]) {
	string *slab = new string(lab);
	int ret = getParamIndex(slab);
	delete(slab);
	return(ret);
}

string *ParamList::getParamLabel(int i) {
	if(i < 0 || i > length - 1)
		return(NULL);
	else
		return(&labels[i]);
}

double ParamList::getParamValue(int i) {
	if(i < 0 || i > length - 1)
		return(VALUE_ERROR);
	else
		return(values[i]);
}

bool ParamList::isParamFree(int i) {
	if(i < 0 || i > length - 1 || types[i] == pt_error)
		return(false);
	else
		return(types[i] != pt_fixed);
}

bool ParamList::isParamEstPop(int i) {
	if(i < 0 || i > length - 1 || types[i] == pt_error)
		return(false);
	else
		return(types[i] == pt_estPop);
}

bool ParamList::isParamEstPers(int i) {
	if(i < 0 || i > length - 1 || types[i] == pt_error)
		return(false);
	else
		return(types[i] == pt_estPers);
}


double ParamList::getMinValue(int i) {
	if(i < 0 || i > length - 1)
		return(VALUE_ERROR);
	else
		return(minVals[i]);
}


double ParamList::getMaxValue(int i) {
	if(i < 0 || i > length - 1)
		return(VALUE_ERROR);
	else
		return(maxVals[i]);
}

double ParamList::getParamSE(int i) {
	if(i < 0 || i > length - 1)
		return(VALUE_ERROR);
	else
		return(stdErrEst[i]);
}

vector<double> ParamList::getParamCI(int i) {
	if(i < 0 || i > length - 1)
		return(vector<double>(2, VALUE_ERROR));
	else
		return(confInterval[i]);
}

int ParamList::setParamLabel(int i, string *lab) {
	if(i < 0 || i > length - 1)
		return(-1);
	else
		labels[i] = string(*lab);

	return i;
}

int ParamList::setParamValue(int i, double val) {
	if(i < 0 || i > length - 1)
		return(-1);
	else {
		if(val > maxVals[i])
			values[i] = maxVals[i];
		else if(val < minVals[i])
			values[i] = minVals[i];
		else
			values[i] = val;
	}

	return i;
}


paramComp_t ParamList::getParamComponent(int i) {
	if(i < 0 || i > length - 1)
		return(pc_error);
	return(this->modelComponents[i]);
}

int ParamList::setParamComponent(int i, paramComp_t pc) {
	if(i < 0 || i > length - 1)
		return(-1);
	this->modelComponents[i] = pc;
	return(i);
}

ParamList* ParamList::getParamSubList(int i) {
	if(i < 0 || i > length - 1)
		return(NULL);
	return(this->paramSubList[i]);
}

int ParamList::setParamSubList(int i, ParamList* psl) {
	if(i < 0 || i > length - 1)
		return(-1);
	this->paramSubList[i] = psl;
	return(i);
}

int ParamList::addParam(Param p) {
	int aInd = NA_REF, bInd = NA_REF;
	if(p.hypParamA != NULL)
		aInd = this->getParamIndex(&(p.hypParamA->label));
	if(p.hypParamB != NULL)
		bInd = this->getParamIndex(&(p.hypParamB->label));

	addParam(&p.label, p.value, p.type, p.modComponent, p.distribution, aInd, bInd, p.stdErrEst, p.confInterval[0], p.confInterval[1]);
	return(MDP_SUCCESS);
}

int ParamList::setParam(int i, Param p) {
	if(i < 0 || i > length - 1)
		return(MDP_ERROR);

	int aInd = NA_REF, bInd = NA_REF;
	if(p.hypParamA != NULL)
		aInd = this->getParamIndex(&(p.hypParamA->label));
	if(p.hypParamB != NULL)
		bInd = this->getParamIndex(&(p.hypParamB->label));

	labels[i] = p.label;
	values[i] = p.value;
	distributions[i] = p.distribution;
	modelComponents[1] = p.modComponent;
	hypParamA_index[i] = aInd;
	hypParamB_index[i] = bInd;
	confInterval[i].clear();
	confInterval[i].push_back(p.confInterval[0]);
	confInterval[i].push_back(p.confInterval[1]);
	stdErrEst[i] = p.stdErrEst;
	types[i] = p.type;

	return(MDP_SUCCESS);
}

Param *ParamList::getParam(int i) {
	if(i < 0 || i > length - 1)
		return(NULL);

	Param *hpA = NULL, *hpB = NULL;

	// TODO: For this to work we need to make sure there are no loops in the param defs.  Hmm.
	if(hypParamA_index[i] >= 0)
		hpA = getParam(hypParamA_index[i]);
	if(hypParamB_index[i] >= 0)
		hpA = getParam(hypParamB_index[i]);

	return(new Param(labels[i], values[i], types[i], modelComponents[i], distributions[i], hpA, hpB, confInterval[i], stdErrEst[i]));
}

vector<double> *ParamList::getValuesArray() {
	return &values;
}

Param::Param(string l, double v, paramType_t t, paramComp_t c, paramDist_t dt, Param *hpA, Param *hpB)
{
	label = l;
	value = v;
	type = t;
	modComponent = c;
	distribution = dt;
	hypParamA = hpA;
	hypParamB = hpB;

	confInterval.push_back(v);
	confInterval.push_back(v);
	stdErrEst = 0;

	if(dt == pd_unif && (hpA != NULL && hpB != NULL)) {
		minVal = hpA->value;
		maxVal = hpB->value;
	}
	if(dt == pd_norm && (hpA != NULL && hpB != NULL)) {
		minVal = hpA->value - 5 * hpB->value;
		maxVal = hpA->value + 5 * hpB->value;
	}
	if(dt == pd_lnorm && (hpA != NULL && hpB != NULL)) {
		minVal = exp(hpA->value - 5 * hpB->value);
		maxVal = exp(hpA->value + 5 * hpB->value);
	}
	if(dt == pd_constant) {
		minVal = v;
		maxVal = v;
	}
}

Param::Param(string l, double v, paramType_t t, paramComp_t c, paramDist_t dt, Param *hpA, Param *hpB, vector<double> ci, double se)
{
	label = l;
	value = v;
	type = t;
	modComponent = c;
	distribution = dt;
	hypParamA = hpA;
	hypParamB = hpB;

	confInterval.push_back(ci[0]);
	confInterval.push_back(ci[1]);
	stdErrEst = se;

	if(dt == pd_unif && (hpA != NULL && hpB != NULL)) {
		minVal = hpA->value;
		maxVal = hpB->value;
	}
	if(dt == pd_norm && (hpA != NULL && hpB != NULL)) {
		minVal = hpA->value - 5 * hpB->value;
		maxVal = hpA->value + 5 * hpB->value;
	}
	if(dt == pd_lnorm && (hpA != NULL && hpB != NULL)) {
		minVal = exp(hpA->value - 5 * hpB->value);
		maxVal = exp(hpA->value + 5 * hpB->value);
	}
	if(dt == pd_constant) {
		minVal = v;
		maxVal = v;
	}
}

Param::~Param() {
}

const vector<double>& Param::getConfInterval() const {
	return confInterval;
}

void Param::setConfInterval(double lowBound, double highBound) {
	this->confInterval.clear();
	this->confInterval.push_back(lowBound);
	this->confInterval.push_back(highBound);
}

double Param::getStdErrEst() const {
	return stdErrEst;
}

void Param::setStdErrEst(double stdErrEst) {
	this->stdErrEst = stdErrEst;
}

double Param::getValue() const {
	return value;
}

void Param::setValue(double value) {
	this->value = value;
}



}/* namespace MDPLIB */

