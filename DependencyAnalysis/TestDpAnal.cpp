/////////////////////////////////////////////////////////////////////
// Executive.cpp - Directs Code Analysis                           //
// ver 1.4                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2016                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: Project #2, CSE687 - Object Oriented Design, S2015 //
// Author:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////

#include "TestDpAnal.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "DependencyAnalysis.h"
#include "../FileMgr/FileMgr.h"
#include "../FileSystem/FileSystem.h"
#include "../Parser/Parser.h"
#include "../TypeAnalysis/TypeAnalysis.h"
#include "../TypeTable/TypeTable.h"
#include "../Utilities/Utilities.h"


/////////////////////////////////////////////////////////////////////
// AnalFileMgr class
// - Derives from FileMgr to make application specific file handler
//   by overriding FileMgr::file(), FileMgr::dir(), and FileMgr::done()

using namespace CodeAnalysis;
using namespace FileManager;

//----< initialize application specific FileMgr >--------------------
/*
* - Accepts CodeAnalysisExecutive's path and fileMap by reference
*/
AnalFileMgr::AnalFileMgr(const Path& path, FileMap& fileMap)
	: FileMgr(path), fileMap_(fileMap) {}

//----< override of FileMgr::file(...) to store found files >------

void AnalFileMgr::file(const File& f)
{
	File fqf = d_ + "\\" + f;
	Ext ext = FileSystem::Path::getExt(fqf);
	Pattern p = "*." + ext;
	fileMap_[p].push_back(fqf);
	++numFiles_;
}
//----< override of FileMgr::dir(...) to save current dir >----------

void AnalFileMgr::dir(const Dir& d)
{
	d_ = d;
	++numDirs_;
}
//----< override of FileMgr::done(), not currently used >------------

void AnalFileMgr::done()
{}
//----< returns number of matched files from search >----------------


/////////////////////////////////////////////////////////////////////
// CodeAnalysisExecutive class
// - 

using File = std::string;
using ASTNodes = std::vector<ASTNode*>;

//----< initialize parser, get access to repository >----------------

CodeAnalysisExecutive::CodeAnalysisExecutive()
{
	pParser_ = configure_.Build();
	if (pParser_ == nullptr)
	{
		throw std::exception("couldn't create parser");
	}
	pRepo_ = Repository::getInstance();
}
//----< cleanup >----------------------------------------------------

CodeAnalysisExecutive::~CodeAnalysisExecutive()
{ }

//----< show command line arguments >--------------------------------

void CodeAnalysisExecutive::showCommandLineArguments(int argc, char* argv[])
{
	std::ostringstream out;
	out << "\n     Path: \"" << FileSystem::Path::getFullFileSpec(argv[1]) << "\"\n     Args: ";
	for (int i = 2; i < argc - 1; ++i)
		out << argv[i] << ", ";
	out << argv[argc - 1];

}
//----< handle command line arguments >------------------------------
/*
* Arguments are:
* - path: possibly relative path to folder containing all analyzed code,
*   e.g., may be anywhere in the directory tree rooted at that path
* - patterns: one or more file patterns of the form *.h, *.cpp, and *.cs
* - options: /m (show metrics), /s (show file sizes), and /a (show AST)
*/
bool CodeAnalysisExecutive::ProcessCommandLine(int argc, char* argv[])
{
	if (argc < 2)
	{
		return false;
	}
	try {
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		if (!FileSystem::Directory::exists(path_))
		{
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}
		for (int i = 2; i < argc; ++i)
		{
			if (argv[i][0] == '/')
				options_.push_back(argv[i][1]);
			else
				patterns_.push_back(argv[i]);
		}
		if (patterns_.size() == 0)
		{
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;
}

//----< searches path for files matching specified patterns >----
void CodeAnalysisExecutive::getSourceFiles()
{
	AnalFileMgr fm(path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
}
//----< helper: is text a substring of str? >--------------------

bool contains(const std::string& str, const std::string& text)
{
	if (str.find(text) < str.length())
		return true;
	return false;
}
//----< retrieve from fileMap all files matching *.h >-----------

std::vector<File>& CodeAnalysisExecutive::cppHeaderFiles()
{
	cppHeaderFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.h"))
		{
			for (auto file : item.second)
				cppHeaderFiles_.push_back(file);
		}
	}
	return cppHeaderFiles_;
}
//----< retrieve from fileMap all files matching *.cpp >---------

std::vector<File>& CodeAnalysisExecutive::cppImplemFiles()
{
	cppImplemFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cpp"))
		{
			for (auto file : item.second)
				cppImplemFiles_.push_back(file);
		}
	}
	return cppImplemFiles_;
}
//----< retrieve from fileMap all files matching *.cs >----------

std::vector<File>& CodeAnalysisExecutive::csharpFiles()
{
	csharpFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cs"))
		{
			for (auto file : item.second)
				csharpFiles_.push_back(file);
		}
	}
	return csharpFiles_;
}

