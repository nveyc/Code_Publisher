#pragma once
/////////////////////////////////////////////////////////////////////
// TypeTable.h - key/value pair in-memory database                 //
// ver 1.0                                                         //
// Chao Yang                                                       //
// Reference:                                                      //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package is based on Fawcett's NoSqlDb
* It contains two classes, Element and Typetable
*
* Element is the signle type map with some type elements
* Every element node has some useful metedata to save package infomation
*
* Typetable provides some basic function to add/remove elements from
* typetable and modify some metadata of element.
*
* Required Files:
* ---------------
"../CppProperties/CppProperties.h"
"../Display/Display.h"
"../Utilities/Utilities.h"
*
* Maintenance History:
*---------------------
* Ver 1.0 : 03/14 2017
* - first release
*/

#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "../CppProperties/CppProperties.h"
#include "../Display/Display.h"
#include "../Utilities/Utilities.h"


namespace TypeTABLE
{
	using namespace Utilities;

	using Key = std::string;
	using Keys = std::vector<Key>;
	
	using TypeName = std::string;
	using Type = std::string;
	using Namespace = std::string;
	using FilePath = std::string;
	using FileName = std::string;

	/////////////////////////////////////////////////////////////////////
	// Element class represents a data record in our NoSql database
	// - in our NoSql database that is just the value in a key/value pair
	// - it needs to store child data, something for you to implement
	//
	template<typename Data>
	class Element
	{
	public:

		Element() {}
		Element(TypeName tName, Type type, Namespace nName, FileName filename,FilePath filepath) : tname(tName), type(type), nameSpace(nName), fileName(filename), filePath(filepath) {};

		Property<TypeName> tname;               // metadata
		Property<Type> type;                    // metadata
		Property<Namespace> nameSpace;          // metadata
		Property<FileName> fileName;            // metadata
		Property<FilePath> filePath;            // metadata
		std::string show();
	};


	template<typename Data>
	std::string Element<Data>::show()
	{
		std::ostringstream out;
		out.setf(std::ios::adjustfield, std::ios::left);
		out << "\n    " << std::setw(10) << "TypeName" << " : " << tname;
		out << "\n    " << std::setw(10) << "Type" << " : " << type;
		out << "\n    " << std::setw(10) << "Namespace" << " : " << nameSpace;
		out << "\n    " << std::setw(10) << "FileName" << " : " << fileName;
		out << "\n    " << std::setw(10) << "FilePath" << " : " << filePath;
		out << "\n";
		return out.str();
	}

	/////////////////////////////////////////////////////////////////////
	// NoSqlDb class is a key/value pair in-memory database
	// - stores and retrieves elements
	// - you will need to add query capability
	//   That should probably be another class that uses NoSqlDb
	//
	template<typename Data>
	class TypeTable
	{
	public:
		using Key = std::string;
		using Keys = std::vector<Key>;

		Keys keys();
		bool hasKey(Key key);
		bool saveRecord(Key key, Element<Data> elem);
		bool saveValue(Key key, Element<Data> elem);
		bool removeRecord(Key key);
		Element<Data> value(Key key);
		size_t count();
		void showtable();
	private:
		using Item = std::pair<Key, Element<Data>>;

		std::unordered_map<Key, Element<Data>> store;
	};

	template<typename Data>
	typename TypeTable<Data>::Keys TypeTable<Data>::keys()
	{
		Keys keys;
		for (Item item : store)
		{
			keys.push_back(item.first);
		}
		return keys;
	}

	template<typename Data>
	bool TypeTable<Data>::hasKey(Key key)
	{
		if (store.find(key) == store.end())
			return false;
		return true;
	}

	template<typename Data>
	bool TypeTable<Data>::saveRecord(Key key, Element<Data> elem)
	{
		if (store.find(key) != store.end())
			return false;
		store[key] = elem;
		return true;
	}

	template<typename Data>
	bool TypeTable<Data>::removeRecord(Key key)
	{
		if (store.find(key) == store.end())
			return false;
		size_t numErased = store.erase(key);
		if (numErased == 1)
		{
			return true;
		}
		return false;
	}

	template<typename Data>
	bool TypeTable<Data>::saveValue(Key key, Element<Data> elem)
	{
		if (store.find(key) == store.end())
			return false;
		store[key] = elem;
		return true;
	}

	template<typename Data>
	Element<Data> TypeTable<Data>::value(Key key)
	{
		if (store.find(key) != store.end())
			return store[key];
		return Element<Data>();
	}

	template<typename Data>
	size_t TypeTable<Data>::count()
	{
		return store.size();
	}

	template<typename Data>
	void TypeTable<Data>::showtable()
	{

		std::cout << "\n                                               TypeTable";
		std::cout << "\n =========================================================================================================\n";
		std::cout << std::left << std::setw(32) << "  TypeName" << std::setw(18) << "Type" << "FilePath\n " ;
		std::cout << "------------------------------" << "  ----------------" << "  -------------------------------------------------------";

		for (Key key : keys())
		{
			std::cout << "\n  " << std::setw(32) << key;
			std::cout << std::setw(18) << value(key).type;
			std::cout << value(key).filePath;
		}

		std::cout << "\n";
	}
}
