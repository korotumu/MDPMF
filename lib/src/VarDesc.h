/*
 * VarDesc.h
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

#ifndef VARDESC_H_
#define VARDESC_H_


#include <vector>
#include <string>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "GlobalDefs.h"

namespace MDPLIB {

// Allowed variable types:
#define TYPE_ERROR -1
#define DISCRETE 1
#define CONTINUOUS 2
#define CATEGORY 3


using namespace std;


class VarDesc {
public:
	VarDesc();
	VarDesc(string name, int type, float min, float max);
	VarDesc(string name, int type, float min, float max, float step);
	virtual ~VarDesc();

    vector<string> getCatLabels() const;
    int getCatNum() const;
    float getMax() const;
    float getMin() const;
    int getType() const;
    void setCatLabels(vector<string> catLabels);
    void setCatNum(int catNum);
    int addCategoryValue(string label);
    void setMax(float cmax);
    void setMin(float cmin);
    void setType(int type);
	float getStep() const;
	void setStep(float step);

	string getName() const;
	void setName(string name);


protected:
    string name;
	int type;
	float min;
	float max;
	float step;
	int catNum;
	vector<string> catLabels;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    	ar & name;
        ar & type;
        ar & min;
        ar & max;
        ar & step;
        ar & catNum;
        ar & catLabels;
    }
};

} /* namespace MDP */

#endif /* VARDESC_H_ */
