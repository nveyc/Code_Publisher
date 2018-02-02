#pragma once

/////////////////////////////////////////////////////////////////////////////
// Publisher.h - Demonstrate how to publish code with html pages           //
//                                                                         //
// Chao Yang                                                               //
// Reference:                                                              //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017               //
/////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* It contains provides fileToHtml() function to convert source code to html file
*
* This supports user to import css stylesheet to modify the style of html
*
* An anlysis result in the form of Element should be imported as parameter to build
* the dependency in html files
*
* Required Files:
* ---------------
* "../NoSqlDb/NoSqlDb.h"
*
* Maintenance History:
*---------------------
* Ver 1.0 : 04/07 2017
* - first release
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include "../NoSqlDb/NoSqlDb.h"

using namespace NoSQLDB;

class publisher
{
public:
	void fileToHtml(std::string path, NoSQLDB::Element<std::string> elem);
	void CreateIndex(NoSqlDb<std::string>&db);
};