//----< report number of files processed >-----------------------

void CodeAnalysisExecutive::processSourceCode(bool showProc)
{
	for (auto file : cppHeaderFiles())
	{
		pRepo_->package() = FileSystem::Path::getName(file);
		if (!configure_.Attach(file))
		{
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			continue;
		}// parse file
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();
		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
	for (auto file : cppImplemFiles()){
		pRepo_->package() = FileSystem::Path::getName(file);
		if (!configure_.Attach(file)){
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			continue;
		}// parse file
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();
		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
	for (auto file : csharpFiles()){
		pRepo_->package() = FileSystem::Path::getName(file);
		if (!configure_.Attach(file)){
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			continue;
		}
		// parse file
		pRepo_->language() = Language::CSharp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();
		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
	std::ostringstream out;
}

struct FileHandler : IFileEventHandler
{
	void execute(const std::string& fileSpec)
	{
		std::cout << "\n  --   " << fileSpec;
	}
};

struct DirHandler : IDirEventHandler
{
	void execute(const std::string& dirSpec)
	{
		std::cout << "\n  ++ " << dirSpec;
	}
};


struct DoneHandler : IDoneEventHandler
{
	void execute(size_t numFilesProcessed)
	{
		std::cout << "\n\n  Processed " << numFilesProcessed << " files";
	}
};

//----< conduct code analysis >--------------------------------------

#include <fstream>

int main(int argc, char* argv[])
{
	using namespace CodeAnalysis;
	CodeAnalysisExecutive exec;
	NoSQLDB::NoSqlDb<std::string> db;
	try {
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		if (!succeeded)
			return 1;
		std::ostringstream tOut("Test TypeAnalysis");
		Utils::sTitle(tOut.str(), 3, 92, tOut, '=');
		exec.showCommandLineArguments(argc, argv);
		exec.getSourceFiles();
		exec.processSourceCode(true);
		TypeAnal ta;
		TypeTable<StrData> tt = ta.doTypeAnal();
		std::cout << "\n\n  Testing FileMgr";
		std::cout << "\n =================";
		//std::string path = FileSystem::Path::getFullFileSpec("..");
		std::string path = "../Utilities";
		IFileMgr* pFmgr = FileMgrFactory::create(path);
		FileHandler fh;
		DirHandler dh;
		DoneHandler dnh;
		pFmgr->regForFiles(&fh);
		pFmgr->regForDirs(&dh);
		pFmgr->regForDone(&dnh);
		pFmgr->addPattern("*.h");
		pFmgr->addPattern("*.cpp");
		pFmgr->addPattern("*.cs");
		std::vector<std::string> files = pFmgr->search();
		for (unsigned i = 0; i < files.size(); i++)
			std::cout << "\n  " + files[i];
		DepAnal da("..\\Utilities\\Utilities.cpp",tt);
		da.saveToken();
		NoSQLDB::Element<std::string> tempElem = da.DepAnalyzer();
		da.savetoDb(tempElem, db);
	}
	catch (std::exception& except)
	{
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		return 1;
	}
	std::cout << "\n\n";
	system("pause");
	return 0;
}

