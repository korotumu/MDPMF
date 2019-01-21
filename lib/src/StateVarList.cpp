/*
 * StateVarList.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: Michelle LaMar
 *
 *      Copyright (C)  2014  Michelle M. LaMar

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

#include "StateVarList.h"

namespace MDPLIB {

StateVarList::StateVarList(int id)
	: State(id)
{
	this->numVars = 0;
}

StateVarList::StateVarList(int id, vector<VarDesc *> *descList, vector<float> values)
	: State(id), varData(values)
{
	this->numVars = descList->size();
	this->varDescList = descList;
}

StateVarList::~StateVarList() {
	// TODO Auto-generated destructor stub
}

int StateVarList::getId() const
{
	return id;
}

void StateVarList::setId(int id)
{
	this->id = id;
}

int StateVarList::getVarType(string name) {
	// Probably should implment this as a hash table.  For now, doing it the slow way ...
	int i;
	for(i=0; i<numVars; i++) {
		if(name.compare(varDescList->at(i)->getName()) == 0)
			return varDescList->at(i)->getType();
	}
	return TYPE_ERROR;
}

int StateVarList::getVarType(int index)
{
	return (index >= numVars || index < 0) ? TYPE_ERROR : varDescList->at(index)->getType();
}

float StateVarList::getVarValue(string name) {
	// Probably should implment this as a hash table.  For now, doing it the slow way ...
	int i;
	for(i=0; i<numVars; i++) {
		if(name.compare(varDescList->at(i)->getName()) == 0)
			return varData[i];
	}
	return VALUE_ERROR;
}

float StateVarList::getVarValue(int index) {
	return (index >= numVars || index < 0) ? VALUE_ERROR : varData[index];
}

int StateVarList::getDiscreteVarValue(int index) {
	if( index < 0 || index >= numVars || varDescList->at(index)->getType() != DISCRETE)
		return (int)(varDescList->at(index)->getMin()) - 1;

	return (int)varData[index];
}

string StateVarList::getCategoryVarValue(int index) {
	if( index < 0 || index >= numVars || varDescList->at(index)->getType() != CATEGORY)
		return "VALUE_ERROR";

	return varDescList->at(index)->getCatLabels()[(int)varData[index]];
}


vector<float>* StateVarList::getVarData() {
	return(&varData);
}

string StateVarList::getLabel() {
	string lab;

	int i;
	for(i=0; i<numVars; i++) {
		lab += varDescList->at(i)->getName();
		lab += "=";
		lab += to_string(varData[i]);
		lab += " ";
	}
	return(lab);
}


} /* namespace MDPLIB */

