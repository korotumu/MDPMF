/*
 * StateSpaceVarList.h
 *
 *  Created on: Jan 7, 2014
 *      Author: Michelle LaMar
 *
 *      Copyright (C) 2014  Michelle M. LaMar

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

#ifndef STATESPACEVARLIST_H_
#define STATESPACEVARLIST_H_

#include <math.h>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "StateSpace.h"
#include "StateVarList.h"
#include "VarDesc.h"
#include "GlobalDefs.h"

namespace MDPLIB {

/*! StateSpaceVarList
 *  \brief The definition of all possible states in which the system can be.
 *
 *  The StateSpace is defined by a set of variables, or dimensions, each of which
 *  can take on a set of values.  The variables can be discrete, as in integers,
 *  continuous, as real numbers, or categorical, which are labeled discrete variables.
 */
class StateSpaceVarList: public MDPLIB::StateSpace {
public:
	StateSpaceVarList();
	virtual ~StateSpaceVarList();

	virtual int getNumStates();
	virtual State *startIterator();
	virtual State *nextState();
	virtual State *randState();
	virtual StateVarList *getState(int id);

	/*! \fn int addDiscreteVar(string name, int min, int max)
	 *  \brief Add an integer variable to the State Space
	 *  \param name The name of the variable.
	 *  \param min The minimum value the variable can take.
	 *  \param max The maximum value the variable can take.
	 */
	virtual int addDiscreteVar(string name, int min, int max);
	/*! \fn int addContinuousVar(string name, float min, float max)
	 *  \brief Add a continuous variable to the State Space
	 *  \param name The name of the continuous variable.
	 *  \param min The minimum value the variable can take.
	 *  \param max The maximum value the variable can take.
	 *  \param step The increment used between states along this dimension.
	 */
	virtual int addContinuousVar(string name, float min, float max, float step);
	/*! \fn int addCategoryVar(string name)
	 *  \brief Add a categorical variable to the State Space
	 *
	 *  Category values must be added individually using the addCategoryToVar function.
	 *  \param name The name of the categorical variable.
	 */
	virtual int addCategoryVar(string name);
	/*! \fn int addCategoryToVar(string name)
		 *  \brief Add a new category value to a categorical variable
		 *
		 *  Categories must be added individually using the addCategoryToVar function.
		 *  \param name The name of the categorical variable.
		 *  \param catLabel The label of the new category value.
		 */
	virtual int addCategoryToVar(string name, string catLabel);
	virtual int getNumVars();
	virtual StateVarList *getState(vector<float> data);
	virtual int getStateId(vector<float> data);
	virtual StateVarList *getState(vector<double> data);
	virtual int getStateId(vector<double> data);
	virtual int getVarInd(string name);
	virtual string getStateVarName(int vId);
	virtual double getStateVarValue(int sId, int vId);
	VarDesc *getStateVarDesc(int vInd);
	int compareStateVarValue(int vId, int s1Id, int s2Id);

	int setReachableStates(vector<int> reachableIDs);

protected:
	int numVars;
	vector<VarDesc *> varDescList;
    vector<string> varLabels;
    vector<int> varType;
	vector<int> varNumValues;
	vector<float> varMinValues;
	vector<float> varMaxValues;
	vector<float> varStepSize;
	vector<int> varChunkSize;
	vector<int> curVarIndex;

	// For efficiency, we can store a list of only the reachable states:
	bool pruneToReachable;
	int numReachable;
	vector<int> reachableStates;
	vector<int> origID2reachableID;

	void recalcChunks();
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<MDPLIB::StateSpace>(*this);
        ar & numVars;
        ar & varDescList;
        ar & varLabels;
        ar & varType;
        ar & varNumValues;
        ar & varMinValues;
        ar & varMaxValues;
        ar & varStepSize;
        ar & varChunkSize;
        ar & curVarIndex;
        ar & pruneToReachable;
        ar & numReachable;
        ar & reachableStates;
        ar & origID2reachableID;
    }
};

} /* namespace MDPLIB */

#endif /* STATESPACEVARLIST_H_ */
