/////////////////////////////////////////////////////////////////////
//  DependencyAnalysis.h                                           //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015                                 //
//  Application:   Used to analyze relationship between packages   //
//  Author:        Chao Yang, Syracuse University                  //
/////////////////////////////////////////////////////////////////////
/*
Package Operations:
==================
This package defines an AbstrSynTree class and an ASTNode struct.
Those provides support for building Abstract Syntax Trees during
static source code analysis.

Public Interface:
=================
std::vector<std::string> saveToken();  // get token and save to a vector
NoSQLDB::Element<std::string> DepAnal::DepAnalyzer();  //  compare the token with typetable and return the element
bool savetoDb(NoSQLDB::Element<std::string> elem, NoSQLDB::NoSqlDb<std::string>&db);  //  save dependency to database
std::vector<std::string> DepAnal::splitPath(const std::string str, char delim);
DepAnal(std::string path, TypeTable<std::string> tt);

Build Process:
==============
Required files
"../Tokenizer/Tokenizer.h"
"../NoSqlDb/NoSqlDb.h"
"../TypeTable/TypeTable.h"
"../Utilities/Utilities.h"

Maintenance History:
====================

ver 1.0 : 03/13/2017
- first release

*/

#pragma once
#include "../Tokenizer/Tokenizer.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../TypeTable/TypeTable.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <vector>

using namespace TypeTABLE;

namespace CodeAnalysis
{
	class DepAnal
	{
	public:
		std::vector<std::string> saveToken();  // get token and save to a vector
		NoSQLDB::Element<std::string> DepAnal::DepAnalyzer();  //  compare the token with typetable and return the element
		bool savetoDb(NoSQLDB::Element<std::string> elem, NoSQLDB::NoSqlDb<std::string>&db);  //  save dependency to database
		std::vector<std::string> DepAnal::splitPath(const std::string str, char delim);
		DepAnal(std::string path, TypeTable<std::string> tt);
	private:
		std::vector<std::string> tokVec;
		std::string _file;
		std::string _fullName;
		TypeTable<std::string> _tt;
	};
}