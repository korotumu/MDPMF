/*
 * Policy.cpp
 *
 *  Created on: Jan 8, 2013
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

#include "Policy.h"

namespace MDPLIB {


Policy::Policy(MDP *mdp, DecisionRule *dr)
	: valueForState(mdp->getStates()->getNumStates(), 0.0),
	  probsSetForState(mdp->getStates()->getNumStates(), 0)
{
	this->mdp = mdp;
	dRule = dr;
	actionIdsForState = new vector< vector<int> *>();
	int numStates = mdp->getStates()->getNumStates();
	int s;

	int a, na = mdp->getActions()->getNumActions();
	for(a=0; a < na; a++) {
		allActionIds.push_back(a);
	}
	// Initialize vectors with default values.  All state values are set to zero
	// Actions are determined by the transition function.
	// Action probabilities are set by the decision rule.


	for(s = 0; s < numStates; s++) {
		if(mdp->getTrans()->areActionsDiffByState()) {
			actionIdsForState->push_back(new(vector<int>)());
			mdp->getTrans()->possibleActions(*(actionIdsForState->at(s)), mdp->getActions(), s);
		} else {
			actionIdsForState->push_back(&allActionIds);
		}
		probsForState.push_back(dRule->getActionProbsForState(s, mdp, *(actionIdsForState->at(s)), valueForState));
	}
	maxIters = 500;
	convCritPolicy = 0.01;
	convCritValue = 0.01;
	learningRate = 0.9;
	optimized = 0;
}


Policy::Policy(Policy *op)
	: valueForState(op->valueForState), allActionIds(op->allActionIds),
	  probsSetForState(op->probsSetForState)
{
	mdp = op->mdp;
	dRule = op->dRule;
	int s, numStates = mdp->getStates()->getNumStates();
//	vector<double> *aprobs;

	// We assume we can use the same actionIdsForState as in the original.  If not, trouble.
	actionIdsForState = op->actionIdsForState;

	for(s = 0; s < numStates; s++) {
		probsForState.push_back(vector<double>(op->probsForState[s]));
	}

	maxIters = op->maxIters;
	convCritPolicy = op->convCritPolicy;
	convCritValue = op->convCritValue;
	learningRate = op->learningRate;
	optimized = op->optimized;
}

Policy::~Policy() {
	// TODO Auto-generated destructor stub
}

void Policy::zeroOutActionProbs() {
	int s, a;
	int numStates = mdp->getStates()->getNumStates();

	for(s = 0; s < numStates; s++) {
		for(a=0; a < (int)actionIdsForState->at(s)->size(); a++) {
			probsForState[s][a] = 0.0;
		}
		probsSetForState[s] = 0;
	}
}

void Policy::resetValues() {
	int s;
	int numStates = mdp->getStates()->getNumStates();

	valueForState.clear();
	vector<double>::iterator vPtr;
	vPtr = valueForState.begin();
	valueForState.insert(vPtr, (size_t)numStates, 0.0);
	probsForState.clear();
	for(s = 0; s < numStates; s++) {
		probsForState.push_back(dRule->getActionProbsForState(s, mdp, *(actionIdsForState->at(s)), valueForState));
	}
	this->optimized = 0;
}


void Policy::setMDP(MDP* mdp) {
	this->mdp = mdp;
	this->resetValues();
	this->optimized = 0;
}

/* Changing the decision rule will change the action probabilities
 * so we reset these, but leave the state values.
 */
void Policy::setDecisionRule(DecisionRule* rule) {
	dRule = rule;
	this->zeroOutActionProbs();
}

vector<double> Policy::getActionProbsForState(State* state) {
	return(getActionProbsForState(state->getId()));
}

vector<double> Policy::getActionProbsForState(int id) {

	if(!probsSetForState[id]) {
			getActionProbsForStateDyn(id);
	}
	if(id < (int)probsForState.size()) {
		return(probsForState[id]);
	} else
		return vector<double>();
}

double Policy::getActionProbForState(State *state, Action *action) {
	return(getActionProbForState(state->getId(), action->getId()));
}

