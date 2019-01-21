/*
 * RewardTable.cpp
 *
 *  Created on: Feb 18, 2013
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

#include "RewardTable.h"

namespace MDPLIB {

RewardTable::RewardTable(int na, int ns)
	: rewardTable(ns, vector< vector<float> >(ns, vector<float>(na, 0.0)))
{
	numActions = na;
	numStates = ns;
	/*
	vector< vector<float> >l2rt(ns);

	int s1, s2;
	for(s1 = 0; s1 <= ns; s1++) {
		vector< vector<float> >l2rt;
		for(s2 = 0; s2 <= ns; s2++) {
			vector<float> l3rt(na, 0.0);
			l2rt.push_back(l3rt);
		}
		rewardTable.push_back(l2rt);
	}
	*/

}

RewardTable::RewardTable(const RewardTable& origRT)
	: rewardTable(origRT.numStates, vector< vector<float> >(origRT.numStates))
{
	numActions = origRT.numActions;
	numStates = origRT.numStates;

	int s1, s2;
	for(s1 = 0; s1 < numStates; s1++) {
		for(s2 = 0; s2 < numStates; s2++) {
			rewardTable[s1][s2] = vector<float>(origRT.rewardTable[s1][s2]);
		}
	}
}

RewardTable::~RewardTable() {
	// TODO Auto-generated destructor stub
}

float RewardTable::rewardValue(Action* action, State* s1, State* s2) {

	// TODO fix this error return -- should not be a valid return value.
	if(s1->getId() >= numStates || s2->getId() >= numStates || action->getId() >= numActions)
		return PROB_ERR;

	return rewardTable[s1->getId()][s2->getId()][action->getId()];
}


float RewardTable::rewardValue(int aId, int s1Id, int s2Id) {

	// TODO fix this error return -- should not be a valid return value.
	if(s1Id >= numStates || s2Id >= numStates || aId >= numActions)
		return PROB_ERR;

	return rewardTable[s1Id][s2Id][aId];
}

int RewardTable::setActionRewards(State* s1, State* s2, vector<float> aRewards) {
	int a;
	int s1_id = s1->getId();
	int s2_id = s2->getId();
	if((int)aRewards.size() != numActions)
		return MDP_ERROR;

	for(a = 0; a < numActions; a++)
		rewardTable[s1_id][s2_id][a] = aRewards[a];
	return MDP_SUCCESS;
}


} /* namespace MDP */
