/*
 * MDP.h
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

#ifndef MDP_H_
#define MDP_H_

#include "ActionSet.h"
#include "StateSpace.h"
#include "Transition.h"
#include "Reward.h"

#define MIN_TMPLIST_LEN 10
namespace MDPLIB {



/*! MDP
 * \brief A Markov Decision Process class.
 *
 *  The MDP class defines a Markov Decision Problem which consists of  the states, available actions,
 *  transition probabilities and reward function.  Once defined the MDP can be solved using the MDP_Solver class.
 *
 *  Generally the states of the state space and the available actions in the action set can be
 *  defined through the class interface, however it is likely that the transition probabilities and
 *  the reward functions will be most efficiently implemented by subclassing the respective
 *  classes: Transition and Reward.
 */
class MDP {
public:
	MDP();
	MDP(StateSpace *ss, ActionSet *as, Transition *t, Reward *r, double disc);
	virtual ~MDP();

	// Getters and Setters:
    ActionSet *getActions() const;
    double getDiscount() const;
    Reward *getReward() const;
    StateSpace *getStates() const;
    Transition *getTrans() const;
    void setActions(ActionSet *actions);
    void setDiscount(double discount);
    void setReward(Reward *reward);
    void setStates(StateSpace *states);
    void setTrans(Transition *trans);

    // Main methods:

    /*! \fn Policy *findOptPolicy()
     *  \brief Find a single optimal policy for this MDP.
     */
//    Policy *findOptPolicy();

    /*! \fn ActionSet *getActionProbsForState(State *s)
     * \brief Calculate the probability of each action in the action set being the most optimal action to take in this state.
     *  \param s The state for which we wish to get optimal action probabilities.
     */
//    ActionSet *getActionProbsForState(State *s);

    /*! \fn Action * bestActionForState(State *s)
     *  \brief Return an action that maximizes the chances of maximal reward.
     *  \param s The state for which we wish to find an optimal action.
     *  \return A pointer to an action which optimized the chance of maximal reward.
     */
//    Action *bestActionForState(State *s);

    /*! \fn double getActionValueForState(State *s, Action *a, vector<double> stateVals)
     * \brief Returns the Q value of the MDP for state s and Action a.
     * \param s The state which we want to find the value of action a.
     * \param a The action of which we want to find the value in state s.
     * \stateVals The assumed marginal values of each state in the state space.
     * \return A value for taking action a in state s.
     */
    double getActionValueForState(State *s, Action *a, const vector<double> &stateVals);
    double getActionValueForState(const int& sId, const int& aId, const vector<double> &stateVals);
    void setStopAction(int saInd);
    void setStartStates(vector <int> sStates);
    void paramsUpdated();  // A notification that should be passed on to trans and reward

	int getStopActionInd() const {
		return stopActionInd;
	}

protected:
	StateSpace *states;  /*!< The set of possible system states over which the MDP will optimize.  The system must be in exactly one state at any given time. */
	ActionSet *actions;  /*!< The set of actions that can be taken.  Exactly one action will occur each time interval. */
	Transition *trans;   /*!< The transition function, specifying probabilities of tranitioning into state s2 from state s1 given action a. */
	Reward *reward;      /*!< The reward function, specifying what objective reward is (gained/lost) for each s1->a->s2 transition. */
	double discount;     /*!< The amount that future rewards are discounted for each timestep that they are in the future.  0 <= discount <= 1 */
	int stopActionInd;
//	vector <int>startStates;  /*!< The possible start states for this MDP.  An empty list implies all states are possible start states. */
//	vector <int>reachableStates; /*!< The list of all states that are reachable given the list of possible start states. */
	vector <int>tmpStateList;
};

} /* namespace MDP */
#endif /* MDP_H_ */
