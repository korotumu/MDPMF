/*
 * rcpp_MDPMF_API.cpp
 *
 *  Created on: Jan 24th 2021
 *      Author: Michelle LaMar
 *
 * This file is part of the mdpmf R package.
 * Copyright 2021 Michelle LaMar
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


#include "Mdpmf.h"
//#include "CompiledModel.h"
#include <iostream>
#include <IOTable.h>
#include <TableIOUtils.h>
#include <Rcpp.h>
using namespace Rcpp;
using namespace MDPLIB;
ActionSet *genActionSet(ActionEffectTable *aeTab);
StateSpaceVarList *genStateSpace(ActionEffectTable *aeTab);
vector<ActionSequence *> *generateSimulationData(MDP *mdp, PersonParams personParams, bool optScores, StateVarList *startState, ActionEffectTrans *wTrans);
int matchString(string str, vector<string> strList);
void mergeLists(vector<int> &v1, vector<int> v2);
int pruneStateSpaceToReachable(StateSpaceVarList *ss, StateVarList *startState, ActionSet *as, ActionEffectTrans *t, ActionEffectTrans *rwt);
void checkReachability(StateSpaceVarList *ss, int sId, ActionSet *as, Transition *t1, Transition *t2, vector<bool> *reached, int depth, vector<int> *uncheckedStates);
//ActionEffectTrans *transFromDF(DataFrame aeDF);
MDP *MDPFromDFs(DataFrame aeDF, DataFrame rDF) ;

MDP *MDPFromDFs(DataFrame aeDF, DataFrame rewDF, float discount) {
  ActionEffectTrans *trans = NULL;
  StateSpaceVarList *states = NULL;
  ActionSet *actions = NULL;
  IOTable aetStringTab = tableFromDataFrame(aeDF);
  ActionEffectTable *aeTab = new ActionEffectTable();
  if(aeTab->readFromIOTable(aetStringTab) != MDP_SUCCESS) {
    Rcout << "Unable to parse the Action Effect table.\n";
    return NULL;
  }
  
  Rcout << "Read Action Effect table from df.\n";
  
  std::string	stopAction("EndMission");
  // 		TODO: Allow stop action to be specified on command line.
  
  states = genStateSpace(aeTab);
  actions = genActionSet(aeTab);
  Action *stopAct = actions->getAction(stopAction);
  
  Rcout << "Found num actions: " << actions->getNumActions()  << "\n";
  Rcout << "Stop action " << stopAction << " is id " << stopAct->getId() << "\n";
  
  if(stopAct->getId() < 0) {
    Rcout << "Stop action " << stopAction << " not found in action effect file.\n";
    return NULL;
  }
  actions->setStopAction(stopAct->getId());
  
  trans = new ActionEffectTrans(*aeTab, states, actions);
  trans->initActResStates();
  
  // Now the rewards
  IOTable rewardStringTab = tableFromDataFrame(rewDF);
  ActionEffectReward *reward = NULL;
  reward = new ActionEffectReward(states, actions);
  // Initialize this before the transitions to find file format issues before doing the
  // long slog through the transition initialization.  We may have to re-init afterward if
  // the state space changed.
  if(reward->initFromIOTable(rewardStringTab) != MDP_SUCCESS) {
    Rcout << "Failed to load reward file.\n";
    return NULL;
  }
  
  Rcout << "Finished setting up rewards.\n";
  
  MDP *newMDP = new MDP(states, actions, trans, reward, discount);
  
  return(newMDP);
}

// [[Rcpp::export]]
DataFrame simData(DataFrame aeDF, DataFrame rewDF, DataFrame persParamDF) {
  
  DataFrame errRet;
  
  // Create the MDP
  double discount = 1.0;
  MDP *simMDP = MDPFromDFs(aeDF, rewDF, discount);
  if(simMDP == NULL) {
      Rcout << "ERROR: Failed to create MDP model from definition files.\n";
      return errRet;
  }
  
  StateSpaceVarList *states = (StateSpaceVarList *)simMDP->getStates();
  ActionSet *actions = simMDP->getActions();
  
  // *** KLUGE ALERT  Should not need to reparse table for start state
  IOTable aetStringTab = tableFromDataFrame(aeDF);
  ActionEffectTable *aeTab = new ActionEffectTable();
  if(aeTab->readFromIOTable(aetStringTab) != MDP_SUCCESS) {
    Rcout << "ERROR: Unable to parse the Action Effect table.\n";
    return errRet;
  }
  //return(tableToDataFrame(aetStringTab));
  
  StateVarList *startState = states->getState(aeTab->getStartVarVals());;
  if(startState == NULL || startState->getId() < 0) {
    Rcout << "ERROR: Invalid start state.\n";
    return errRet;
  }		
  
  ActionEffectTrans *trans =  (ActionEffectTrans *)simMDP->getTrans();  
  ActionEffectTrans *worldTrans =  (ActionEffectTrans *)simMDP->getTrans();  // default to student model trans
  		
// Read in the person parameters (betas):
  IOTable persParamTab = tableFromDataFrame(persParamDF);
  PersonParams persParams;
  
  if(persParams.readFromTable(persParamTab, 0) != MDP_SUCCESS) {
  		  Rcout << "Failed to load person parameter table.\n";
  		  return errRet;
  }
  		
  		Rcout << "Finished reading in pers params.\n";
  		
  		Rcout << states->getNumStates() << "  states.\n";

  		// DEBUGGING:
  	vector <MDPLIB::Action *> sActs = trans->possibleActions(startState->getId());
  	Rcout << "Actions from Start: " << sActs.size() << "\n";
  	for(int i = 0; i < sActs.size(); i++) {
  	  Rcout << "  " << sActs[i]->getId() << " : " << sActs[i]->getLabel() << "\n";
  	}
  	vector<int> *possStates = trans->nonZeroTransProbs(1, startState->getId());
  	Rcout << "Trans info for Fish: " << possStates->size() << "\n";

    
    //vector<ActionSequence *> noData;
    //noData.push_back(new ActionSequence());
    //IOTable recTab = writePlayRecordsToTable( noData, states,	actions, startState, 0);
    //DataFrame recDF = tableToDataFrame(recTab);
    //return(recDF);
    
    vector<ActionSequence *> *simData = generateSimulationData(simMDP, persParams, 0, startState, worldTrans);
    int writeRewardsOption = 0;
    if(simData != NULL) {
      IOTable recTab = writePlayRecordsToTable( *simData, states,	actions, startState, writeRewardsOption);
      DataFrame recDF = tableToDataFrame(recTab);
      return(recDF);
    }
  
  return errRet;
  
}

// /*
#define MAX_RDEPTH 4096
// 
// 
// 

// 

// int printPlayRecProbs(vector<ActionSequence *> data, MDP *mdp, vector<double> *ability);
// int printPlayRecProbStats(vector<ActionSequence *> data, MDP *mdp, vector<double> *ability);
// void appendRecActionProbs(FILE *fptr, Policy *pol, ActionSequence *rec);
// ParamList *readParamEstSpecs(string paramEstSpecFile);
// ModelSpecList *readModelSpecs(string modelEstSpecFile);
// bool is_file_exist(string fileName);
// int checkFormat(const char *s, const char* format);
// 
// #define MAX_LINE 256
// 
// bool record_action_choices = false;
// bool verbose = false;
// string actionChoiceFileName("");
// 
// int main(int arc,  char* argv[]) {
// 	string actionET_filename;
// 	string reward_filename;
// 	string paramEstSpec_filename;
// 	string modelConfSpec_filename;
// 	string compiledModel_filename;
// 	vector<double> rewardVals;
// 	PersonParams persParams;
// 	string outfile;
// 	string datafile = string("");
// 	vector<ActionSequence *> *data = NULL;
// 	int i;
// 	std::string stopAction;
// 	ParamList *pl = NULL;
// 	bool fastRun = false;
// 	//	ModelSpecList *ms;
// 	//	int numThreads = 1;
// 	CompiledModel cMod;
// 	MDP *simMDP;
// 	StateSpaceVarList *states = NULL;
// 	ActionSet *actions = NULL;
// 	ActionEffectReward *reward = NULL;
// 	ActionEffectTrans *trans = NULL;
// 	ActionEffectTrans *worldTrans = NULL;
// 	StateVarList *startState = NULL;
// 	ActionEffectTable *aeTab = NULL;
// 	ActionEffectTable *worldAETab = NULL;
// 
// 
// 	printf("Starting %s - %d\n", argv[0], arc);
// 	// Parse command line.  There are three main options:
// 	//   --genData simulates play data with the specified settings
// 	//   --estModelParams estimates any of the model (non-person) parameters based on random person params
// 	//   --estPersonParams estimates the person parameters, based on fixed model params
// 	//
// 
// 	po::variables_map varMap;
// 
// 	try{
// 		varMap = parseCommandLine(arc, argv );
// 
// 	if (varMap.count("help")) {
// 		// Help already given in the parseCommandLine function
// 		return 1;
// 	}
// 
// 	// Make sure they requested one of the valid modes:
// 	if (!varMap.count("genData") && !varMap.count("estPersonParams") && !varMap.count("estModelParams") && !varMap.count("recActionChoice") && !varMap.count("compileModel"))  {
// 		cout << "Please specify the mode of execution (--genData, --estPersonParams, --estModelParams, --recActionChoice, or --compileModel)";
// 		return 1;
// 	}
// 
// 	int numThreads;
// 	if (varMap.count("numThreads")) {
// 		numThreads = varMap["numThreads"].as<int>();
// 	}
// 
// 	if (varMap.count("fast")) {
// 		fastRun = true;
// 	}
// 
// 	if (varMap.count("verbose")) {
// 		verbose = true;
// 	}
// 
// 	//Reading Parameter Estimation Specification File
// 	if(varMap.count("paramEstSpecsFile")) {
// 		if (varMap.count("paramEstSpecsFile") == 0){
// 			cout <<"Parameter Estimation Specification file was not specified after --paramEstSpecsFile option\n";
// 			return 1;
// 		}
// 		else{
// 			paramEstSpec_filename.assign(varMap["paramEstSpecsFile"].as<string>());
// 			if (is_file_exist(paramEstSpec_filename) != 1){
// 				cout << "Unable to find the Parameter Estimation Specification file " << paramEstSpec_filename << "\n";
// 				return 1;
// 			}
// 			else {
// 				pl = readParamEstSpecs(paramEstSpec_filename);
// 				if(pl == NULL) {
// 					cout << "Unable to parse Parameter Estimation Specification file " << paramEstSpec_filename << "\n";
// 					return 1;
// 				}
// 			}
// 
// 		}
// 	}
// 
// 	//Reading Model Specification Configuration File
// 	ModelSpecList *modelSpec = NULL;
// 	if(varMap.count("modelSpecConfFile")) {
// 		if (varMap.count("modelSpecConfFile") == 0){
// 			cout <<"Model Specification Configuration file was not specified after --modelEstSpecsFile option\n";
// 			return 1;
// 		}
// 		else{
// 			modelConfSpec_filename.assign(varMap["modelSpecConfFile"].as<string>());
// 			if (is_file_exist(modelConfSpec_filename) != 1){
// 				cout << "Unable to find  the Model Specification Configuration file " << modelConfSpec_filename << "\n";
// 				return MDP_ERROR;
// 			}
// 			else {
// 				modelSpec = readModelSpecs(modelConfSpec_filename);
// 			}
// 		}
// 	}
// 
// 	// These parameters are general purpose, so parse them here:
// 	if(varMap.count("recActionChoice")) {
// 		char baseStr[256], suffix[32];
// 		string base;
// 		if(varMap.count("dataOutputFile") != 0) {
// 			base = varMap["dataOutputFile"].as<string>();
// 		} else if(varMap.count("dataInputFile") != 0) {
// 			base = varMap["dataInputFile"].as<string>();
// 		} else {
// 			base = string("ActionChoiceProbs.foo");
// 		}
// 		sscanf(base.c_str(), "%s.%[^.]", baseStr, suffix);
// 		actionChoiceFileName.assign(string(baseStr) + ".acp");
// 		record_action_choices = 1;
// 	}
// 
// 	string persParamFile;
// 	if(varMap.count("personParamFile") == 0 &&
// 			(varMap.count("estPersonParams") || varMap.count("recActionChoice") || varMap.count("genData"))) {
// 		cout << "Person parameters must be specified using the --personParamFile option";
// 		return 1;
// 	}
// 
// 	if(varMap.count("personParamFile")) {
// 		persParamFile = varMap["personParamFile"].as<string>();
// 	}
// 
// 
// 	float discount = 1.0; // defaults to 1.0
// 	if(varMap.count("discount") != 0) {
// 		float d = varMap["discount"].as<float>();
// 		if(d >= 0.0 && d <= 1.0)
// 			discount = d;
// 	}
// 
// 	// Check mode-specific arguments early so that we don't do all the work of setting up the state space to *then* find we have to exit:
// 	if (varMap.count("genData")) {
// 
// 		if(varMap.count("personParamFile") == 0) {
// 			cout << "Person parameters must be specified using the --personParamFile option\n";
// 			return 1;
// 		}
// 
// 		// Default to "simOutRecs.txt" if output file is not specified:  (should we just out to STDIO?)
// 		if(varMap.count("dataOutputFile") != 0) {
// 			outfile = varMap["dataOutputFile"].as<string>();
// 		} else {
// 			outfile = string("simOutRecs.txt");
// 		}
// 
// 		// Read in the person params:
// 		if(persParams.readFromFile(persParamFile) < 0) {
// 			cout << "Person parameter file " << persParamFile << " could not be read.\n";
// 			return 1;
// 		}
// 	}
// 
// 	if(varMap.count("estPersonParams") || varMap.count("estModelParams")) {
// 		// Need play records
// 		if(varMap.count("dataInputFile") == 0) {
// 
// 			cout << "Input data must be specified using the --dataInputFile option";
// 			return 1;
// 
// 		} else {
// 			datafile = varMap["dataInputFile"].as<string>();
// 		}
// 
// 		string modParamFile;
// 		if(varMap.count("modelParamFile") == 0) {
// 			if(varMap.count("estModelParams")) {
// 				cout << "Model parameter output file must be specified using the --modelParamFile option";
// 				return 1;
// 			}
// 		}
// 	} else if(varMap.count("recActionChoice") && !varMap.count("genData")){
// 		// Just doing the action prob calculations:
// 		if(varMap.count("dataInputFile") == 0) {
// 
// 			cout << "Input data must be specified using the --dataInputFile option";
// 			return 1;
// 
// 		} else {
// 			datafile = varMap["dataInputFile"].as<string>();
// 		}
// 		if(varMap.count("personParamFile") == 0) {
// 			cout << "Person parameters must be specified using the --personParamFile option\n";
// 			return 1;
// 		}
// 		if(persParams.readFromFile(persParamFile) < 0) {
// 			cout << "Person parameter file " << persParamFile << " could not be read.\n";
// 			return 1;
// 		}
// 	}
// 
// 	// Load the compiled model if we have one
// 	bool modelLoaded = false;
// 	if(varMap.count("compModelFile")) {
// 		if (varMap.count("compModelFile") == 0){
// 			cout <<"Compiled Model file was not specified after --compModelFile option\n";
// 			return 1;
// 		}
// 		// Read in the compiled model
// 		compiledModel_filename.assign(varMap["compModelFile"].as<string>());
// 		if (is_file_exist(compiledModel_filename) != 1){
// 			cout << "Unable to find  the Compiled Model  file " << compiledModel_filename << "\n";
// 			return MDP_ERROR;
// 		}
// 
// 		std::ifstream ifs(compiledModel_filename, ios::binary);
// 		boost::archive::binary_iarchive ia(ifs);
// 		ia >> cMod;
// 		modelLoaded = true;
// 		states = cMod.getStates();
// 		actions = cMod.getActions();
// 		trans = cMod.getTrans();
// 		reward = cMod.getReward();
// 		worldTrans = cMod.getWorldTrans();
// 		startState = cMod.getStartState();
// 
// 		// We allow an override of the reward structure, as the state space initialization does not depend upon it:
// 		// Need a Reward specification table:
// 		if(varMap.count("rewardFile") != 0) {
// 			reward_filename.assign(varMap["rewardFile"].as<string>());
// 
// 			cout << "Overriding compiled Rewards with " << reward_filename << "\n";
// 
// 			reward = new ActionEffectReward(states, actions);
// 			IOTable rewardStringTab = readTableFromFile(reward_filename, ",\n");
// 			if(reward->initFromIOTable(rewardStringTab) != MDP_SUCCESS) {
// 				cout << "Failed to load reward file: " << reward_filename << "\n";
// 				return 1;
// 			}
// 		}
// 	}
// 
// 	if(!modelLoaded) {
// 		// Need an action effect table:
// 		if(varMap.count("actionEffectFile") == 0) {
// 			cout << "Action Effect table file must be specified using the --actionEffectFile option\n";
// 			return 1;
// 		}
// 
// 		actionET_filename.assign(varMap["actionEffectFile"].as<string>());
// 
// 		aeTab = new ActionEffectTable;
// 
// 		stopAction = "EndMission";
// 		//If Stop Action is specified using command-line
// 		if(varMap.count("stopAction"))
// 			stopAction = varMap["stopAction"].as<string>();
// 
// 		IOTable aetStringTab = readTableFromFile(actionET_filename, ",\n");
// 		if(aeTab->readFromIOTable(aetStringTab) != MDP_SUCCESS) {
// 			cout << "Unable to find or parse the Action Effect table file " << actionET_filename << "\n";
// 			return 1;
// 		}
// 
// 		// Need a Reward specification table:
// 		if(varMap.count("rewardFile") == 0) {
// 			cout << "Reward Structure file must be specified using the --rewardFile option\n";
// 			return 1;
// 		}
// 		reward_filename.assign(varMap["rewardFile"].as<string>());
// 
// 		if(varMap.count("worldEffectFile") == 1) {
// 			string world_AET_filename(varMap["worldEffectFile"].as<string>());
// 			worldAETab = new ActionEffectTable;
// 
// 			IOTable waetStringTab = readTableFromFile(world_AET_filename, ",\n");
// 			if(aeTab->readFromIOTable(waetStringTab) != MDP_SUCCESS) {
// 				cout << "Unable to find or parse the world Action Effect table file " << world_AET_filename << "\n";
// 				return 1;
// 			}
// 
// 			// Confirm that this action effect table has the same state space and action set as the agent aet:
// 			if(!aeTab->isCompatibleAET(worldAETab)) {
// 				cout << "ERROR: The agent action effect table is not compatible with the World Effect table.\n";
// 				return 1;
// 			}
// 		}
// 
// 		vector<bool> estRVs;
// 
// 		states = genStateSpace(aeTab);
// 		actions = genActionSet(aeTab);
// 		Action *stopAct = actions->getAction(stopAction);
// 		if(stopAct->getId() < 0) {
// 			cout << "Stop action " << stopAction << " not found in action effect file " << actionET_filename << "\n";
// 			return 1;
// 		}
// 		actions->setStopAction(stopAct->getId());
// 
// 		reward = new ActionEffectReward(states, actions);
// 		// Initialize this before the transitions to find file format issues before doing the
// 		// long slog through the transition initialization.  We may have to re-init afterward if
// 		// the state space changed.
// 		IOTable rewardStringTab = readTableFromFile(reward_filename, ",\n");
// 		if(reward->initFromIOTable(rewardStringTab) != MDP_SUCCESS) {
// 			cout << "Failed to load reward file: " << reward_filename << "\n";
// 			return 1;
// 		}
// 
// 		trans = new ActionEffectTrans(*aeTab, states, actions);
// 		worldTrans = trans;  // default to student model trans
// 
// 		// The Start State values are configured in the action effect table.
// 		startState = states->getState(aeTab->getStartVarVals());
// 		if(startState == NULL || startState->getId() < 0) {
// 			cout << "Invalid start state.\n";
// 			return 1;
// 		}
// 
// 		if(varMap.count("worldEffectFile") == 1) {
// 			worldTrans = new ActionEffectTrans(*worldAETab, states, actions);
// 		}
// 
// 		// Initializing the transitions takes time, try to parallelize them in the case that we have
// 		// both belief and real-world transition models
// #pragma omp parallel
// 		{
// #pragma omp sections
// 			{
// #pragma omp section
// 				{
// 					//	trans->setNumThreads(numThreads);
// 					trans->initActResStates();
// 				}
// #pragma omp section
// 				{
// 					if(varMap.count("worldEffectFile") == 1) {
// 						worldTrans->initActResStates();
// 					}
// 				}
// 
// 			} // end omp sections
// 		} // end omp parallel
// 
// 		if(verbose) {
// 			cout << "Finished initializing transitions functions.\n";
// 			cout.flush();
// 		}
// 
// 		// We prune the state space unless they asked not to OR they specified a separate world transition file
// 		if(varMap.count("noPruneStateSpace") == 0) {
// 			cout << states->getNumStates() << " original states.\n";
// 			cout.flush();
// 			if(varMap.count("worldEffectFile") == 0)
// 				pruneStateSpaceToReachable(states, startState, actions, trans, NULL);
// 			else
// 				pruneStateSpaceToReachable(states, startState, actions, trans, worldTrans);
// 
// 			cout << states->getNumStates() << " reachable states.\n";
// 			cout.flush();
// 
// 			// Get the startState again, as the stateSpace has changed ...
// 			startState = states->getState(aeTab->getStartVarVals());
// 
// 			// And re-init the reward file.
// 			IOTable rewardStringTab = readTableFromFile(reward_filename, ",\n");
// 			if(reward->initFromIOTable(rewardStringTab) != MDP_SUCCESS) {
// 				cout << "Failed to load reward file: " << reward_filename << "\n";
// 				return 1;
// 			}
// 
// 		} else {
// 			cout << states->getNumStates() << " total states.\n";
// 			cout.flush();
// 		}
// 
// 		// TODO: implement parameters based on the paramSpec file and the reward labels
// 		reward->setParams(rewardVals);
// 		reward->setEstParams(estRVs);
// 
// 		if(varMap.count("compileModel")) {
// 			compiledModel_filename.assign(actionET_filename.begin(), actionET_filename.end()-4);
// 			compiledModel_filename += ".cmdp";
// 			cMod.setActions(actions);
// 			cMod.setStates(states);
// 			cMod.setReward(reward);
// 			cMod.setTrans(trans);
// 			cMod.setWorldTrans(worldTrans);
// 			cMod.setStartState(startState);
// 
// 			std::ofstream ofs(compiledModel_filename, ios::binary);
// 			boost::archive::binary_oarchive oa(ofs);
// 			oa << cMod;
// 		}
// 
// 	}
// 
// 	simMDP = new MDP(states, actions, trans, reward, discount);
// 
// 	if (varMap.count("genData")) {
// 
// 		vector<ActionSequence *> *simData;
// 		simData = generateSimulationData(simMDP, persParams, varMap.count("useOptScore") != 0, startState, worldTrans);
// 
// 		bool writeRewardsOption = 0;
// 		if(varMap.count("recRewardValues"))
// 			writeRewardsOption = 1;
// 
// 		if(simData != NULL) {
// 			IOTable recTab = writePlayRecordsToTable( *simData, states,	actions, startState, writeRewardsOption);
// 			writeTableToFile(recTab, outfile, " ");
// 		}
// 
// 		return 0;
// 	}
// 
// 	// Estimation
// 	if(varMap.count("estPersonParams") || varMap.count("estModelParams")) {
// 
// 		bool readModParams = 0;
// 		bool outputModParams = 0;
// 
// 		string modParamFile;
// 		if(varMap.count("modelParamFile") > 0) {
// 
// 			modParamFile = varMap["modelParamFile"].as<string>();
// 			outputModParams = 1;
// 
// 			if(varMap.count("estModelParams")) {
// 				readModParams = 0;
// 				outputModParams = 1;
// 			} else {
// 				FILE *fptr;
// 				if( (fptr = fopen(modParamFile.c_str(), "r")) == NULL) {
// 					outputModParams = 1;
// 				} else {
// 					readModParams = 1;
// 					fclose(fptr);
// 				}
// 			}
// 		}
// 
// 		// Read in the data file:
// 		// Use the world trans for calculating any missing data in the performance record:
// 		if(data == NULL) {
// 			MDP *recMDP = new MDP(states, actions, worldTrans, reward, discount);
// 			IOTable recTab = readTableFromFile(datafile, string(" \t"));
// 			data = readPlayRecordsFromTable(recTab, recMDP, false, startState, verbose);  // enforcing state transitions are legal
// 			if(data == NULL) {
// 				cout << "Failed to load data from file " << datafile << "\n";
// 				return 1;
// 			}
// 			cout << "Loaded data from file " << datafile << "\n";
// 			cout.flush();
// 		}
// 
// 
// 		// Create MDPIRT model:
// 		bool optScore = 0;
// 		if(varMap.count("useOptScore"))
// 			optScore = 1;
// 
// 		vector<MDP *> mdpList;
// 		mdpList.push_back(simMDP);
// 		MDP_IRT_Model mdpirtMod(mdpList, pl);
// 
// 		mdpirtMod.setUseOptScore(optScore);
// 
// 		if (varMap.count("useMLE"))
// 			mdpirtMod.setPersonEstType(MDPIRT_MLE);
// 		else
// 			mdpirtMod.setPersonEstType(MDPIRT_MAP);
// 
// 		// Set based on the number of params to be estimated:
// 		int numPE = 0;
// 		if(pl == NULL) {
// 			numPE = 2;
// 		} else {
// 			for(i=0; i<pl->getLength(); i++) {
// 				if(pl->isParamEstPop(i))
// 					numPE++;
// 			}
// 		}
// 		int numIter = (int)(std::min(std::max(200.0, pow(5, numPE)), 16000.0));
// 		if(fastRun)
// 			numIter = numIter/2;
// 		mdpirtMod.setNumGlobalIterations(numIter);
// 		if(varMap.count("maxIters") && varMap["maxIters"].as<int>() > 0) {
// 			mdpirtMod.setMaxPolicyIterations(varMap["maxIters"].as<int>());
// 		}
// 
// 		// Read or Estimate the model parameters
// 		ParamList *modParamsPtr;
// 		if(readModParams) {
// 			modParamsPtr = readInModelParams(modParamFile);
// 		} else {
// 			modParamsPtr = mdpirtMod.estimateModelParameters(data);
// 			printf("Population Estimates: beta mu: %.4f, beta sigma: %.4f\n",
// 					modParamsPtr->getParamValue(mdpirtMod.getBetaMuParamIndex()), modParamsPtr->getParamValue(mdpirtMod.getBetaSigmaParamIndex()));
// 		}
// 
// 		// Output model parameters:
// 		if(outputModParams) {
// 			int res;
// 			res = writeOutModelParams(*(modParamsPtr), modParamFile, false);
// 			printf("Wrote %d parameter out to %s.\n", res, modParamFile.c_str());
// 		}
// 
// 		if(varMap.count("estPersonParams")) {
// 			/*
// 			 *
// 			int bMuInd = modParamsPtr->getParamIndex("betaMu");
// 			int bSigInd = modParamsPtr->getParamIndex("betaSigma");
// 			if(bMuInd < 0 || bSigInd < 0) {
// 				cout<< "ERROR: Hyper parameters for the beta distribution must be estimated or specified as betaMu and betaSigma.\n";
// 				return(1);
// 			}
// 			// Set the beta distribution params:
// 			mdpirtMod.setBetaMu(modParamsPtr->getParamValue(bMuInd));
// 			mdpirtMod.setBetaSigma(modParamsPtr->getParamValue(bSigInd));
// 			 */
// 
// 			// Set the reward parameters:
// 			/*
// 			 * We should have a parameter configuration file that has the parameter name
// 			 * and min-max values for each.  If min=max that implies a fixed value.  Otherwise
// 			 * it is the search range.
// 			 *
// 		int rInd = -1;
// 		rInd = modParamsPtr->getParamIndex("MovePenalty");
// 		if(rInd >= 0) {
// 			rewardVals[2] = modParamsPtr->getParamValue(rInd);
// 			reward->setMoveCost(modParamsPtr->getParamValue(rInd));
// 			reward->setParams(rewardVals);
// 		}
// 			 */
// 
// 			// Now estimate individual betas:
// 			vector<ParamList *> *pEstLists = mdpirtMod.estimatePersonParametersOld(data);
// 
// 			ParamList *betaList = pEstLists->at(0);
// 			ParamList *logLikList = pEstLists->at(1);
// 			ParamList *eLogLikList = pEstLists->at(2);
// 			ParamList *vLogLikList = pEstLists->at(3);
// 
// 			int betaInd = -1;
// 			for(i=0; i<mdpirtMod.getParams()->getLength(); i++) {
// 				if(mdpirtMod.getParams()->getParamComponent(i) == pc_beta && mdpirtMod.getParams()->isParamEstPers(i))
// 					betaInd = i;
// 			}
// 
// 			// Get parametric bootstrapped estimation errors and conf intervals.  Only works for the beta parameter right now:
// 			if(betaInd >= 0 && !fastRun) {
// 				BootstrapEstimator bEst(&mdpirtMod, 50, 100, 0.01, 10, 0.95);
// 				bEst.persListEstBootstrap(betaInd, data);
// 				betaList = mdpirtMod.getParams()->getParamSubList(betaInd);
// 			}
// 
// 			// Write out list:
// 			int p, np = betaList->getLength();
// 			for(p=0; p < np; p++) {
// 				persParams.addPerson(betaList->getParam(p), (float)logLikList->getParamValue(p),
// 						(float)eLogLikList->getParamValue(p), (float)vLogLikList->getParamValue(p));
// 
// 			}
// 			persParams.writeToFile(persParamFile);
// 
// 		}
// 
// 		delete(modParamsPtr);
// 
// 	}
// 
// 	// Output action probabilities.  This could be from an estimation or from a fixed set of parameters
// 	if(record_action_choices) {
// 		if(persParams.getLength() <= 0){
// 			// Read in the person parameters:
// 			if(persParams.readFromFile(persParamFile) < 0) {
// 				cout << "Person parameter file " << persParamFile << " could not be read.\n";
// 				return 1;
// 			}
// 		}
// 		if(data == NULL) {
// 			// Need play records
// 			string datafile;
// 			if(varMap.count("dataInputFile") == 0) {
// 
// 				cout << "Input data must be specified using the --dataInputFile option";
// 				return 1;
// 
// 			} else {
// 				datafile = varMap["dataInputFile"].as<string>();
// 			}
// 
// 			MDP *recMDP = new MDP(states, actions, worldTrans, reward, discount);
// 			IOTable recTab = readTableFromFile(datafile, string(" \t"));
// 			data = readPlayRecordsFromTable(recTab, recMDP, false, startState, verbose);  // enforcing state transitions are legal
// 			if(data == NULL) {
// 				cout << "Failed to load data from file " << datafile << "\n";
// 				return 1;
// 			}
// 		}
// 		printPlayRecProbs(*data, simMDP, persParams.getBetaList());
// 	}
// 
// 	} catch(std::exception& e)
// 	{
// 		std::cerr << "Error: " << e.what() << "\n";
// 		return -1;
// 	}
// 
// 
// 	return 0;
// 
// }
// 
// 
ActionSet *genActionSet(ActionEffectTable *aeTab) {
	ActionSet *as = new ActionSet();

	vector<string> actionNames = aeTab->getActionList();

	unsigned int i;

	for(i=0; i < actionNames.size(); i++) {
		as->addAction(actionNames[i]);
	}
	return(as);
}

