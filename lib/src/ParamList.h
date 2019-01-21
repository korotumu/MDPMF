/*
 * ParamList.h
 *
 *  Created on: Dec 5, 2013
 *      Author: Michelle LaMar
 *
 *	Revised on: Apr 18, 2017
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

#ifndef PARAMLIST_H_
#define PARAMLIST_H_

#include <vector>
#include <string>
#include <math.h>
#include <string.h>
#include <cstdlib>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "GlobalDefs.h"

using namespace std;

namespace MDPLIB {

#define POS_INF 999999.0
#define NEG_INF -999999.0
#define NA_REF -1
#define BAD_REF -2

enum paramType_t {pt_error, pt_fixed, pt_estPop, pt_estPers, pt_result};
enum paramDist_t {pd_error, pd_norm, pd_lnorm, pd_unif, pd_constant};
enum paramComp_t {pc_error, pc_beta, pc_reward, pc_trans, pc_gamma, pc_none, pc_diagnostic};



class ParamDist {
public:
	ParamDist(){
		this->distType = pd_error;
		this->hypParamA_ind = -1;
		this->hypParamB_ind = -1;
	}
	ParamDist(const MDPLIB::ParamDist& d) {
		this->distType = d.distType;
		this->hypParamA_lab = string(d.hypParamA_lab);
		this->hypParamB_lab = string(d.hypParamB_lab);
		this->hypParamA_ind = d.hypParamA_ind;
		this->hypParamB_ind = d.hypParamB_ind;
	}


	~ParamDist() {
	}

	paramDist_t distType;
	string hypParamA_lab;
	string hypParamB_lab;
	int hypParamA_ind;
	int hypParamB_ind;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & distType;
        ar & hypParamA_lab;
        ar & hypParamB_lab;
        ar & hypParamA_ind;
        ar & hypParamB_ind;
    }
};

/* This class packages up all the information about a parameter into a single packet.  Note that for estimation
 * it is still most efficient to store parameters in the ParamList below, as the vector of values can be passed easily.
 */

class Param {
public:
	Param(string l, double v, paramType_t t, paramComp_t pc, paramDist_t d, Param *hpA, Param *hp);
	Param(string l, double v, paramType_t t, paramComp_t pc, paramDist_t d, Param *hpA, Param *hp, vector<double> ci, double se);
	~Param();
	const vector<double>& getConfInterval() const;
	void setConfInterval(double lb, double hb);
	double getStdErrEst() const;
	void setStdErrEst(double stdErrEst);
	double getValue() const;
	void setValue(double value);

	string label;
	double value;
	paramType_t type;
	paramComp_t modComponent;
	double minVal;
	double maxVal;
	vector<double> confInterval;
	double stdErrEst;
	paramDist_t distribution;
	Param *hypParamA;
	Param *hypParamB;
};


class ParamList {
public:
	ParamList();
	virtual ~ParamList();

	int addParams(ParamList *pl);
	int addParam(string *lab, double val, double min, double max);
	int addParam(const char lab[], double val, double min, double max);
	int addParam(string* lab, double val, paramType_t pt, paramComp_t pc);
	int addParam(const char lab[], double val, paramType_t pt, paramComp_t pc);
	int addParam(const char lab[], double val, paramType_t type, paramComp_t pc, paramDist_t dist);
	int addParam(const char lab[], double val, paramType_t type, paramComp_t pc, paramDist_t dist, int hpA, int hpB);
	int addParam(string *lab, double val, paramType_t type, paramComp_t pc, paramDist_t dist, int hpA, int hpB);
	int addParam(string *lab, double val, paramType_t type, paramComp_t pc, paramDist_t dist, int hpA, int hpB, double se, double ciLow, double ciHigh);
	int addParam(string lab, double val, paramType_t type, paramComp_t pc, paramDist_t dist, int hpA, int hpB, double se, double ciLow, double ciHigh);
	int addParam(Param p);
	int addConstant(const char lab[], double val);
	int addConstant(string *lab, double val);
	int addParamDistribution(int i, string distStr);
	int updateParamRange(int i);
	int getLength();
	int removeParam(int i);
	int getParamIndex(string *lab);
	int getParamIndex(const char lab[]);
	string *getParamLabel(int i);
	double getParamValue(int i);
	double getParamSE(int i);
	Param *getHyperParamA(int i);
	Param *getHyperParamB(int i);
	int getHyperParamAIndex(int i);
	int getHyperParamBIndex(int i);
	double getHyperParamAValue(int i);
	double getHyperParamBValue(int i);
	vector<double> getParamCI(int i);
	paramComp_t getParamComponent(int i);
	bool isParamFree(int i);
	bool isParamEstPop(int i);
	bool isParamEstPers(int i);
	double getMinValue(int i);
	double getMaxValue(int i);
	int setParamLabel(int i, string *lab);
	int setParamValue(int i, double val);
	int setParamRange(int i, double min, double max);
	int setParamComponent(int i, paramComp_t pc);
	int setParam(int i, Param p);
	Param *getParam(int i);
	vector<double> *getValuesArray();
	ParamList *getParamSubList(int i);
	int setParamSubList(int i, ParamList *psl);

private:
	int length;
	vector<string> labels;
	vector<double> values;
	vector<paramType_t> types;  // One of the paraType_t values above: fixed, estPop, or estPers
	vector<paramComp_t> modelComponents; // Which part of the model does this parameter come from -- will effect the best way to estimate it
	vector<double> minVals;
	vector<double> maxVals;
	vector<paramDist_t> distributions;
	vector<int> hypParamA_index;
	vector<int> hypParamB_index;
	vector< vector<double> > confInterval; // vector of pairs
	vector<double> stdErrEst;
	vector<ParamList *> paramSubList;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & length;
        ar & labels;
        ar & values;
        ar & types;  // One of the paraType_t values above: fixed, estPop, or estPers
        ar & modelComponents; // Which part of the model does this parameter come from -- will effect the best way to estimate it
        ar & minVals;
        ar & maxVals;
        ar & distributions;
        ar & hypParamA_index;
        ar & hypParamB_index;
        ar &  confInterval; // vector of pairs
        ar & stdErrEst;
        ar & paramSubList;

    }
};

} /* namespace MDPLIB */
#endif /* PARAMLIST_H_ */
