/*
 * DRIRT.h
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

#ifndef DRIRT_H_
#define DRIRT_H_

#include "DecisionRule.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>

namespace MDPLIB {

class DR_IRT: public MDPLIB::DecisionRule {
public:
	DR_IRT(float theta);
	virtual ~DR_IRT();

	std::vector<double> getActionProbsForState(State *s, MDP *mdp, vector<int> actionIds, const vector<double> &stateVals);
	std::vector<double> getActionProbsForState(int sId, MDP *mdp, vector<int> actionIds, const vector<double> &stateVals);
	int getActionProbsForState(std::vector<double> &aprobs, int sId, MDP *mdp, vector<int> actionIds, const vector<double> &stateVals);
	Action *selectActionForState(State *s, MDP *mdp, vector<int> actionIds, vector<double> stateVals);
	Action *selectActionFromProbs(State *s, MDP *mdp, vector<int> actionIds, vector<double> actProbs);
	int selectActionForState(int sId, MDP *mdp, vector<int> actionIds, vector<double> stateVals);
	int selectActionFromProbs(int sId, MDP *mdp, vector<int> actionIds, vector<double> actProbs);

	float getTheta() const {
		return theta;
	}

	float getLogTheta() const {
		return log(theta);
	}

	void setTheta(float theta) {
		this->theta = theta;
	}

	void setLogTheta(float theta) {
		this->theta = exp(theta);
	}

private:
	float theta;
};
} /* namespace MDPLIB */
#endif /* DRIRT_H_ */
