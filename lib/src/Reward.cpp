/*
 * Reward.cpp
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

#include "Reward.h"

namespace MDPLIB {

Reward::Reward() {
	pList = new ParamList();

}

Reward::~Reward() {
	// TODO Auto-generated destructor stub
}

float Reward::rewardValue(Action* action, State* s1, State* s2) {
	// Just a stub function.  The real work will be in a sub-class.
	return 0.0;
}

float Reward::rewardValue(int a, int s1, int s2) {
	// Just a stub function.  The real work will be in a sub-class.
	return 0.0;
}

ParamList *MDPLIB::Reward::getParamList() {
	return(pList);
}

void MDPLIB::Reward::setParamList(ParamList* params, vector<int> pIndex) {
	pList = params;
}


int MDPLIB::Reward::getNumParams() {
	return(0);
}

void MDPLIB::Reward::setParams(vector<double> vals) {
}

void MDPLIB::Reward::paramsUpdated() {

}

}


/* namespace MDP */
