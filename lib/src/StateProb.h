/*
 * StateProb.h
 *
 *  Created on: May 2, 2016
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

#ifndef STATEPROB_H_
#define STATEPROB_H_


#include <vector>
#include <string>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace MDPLIB {

class StateProb {
public:
	StateProb() {
		stateId = -1;
		prob = 0;
	}

	StateProb(float p, int sId) {
		stateId = sId;
		prob = p;
	}

	virtual ~StateProb() {
		// TODO Auto-generated destructor stub
	}

	float getProb() const {
		return prob;
	}

	void setProb(float prob) {
		this->prob = prob;
	}

	int getStateId() const {
		return stateId;
	}

	void setStateId(int stateId) {
		this->stateId = stateId;
	}

	int stateId;
	float prob;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & stateId;
        ar & prob;
    }
};


} /* namespace MDPLIB */

#endif /* STATEPROB_H_ */