StateSpaceVarList *genStateSpace(ActionEffectTable *aeTab) {
	StateSpaceVarList *ss = new StateSpaceVarList();
	vector<VarDesc *> stateDescs = aeTab->getStateVarList();

	// We need to create ranges for each variable.  As that information is not yet in a
	// data file, we hard code all possible variables here and select them based on the
	// Action Effect Table.  Note that ActionEffectTrans depends on the state space vars being
	// in the same order as the actionEffect columns:

	unsigned int i;

	for(i=0; i < stateDescs.size(); i++) {
		// Add the variables as described in the action table:
		if(stateDescs[i]->getType() == DISCRETE)
			ss->addDiscreteVar(stateDescs[i]->getName(), stateDescs[i]->getMin(), stateDescs[i]->getMax());
	}

	return(ss);
}

// This will be recursive following the action tree
int pruneStateSpaceToReachable(StateSpaceVarList *ss, StateVarList *startState, ActionSet *as, ActionEffectTrans *t, ActionEffectTrans *rwt) {
	unsigned int ns_orig = ss->getNumStates();
	vector<bool> *reached = new vector<bool>(ns_orig, false);
	vector<int> *uncheckedStates = new vector<int>();
	unsigned int i;

	// We need to save the startState data so that we can recreate this state:
	// vector<float> startData(*(startState->getVarData()));

	// We need to use checkpoints to prevent stack size overflow.
	uncheckedStates->push_back(startState->getId());
	while(uncheckedStates->size() > 0) {
		vector<int> checkpoints(*uncheckedStates);
		uncheckedStates->clear();
		for(i=0; i < checkpoints.size(); i++) {
			checkReachability(ss, checkpoints[i], as, t, rwt, reached, 0, uncheckedStates);
		}
	}

	// Construct the reachable state id list:
	vector<int> reachable;
	for(i=0; i < ns_orig; i++) {
		if(reached->at(i))
			reachable.push_back(i);
	}

	// Prune the State Space:
	ss->setReachableStates(reachable);

#pragma omp parallel
	{
#pragma omp sections
		{
#pragma omp section
			{
				// Re-initialize the transition states:
				t->initActResStates();
			}
#pragma omp section
			{
				if(rwt)
					rwt->initActResStates();
			}
		}
	}

	// Reset the start state:  or not.
	// delete(startState);
	// startState = ss->getState(startData);

	delete(reached);
	delete(uncheckedStates);
	return ss->getNumStates();
}

