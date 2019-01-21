/*
 * CompiledModel.h
 *
 *  Created on: Apr 12, 2018
 *      Author: Michelle LaMar 
 * 
 * This file is part of the mdpmf application.
 * Copyright 2014-2018  Michelle LaMar
 *
	This application is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this software; if not, see https://www.gnu.org/licenses/.

	mlamar@ets.org
	Educational Testing Service
	660 Rosedale Road
	Princeton, NJ 08541
 */

#ifndef COMPILEDMODEL_H_
#define COMPILEDMODEL_H_


#include "MDPlib.h"
#include "ActionEffectTable.h"
#include "ActionEffectTrans.h"
#include "ActionEffectReward.h"
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace MDPLIB {

class CompiledModel {
public:
	CompiledModel() {
		actions = NULL;
		states = NULL;
		reward = NULL;
		trans = NULL;
		worldTrans = NULL;
		startState = NULL;
	}

	CompiledModel(ActionSet *a, StateSpaceVarList *s, ActionEffectReward *r, ActionEffectTrans *t, ActionEffectTrans *wt, StateVarList *ss) {
		actions = a;
		states = s;
		reward = r;
		trans = t;
		worldTrans = wt;
		startState = ss;
	}

	virtual ~CompiledModel() {
		// TODO Auto-generated destructor stub
	}

	ActionSet* getActions() const {
		return actions;
	}

	void setActions(ActionSet* actions) {
		this->actions = actions;
	}

	ActionEffectReward* getReward() const {
		return reward;
	}

	void setReward(ActionEffectReward* reward) {
		this->reward = reward;
	}

	StateSpaceVarList *getStates() const {
		return states;
	}

	void setStates(StateSpaceVarList* states) {
		this->states = states;
	}

	 ActionEffectTrans* getTrans() const {
		return trans;
	}

	void setTrans(ActionEffectTrans* trans) {
		this->trans = trans;
	}

	ActionEffectTrans* getWorldTrans() const {
		return worldTrans;
	}

	void setWorldTrans(ActionEffectTrans* worldTrans) {
		this->worldTrans = worldTrans;
	}

	StateVarList* getStartState() const {
		return startState;
	}

	void setStartState(StateVarList* startState) {
		this->startState = startState;
	}

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & actions;
        ar & states;
        ar & reward;
        ar & trans;
        ar & worldTrans;
        ar & startState;
    }
	ActionSet *actions;
	StateSpaceVarList *states;
	ActionEffectReward *reward;
	ActionEffectTrans *trans;
	ActionEffectTrans *worldTrans;
	StateVarList *startState;
};

} /* namespace MDPLIB */

#endif /* COMPILEDMODEL_H_ */