double Policy::getActionProbForState(int sid, int aid) {
	if(!probsSetForState[sid]) {
		getActionProbsForStateDyn(sid);
	}

	if(mdp->getTrans()->areActionsDiffByState()) {
		int i, na = (int)actionIdsForState->at(sid)->size();

		for(i = 0; i < na; i++) {
			if( actionIdsForState->at(sid)->at(i) == aid )
				return(probsForState[sid][i]);
		}
		return(0.0);
	} else
		return(probsForState[sid][aid]);

}

/* Alternate implementation:
 * If action probs have not been set we need to dynamically calculate them.
 * We then store them for future use.
 *
 * This function is most useful for calculating play record likelihoods, where we are changing the
 * parameters of the d-rule frequently, but only need to look at a sub-set of state probs.
 */
vector<double> Policy::getActionProbsForStateDyn(State* state) {
	return(getActionProbsForStateDyn(state->getId()));
}

vector<double> Policy::getActionProbsForStateDyn(int id) {
	if(id >= (int)probsForState.size()) {
		return vector<double>();
	}

	if(probsSetForState[id])
		return(probsForState[id]);
	else {
		int a, na = actionIdsForState->at(id)->size();
		vector<double> aProbs(na);

		dRule->getActionProbsForState(aProbs, id, mdp, *(actionIdsForState->at(id)), valueForState);
		for(a=0; a < na; a++) {
			probsForState[id][a] = aProbs[a];
		}
		probsSetForState[id] = 1;
		return(probsForState[id]);
	}

}

/* diffPolicyStateVals
 *
 * returns the maximum difference in state values
 */
float Policy::diffPolicyStateVals(Policy *otherPolicy) {
	float maxDiff = 0.0;
	int i, ns;
	ns = mdp->getStates()->getNumStates();

	for(i=0; i<ns; i++) {
		if(abs(otherPolicy->valueForState[i] - valueForState[i]) > maxDiff)
			maxDiff = abs(otherPolicy->valueForState[i] - valueForState[i]);
	}

	return maxDiff;
}

Action* Policy::chooseActionForState(State* state) {
	if(optimized) {
		if(!probsSetForState[state->getId()]) {
				getActionProbsForStateDyn(state->getId());
		}
		return(dRule->selectActionFromProbs(state, mdp,
				*(actionIdsForState->at(state->getId())), probsForState[state->getId()]));
	} else
		return(dRule->selectActionForState(state, mdp, *(actionIdsForState->at(state->getId())), valueForState));
}

int Policy::chooseActionForState(int sId) {
	if(optimized) {
		if(!probsSetForState[sId]) {
				getActionProbsForStateDyn(sId);
		}
		return(dRule->selectActionFromProbs(sId, mdp,
				*(actionIdsForState->at(sId)), probsForState[sId]));
	} else
		return(dRule->selectActionForState(sId, mdp, *(actionIdsForState->at(sId)), valueForState));
}

int Policy::optimize() {
	return(optimize(convCritPolicy, convCritValue));
}

/* The old way ...
int Policy::optimize(double convergeCritPolicy, double convergeCritValue) {

	if(optimized == 1)
		return MDP_SUCCESS;

	double policyChange = 99;
	double valueChange = 99;
	vector<float> valDiff(mdp->getStates()->getNumStates(), 0.0);
	int i=0;
	int maxIter = 1000;

	while((policyChange > convergeCritPolicy || valueChange > convergeCritValue) && i++ < maxIter){
		policyChange = updateActionProbs();
		valueChange = updateStateValues(valDiff);
	}

	if(i < maxIter) {
		if(MDP_DEBUG) {printf("MDP Converged after %d iterations\n", i); fflush(stdout); }
	} else {
		printf("MDP failed to converge after %d iterations.\n\tLast policy diff = %f\n\tLast value diff = %f.\n",
				maxIter, policyChange, valueChange);
		printModelParams();
	}

	optimized = 1;
	return(MDP_SUCCESS);
}
*/

