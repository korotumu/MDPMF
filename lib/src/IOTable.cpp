/*
 * IOTable.cpp
 *
 *  Created on: Jun 26, 2018
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
License along with this library; if not, see website. 

mlamar@ets.org 
Educational Testing Service 
660 Rosedale Road 
Princeton, NJ 08541
 */

#include "IOTable.h"

namespace MDPLIB {


int MDPLIB::IOTable::addColumn(string name, vector<string> values) {
	// If there are other columns, ensure that this one is the same size
	if(nCol > 0 && (values.size() != nRow))
		return MDP_ERROR;

	colNames.push_back(name);
	cellValues.push_back(values);
	if(nCol == 0)
	  nRow = values.size();
	nCol++;

	return MDP_SUCCESS;
}

int MDPLIB::IOTable::addRow(vector<string> values) {
	if(values.size() != nCol)
		return MDP_ERROR;

	for(unsigned int i = 0; i < nCol; i++){
		cellValues[i].push_back(values[i]);
	}
	nRow++;

	return MDP_SUCCESS;
}

int MDPLIB::IOTable::setColNames(vector<string> colnames) {
	// only allow this if we currently have no columns or if
	// the number of columns is unchanged:
	if(nCol > 0 && nCol != colnames.size())
		return MDP_ERROR;

	if(nCol == 0) {
		// We need to size the cell value table
		cellValues = vector< vector<string> >(colnames.size(), vector<string>());
	}
	colNames = colnames;
	nCol = colNames.size();
	return MDP_SUCCESS;
}

vector<string> MDPLIB::IOTable::getColNames() {
	return(colNames);
}

vector<string> MDPLIB::IOTable::getRow(unsigned int r) {
	vector<string> ret;

	if(r >= nRow)
		return ret;

	for(unsigned int c = 0; c < nCol; c++){
		ret.push_back(cellValues[c][r]);
	}
	return ret;
}

vector<string> MDPLIB::IOTable::getCol(unsigned int i) {
	if(i >= nCol)
		return(vector<string>());

	return(cellValues[i]);
}

vector<string> MDPLIB::IOTable::getCol(string name) {
	// Find the column
	unsigned int c = 0;
	while(c < nCol && name != colNames[c])
		c++;

	if(c >= nCol)
		return(vector<string>());

	return(cellValues[c]);
}


unsigned int MDPLIB::IOTable::getNumRows() {
	return nRow;
}

unsigned int MDPLIB::IOTable::getNumCols() {
	return nCol;
}
/*
vector< vector<string> > MDPLIB::IOTable::RgetCellValues() {
	return cellValues;
}

void MDPLIB::IOTable::RsetCellValues(vector< vector<string> > values) {
	cellValues = values;
	return;
}

void MDPLIB::IOTable::RsetColNames(vector<string> colnames) {
	MDPLIB::IOTable::setColNames(colnames);
	return;
}
*/



} /* namespace MDPLIB */
