/*
 * ModelSpec.h
 *
 *  Created on: Apr 25, 2017
 *      Author: Rutuja Ubale, Michelle LaMar
 *
 *      Copyright (C) 2017, 2018  Michelle M. LaMar
 *
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

#ifndef MODELSPEC_H_
#define MODELSPEC_H_

#include <vector>
#include <string>
#include "GlobalDefs.h"

using namespace std;

namespace MDPLIB {

#define POS_INF 999999.0
#define NEG_INF -999999.0

class ModelSpecList {
public:
	ModelSpecList();
	virtual ~ModelSpecList();

	int addSpec(ModelSpecList *ps);
	string *getModelName(int i);
	string *getCogProfile(int i);
	string *getTaskName(int i);
	string *getAetPlayer(int i);
	string *getRtPlayer(int i);
	string *getAetWorld(int i);
	string *getParamSpec(int i);
	int getLength();

	int addSpec(const char model[], const char cogProfile[], const char taskName[],
			const char aet_player[], const char rt_player[], const char aet_world[],
			const char paramSpec[]);
	int addSpec(string *modelName, string *cogProfile, string *taskName, string *AET_player,
			string *RT_player, string *AET_world, string *ParamSpec);


private:
	int length;
	vector<string> modelNames;
	vector<string> cogProfiles;
	vector<string> taskNames;
	vector<string> aet_players;
	vector<string> rt_players;
	vector<string> aet_worlds;
	vector<string> paramSpecs;
	vector<string> logFilters;
};

} /* namespace MDPLIB */



#endif /* MODELSPEC_H_ */
