/*
 * IOTable.h
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

#ifndef IOTABLE_H_
#define IOTABLE_H_

#include<vector>
#include<string>
#include "GlobalDefs.h"
//#include <Rcpp.h>

using namespace std;
namespace MDPLIB {

class IOTable {
public:
	IOTable() {
		// Should auto-initialize with zero length vectors, which is what we want.
		nCol = 0;
		nRow = 0;
	}
	virtual ~IOTable() {
		// TODO Auto-generated destructor stub
	}

	int addColumn(string name, vector<string> values);
	int addRow(vector<string> values);
	int setColNames(vector<string> colnames);
	vector<string> getColNames();
	vector<string> getRow(unsigned int i);
	vector<string> getCol(unsigned int i);
	vector<string> getCol(string name);
	unsigned int getNumRows();
	unsigned int getNumCols();

//	vector< vector<string> > RgetCellValues();
//	void RsetCellValues(vector< vector<string> > values);
//	void RsetColNames(vector<string> colnames);

private:
	unsigned int nCol;
	unsigned int nRow;
	vector<string> colNames;
	// Note that this 2-d array will be a list of columns.  Each column is a list of strings.  So rows will cut across the vectors
	vector< vector <string> > cellValues;
};


} /* namespace MDPLIB */

/*RCPP_MODULE(iotable_module) {
	using namespace Rcpp;
	
	class_<MDPLIB::IOTable>("IOTable")

	.constructor()
    .property("nCol", &MDPLIB::IOTable::getNumCols, "number of columns")
	.property("nRow", &MDPLIB::IOTable::getNumRows, "number of rows")
	.property("colNames", &MDPLIB::IOTable::getColNames, &MDPLIB::IOTable::RsetColNames, "column names")
	.property("cellValues", &MDPLIB::IOTable::RgetCellValues, &MDPLIB::IOTable::RsetCellValues, "cell values")

	.method("addColumn", &MDPLIB::IOTable::addColumn)
	.method("addRow", &MDPLIB::IOTable::addRow)
	;
} */


#endif /* IOTABLE_H_ */