void checkReachability(StateSpaceVarList *ss, int sId, ActionSet *as, Transition *t1, Transition *t2, vector<bool> *reached, int depth, vector<int> *uncheckedStates) {

	// First note that this state was reached!
	(*reached)[sId] = true;

	// If we are at maxDepth, add to unchecked States list and return:
	if(depth > MAX_RDEPTH) {
		uncheckedStates->push_back(sId);
		return;
	}

	vector<int> possResStates, possResStates2;

	// Find all actions that can be taken from this state
	vector<int> actionList, actionList2;
	t1->possibleActions(actionList, as, sId);
	if(t2) {
		t2->possibleActions(actionList2, as, sId);
		mergeLists(actionList, actionList2);
	}
	unsigned int a, s;

	// Find all states that can be reached from this state:
	for(a = 0; a < actionList.size(); a++) {
		// Find the states resulting from this action and mark them as reached:
		t1->nonZeroTransProbs(possResStates, actionList[a], sId);
		if(t2) {
			t2->nonZeroTransProbs(possResStates2, actionList[a], sId);
			// These need to be sorted, which they may not be by default:
			std::sort(possResStates.begin(), possResStates.end());
			std::sort(possResStates2.begin(), possResStates2.end());
			mergeLists(possResStates, possResStates2);
		}
		for(s = 0; s < possResStates.size(); s++) {
			if(!reached->at(possResStates[s])) {
				checkReachability(ss, possResStates[s], as, t1, t2, reached, depth+1, uncheckedStates);
			}
		}
	}

}

