#pragma once
/////////////////////////////////////////////////////////////////////////
// Server.h - Two-way HTTP style messaging and file transfer           //
//                                                                     //
// Chao Yang, SUID: #461352435                                         //
//                                                                     //
// Reference:                                                          //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015                                     //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and send message to client
*
* The two-way communication supports to send HTTP style message and files
*
*/
/*
* Required Files:
* HttpMessage.h, Sockets.h, FileSystem.h, Logger.h, Utilities.h
*/
/*
* Functions:
* ServerMakeMessage - Make a HTTP msg form Server
* readMessage - Parse a HTTP msg form Server
* processMsg - help to process message
* readFile - accept file from client
* sendMessage - send message to Client
* sendFile  - send message to Client
*
* Maintenance History:
* --------------------
* ver 1.0 : May.4 2017
*/
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>

using namespace Logging;
using namespace Async;

using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
///////////////////////////////////////////
//this class is for server
////////////////////////////////////////////
class Server {
public:
	using EndPoint = std::string;

	Server() 
	{ msgQ = new BlockingQueue<HttpMessage>();  ss = new SocketSystem(); }

	void operator()(Socket socket);

	void serverStartListen();
	void ConnecttoClient(std::string port);

	HttpMessage ServerMakeMessage(size_t n, const std::string& msgBody, const EndPoint& ep, std::string category);
	HttpMessage readMessage(Socket& socket);
	void processMsg(HttpMessage msg, std::string port);
	void readFile(const std::string& filename, size_t fileSize, std::string category,Socket& sockets);	

private:
	bool connectionClosed_;
	SocketSystem *ss;
	SocketConnecter *si;
	SocketListener * sl;
	BlockingQueue<HttpMessage> * msgQ;
	std::string repoPath = "../ServerRepository";

	void sendMessage(HttpMessage& msg);
	bool sendFile(const std::string& filename, std::string category, std::string to);
	void reAllcatagorie(std::string port);
	void reFilesInCategory(std::string category, std::string port);
	void reHTMLs(std::string port);
	void reFilesWithNP(std::string category, std::string port);
	void reDeleteFile(std::string file);
	void split(const std::string &s, char delim, std::vector<std::string>& elems);
};