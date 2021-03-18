/*
 * PersonParams.cpp
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

#include "PersonParams.h"

namespace MDPLIB {

PersonParams::PersonParams() {
	betaParams = new ParamList();

}

PersonParams::~PersonParams() {
//	delete betaParams;  For some reason this causes a SIGSEGV -- so I guess we don't need it?
}

} /* namespace MDPLIB */

int MDPLIB::PersonParams::addPerson(std::string idStr, float th, float ll) {
	betaParams->addParam(idStr.c_str(), th, pt_estPers, pc_beta);
	logLik.push_back(ll);
	eLogLik.push_back(0);
	varLogLik.push_back(0);
	return 1;
}

int MDPLIB::PersonParams::addPerson(std::string idStr, float th, float ll, float ell, float vll) {
	betaParams->addParam(idStr.c_str(), th, pt_estPers, pc_beta);
	logLik.push_back(ll);
	eLogLik.push_back(ell);
	varLogLik.push_back(vll);
	return 1;
}

int MDPLIB::PersonParams::addPerson(Param* betaP, float ll, float ell, float vll) {
	betaParams->addParam(*betaP);
	logLik.push_back(ll);
	eLogLik.push_back(ell);
	varLogLik.push_back(vll);
	return 1;
}


int MDPLIB::PersonParams::setThetaForInd(int i, float th) {
	// Tricky -- this is based on the vector index, not the person id:
	if(i >= betaParams->getLength() || i < 0)
		return -1;

	betaParams->setParamValue(i, th);
	return i;
}

int MDPLIB::PersonParams::setLogLikForInd(int i, float ll) {
	// Tricky -- this is based on the vector index, not the person id:
	if(i >= (int)logLik.size() || i < 0)
		return -1;

	logLik[i] = ll;
	return i;
}

int MDPLIB::PersonParams::setELogLikForInd(int i, float ell) {
	// Tricky -- this is based on the vector index, not the person id:
	if(i >= (int)eLogLik.size() || i < 0)
		return -1;

	eLogLik[i] = ell;
	return i;
}

int MDPLIB::PersonParams::setVarLogLikForInd(int i, float vll) {
	// Tricky -- this is based on the vector index, not the person id:
	if(i >= (int)varLogLik.size() || i < 0)
		return -1;

	varLogLik[i] = vll;
	return i;
}

int MDPLIB::PersonParams::readFromFile(std::string filename) {
	return(readFromFile(filename, 0));
}

int MDPLIB::PersonParams::writeToFile(std::string filename) {
	return(writeToFile(filename, 0));
}

int MDPLIB::PersonParams::readFromFile(std::string filename, bool append) {
	FILE *fptr;

	fptr = fopen(filename.c_str(), "r");
	if(fptr == NULL)
		return -1;

	if(!append) {
		// clear the lists
		delete(betaParams);
		betaParams = NULL;
		logLik.clear();
		eLogLik.clear();
		varLogLik.clear();
		betaParams = new ParamList();
	}

	char str[MAX_LINE], idStr[MAX_LINE];
	float newTheta;

	while(fgets(str, MAX_LINE, fptr) != NULL) {
		sscanf(str, "%s %f", idStr, &newTheta);
		addPerson(std::string(idStr), newTheta, 1.0);
	}

	fclose(fptr);
	return 1;
}

int MDPLIB::PersonParams::writeToFile(std::string filename, bool append) {
	FILE *fptr;
	int i;

	if(append)
		fptr = fopen(filename.c_str(), "a");
	else
		fptr = fopen(filename.c_str(), "w");

	if(fptr == NULL)
		return -1;

	int numPers = betaParams->getLength();

	fprintf(fptr, "PersonID LogBeta_Est LogBeta_SE (LogBeta_CI) RecLogLik E(RecLogLik) V(RecLogLik)\n");
	for(i=0; i < numPers; i++){

		vector<double> ci(betaParams->getParamCI(i));

		fprintf(fptr, "%s %.4f %.4f (%.4f,%.4f) %.4f %.4f %.4f \n", betaParams->getParamLabel(i)->c_str(), betaParams->getParamValue(i),
				betaParams->getParamSE(i), ci[0], ci[1],
				logLik[i], eLogLik[i], varLogLik[i]);
	}

	fclose(fptr);
	return 1;
}

int MDPLIB::PersonParams::readFromTable(IOTable ppTab, bool append) {
  
  if(ppTab.getNumRows() == 0 || ppTab.getNumCols() < 2)
    return -1;
  
  if(!append) {
    // clear the lists
    delete(betaParams);
    betaParams = NULL;
    logLik.clear();
    eLogLik.clear();
    varLogLik.clear();
    betaParams = new ParamList();
  }
  
  vector<string> rowValStrs;
  float newTheta;
  int nRow = ppTab.getNumRows();
  
  for(int r = 0; r < nRow; r++)  {
    rowValStrs = ppTab.getRow(r);
    newTheta = stof(rowValStrs[1]);
    addPerson(rowValStrs[0], newTheta, 1.0);
  }
  
  return MDP_SUCCESS;
}


int MDPLIB::PersonParams::writeToTable(IOTable ppTab, bool append) {
  return -1;
}