int matchString(string str, vector<string> strList) {
	unsigned int i;
	for(i=0; i < strList.size(); i++) {
		if(str == strList[i])
			return i;
	}
	return -1;
}

void mergeLists(vector<int> &v1, vector<int> v2) {
	unsigned int i,j, als = v1.size();
	for(i=0, j=0; j < v2.size(); ) {
		if(i < als) {
			if(v1[i] == v2[j]) {
				i++;
				j++;
			} else if(v1[i] < v2[j]) {
				i++;
			} else {
				v1.push_back(v2[j]);
				j++;
			}
		} else {
			v1.push_back(v2[j]);
			j++;
		}
	}
}

// int printPlayRecProbs(vector<ActionSequence *> data, MDP *mdp, vector<double> *ability){
// 	FILE *acFptr;
// 	unsigned int s;
// 	int a;
// 	Policy *simStudent = NULL;
// 	DR_IRT *irtDR = new DR_IRT(0);
// 	ActionSequence *pr;
// 
// 	acFptr = fopen(actionChoiceFileName.c_str(), "w");
// 
// 	if(acFptr == NULL) {
// 		cout << "ERROR: Cannot open file" << actionChoiceFileName << "\n";
// 		return -1;
// 	}
// 
// 	if(data.size() != ability->size()) {
// 		cout << "ERROR: Performance record length does not match ability vector length in printPlayRecProbs.\n";
// 		return -1;
// 	}
// 
// 	fprintf(acFptr, "PersonID Action NumOptions");
// 	int numActions = mdp->getActions()->getNumActions();
// 	for(a = 0; a < numActions; a++){
// 		fprintf(acFptr, " %s", mdp->getActions()->getAction(a)->getLabel().c_str());
// 	}
// 
// 	fprintf(acFptr, "\n");
// 
// 	simStudent = new Policy(mdp, irtDR);
// 
// 	for(s = 0; s < data.size(); s++) {
// 
// 		// Set the policy with the appropriate ability score
// 		irtDR->setTheta(ability->at(s));
// 		simStudent->setDecisionRule(irtDR);
// 		if(s == 0 || ability->at(s) != ability->at(s-1)) {
// 			simStudent->resetValues();
// 			simStudent->optimize();
// 		}
// 
// 		pr = data[s];
// 
// 		appendRecActionProbs(acFptr, simStudent, pr);
// 	}
// 	fclose(acFptr);
// 
// 	return 0;
// }
// 
// 
// int printPlayRecProbStats(vector<ActionSequence *> data, MDP *mdp, vector<double> *ability){
// 	FILE *acFptr;
// 	unsigned int s;
// 	int a;
// 	int sId, aId;
// 	State *curState = NULL;
// 	Action *action = NULL;
// 	Policy *simStudent = NULL;
// 	DR_IRT *irtDR = new DR_IRT(0);
// 	ActionSequence *pr;
// 	double prob, sProb;
// 	unsigned int numActs;
// 	vector <double> actProbs;
// 	double ell_Rand = 0.0, e_ell_Rand, e_numActs, vll_Rand, vll_RandSum, e_vll_Rand;
// 
// 	unsigned int i, j, nStates;
// 	vector<StateProb> *sDist;
// 
// 	acFptr = fopen(actionChoiceFileName.c_str(), "w");
// 
// 	if(acFptr == NULL) {
// 		cout << "ERROR: Cannot open file" << actionChoiceFileName << "\n";
// 		return -1;
// 	}
// 
// 	if(data.size() != ability->size()) {
// 		cout << "ERROR: Performance record length does not match ability vector length in printPlayRecProbs.\n";
// 		return -1;
// 	}
// 
// 
// 	fprintf(acFptr,"PID Action ActProb NumActChoices ELL_RandChoice VLL_RandChoice\n");
// 
// 	simStudent = new Policy(mdp, irtDR);
// 
// 	for(s = 0; s < data.size(); s++) {
// 
// 		// Set the policy with the appropriate ability score
// 		irtDR->setTheta(ability->at(s));
// 		simStudent->setDecisionRule(irtDR);
// 		if(s == 0 || ability->at(s) != ability->at(s-1)) {
// 			simStudent->resetValues();
// 			simStudent->optimize();
// 		}
// 
// 		pr = data[s];
// 
// 		if(pr->getStatesObserved() == so_fully) {
// 			curState = pr->getStartState();
// 			// curState = ((StateVarList *)(mdp->getStates()))->State(start->getId());
// 			for(a = 0; a < pr->getNumSteps(); a++) {
// 				action = pr->getActionAtStep(a);
// 
// 				fprintf(acFptr,"%s %s ", pr->getId().c_str(), action->getLabel().c_str());
// 
// 				prob = simStudent->getActionProbForState(curState, action);
// 				actProbs = simStudent->getActionProbsForState(curState);
// 				numActs = actProbs.size();
// 				for(i = 0, ell_Rand = 0.0; i < numActs; i++) {
// 					ell_Rand += log(actProbs[i]);
// 				}
// 				ell_Rand = ell_Rand/numActs;
// 				for(i = 0, vll_RandSum = 0.0; i < numActs; i++) {
// 					vll_RandSum += pow(ell_Rand - log(actProbs[i]), 2.0);
// 				}
// 				vll_Rand = vll_RandSum/numActs;
// 
// 				fprintf(acFptr,"%.5f %d %.5f %.5f\n", prob, numActs, ell_Rand, vll_Rand);
// 				curState = pr->getStateAtStep(a);
// 			}
// 		} else if(pr->getStatesObserved() == so_partial) {
// 			double sVal = 0.0;
// 			int step;
// 
// 			// First get startState distribution
// 			sDist = pr->getStartStateDist();
// 
// 			// Now loop over all actions:
// 			for(step = 0; step < pr->getNumSteps(); step++) {
// 				if(sDist->size() == 0)
// 					continue;
// 				action = pr->getActionAtStep(step);
// 
// 				fprintf(acFptr,"%s %s ", pr->getId().c_str(), action->getLabel().c_str());
// 
// 				aId = action->getId();
// 				// Marginalize over all possible states:
// 				nStates = sDist->size();
// 				for(i = 0, sVal = 0.0, e_numActs = 0.0, e_ell_Rand = 0.0, e_vll_Rand = 0.0; i < nStates; i++) {
// 					sId = sDist->at(i).getStateId();
// 					sProb = sDist->at(i).getProb();
// 
// 					prob = simStudent->getActionProbForState(sId, aId);
// 					actProbs = simStudent->getActionProbsForState(sId);
// 					numActs = actProbs.size();
// 					for(j = 0, ell_Rand = 0.0; j < numActs; j++) {
// 						ell_Rand += log(actProbs[j]);
// 					}
// 					ell_Rand = ell_Rand/numActs;
// 					for(j = 0, vll_RandSum = 0.0; j < numActs; j++) {
// 						vll_RandSum += pow(ell_Rand - log(actProbs[j]), 2.0);
// 					}
// 					vll_Rand = vll_RandSum/numActs;
// 
// 					e_ell_Rand += ell_Rand * sProb;
// 					e_vll_Rand += vll_Rand * sProb;
// 					sVal += prob * sProb;
// 					e_numActs += numActs * sProb;
// 
// 				}
// 
// 				fprintf(acFptr,"%.5f %.2f %.5f %.5f\n", sVal, e_numActs, e_ell_Rand, e_vll_Rand);
// 				sDist = pr->getStateProbsAtStep(step);
// 			}
// 
// 		}
// 	}
// 	fclose(acFptr);
// 
// 	return 0;
// }
// 
vector<ActionSequence *> *generateSimulationData(MDP *mdp, PersonParams personParams, bool optScores, StateVarList *startState, ActionEffectTrans *wTrans) {
	vector<double> *ability = personParams.getBetaList();
	int numStudents = (int)(ability->size());
	vector<ActionSequence *> *prRecs = new vector<ActionSequence *>();
	ActionSequence *pr;

	Policy *simStudent = NULL;
	SimEngine *simEng = NULL;
	DR_Max *maxDR = new DR_Max();
	DR_IRT *irtDR = new DR_IRT(0);

	// FILE *acFptr = NULL;

	simStudent = new Policy(mdp, maxDR);

	if(optScores) {
		simStudent->optimize();
	}

	// if(record_action_choices) {
	// 	acFptr = fopen(actionChoiceFileName.c_str(), "w");
	// 
	// 	fprintf(acFptr, "PersonID Action NumOptions");
	// 	int a, numActions = simStudent->getMDP()->getActions()->getNumActions();
	// 	for(a = 0; a < numActions; a++){
	// 		fprintf(acFptr, " %s", simStudent->getMDP()->getActions()->getAction(a)->getLabel().c_str());
	// 	}
	// 
	// 	fprintf(acFptr, "\n");
	// }

	for(int sId = 0; sId < numStudents; sId++) {

		if(sId == 0 || ability->at(sId) != ability->at(sId-1)) {
			irtDR->setTheta(ability->at(sId));
			simStudent->setDecisionRule(irtDR);
			if(!optScores) {
				simStudent->resetValues();
				if(simStudent->optimize() == MDP_ERROR) {
					cout << "Unable to calculate policy for sim student " << *(personParams.getLabel(sId)) << " with ability " << ability->at(sId) << "\n";
					break;
					cout.flush();
				}
			}
		}

		simEng = new SimEngine(simStudent, wTrans);
		// if(record_action_choices)
		// 	simEng->setRecordActionChoices(true);
		pr = simEng->simActions(*(personParams.getLabel(sId)), startState);
		if(pr == NULL) {
			Rcout << "Failed to simulate data for sim student "  << *(personParams.getLabel(sId)) << "\n";
			break;
		} else{
		  prRecs->push_back(pr);
		}

		// if(record_action_choices) {
		// 	appendRecActionProbs(acFptr, simStudent, pr);
		// }
	}

	// if(record_action_choices) {
	// 	fclose(acFptr);
	// }
	return(prRecs);
}

