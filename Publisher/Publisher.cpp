///////////////////////////////////////////////////////////////////////
// Publisher.cpp - publish the source code                           //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
//                                                                   //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Chao Yang, Syracuse University                       //
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>

#include "Publisher.h"
#include "../FileSystem/FileSystem.h"

using namespace FileSystem;

void publisher::fileToHtml(std::string path, NoSQLDB::Element<std::string> elem)
{
	std::string outputname;
	std::ifstream inStream;
	std::ofstream outStream;
	inStream.open(path);  //  import the file
	if (inStream.fail()) {
		std::cout << "failure opening file." << std::endl;
		exit(1);
	}
	std::string filename = elem.name;
	outputname = "..\\ServerRepository\\HTML\\" + filename + ".htm";  // Create the output file
	outStream.open(outputname);
	outStream << "<html>" << "<head>" << "<link rel=\"stylesheet\"type = \"text/css\" href=\"stylesheet.css\">"   //  import css style	
		<< "<script type=\"text/javascript\" src=\"script.js\"></script>"  //  import javascricpt
		<< "</head>"<< "<body>"<< "<h3>" << filename << "</h3>" << "<hr />" << "<div class = \"indent\">" << "<h4>Dependencies:</h4>";
	for (unsigned i = 0; i < elem.children.getValue().size(); i++){
		if (i != elem.children.getValue().size() - 1)
			outStream << "<a href =\""<< elem.children.getValue()[i]<< ".htm\">"<< elem.children.getValue()[i]<< "</a>, ";
		else
			outStream << "<a href =\""<< elem.children.getValue()[i]<< ".htm\">"<< elem.children.getValue()[i]<< "</a>";
	}
	outStream << "</div>" << "<hr />" << "<pre>";	// Output /div, hr, pre tag
	char ch;
	int count = 0;
	while (!inStream.eof()) {
		inStream.get(ch);    // get one character							
		if (ch == '<')       // replace '<' with "&lt"
			outStream << "&lt;";
		else if (ch == '>')  // replace '>' with "&gt"
			outStream << "&gt;";
		
		else if (ch == '{'){
			outStream << "<button onclick=\"javascript:showHide('div_" << count <<"'); \" type=\"button\" sytle=\"display:inline\">+</button>";
			outStream << ch;
			outStream << "<div id=\"div_" << count << "\"style=\"display: none; \">";
			count++;
		}	
		else if (ch == '}'){
			outStream << "</div>";
			outStream << ch;
		}	
		else outStream << ch;
	}
	outStream << "</pre>" << "</body>" << "</html>" << std::endl;  // Output /pre, /body, /html tag
	inStream.close();
	outStream.close();
	std::cout <<  std::endl <<"  Publish done.  Results are in " << outputname;
}


void publisher::CreateIndex(NoSqlDb<std::string>&db)
{
	std::ofstream outStream;
	std::string head = "<html><head><link rel=\"stylesheet\"type=\"text/css\" href=\"stylesheet.css\"/></head>";
	head += "<body><h3>Index</h3>";
	head += "<hr /><div class=\"indent\">";
	NoSQLDB::Keys keys = db.keys();
	for (auto key : keys)
	{
		head += "<a href=\"" + key + ".htm\">"  + key + "</a><br>";
	}
	head += "</pre></body></html>";
	if (!FileSystem::Directory::exists("../Results"))
		FileSystem::Directory::create("../Results");
	std::ofstream out("../ServerRepository/HTML/Index.htm");
	out << head;
	out.close();
}

#ifdef TEST_PUBLISHER
//     main function
int main() {
	publisher pu;

	// Input file to convert
	std::string filename;
	// Output file with .html on the end

	std::ifstream inStream;
	std::ofstream outStream;

	std::cout << "Enter filename you woudl like to convert: " << std::endl;
	std::cin >> filename;

	Element<std::string> testElem;

	pu.fileToHtml(filename.c_str(), testElem);
}
#endif