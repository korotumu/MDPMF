/*
 * PersonParams.h
 *
 *  Created on: Jan 17, 2014
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
	Princeton, NJ 08541 *
 */

#ifndef PERSONPARAMS_H_
#define PERSONPARAMS_H_

#include <vector>
#include <string>
#include <MDPlib.h>

namespace MDPLIB {

#define MAX_LINE 256

class PersonParams {
public:
	PersonParams();
	virtual ~PersonParams();

//	int addPerson(Param p);
	int addPerson(std::string idStr, float th, float ll);
	int addPerson(std::string idStr, float th, float ll, float ell, float vll);
	int addPerson(Param *betaP, float ll, float ell, float vll);
//	int addBetaList(ParamList *pl);
	int setThetaForInd(int i, float th);
	int setLogLikForInd(int i, float ll);
	int setELogLikForInd(int i, float ll);
	int setVarLogLikForInd(int i, float ll);
	int readFromFile(std::string filename);
	int writeToFile(std::string filename);
	int readFromFile(std::string filename, bool append);
	int writeToFile(std::string filename, bool append);

	string *getLabel(int i) {
		if(i < betaParams->getLength() ) {
			return(betaParams->getParamLabel(i));
		} else
			return NULL;
	}

	std::vector<double> *getBetaList() const {
		return betaParams->getValuesArray();
	}

	const std::vector<float>& getLogLik() const {
		return logLik;
	}

	void setLogLik(const std::vector<float>& logLik) {
		this->logLik = logLik;
	}

	int getLength() {
		return(betaParams->getLength());
	}

private:
	ParamList *betaParams;
	std::vector<float> logLik;
	std::vector<float> eLogLik;
	std::vector<float> varLogLik;
};

} /* namespace MDPLIB */

#endif /* PERSONPARAMS_H_ */
