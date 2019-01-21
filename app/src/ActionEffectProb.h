/*
 * StateProb.h
 *
 *  Created on: May 2, 2016
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

#ifndef ACTIONEFFECTPROB_H_
#define ACTIONEFFECTPROB_H_


#include <vector>
#include <string>
#include <muParser.h>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "StateProb.h"

namespace MDPLIB {

class ActionEffectProb {
public:
	ActionEffectProb() {
		prob = 0;
	}

	ActionEffectProb(float p) {
		prob = p < 0 ? 0.0 : p;
	}

	ActionEffectProb(float p, std::vector<std::string> exprList, std::vector<bool> isNullOpList) {
		assert(exprList.size() == isNullOpList.size());
		prob = p < 0 ? 0.0 : p;
		for(unsigned int i = 0; i <  exprList.size(); i++) {
			stateEqVect.push_back(mu::Parser());
			stateEqVect[i].SetExpr(exprList[i]);
			stateEqStringVect.push_back(exprList[i]);
			isNullOp.push_back(isNullOpList[i]);
		}
	}

	ActionEffectProb(float p, std::vector<std::string> exprList) {
		prob = p < 0 ? 0.0 : p;
		for(unsigned int i = 0; i <  exprList.size(); i++) {
			stateEqVect.push_back(mu::Parser());
			stateEqVect[i].SetExpr(exprList[i]);
			stateEqStringVect.push_back(exprList[i]);
			isNullOp.push_back(false);
		}
	}

	ActionEffectProb(std::string pExpr, std::vector<std::string> exprList, std::vector<bool> isNullOpList) {
		assert(exprList.size() == isNullOpList.size());
		prob = -1.0;
		probEq.SetExpr(pExpr);
		probEqString = pExpr;
		for(unsigned int i = 0; i <  exprList.size(); i++) {
			stateEqVect.push_back(mu::Parser());
			stateEqVect[i].SetExpr(exprList[i]);
			stateEqStringVect.push_back(exprList[i]);
			isNullOp.push_back(isNullOpList[i]);
		}
	}

	ActionEffectProb(std::string pExpr, std::vector<std::string> exprList) {
		prob = -1.0;
		probEq.SetExpr(pExpr);
		probEqString = pExpr;
		for(unsigned int i = 0; i <  exprList.size(); i++) {
			stateEqVect.push_back(mu::Parser());
			stateEqVect[i].SetExpr(exprList[i]);
			stateEqStringVect.push_back(exprList[i]);
			isNullOp.push_back(false);
		}
	}

	virtual ~ActionEffectProb() {
		// TODO Auto-generated destructor stub
	}

	mu::Parser getActionVarEffect(int i) {
		if(i < (int)stateEqVect.size())
			return(stateEqVect[i]);
		else
			return(mu::Parser());
	}

	bool isActionVarEffectNullOp(int i) {
		if(i < (int)isNullOp.size())
			return(false);
		else
			return(isNullOp[i]);
	}

	std::vector<bool> isNullOp;
	std::vector<string> stateEqStringVect;
	std::vector<mu::Parser> stateEqVect;
	string probEqString;
	mu::Parser probEq;
	float prob;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    	ar & isNullOp;
    	ar & stateEqStringVect;
    	ar & probEqString;
        ar & prob;
    }
};

} /* namespace MDPLIB */

#endif /* ACTIONEFFECTPROB_H_ */
