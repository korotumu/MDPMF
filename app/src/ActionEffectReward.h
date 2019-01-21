/*
 * ActionEffectReward.h
 *
 *  Created on: Dec 4, 2015
 *      Author: Michelle LaMar
 *
 * This file is part of the mdpmf application.
 * Copyright 2014-2018  Michelle LaMar
 *
	This application is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this software; if not, see https://www.gnu.org/licenses/.

	mlamar@ets.org
	Educational Testing Service
	660 Rosedale Road
	Princeton, NJ 08541
 */

#ifndef ACTIONEFFECTREWARD_H_
#define ACTIONEFFECTREWARD_H_

#include <vector>
#include <string>
#include <MDPlib.h>
#include <Reward.h>
#include <iostream>
#include <stdio.h>
#include <muParser.h>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#define NUM_RPARAMS 6
#define R_MAX_LINE 1028
#define SVR_WILDCARD -99


namespace MDPLIB {

class ActionEffectReward: public MDPLIB::Reward {
public:
	ActionEffectReward() {
		states = NULL;
		actions = NULL;
		params = NULL;
		numRParams = 0;
	}
	ActionEffectReward(StateSpaceVarList *ss, ActionSet *as);
//	ActionEffectReward(string filename, StateSpaceVarList *ss, ActionSet *as);
	virtual ~ActionEffectReward();

	int initFromIOTable(IOTable iot);

	float rewardValue(Action *action, State *s1, State *s2);
	float rewardValue(int aId, int s1Id, int s2Id);

	ParamList *getParamList();
	void setParamList(ParamList *params, vector<int> pIndex);
	void setParams(vector<double> vals);
	int setEstParams(vector<bool> ep);


private:
	StateSpaceVarList *states;
	ActionSet *actions;
	ParamList *params;

	unsigned int numRParams;
	vector<string> paramNames;
	vector<int> actionIds;
	vector<float> defaultValues;
	vector<float> currValues;
	vector<string> condExprStr;
	vector<mu::Parser> condExpr;

	vector<string> stateVarNames;
	vector<double> stateVarValues;

	vector<int> pListMap;

	// The cached reward values:
//	vector< vector<float> > rewardCache;

	// For each state, which of the conditions does it meet?
	vector< vector<bool> > condMap;
	// For each action, which of the rewards apply?
	vector< vector<bool> > actionMap;

	vector<bool> estParams;
	vector<float> paramMin;
	vector<float> paramMax;


	int loadRewardsFromIOTable(IOTable iot);
	void resetParamIndicies();
	void setCondMap();
	bool evalCond(mu::Parser cond, int stateId);
	float dotProd(vector<bool> b1, vector<bool> b2, vector<float> f);
	void loadStateValues(int sId);

	double getParamValueAt(int i) {
		return(estParams[i] ? params->getParamValue(pListMap[i]) : currValues[i]);
	}

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<MDPLIB::Reward>(*this);
        ar & states;
        ar & actions;
        ar & params;
        ar & numRParams;
        ar & paramNames;
        ar & actionIds;
        ar & defaultValues;
        ar & currValues;
        ar & condExprStr;
//        ar &vector<mu::Parser> condExpr;
        ar & stateVarNames;
        ar & stateVarValues;
        ar & pListMap;
        ar & condMap;
        ar & actionMap;
        ar & estParams;
        ar & paramMin;
        ar & paramMax;
    }
};

} /* namespace MDPLIB */

#endif /* ACTIONEFFECTREWARD_H_ */
