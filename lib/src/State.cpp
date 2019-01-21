/*
 * State.cpp
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

#include "State.h"

namespace MDPLIB {


State::State(int id) {

	this->id = id;
	this->label = string("");
}

State::State(int id, string lab)
{

	this->id = id;
	this->label = string(lab);
}

State::~State() {
	// TODO Auto-generated destructor stub
}

int State::getId() const
{
	return id;
}

void State::setId(int id)
{
	this->id = id;
}

string State::getLabel() {
	return this->label;
}

}/* namespace MDP */
