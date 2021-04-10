/*
 * TableIOUtils.cpp
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

#include "TableIOUtils.h"
using namespace MDPLIB;

// Read a generic table from file and return an IOTable object.
// In the case of an error, an empty IOTable object is returned.
//
// filename - the name of the file on disk
// sepStr - string containing all characters that count as string separators

IOTable readTableFromFile(string filename, string sepStr) {
	ifstream inFile;
	string line;
	vector< string > rowVect;
	string tmpStr;
	int lineNum = 0;
	IOTable retTab;

	// Open the file
	if(filename.length() == 0)
		return retTab;

	inFile.open(filename);
	if(!inFile.is_open()){
		cout << "ERROR: Failed to open file " << filename << "\n";
		return(retTab);
	}

	// Set up the boot tokenizer
	if(sepStr.length() == 0)
		sepStr = string(" \n\t");

	boost::char_separator<char> sep(sepStr.c_str());


	// Get the header line.
	if(!getline(inFile, line)) {
		cout << "ERROR: Input file " << filename << " is empty.\n";
		inFile.close();
		return(retTab);
	}

	tokenizer tok(line, sep);

	// Get the column names:
	for (tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it)	{
		tmpStr = *it;
//		tmpStr = trim(tmpStr);
		rowVect.push_back(string(tmpStr));
	}
	int iotRet = retTab.setColNames(rowVect);
	unsigned int numCols = rowVect.size();


	// Now start iterating through the main data:
	lineNum = 2;
	while(getline(inFile, line)) {
		rowVect.clear();
		tokenizer tok(line, sep);
		for (tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it)	{
			tmpStr = *it;
	//		tmpStr = trim(tmpStr);
			rowVect.push_back(string(tmpStr));
		}
		if(rowVect.size() != numCols) {
			cout << "ERROR: Line " << lineNum << " of file " << filename << " has the wrong number of values.\n";
			inFile.close();
			return(IOTable());
		}
		retTab.addRow(rowVect);
		lineNum++;
	}

	inFile.close();
	return(retTab);
}

int writeTableToFile(IOTable table, string filename, string sep) {
	ofstream outFile;
	string line;
	vector< string > vect;
	string tmpStr;
	IOTable retTab;
	unsigned int c, r;
	unsigned int numRows = table.getNumRows();
	unsigned int numCols = table.getNumCols();

	// Open the file
	if(filename.length() == 0)
		return MDP_ERROR;

	outFile.open(filename);
	if(!outFile.is_open()){
		cout << "ERROR: Failed to open file " << filename << "\n";
		return MDP_ERROR;
	}

	// Write out the header (column names)
	vect = table.getColNames();
	outFile << vect[0]; // First one does not have a separator char
	for(c = 1; c < numCols; c++) {
		outFile << sep << vect[c];
	}
	outFile << "\n";

	// Write out the data
	for(r = 0; r < numRows; r++) {
		vect = table.getRow(r);
		outFile << vect[0]; // First one does not have a separator char
		for(c = 1; c < numCols; c++) {
			outFile << sep << vect[c];
		}
		outFile << "\n";
	}

	outFile.close();
	return MDP_SUCCESS;
}
