/*
 * TableIOUtils.h
 *
 *  Created on: Jun 26, 2018
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
	Princeton, NJ 08541
 */

#ifndef TABLEIOUTILS_H_
#define TABLEIOUTILS_H_

#include "IOTable.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

using namespace MDPLIB;
IOTable readTableFromFile(string filename, string sep);
int writeTableToFile(IOTable table, string filename, string sep);

#endif /* TABLEIOUTILS_H_ */
