/*
 * VarDesc.cpp
 *
 *  Created on: Jan 9, 2013
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

#include "VarDesc.h"

namespace MDPLIB {

VarDesc::VarDesc() {
	// TODO Auto-generated constructor stub

}

VarDesc::VarDesc(string name, int type, float min, float max) {
	this->name = name;
	this->type = type;
	this->min = min;
	this->max = max;
	this->step = 1;
	this->catNum = 0;
}

VarDesc::VarDesc(string name, int type, float min, float max, float step) {
	this->name = name;
	this->type = type;
	this->min = min;
	this->max = max;
	this->step = step;
	this->catNum = 0;
}

VarDesc::~VarDesc() {
	// TODO Auto-generated destructor stub
}


string VarDesc::getName() const {
	return name;
}

void VarDesc::setName(string name) {
	this->name = name;
}

vector<string> VarDesc::getCatLabels() const
{
	return catLabels;
}

int VarDesc::getCatNum() const
{
	return catNum;
}

int VarDesc::addCategoryValue(string label) {
	if(type != CATEGORY)
		return MDP_ERROR;

	catLabels.push_back(label);
	min = 1;
	max = catNum = catLabels.size();

	return MDP_SUCCESS;
}


float VarDesc::getMax() const {
	return(this->max);
}

float VarDesc::getMin() const {
	return(this->min);
}

int VarDesc::getType() const {
	return(this->type);
}

float VarDesc::getStep() const {
	return(this->step);
}

void VarDesc::setMax(float cmax) {
	this->max = cmax;
}

void VarDesc::setMin(float cmin) {
	this->min = cmin;
}

void VarDesc::setType(int type) {
	this->type = type;
}

void VarDesc::setStep(float step) {
	this->step = step;
}

} /* namespace MDP */
