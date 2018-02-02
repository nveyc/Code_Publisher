#pragma once
/////////////////////////////////////////////////////////////////////////
// Client.h - Two-way HTTP style messaging and file transfer           //
//                                                                     //
// Chao Yang, SUID: #461352435                                         //
//                                                                     //
// Reference:                                                          //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015                                     //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that receives HTTP style messages and
* files from multiple concurrent clients and send message to client
*
* The two-way communication supports to send HTTP style message and files
*
/*
* Required Files:
* HttpMessage.h,Sockets.h, FileSystem.h, Logger.h, Utilities.h
*/
/*
* Functions:
* cMakeMessage - Make a HTTP msg form Client
* readMessage - Parse a HTTP msg form Client
* readFile - accept file from Server
* sendMessage - send message to Server
* sendFile  - send message to Server
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
//this class is created for both client and server
//combine all their functions together because they have many same features 
//
////////////////////////////////////////////
class Client {
public:
	using EndPoint = std::string;

	Client() {
		ss = new SocketSystem();
		si = new SocketConnecter();
		msgQ = new BlockingQueue<HttpMessage>();
		ConnectToServer();

		threadid = Utilities::Convert<std::thread::id>::toString(std::this_thread::get_id());
		std::cout << "  Client " + threadid + " started listen\n";
		std::thread t1([&]() {
			cStartListen();
		});
		t1.detach();
	}
	void operator()(Socket socket);
	HttpMessage cMakeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);//make message according to message mode
	void cStartListen();
	void ConnectToServer();
	void sendMessage(HttpMessage& msg);
	bool sendFile(const std::string& fqname,std::string category);
	void readFile(const std::string& filename, size_t fileSize, std::string category, Socket& socket);
	HttpMessage readMessage(Socket& socket);// will use functions in the process

	BlockingQueue<HttpMessage>* displayQueue() { return displayQ; }
	BlockingQueue<HttpMessage>* displayQ = new BlockingQueue<HttpMessage>();
	void DemoProject();

private:
	SocketSystem *ss;
	SocketConnecter *si;
	SocketListener * sl;
	std::string threadid;
	BlockingQueue<HttpMessage>* msgQ;
	std::string clientRepositoryPath;
	bool connectionClosed_;
	void DemoRequirement58();
	void DemoRequirement346();
	void DemoRequirement78();
};