// void appendRecActionProbs(FILE *fptr, Policy *pol, ActionSequence *rec) {
// 	pol->optimize();  // Just to be sure, also this won't do anything if it is already optimized
// 	assert(fptr != NULL);
// 
// 	unsigned int i, nStates;
// 	int step, a;
// 	int totNumActs;
// 	State *curState;
// 	Action *action;
// 	double sProb, aProb, sVal;
// 	vector<StateProb> *sDist;
// 	vector <int> possActs;
// 	int sumNumPossActs;
// 	double actProb = 0.0, probFudge = 0.00001;
// 
// 	totNumActs = pol->getMDP()->getActions()->getNumActions();
// 	vector <bool> isActPossible(totNumActs, false);
// 
// 	if(rec->getStatesObserved() == so_fully) {
// 		curState = rec->getStartState();
// 
// 		for(step = 0; step < rec->getNumSteps(); step++) {
// 			action = rec->getActionAtStep(step);
// 
// 			fprintf(fptr,"%s %s", rec->getId().c_str(), action->getLabel().c_str());
// 
// 			// Print the number of possible actions (how many real choices):
// 			pol->getMDP()->getTrans()->possibleActions(possActs, pol->getMDP()->getActions(), curState->getId());
// 			fprintf(fptr, " %d", (int)possActs.size());
// 
// 			// Flag the possible actions:
// 			for(a = 0; a < (int)possActs.size(); a++) {
// 				isActPossible[possActs[a]] = true;
// 			}
// 
// 			// Now print all action probs in order:
// 			for(a = 0; a < totNumActs; a++) {
// 				actProb = pol->getActionProbForState(curState->getId(), a);
// 				if(isActPossible[a] && actProb < probFudge)
// 					actProb += probFudge;
// 				fprintf(fptr, " %.5f",  actProb);
// 				isActPossible[a] = false;  // Setting up for the next loop
// 			}
// 
// 			fprintf(fptr,"\n");
// 			curState = rec->getStateAtStep(step);
// 		}
// 	} else if(rec->getStatesObserved() == so_partial) {
// 
// 		// First get startState distribution
// 		sDist = rec->getStartStateDist();
// 
// 		// Now loop over all steps:
// 		for(step = 0; step < rec->getNumSteps(); step++) {
// 			if(sDist->size() == 0)
// 				continue;
// 			action = rec->getActionAtStep(step);
// 
// 			fprintf(fptr,"%s %s", rec->getId().c_str(), action->getLabel().c_str());
// 
// 			// Now print all action probs in order:
// 			nStates = sDist->size();
// 			// Print the number of possible actions (how many real choices):
// 
// 			for(i = 0, sumNumPossActs = 0; i < nStates; i++) {
// 				pol->getMDP()->getTrans()->possibleActions(possActs, pol->getMDP()->getActions(), sDist->at(i).getStateId());
// 				sumNumPossActs += (int)possActs.size();
// 				for(a = 0; a < (int)possActs.size(); a++) {
// 					isActPossible[possActs[a]] = true;
// 				}
// 			}
// 			fprintf(fptr, " %.2f", (double)sumNumPossActs/(double)nStates);
// 
// 			for(a = 0; a < totNumActs; a++) {
// 				for(i = 0, sVal = 0.0; i < nStates; i++) {
// 					aProb = pol->getActionProbForState(sDist->at(i).getStateId(), a);
// 					sProb = sDist->at(i).getProb();
// 					sVal += aProb * sProb;
// 				}
// 				if(isActPossible[a] && sVal < probFudge)
// 					sVal += probFudge;
// 				fprintf(fptr, " %.5f", sVal);
// 				isActPossible[a] = false;  // Setting up for the next loop
// 			}
// 
// 			fprintf(fptr,"\n");
// 
// 			sDist = rec->getStateProbsAtStep(step);
// 		}
// 	}
// 	return;
// }
