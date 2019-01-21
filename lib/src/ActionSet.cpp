/*
 * ActionSet.cpp
 *
 *  Created on: Jan 7, 2013
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

#include "ActionSet.h"

namespace MDPLIB {

ActionSet::ActionSet() : endAction(END_OF_ACTIONS, "END_OF_ACTIONS"){
	length = 0;
	index = -1;
	stopAct = -1;
}


ActionSet::~ActionSet() {
	// Delete all the actions:
	int a;
	for(a=0; a < length; a++)
		delete(list[a]);
}

int ActionSet::getNumActions() {
	return length;
}

int ActionSet::addAction(string al){
	Action *newAction = new Action(length, al);
	length++;
	list.push_back(newAction);

	return (length-1);
}

// TODO: Make this consistent with StateSpace iterator.
void ActionSet::startIterator() {
	index = -1;
}

Action *ActionSet::nextAction() {
	if(index+1  < length) {
		index++;
		return(list[index]);
	}
	return(&endAction);
}


Action *ActionSet::getAction(int id) {
	if(id < length) {
		return(list[id]);
	}
	return(&endAction);
}

Action* ActionSet::getAction(string lab) {
	int i;
	for(i = 0; i<length; i++) {
		if(lab.compare(list[i]->getLabel()) == 0) {
			return list[i];
		}
	}
	return(&endAction);
}

vector<Action*> ActionSet::getActionList() {
	return(list);
}


} /* namespace MDP */
