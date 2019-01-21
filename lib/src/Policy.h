/*
 * Policy.h
 *
 *  Created on: Jan 8, 2013
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

#ifndef POLICY_H_
#define POLICY_H_

#include "MDP.h"
#include "DecisionRule.h"
#include "GlobalDefs.h"
#include <vector>
#include <cmath>
#include <stdio.h>

namespace MDPLIB {

/*! Policy
 * \brief A mapping from states to the action taken in that state.
 */
class Policy {
public:
	Policy(MDP *mdp, DecisionRule *dr);
	Policy(Policy *origPol);
	virtual ~Policy();

	int optimize();
	int optimize(double convergeCritPol, double convergeCritVal);
	int optimize2(double convergeCritPol, double convergeCritVal);
	int isOptimized();
	void setOptimized(int opt);
	int exportActionProbs(string filename);
//	int setActionForState(State *state, Action *action, float prob);
//	Action *getActionForState(State *state);
	vector<double> getActionProbsForState(State *state);
	vector<double> getActionProbsForState(int sId);
	vector<double> getActionProbsForStateDyn(State *state);
	vector<double> getActionProbsForStateDyn(int sId);
	double getActionProbForState(State *state, Action *action);
	double getActionProbForState(int sId, int aId);
	Action *chooseActionForState(State *state);
	int chooseActionForState(int sId);
//	Policy *copyPolicy();
	float diffPolicyActionProbs(Policy *otherPolicy);
	float diffPolicyStateVals(Policy *otherPolicy);

	DecisionRule* getDecisionRule() const {
		return dRule;
	}

	void setDecisionRule(DecisionRule* rule);

	MDP* getMDP() const {
		return mdp;
	}

	void setMDP(MDP* mdp);
	void resetValues();
	void zeroOutActionProbs();
	double updateActionProbs(void);
    void printModelParams();

	double getConvCritPolicy() const {
		return convCritPolicy;
	}

	void setConvCritPolicy(double convCritPolicy) {
		this->convCritPolicy = convCritPolicy;
	}

	double getConvCritValue() const {
		return convCritValue;
	}

	void setConvCritValue(double convCritValue) {
		this->convCritValue = convCritValue;
	}

	int getMaxIters() const {
		return maxIters;
	}

	void setMaxIters(int maxIters) {
		this->maxIters = maxIters;
	}


protected:
	MDP *mdp;
	DecisionRule *dRule;
	vector<double> valueForState;
	vector<int> allActionIds;
	vector< vector<int> *> *actionIdsForState;
	vector< vector<double> > probsForState;
	vector<bool> probsSetForState;
	int maxIters;
	double convCritPolicy;
	double convCritValue;
	double learningRate;
	int optimized;

	double getStateValue(int sId);
	int updateStateValues(vector<float> &diffVals, vector<bool> &update, double diffCrit);
	double updateStateValues(vector<float> &diffVals);
};

} /* namespace MDP */
#endif /* POLICY_H_ */
