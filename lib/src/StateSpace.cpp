/*
 * StateSpace.cpp
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


#include "StateSpace.h"

namespace MDPLIB {

StateSpace::StateSpace() {
	numStates = 0;
	curStateIndex = 0;
}

StateSpace::~StateSpace() {
	int i;
	for(i=1; i<numStates; i++){
		delete(stateList[i]);
	}
}


int StateSpace::addState(State *s) {

	s->setId(numStates);
	stateList.push_back(s);
	numStates++;

	return MDP_SUCCESS;
}

State *StateSpace::startIterator()
{
	if(numStates == 0)
		return new State(END_OF_STATES);

	curStateIndex = 0;

	return(stateList[curStateIndex]);
}

State *StateSpace::nextState()
{

	if(curStateIndex +1 >= numStates)
		return new State(END_OF_STATES);

	curStateIndex++;

	return(stateList[curStateIndex]);
}

State *MDPLIB::StateSpace::randState() {
	// TODO implement this or delete the API call.
	return ((State *) (0));
}

int StateSpace::getNumStates() {
	return numStates;
}

State *StateSpace::getState(int id) {
	if(id >= numStates)
			return (State *)0;
	else
		return(stateList[id]);
}





} /* namespace MDP */
