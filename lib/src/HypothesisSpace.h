/*
 * HypothesisSpace.h
 *
 *  Created on: Mar 7, 2013
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

#ifndef HYPOTHESISSPACE_H_
#define HYPOTHESISSPACE_H_

#include "MDP.h"
#include "Policy.h"
#include "ActionSequence.h"
#include <vector>

namespace MDPLIB {

class HypothesisSpace {
public:
	HypothesisSpace(vector<Policy> hlist, vector<float> plist);
	HypothesisSpace();
	virtual ~HypothesisSpace();

	int getNumHypotheses();
//	void setHypotheses(vector<Policy> hlist);
//	void setProbabilities(vector<float> plist);
	void addHypothesis(Policy hyp, float prob);
	void normalizePriors();
	Policy *getHypothesis(int ind);
	float getPosteriorProb(int ind);
	float getPriorProb(int ind);

	// This is the main Inverse Planning function.  Perhaps it should be moved
	// to another class?
	virtual int setPosteriorsForActionSeq(ActionSequence *as);

	// Prior management:
	int setUniformPriors();
	int setPosteriorsToPriors();

	double getLogLiklihoodForActionSeq(ActionSequence *as, int hypInd);
	vector< vector<double> > getActionProbsForActionSeq(ActionSequence *as);

private:
	int numHyp;
	vector<Policy> hypotheses;
	vector<float> priorProbs;
	vector<float> posteriorProbs;


};

} /* namespace MDPLIB */
#endif /* HYPOTHESISSPACE_H_ */
