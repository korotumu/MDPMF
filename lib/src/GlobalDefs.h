/*
 * GlobalDefs.h
 *
 *  Created on: Jan 14, 2013
 *      Author: Michelle LaMar
 *
 *  Revised on: Apr 25, 2017
 *  	Author: Rutuja Ubale added estimation parameter codes
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

#ifndef GLOBALDEFS_H_
#define GLOBALDEFS_H_

// Some return codes:
#define MDP_SUCCESS 1
#define MDP_ERROR 2

//Estimation parameter codes
#define MDP_PARAM_FIXED 0
#define MDP_PARAM_FREE 1
#define MDP_PARAM_PERSON 2
#define MDP_PARAM_POP 3

#define PROB_ERR -1.0
#define VALUE_ERROR -9999.999

// DEBUG 1 is for more warning messages, 0 for fewer
#define MDP_DEBUG 0


#endif /* GLOBALDEFS_H_ */
