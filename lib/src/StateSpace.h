/*
 * StateSpace.h
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

#ifndef STATESPACE_H_
#define STATESPACE_H_

#include <vector>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "State.h"
#include "GlobalDefs.h"

namespace MDPLIB {

#define END_OF_STATES -2
#define NO_SUCH_STATE -3

/*! StateSpace
 *  \brief The definition of all possible states in which the system can be.
 *
 *  The foundation class StateSpace is defined by a simple list of states.
 */
class StateSpace {
public:
	StateSpace();
	virtual ~StateSpace();

	virtual int getNumStates();

	/*! \fn State *startIterator(void)
	 *  \brief Set the StateSpace iterator to the first State and return the first State.
	 */
	virtual State *startIterator();

	/*! \fn State *nextState(void)
	 *  \brief Return the next state in the StateSpace iterator.
	 *
	 *  When the iterator runs out of states, it will return a state with an id of END_OF_STATES.
	 */
	virtual State *nextState();
	virtual State *randState();
	virtual State *getState(int id);
	int addState(State *s);

protected:
	int numStates;
	int curStateIndex;
	vector<State *> stateList;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & numStates;
        ar & curStateIndex;
        ar & stateList;
    }
};

}   /* namespace MDP */
#endif /* STATESPACE_H_ */
