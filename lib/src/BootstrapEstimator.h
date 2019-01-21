/*
 * BootstrapEstimator.h
 *
 *  Created on: Jun 12, 2017
 *      Author: Michelle LaMar 
 * 
 *      Copyright (C) 2017, 2018  Michelle M. LaMar

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

#ifndef BOOTSTRAPESTIMATOR_H_
#define BOOTSTRAPESTIMATOR_H_

#include <boost/serialization/array_wrapper.hpp>
#include <vector>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/tail.hpp>
#include <boost/accumulators/statistics/tail_quantile.hpp>

#include "MDPIRTModel.h"
#include "SimEngine.h"
using namespace boost::accumulators;


namespace MDPLIB {

class MDP_IRT_Model;

class BootstrapEstimator {
public:
	BootstrapEstimator(MDP_IRT_Model *mdpm);

	BootstrapEstimator(MDP_IRT_Model *mdpm, int min, int max, double crit, int niCrit, double cis);

	virtual ~BootstrapEstimator() {
		// TODO Auto-generated destructor stub
	}

	// Multi task version: data is list of (per person task lists)
	int persListEstBootstrap(ParamList *pl, vector< vector <ActionSequence *> *> data);
	// Single task version: data is list of persons and their task-data
	int persListEstBootstrap(ParamList *pl, vector<ActionSequence *> *data);
	// Version that uses a param-sub list from the params in the mdpmodel. Someday this will be the generalizable interface ....
	int persListEstBootstrap(int i, vector< vector <ActionSequence *> *> data);
	int persListEstBootstrap(int i, vector<ActionSequence *> *data);

	Param *persEstBootstrap(Param *p, vector<ActionSequence *> *data);

	// The Getters and Setters:
	double getChangeCrit() const {
		return changeCrit;
	}

	void setChangeCrit(double changeCrit) {
		this->changeCrit = changeCrit;
	}

	int getMaxIterations() const {
		return maxIterations;
	}

	void setMaxIterations(int maxIterations) {
		this->maxIterations = maxIterations;
	}

	int getMinIterations() const {
		return minIterations;
	}

	void setMinIterations(int minIterations) {
		this->minIterations = minIterations;
	}

	int getNumUnderCrit() const {
		return numUnderCrit;
	}

	void setNumUnderCrit(int numUnderCrit) {
		this->numUnderCrit = numUnderCrit;
	}

	float getCiSpan() const {
		return ciSpan;
	}

	void setCiSpan(float ciSpan) {
		this->ciSpan = ciSpan;
	}

private:
	MDP_IRT_Model *mdpMod; // The full model for estimation

	float ciSpan;  // Proportion span for confidence interval.  Must be between 0 and 1
	int minIterations; // The minimum number of bootstrap iterations
	int maxIterations;  // The maximum number of bootstrap iterations
	double changeCrit;  // Stop if the se change is less than this value for numUnderCrit
	int numUnderCrit;  // Stop if the se change is less than changeCrit for this number of iteractions

	vector<double> estList;
	vector<int> recLenList;
	vector<double> seList;
};

} /* namespace MDPLIB */

#endif /* BOOTSTRAPESTIMATOR_H_ */