int Policy::optimize(double convergeCritPolicy, double convergeCritValue) {

	if(optimized == 1)
		return MDP_SUCCESS;

	double policyChange = 99;
	double maxValueChange = 99, subCrit, olr = learningRate;
	int numStates = mdp->getStates()->getNumStates();
	int i=0, j=0, s;
	int numUpdate = numStates;
	vector<float> valDiff(mdp->getStates()->getNumStates(), 0.0);
	vector<bool> updateState(mdp->getStates()->getNumStates(), true);

	while((policyChange > convergeCritPolicy || maxValueChange > convergeCritValue) && i++ < maxIters){
		learningRate = min(olr, 0.3 + 2/i);
		policyChange = updateActionProbs();
		maxValueChange = updateStateValues(valDiff);
		subCrit = max(maxValueChange/2.0, convergeCritValue);
		// Set the boolean values for which states to update
		for(s=0; s < numStates; s++) {
			if(abs(valDiff[s]) > subCrit) {
				updateState[s] = 1;
				numUpdate++;
			} else
				updateState[s] = 0;
		}
		j = 0;
		while(numUpdate > 0 && j++ < maxIters){
			numUpdate = updateStateValues(valDiff, updateState, subCrit);
		}
		if(j >= maxIters)
			i = i + 10;  // Rather than breaking, just advance i faster.
	}

	if(i < maxIters && j < maxIters) {
		if(MDP_DEBUG) {printf("MDP Converged after %d iterations\n", i); fflush(stdout); }
	} else {
		printf("MDP failed to converge after %d iterations.\n\tLast policy diff = %f\n\tLast value diff = %f.\n",
				maxIters, policyChange, maxValueChange);
		// For debugging, probably should be controlled by a switch to turn this off when unwanted:
		policyChange = updateActionProbs();
		maxValueChange = updateStateValues(valDiff);
		subCrit = max(maxValueChange/2.0, convergeCritValue);
		// Set the boolean values for which states to update
		for(s=0; s < numStates; s++) {
			if(abs(valDiff[s]) > subCrit) {
				updateState[s] = 1;
				numUpdate++;
			} else
				updateState[s] = 0;
		}
		if(numUpdate > 0 && numUpdate < 100) {
			printf("First 20 states with value differences (out of %d):\n", numUpdate);
			for(s=0, j=0; s < numStates && j < 20; s++) {
				if(updateState[s]) {
					cout << mdp->getStates()->getState(s)->getLabel() << " has value: " << valueForState[s] << " with diff of: " << valDiff[s] << "\n";
					j++;
				}
			}
		}
		learningRate = olr;
		return(MDP_ERROR);
	}

	optimized = 1;
	learningRate = olr;
	return(MDP_SUCCESS);
}

int Policy::isOptimized() {
	return(optimized);
}

void Policy::setOptimized(int opt) {
	optimized = opt;
}

double Policy::updateActionProbs(){
	int s, ns, a;
	unsigned int na;
	double maxChange = 0.0;
	ns = mdp->getStates()->getNumStates();
	vector<double> aProbs;

	for(s=0; s < ns; s++) {
		na = actionIdsForState->at(s)->size();
		aProbs.resize(na, 0.0);
		dRule->getActionProbsForState(aProbs, s,
				mdp, *(actionIdsForState->at(s)), valueForState);
		for(a=0; a < (int)na; a++) {
			if(abs(probsForState[s][a] - aProbs[a]) > maxChange)
				maxChange = abs(probsForState[s][a] - aProbs[a]);
			probsForState[s][a] = aProbs[a];
		}
		probsSetForState[s] = 1;
	}
	return(maxChange);
}

