/*
 * ActionSet.h
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

#ifndef ACTIONSET_H_
#define ACTIONSET_H_

#include "Action.h"
#include "GlobalDefs.h"
#include <vector>

#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#define END_OF_ACTIONS -2

namespace MDPLIB {

using namespace std;

class ActionSet {
public:
	ActionSet();
	virtual ~ActionSet();
	int getNumActions();
	int addAction(string actLab);
	void startIterator();
	Action *nextAction();
	Action *getAction(int id);
	Action *getAction(string lab);
	vector< Action *> getActionList();

	int getStopAction() const {
		return stopAct;
	}

	void setStopAction(int stopAct) {
		this->stopAct = stopAct;
	}

protected:
	int length;
	int index;
	int stopAct;
	vector<Action *> list;
//	vector<float> probability;
	Action endAction;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & length;
        ar & index;
        ar & stopAct;
        ar & list;
        ar & endAction;
    }
};

} /* namespace MDP */
#endif /* ACTIONSET_H_ */
