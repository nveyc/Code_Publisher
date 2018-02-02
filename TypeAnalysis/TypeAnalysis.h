#pragma once
/////////////////////////////////////////////////////////////////////////////
// TypeAnalysis.h - Demonstrate how to analyze the dependency in a package //
//                                                                         //
// Chao Yang                                                               //
// Reference:                                                              //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017               //
/////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package is based on Fawcett's DepAnal.h
* It contains provides doTypeAnal() function to analyze the dependency
*
* I use AST to find the first level type name by searching the type name
* which the parent type is namespace
*
* It usually contains three kinds of type: class, global function and struct
* After analysis, doTypeAnal() save the package infomation to an element
* and save into the typetable database
*
* Required Files:
* ---------------
* "../Parser/ActionsAndRules.h"
* "../TypeTable/TypeTable.h"
*
* Maintenance History:
*---------------------
* Ver 1.0 : 03/14 2017
* - first release
*/

#include "../Parser/ActionsAndRules.h"
#include "../TypeTable/TypeTable.h"
#include <iostream>
#include <functional>

#pragma warning (disable : 4101)  // disable warning re unused variable x, below

using namespace TypeTABLE;
using StrData = std::string;

namespace CodeAnalysis
{
	class TypeAnal
	{
	public:
		using SPtr = std::shared_ptr<ASTNode*>;

		TypeAnal();
		TypeTable<StrData> doTypeAnal();
	private:
		void DFS(ASTNode* pNode);
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		Scanner::Toker& toker_;
		TypeTable<StrData> tt;
	};

	inline TypeAnal::TypeAnal() :
		ASTref_(Repository::getInstance()->AST()),
		scopeStack_(Repository::getInstance()->scopeStack()),
		toker_(*(Repository::getInstance()->Toker()))
	{
		std::function<void()> test = [] { int x; };  // This is here to test detection of lambdas.
	}                                              // It doesn't do anything useful for dep anal.

	inline bool doDisplay(ASTNode* pNode)
	{
		static std::string toDisplay[] = {
			"function", "lambda", "class", "struct", "enum", "alias", "typedef"
		};
		for (std::string type : toDisplay)
		{
			if (pNode->type_ == type)
				return true;
		}
		return false;
	}
	inline void TypeAnal::DFS(ASTNode* pNode)
	{
		static std::string path = "";
		if (pNode->path_ != path)
		{
			std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
			path = pNode->path_;
		}

		if (doDisplay(pNode) && pNode->parentType_ == "namespace" && pNode->name_ != "main")
		{
			Element<StrData> temp;

			temp.tname = pNode->name_;
			temp.fileName = pNode->package_;
			temp.filePath = pNode->path_;
			temp.type = pNode->type_;

			tt.saveRecord(temp.tname, temp);

			std::cout << "\n  " << pNode->name_;
			std::cout << ", " << pNode->package_;
			std::cout << ", " << pNode->type_;
			std::cout << ", " << pNode->parentType_;
		}

		for (auto pChild : pNode->children_)
			DFS(pChild);
	}

	inline TypeTable<StrData> TypeAnal::doTypeAnal()
	{
		std::cout << "\n  starting type analysis:\n";
		std::cout << "\n  scanning AST and displaying important things:";
		std::cout << "\n -----------------------------------------------";
		ASTNode* pRoot = ASTref_.root();
		DFS(pRoot);
		std::cout << "\n  size of db = " << tt.count() << "\n";
		tt.showtable();
		return tt;
	}
}