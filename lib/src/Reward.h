/*
 * Reward.h
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

#ifndef REWARD_H_
#define REWARD_H_

#include "Action.h"
#include "State.h"
#include "ParamList.h"
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace MDPLIB {

class Reward {
public:
	Reward();

	virtual ~Reward();
	virtual float rewardValue(Action *action, State *s1, State *s2);
	virtual float rewardValue(int aId, int s1Id, int s2Id);
	virtual ParamList *getParamList();
	virtual void setParamList(ParamList *params, vector<int> pIndex);
	virtual int getNumParams();
	virtual void setParams(vector<double> vals);
	virtual void paramsUpdated();
protected:
	ParamList *pList;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & pList;
    }
};

} /* namespace MDP */
#endif /* REWARD_H_ */
