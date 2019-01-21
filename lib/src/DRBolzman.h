/*
 * DRBolzman.h
 *
 *  Created on: Mar 6, 2013
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

#ifndef DRBOLZMAN_H_
#define DRBOLZMAN_H_

#include "DecisionRule.h"

namespace MDPLIB {

class DR_Bolzman: public MDPLIB::DecisionRule {
public:
	DR_Bolzman(float b);
	virtual ~DR_Bolzman();

	std::vector<double> getActionProbsForState(State *s, MDP *mdp, vector<int> actionIds, const vector<double> &stateVals);
	std::vector<double> getActionProbsForState(int sId, MDP *mdp, vector<int> actionIds, const vector<double> &stateVals);
	int getActionProbsForState(std::vector<double> &aprobs, int sId, MDP *mdp, vector<int> actionIds, const vector<double> &stateVals);
	Action *selectActionForState(State *s, MDP *mdp, vector<int> actionIds, vector<double> stateVals);

private:
	float beta;
};

} /* namespace MDPLIB */
#endif /* DRBOLZMAN_H_ */
