/*
 * StateVarList.h
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

#ifndef STATEVARLIST_H_
#define STATEVARLIST_H_

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "VarDesc.h"
#include "State.h"

namespace MDPLIB {

using namespace std;

class StateVarList: public MDPLIB::State {

public:
	StateVarList() {
		id = -1;
		numVars = 0;
		varDescList = NULL;
	}
	StateVarList(int id);
	StateVarList(int id, vector<VarDesc *> *descList, vector<float> values);
	virtual ~StateVarList();
    int getId() const;
    void setId(int id);
    vector<float> *getVarData();
    int getVarType(string name);
    int getVarType(int index);
    float getVarValue(string name);
    float getVarValue(int index);
    int getDiscreteVarValue(int index);
    string getCategoryVarValue(int index);
	string getLabel();

private:
    int numVars;
    vector<VarDesc *> *varDescList;
    vector<float> varData;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<MDPLIB::State>(*this);
        ar & numVars;
        ar & varDescList;
        ar & varData;
    }
};

} /* namespace MDPLIB */

#endif /* STATEVARLIST_H_ */
