/*
 * PlayRecs.h
 *
 *  Created on: Jan 15, 2014
 *      Author: Michelle
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

#ifndef SC_PLAYRECS_H_
#define SC_PLAYRECS_H_

#include "MDP.h"
#include "IOTable.h"
#include "StateSpaceVarList.h"
#include "StateProb.h"
#include "ActionSequence.h"
#include <vector>
#include <string>
#include <cstring>
#include <boost/tokenizer.hpp>

using namespace MDPLIB;

vector<ActionSequence *> *readPlayRecordsFromTable(MDPLIB::IOTable table, MDP *mdp, bool et, StateVarList *defStartState, bool v);
MDPLIB::IOTable writePlayRecordsToTable(vector<ActionSequence *> recs, StateSpaceVarList *ss, ActionSet *as, StateVarList *startState, bool writeRewardsOption);


#endif /* SC_PLAYRECS_H_ */