double Policy::updateStateValues(vector<float> &diffVals) {
	int s, ns;
	int ms = 0;
	double maxChange = 0.0;
	double newValue;
	ns = mdp->getStates()->getNumStates();

	for(s=0; s < ns; s++) {
		newValue = getStateValue(s);
		diffVals[s] = (float)(newValue - valueForState[s]);
		if(abs(diffVals[s]) > maxChange) {
			maxChange = abs(diffVals[s]);
			ms = s;
		}
		valueForState[s] += diffVals[s] * learningRate;
	}
	if(MDP_DEBUG) {printf("State %d (L:%s) changed value by %.5f.\n", ms,
			(mdp->getStates()->getState(ms)->getLabel()).c_str(),
			maxChange); fflush(stdout); }
	return(maxChange);
}


int Policy::updateStateValues(vector<float> &diffVals, vector<bool> &update, double diffCrit) {
	int s, ns;
	int numFlagged = 0;
	double newValue;
	ns = mdp->getStates()->getNumStates();

	for(s=0; s < ns; s++) {
		if(update[s]) {
			newValue = getStateValue(s);
			diffVals[s] = (float)(newValue - valueForState[s]);
			if(abs(diffVals[s]) > diffCrit) {
				numFlagged++;
			} else {
				update[s] = 0;
			}
			valueForState[s] += diffVals[s] * learningRate;
		}
	}

	return(numFlagged);
}

/* getStateValue
 *
 * Implements the V(s) function:
 *    V(s1) = sum_over_actions( Q(a,s) * p(a|s1))
 */
double Policy::getStateValue(int sId){
	double val = 0.0;
	int a, na = (int)actionIdsForState->at(sId)->size();

	for(a=0; a<na; a++) {
		if(probsForState[sId][a] > 0) {
			val += mdp->getActionValueForState(sId, actionIdsForState->at(sId)->at(a), valueForState) * probsForState[sId][a];
		}
	}
	return(val);
}

void Policy::printModelParams() {
	int i;
	// Model parameters include those from the decision rule, the transition function and the reward function
	ParamList *tmpParams = mdp->getTrans()->getParamList();
	if(tmpParams != NULL) {
		for(i=0; i< tmpParams->getLength(); i++) {
			printf("%s: %.4f\n", tmpParams->getParamLabel(i)->c_str(), tmpParams->getParamValue(i));
		}
	}

	tmpParams = mdp->getReward()->getParamList();
	if(tmpParams != NULL) {
		for(i=0; i< tmpParams->getLength(); i++) {
			printf("%s: %.4f\n", tmpParams->getParamLabel(i)->c_str(), tmpParams->getParamValue(i));
		}
	}
	fflush(stdout);
}


/* exportActionProbs
 *
 * Creates a file with the state values and the probabilities for each action in columns
 * for each state (the rows).  Includes each state variables in the first columns for
 * state identification.
 */
int Policy::exportActionProbs(string filename){
	//TODO Fix this function to deal with different types of StateSpaces
	/*
	FILE *fptr = fopen(filename.c_str(), "w");
	int s, ns = mdp->getStates()->getNumStates();
	int a, na = mdp->getActions()->getNumActions();
	int v, nv = mdp->getStates()->getNumVars();
	StateSpace *ss = mdp->getStates();
	ActionSet *as = mdp->getActions();

	if(fptr == NULL)
		return(MDP_ERROR);

	// Print header row
	fprintf(fptr, "aprobs ");
	for(v=0; v<nv; v++) {
		fprintf(fptr, "%s ", ss->getStateVarName(v).c_str());
	}
	fprintf(fptr, "stateValue ");
	for(a=0; a<na; a++) {
		fprintf(fptr, "%s ", as->getAction(a)->getLabel().c_str());
	}
	fprintf(fptr, "\n");

	// Print data
	for(s=0; s<ns; s++ ) {
		fprintf(fptr, "%d ", s);
		for(v=0; v<nv; v++) {
			fprintf(fptr, "%.2f ", ss->getStateVarValue(s, v));
		}
		fprintf(fptr, "%f ", valueForState[s]);
		for(a=0; a<na; a++) {
			fprintf(fptr, "%f ", probsForState[s][a]);
		}
		fprintf(fptr, "\n");
	}
	fclose(fptr);
	*/
	return(MDP_SUCCESS);
}

} /* namespace MDP */

