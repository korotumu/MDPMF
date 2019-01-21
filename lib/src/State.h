/*
 * State.h
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

#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "VarDesc.h"

namespace MDPLIB {

using namespace std;

class State {
public:
	State() {
		id = -1;
		label = "";
	}
	State(int id);
	State(int id, string lab);
	virtual ~State();
    virtual int getId() const;
    virtual void setId(int id);

	virtual string getLabel();

	virtual void setLabel(const string& label) {
		this->label = label;
	}

protected:
    int id;
    string label;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & id;
        ar & label;
    }
};

} /* namespace MDP */
#endif /* STATE_H_ */
