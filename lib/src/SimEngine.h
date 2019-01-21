/*
 * SimEngine.h
 *
 *  Created on: Jun 11, 2017
 *      Author: Michelle LaMar 
 * 
 *      Copyright (C) 2017, 2018  Michelle M. LaMar

This library is free software; you can redistribute it and/or  
modify it under the terms of the GNU Lesser General Public 
License as published by the Free Software Foundation; either 
version 2.1 of the License, or (at your option) any later version. 

This library is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
Lesser General Public License for more details. 

You should have received a copy of the GNU Lesser General Public 
License along with this library; if not, see website. 

mlamar@ets.org 
Educational Testing Service 
660 Rosedale Road 
Princeton, NJ 08541
 */

#ifndef SIMENGINE_H_
#define SIMENGINE_H_

#include "Policy.h"
#include "Transition.h"
#include "GlobalDefs.h"
#include "ActionSequence.h"
#include <string>

namespace MDPLIB {

class SimEngine {
public:
	SimEngine();
	SimEngine(Policy *sp);
	SimEngine(Policy *sp, Transition *et);

	virtual ~SimEngine() {
	}

	ActionSequence *simActions(string label, State *start);

	Transition *getEnvTrans() const {
		return envTrans;
	}

	void setEnvTrans(Transition *envTrans) {
		this->envTrans = envTrans;
	}

	Policy *getSimPol() const {
		return simPol;
	}

	void setSimPol(Policy *simPol) {
		this->simPol = simPol;
		if(this->envTrans == NULL)
			this->envTrans = simPol->getMDP()->getTrans();
	}

	bool isRecordActionChoices() const {
		return record_action_choices;
	}

	void setRecordActionChoices(bool recordActionChoices) {
		record_action_choices = recordActionChoices;
	}

	int getMaxSimRec() const {
		return maxSimRec;
	}

	void setMaxSimRec(int maxSimRec) {
		this->maxSimRec = maxSimRec;
	}

private:
	Policy *simPol;
	Transition *envTrans;
	bool record_action_choices;
	int maxSimRec;
};

} /* namespace MDPLIB */

#endif /* SIMENGINE_H_ */
