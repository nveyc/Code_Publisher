/////////////////////////////////////////////////////////////////////
// TypeTable.cpp - key/value pair in-memory database               //
// ver 1.0                                                         //
//                                                                 //
// Chao Yang                                                       //
// Reference:                                                      //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#include "TypeTable.h"
#include <iostream>

#define TEST_TYPETABLE
#ifdef TEST_TYPETABLE

using namespace TypeTABLE;
using StrData = std::string;

int main()
{
	std::cout << "\n  Demonstrating NoSql Helper Code";
	std::cout << "\n =================================\n";

	std::cout << "\n  Creating and saving NoSqlDb elements with string data";
	std::cout << "\n -------------------------------------------------------\n";

	TypeTable<StrData> db;
	Element<StrData> elem1;
	elem1.tname = "x1";
	elem1.type = "class";
	elem1.nameSpace = "x1Namespace";
	elem1.fileName = "ele1.h";
	elem1.filePath = "../../ele1.h";
	Element<StrData> elem2;
	elem2.tname = "x2";
	elem2.type = "class";
	elem2.nameSpace = "x2Namespace";
	elem2.fileName = "ele2.h";
	elem2.filePath = "../../ele2.h";
	db.saveRecord(elem1.tname, elem1);
	db.saveRecord(elem2.tname, elem2);

	std::cout << "\n  Retrieving elements from NoSqlDb<string>";
	std::cout << "\n ------------------------------------------\n";

	std::cout << "\n  size of db = " << db.count() << "\n";
	Keys keys = db.keys();
	for (Key key : keys)
	{
		std::cout << "\n  " << key << ":";
		std::cout << db.value(key).show();
	}
	std::cout << "\n  Erasing element with key x2.h";
	if (db.removeRecord("x2.h"))
		std::cout << "\n  removed record and all children references to this record";
	std::cout << "\n  size of db = " << db.count() << "\n";
	keys = db.keys();
	for (Key key : keys)
	{
		std::cout << "\n  " << key << ":";
		std::cout << db.value(key).show();
	}
	std::cout << "\n\n";
}
#endif
