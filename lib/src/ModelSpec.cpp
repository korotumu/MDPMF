/*
 * ModelSpec.cpp
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
#include "ModelSpec.h"

namespace MDPLIB {

ModelSpecList::ModelSpecList() {
	length = 0;
}

ModelSpecList::~ModelSpecList() {
}

int ModelSpecList::addSpec(ModelSpecList* ps) {
	if(ps == NULL)
		return(MDP_SUCCESS);

	int i, np = ps->getLength();
	for(i = 0; i < np; i++) {
		addSpec(ps->getModelName(i), ps->getCogProfile(i), ps->getTaskName(i), ps->getAetPlayer(i),
				ps->getRtPlayer(i), ps->getAetWorld(i), ps->getParamSpec(i));

	}
	return(MDP_SUCCESS);
}

int ModelSpecList::addSpec(const char model[], const char cogProfile[], const char taskName[],
			const char aet_player[], const char rt_player[], const char aet_world[],
			const char paramSpec[]){
	string *smod = new string(model);
	string *scp = new string(cogProfile);
	string *stn = new string(taskName);
	string *saetp = new string(aet_player);
	string *srtp = new string(rt_player);
	string *saetw = new string(aet_world);
	string *sps = new string(paramSpec);

	int ret = addSpec(smod, scp, stn, saetp, srtp, saetw, sps);
	//printf("%s %s %s %s %s %s %s\n", model, cogProfile, taskName, aet_player, rt_player, aet_world, paramSpec);
	delete(smod);
	delete(scp);
	delete(stn);
	delete(saetp);
	delete(srtp);
	delete(saetw);
	delete(sps);

	return(ret);

}
int ModelSpecList::addSpec(string *modelName, string *cogProfile, string *taskName, string *AET_player,
			string *RT_player, string *AET_world, string *ParamSpec){
	length++;
	string modStr(*modelName);
	modelNames.push_back(modStr);

	string cogStr(*cogProfile);
	cogProfiles.push_back(cogStr);

	string tn(*taskName);
	taskNames.push_back(tn);

	string aetp(*AET_player);
	aet_players.push_back(aetp);

	string rtp(*RT_player);
	rt_players.push_back(rtp);

	string aetw(*AET_world);
	aet_worlds.push_back(aetw);

	string pspec(*ParamSpec);
	paramSpecs.push_back(pspec);
	return(MDP_SUCCESS);
}

int ModelSpecList::getLength() {
	return(length);
}

string *ModelSpecList::getModelName(int i) {
	if(i < 0 || i > length - 1)
		return(NULL);
	else
		return(&modelNames[i]);
}

string *ModelSpecList::getCogProfile(int i){
	if(i < 0 || i > length - 1)
			return(NULL);
		else
			return(&cogProfiles[i]);
}

string *ModelSpecList::getTaskName(int i){
	if(i < 0 || i > length - 1)
			return(NULL);
		else
			return(&taskNames[i]);
}
string *ModelSpecList::getAetPlayer(int i){
	if(i < 0 || i > length - 1)
			return(NULL);
		else
			return(&aet_players[i]);

}
string *ModelSpecList::getRtPlayer(int i){
	if(i < 0 || i > length - 1)
			return(NULL);
		else
			return(&rt_players[i]);

}
string *ModelSpecList::getAetWorld(int i){
	if(i < 0 || i > length - 1)
			return(NULL);
		else
			return(&aet_worlds[i]);

}
string *ModelSpecList::getParamSpec(int i){
	if(i < 0 || i > length - 1)
			return(NULL);
		else
			return(&paramSpecs[i]);

}

}/* namespace MDPLIB */
