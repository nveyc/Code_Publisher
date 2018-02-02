/////////////////////////////////////////////////////////////////////
// DependencyAnalysis.cpp - Directs Code Analysis                  //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Chao Yang                                                       //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Application: Project #2, CSE687 - Object Oriented Design, S2017 //
// Author:      Chao Yang, Syracuse University                     //
/////////////////////////////////////////////////////////////////////

#include "DependencyAnalysis.h"
#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include <fstream>
#include <exception>

using namespace CodeAnalysis;
using namespace Scanner;
using namespace Utilities;

using Helper = Utilities::StringHelper;

DepAnal::DepAnal(std::string path, TypeTable<std::string> tt)
{
	_fullName = path;
	std::vector<std::string> ve = splitPath(path, '\\');
	_file = ve[ve.size() - 1];
	_tt = tt;
}

std::vector<std::string> DepAnal::saveToken()
{
	std::string fileSpec = _fullName;
	try
	{
		std::ifstream in(fileSpec);
		if (!in.good())
		{
			std::cout << "\n  can't open " << fileSpec << "\n\n";
			return tokVec;
		}

		{
			Toker toker;
			toker.returnComments();
			toker.attach(&in);
			//std::cout << "\n  current line count = " << toker.currentLineCount();
			do
			{
				std::string tok = toker.getTok();
				if (tok == "\n")
					tok = "newline";

				if (tok.find("//") == std::string::npos && tok.find("newline") == std::string::npos
					&& tok.find("/*") == std::string::npos)
				{
					//std::cout << "\n -- " << tok;
					tokVec.push_back(tok);
				}
			} while (in.good());
			//std::cout << "\n  current line count = " << toker.currentLineCount();
		}
	}
	catch (std::logic_error& ex)
	{
		std::cout << "\n  " << ex.what();
	}
	return tokVec;
}

NoSQLDB::Element<std::string> DepAnal::DepAnalyzer()
{
	NoSQLDB::Element<std::string> tempElem;
	Keys keys = _tt.keys();
	for (Key key : keys)
	{
		bool s = false;

		tempElem.name = _file;
		tempElem.category = splitPath(_fullName, '\\')[splitPath(_fullName, '\\').size() - 2];
		tempElem.dateTime = DateTime();

		for (unsigned i = 0; i < tokVec.size(); i++)
		{		
			if (tokVec.size() < 3)
				break;
			
			if (tokVec[i] == "#" && tokVec[i + 1] == "include")
			{	
				std::string filename = splitPath(tokVec[i + 2], '/')[splitPath(tokVec[i + 2], '/').size() - 1];
				if ((filename == "\"" + _tt.value(key).fileName.getValue() + "\"")|| (filename == _tt.value(key).fileName.getValue() + "\""))
					s = true;
			}

		}
		for (unsigned i = 0; i < tokVec.size(); i++)
		{

			if (s == true &&
				tokVec[i] == key && _tt.value(key).fileName.getValue() != _file)
			{				
				tempElem.saveChild(_tt.value(key).fileName);
			}				
		}
	}
	return tempElem;
}

bool DepAnal::savetoDb(NoSQLDB::Element<std::string> elem, NoSQLDB::NoSqlDb<std::string>&db)
{
	db.saveRecord(elem.name, elem);
	return true;
}


std::vector<std::string> DepAnal::splitPath(const std::string str, char delim) {
	std::string temp;
	std::vector<std::string> elems;
	std::string s = str;
	for (size_t i = 0; i < s.size(); i++)
	{
		if (s.at(i) != delim)
		{
			temp.append(s, i, 1);
			if (i == s.size() - 1)
				elems.push_back(temp);
		}
		else if (temp != "")
		{
			elems.push_back(temp);
			temp = "";
		}
	}
	return elems;
}
