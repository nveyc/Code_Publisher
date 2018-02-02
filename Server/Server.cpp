/////////////////////////////////////////////////////////////////////////
// Server.cpp - Two-way HTTP style messaging and file transfer         //
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
* files from multiple concurrent clients and simply displays the messages
* and stores files.
*
* The two-way communication supports to send HTTP style message and files
*/
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*/
/*
* ToDo:
* - pull the receiving code into a Receiver class
* - Receiver should own a BlockingQueue, exposed through a
*   public method:
*     HttpMessage msg = Receiver.GetMessage()
* - You will start the Receiver instance like this:
*     Receiver rcvr("localhost:8080");
*     ClientHandler ch;
*     rcvr.start(ch);
*
* Maintenance History:
* --------------------
* ver 1.0 : May.4 2017
*/

#include<iostream>
#include "Server.h"


void Server::ConnecttoClient(std::string port)
{
	std::cout << "\n  Server connect to client:" + port;
	try
	{
		while (!si->connect("localhost", std::stoi(port)))
		{
			Show::write("\n Server waiting to connect");
			::Sleep(100);
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

void Server::serverStartListen() {
	std::cout << "\n  Server 8080 started listen\n";
	try {
		sl = new SocketListener(8080, Socket::IP6);
		si = new SocketConnecter();
		sl->start(*this);
		while (true)
		{
			HttpMessage msg = msgQ->deQ();			
			std::cout << "\n\n  server recvd message with body contents:\n  \"" + msg.bodyString() + "\"";
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

HttpMessage Server::ServerMakeMessage(size_t n, const std::string& msgBody, const EndPoint& ep, std::string category)
{
	HttpMessage msg;
	EndPoint myEndPoint = "localhost:8080";
	HttpMessage::Attribute attrib;
	switch (n) {
	case 0:// send message
		msg.clear();
		msg.addAttribute(HttpMessage::Attribute("mode", "0"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));
		attrib = HttpMessage::attribute("category", category);
		if (msgBody.size() >= 0)
		{
			attrib = HttpMessage::attribute("content-length", Convert<size_t>::toString(msgBody.size()));
			msg.addAttribute(attrib);
		}
		break;

	case 1: // send file
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("mode", "1"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));
		msg.addAttribute(HttpMessage::attribute("category", category));
		if (msgBody.size() > 0)
		{
			msg.addAttribute(attrib);
			attrib = HttpMessage::attribute("content-length", Convert<size_t>::toString(msgBody.size()));
			msg.addAttribute(attrib);
		}
		break;

	default:
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	msg.addBody(msgBody);
	return msg;
}

void Server::sendMessage(HttpMessage& msg) {
	std::string msgString = msg.toString();
	si->send(msgString.size(), (Socket::byte*)msgString.c_str());
}

bool Server::sendFile(const std::string& filename, std::string category, std::string to)
{
	std::string fqname = filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Convert<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;
	HttpMessage msg = ServerMakeMessage(1, "", "localhost::" + to, category);
	msg.addAttribute(HttpMessage::Attribute("file", fi.name()));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	sendMessage(msg);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		si->send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	::Sleep(500);
	file.close();
	return true;
}

void Server::processMsg(HttpMessage msg, std::string port)
{
	std::string msgBody = msg.bodyString();
	if (msgBody == "requireAllCategory") 
		reAllcatagorie(port);
	if (msgBody == "reFileInCategory")
		reFilesInCategory(msg.findValue("category"), port);
	if (msgBody == "html") 
		reHTMLs(port);
	if (msgBody == "downloadFile") {
		std::string category = msg.findValue("category");
		std::string filename = msg.findValue("file");
		sendFile(FileSystem::Path::getFullFileSpec("../ServerRepository/" + category + "/" + filename), category, port);
	}
	if (msgBody == "CodeAnalysis") 
		system("start ../x64/Debug/TestExecutive.exe ../ServerRepository *.h *.cpp  /f /r");
	if (msgBody == "noParent") 
		reFilesWithNP(msg.findValue("category"), port);
	if (msgBody == "deleteFile") {
		std::string file = msg.findValue("file");
		reDeleteFile(file);
	}
}

HttpMessage Server::readMessage(Socket& socket){
	connectionClosed_ = false;
	HttpMessage msg;
	while (true){
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1){
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
	}
		else break;
	}	
	if (msg.attributes().size() == 0){  // If client is done, connection breaks and recvString returns empty string
		connectionClosed_ = true;
		return msg;
	}
	int mode = std::stoi(msg.findValue("mode"));
	std::string client_port = StringHelper::split(msg.findValue("fromAddr"))[1];
	size_t numBytes = 0;
	size_t pos = msg.findAttribute("content-length");
	std::cout << "\n  Client Port is " << client_port;
	ConnecttoClient(client_port);
	std::string filename = msg.findValue("file");
	if (mode == 0) {
		if (pos < msg.attributes().size()) {
			numBytes = Convert<int>::fromString(msg.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			msg.addBody(msgBody);
			delete[] buffer;
			processMsg(msg, client_port);
		}
	}
	else {
		size_t contentSize;
		std::string sizeString = msg.findValue("content-length");
		if (sizeString != "") contentSize = Convert<size_t>::fromString(sizeString);
		else return msg;
		std::string cate = msg.findValue("category");
		readFile(filename, contentSize, cate, socket);
		msg.removeAttribute("content-length");  // construct message body
		std::string bodyString = "<file>" + filename + "</file>";
		sizeString = Convert<size_t>::toString(bodyString.size());
		msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		msg.addBody(bodyString);
	}
	return msg;
}

void Server::readFile(const std::string& filename, size_t fileSize, std::string category, Socket& socket)
{
	FileSystem::Directory::create("../ServerRepository/" + category);
	std::string fqname = "../ServerRepository/" + category + "/" + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		Show::write("\n\n  can't open file " + fqname);
	}
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	
	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);
		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);
		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
}

void Server::reAllcatagorie(std::string port)
{
	std::vector<std::string> directories = FileSystem::Directory::getDirectories(repoPath);
	std::string msgbody = "";
	for (std::string folder : directories) // for each( string i: directro)
	{
		if (folder == "." || folder == "..") 
			continue;
		if(folder != "NoParent" && folder != "HTML")
			msgbody += folder + ":";
	}
	sendMessage(ServerMakeMessage(0, msgbody, port, ""));
}

void Server::reHTMLs(std::string port)
{
	std::string msgbody = "";
	for (std::string file : FileSystem::Directory::getFiles("../ServerRepository/HTML","*.htm"))
	{
		msgbody += FileSystem::Path::getName(file) + ":";
	}
	HttpMessage msg = ServerMakeMessage(0, msgbody, port, "");
	sendMessage(msg);
}

void Server::reFilesInCategory(std::string category, std::string port) {

	std::string msgbody = "";
	for (std::string file : FileSystem::Directory::getFiles(repoPath + "/" + category))
	{
		msgbody += FileSystem::Path::getName(file) + ":";
	}
	HttpMessage msg = ServerMakeMessage(0, msgbody, port, "");
	HttpMessage::Attribute attrib = HttpMessage::attribute("category", category);
	msg.addAttribute(attrib);
	sendMessage(msg);
}

void Server::reDeleteFile(std::string file)
{
	std::vector<std::string> tmp;
	std::string ffpath;
	split(file, ',', tmp);
	if (tmp[0] == "none") {
		ffpath = "../ServerRepository/" + tmp[1];
		std::vector<std::string> files = FileSystem::Directory::getFiles(ffpath);
		for (auto file : files) {
			std::string fpath = ffpath + '/' + file;
			remove(fpath.c_str());
		}
		std::string fulld = FileSystem::Path::getFullFileSpec(ffpath);
		RemoveDirectory(fulld.c_str());
	}
	else {
		ffpath = "../ServerRepository/" + tmp[1] + "/" + tmp[0];
		std::string fullf = FileSystem::Path::getFullFileSpec(ffpath);
		remove(fullf.c_str());
	}
}

void Server::reFilesWithNP(std::string category, std::string port)
{
	std::string msgbody = "";
	std::string str;
	std::vector<std::string> strV;
	if (FileSystem::Directory::exists("../ServerRepository/NoParent"))
	{
		std::ifstream fileT("../ServerRepository/NoParent/np.txt");
		std::getline(fileT, str);
		split(str, ':', strV);
		std::string filename;
		std::vector<std::string> files = FileSystem::Directory::getFiles("../ServerRepository/" + category);
		bool f = false;
		for (unsigned i = 0; i < files.size(); i++)
			for (unsigned j = 0; j < strV.size(); j++)
				if (files[i] == strV[j])
					filename += (strV[j] + ":");
		msgbody = filename;
	}
	HttpMessage msg = ServerMakeMessage(0, msgbody, port, "");
	sendMessage(msg);
}

void Server::split(const std::string &s, char delim, std::vector<std::string>& elems)
{
	std::string temp;
	for (size_t i = 0; i <s.size(); i++)
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
}


void Server::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ->enQ(msg);
	}
}

#ifdef TEST_SERVER

int main()
{
	::SetConsoleTitle("HttpMessage Server - Runs Forever");

	Show::attach(&std::cout);
	Show::start();
	Show::title("\n  HttpMessage Server started");

	Server cp;
	cp.serverStartListen();

	system("pause");
}
#endif