/*
 * RewardTable.h
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

#ifndef REWARDTABLE_H_
#define REWARDTABLE_H_

#include "Reward.h"
#include "GlobalDefs.h"
#include <vector>

namespace MDPLIB {

class RewardTable: public MDPLIB::Reward {
public:
	RewardTable(int na, int ns);
	RewardTable(const RewardTable& origRT);  // Creates a copy of the reward function
	virtual ~RewardTable();
	float rewardValue(Action *action, State *s1, State *s2);
	float rewardValue(int aId, int s1Id, int s2Id);
	int setActionRewards(State *s1, State *s2, vector<float> aRewards);
private:
	int numActions;
	int numStates;
	vector< vector< vector<float> > > rewardTable;
};

} /* namespace MDP */
#endif /* REWARDTABLE_H_ */
