/*
 * ActionModel.h
 *
 *  Created on: Jan 25, 2013
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

#ifndef DECISIONRULE_H_
#define DECISIONRULE_H_

#include "Action.h"
#include "ActionSet.h"
#include "MDP.h"
#include <vector>

namespace MDPLIB {

/*! ActionModel
 * \brief The model used for choosing an action given a current state and Q values for the available actions
 *
 * This class is intended to be abstract, as actual models will be subclasses.
 */
class DecisionRule {
public:
	DecisionRule();
	virtual ~DecisionRule();

	virtual std::vector<double> getActionProbsForState(State *s, MDP *mdp, vector<int> actionIds, const vector<double> &stateVals);
	virtual std::vector<double> getActionProbsForState(int sId, MDP *mdp, vector<int> actionIds, const vector<double> &stateVals);
	virtual int getActionProbsForState(std::vector<double> &aprobs, int sId, MDP *mdp, vector<int> actionIds, const vector<double> &stateVals);
	virtual Action *selectActionForState(State *s, MDP *mdp, vector<int> actionIds, vector<double> stateVals);
	virtual Action *selectActionFromProbs(State *s, MDP *mdp, vector<int> actionIds, vector<double> actProbs);
	virtual int selectActionForState(int sId, MDP *mdp, vector<int> actionIds, vector<double> stateVals);
	virtual int selectActionFromProbs(int sId, MDP *mdp, vector<int> actionIds, vector<double> actProbs);
};

} /* namespace MDP */
#endif /* DECISIONRULE_H_ */
