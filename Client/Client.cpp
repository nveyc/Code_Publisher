/////////////////////////////////////////////////////////////////////////
// Client.cpp - Two-way HTTP style messaging and file transfer         //
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
* files from server and process the messages
* and stores files.
*
* The two-way communication supports to send HTTP style message and files
*
* The package also provides a demostration which includes all communications
* between client and server

*/
/*
* Required Files:
*   Client.h
*
* Maintenance History:
* --------------------
* ver 1.0 : May.4 2017
*/
#include "Client.h"
#include<iostream>

void Client::operator()(Socket socket)
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

void Client::cStartListen() {
	try {
		//threadid = Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id());
		std::cout << "  Client started listen\n";
		sl = new SocketListener(std::stoi(threadid), Socket::IP6);
		std::cout << "\n  The Client Port is " + threadid << std::endl;
		sl->start(*this);
		while (true)
		{
			HttpMessage msg = msgQ->deQ();
			std::string server_port = StringHelper::split(msg.findValue("fromAddr"))[1];
			Show::write("\n\n  Client recvd message with body contents:\n  " + msg.bodyString());
			std::cout << "\n  from server " << server_port;
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

HttpMessage Client::cMakeMessage(size_t n, const std::string& msgBody, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;

	//threadid = Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id());
	EndPoint myEndPoint = "localhost:" + threadid;
	msg.clear();
	msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
	msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));
	if (msgBody.size() > 0)
	{
		attrib = HttpMessage::attribute("content-length", Convert<size_t>::toString(msgBody.size()));
		msg.addAttribute(attrib);
	}
	msg.addAttribute(HttpMessage::Attribute("mode", Utilities::Convert<size_t>::toString(n)));
	if (n < 0 || n > 1)
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));

	msg.addBody(msgBody);
	return msg;
}

void Client::ConnectToServer() {
	Show::attach(&std::cout);
	Show::start();
	Show::title("Starting HttpMessage client " + threadid);
	try
	{
		while (!si->connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
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

bool Client::sendFile(const std::string& filename, std::string category)
{
	std::string fqname = filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Convert<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;
	HttpMessage msg = cMakeMessage(1, "", "localhost::8080");
	msg.addAttribute(HttpMessage::Attribute("file", fi.name()));
	msg.addAttribute(HttpMessage::Attribute("category", category));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	sendMessage(msg);
	const int BlockSize = 2048;
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

void Client::sendMessage(HttpMessage& msg) {
	std::string msgString = msg.toString();
	si->send(msgString.size(), (Socket::byte*)msgString.c_str());
}


void Client::readFile(const std::string& filename, size_t fileSize, std::string category, Socket& socket)
{
	FileSystem::Directory::create("../ClientRepository/" + category);
	std::string fqname = "../ClientRepository/" + category + "/" + filename;
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

HttpMessage Client::readMessage(Socket& socket){
	connectionClosed_ = false;
	HttpMessage msg; // read message attributes
	while (true){
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1){
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
			break;
	}	
	if (msg.attributes().size() == 0) {// If client is done, connection breaks and recvString returns empty string
		connectionClosed_ = true;
		return msg;
	}
	int mode = std::stoi(msg.findValue("mode"));
	std::string filename = msg.findValue("file");
	int numBytes = 0;
	size_t pos = msg.findAttribute("content-length");
	if (mode == 0){
		if (pos < msg.attributes().size()){
			numBytes = Convert<int>::fromString(msg.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			msg.addBody(msgBody);
			delete[] buffer;
		}
		displayQ->enQ(msg);
	}
	else {
		size_t contentSize;
		std::string sizeString = msg.findValue("content-length");
		std::string category = msg.findValue("category");
		if (sizeString != "")
			contentSize = Convert<size_t>::fromString(sizeString);
		else
			return msg;
		readFile(filename, contentSize, category, socket);
		msg.removeAttribute("content-length");
		std::string bodyString = "<file>" + filename + "</file>";
		sizeString = Convert<size_t>::toString(bodyString.size());
		msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		msg.addBody(bodyString);
	}
	return msg;
}

void Client::DemoProject()
{
	std::cout << "\n\n  The Remote Code Publisher Built in VS 2015 and start with WPF   - Req #1";
	std::cout << "\n\n  The Project was Built with C++ standard library's streams   - Req #2";
	std::cout << "\n\n  Upload Files to Repository Server   - Req #5, Req #6, Req #8";
	std::cout << "\n ===================================";
	DemoRequirement58();

	std::cout << "\n\n\n  Send HTTP Message to Server   - Req #7, Req #8";
	std::cout << "\n =============================";
	DemoRequirement78();


	std::cout << "\n\n\n  Start Remote CodeAnalysis and Pubulisher   - Req #3, Req #4, Req #6";
	std::cout << "\n ==========================================";
	DemoRequirement346();

	::Sleep(300);
	std::cout << "\n\n\n   Auto Demostration Finished   - Req #9";
	std::cout << "\n\n";
}

void Client::DemoRequirement58()
{
	for (auto file : FileSystem::Directory::getFiles("../ClientRepository/DemoFile/", "*.h"))
	{
		std::cout << "\n\n   client send file " + file;
		sendFile("../ClientRepository/DemoFile/" + file, "DemoClient");
	}
	for (auto file : FileSystem::Directory::getFiles("../ClientRepository/DemoFile/", "*.cpp"))
	{
		std::cout << "\n\n   client send file " + file;
		sendFile("../ClientRepository/DemoFile/" + file, "DemoClient");
	}
}

void Client::DemoRequirement78() {

	std::thread t1([&]()
	{
		std::cout << "\n\n   send message \"requireAllCategory\"";
		sendMessage(cMakeMessage(0, "requireAllCategory", "8080"));
		::Sleep(300);
		displayQ->deQ();
	});
	t1.join();
	
	std::thread t2([&]() 
	{
		std::cout << "\n\n ------------------------------------------";
		std::cout << "\n\n   send message \"reFileInCategory\"";
		HttpMessage msg = cMakeMessage(0, "reFileInCategory", "8080");
		msg.addAttribute(HttpMessage::attribute("category", "DemoClient"));
		sendMessage(msg);
		::Sleep(300);
		displayQ->deQ();
	});
	t2.join();
}

void Client::DemoRequirement346() {
	HttpMessage msg = cMakeMessage(0, "CodeAnalysis", "8080");
	std::cout << "\n  Do Remote Type Analysiser " << std::endl;
	sendMessage(msg);

	::Sleep(3000);

	std::thread t1([&]()
	{
		std::cout << "\n\n ------------------------------------------";
		std::cout << "\n\n   send message \"noParent\"";
		msg = cMakeMessage(0, "noParent", "8080");
		msg.addAttribute(HttpMessage::attribute("category", "DemoClient"));
		sendMessage(msg);
		::Sleep(300);
		displayQ->deQ();
	});
	t1.join();
}


#ifdef TEST_CLIENT

int main()
{
	std::cout << "Clients Running on Threads";
	Show::title("Demonstrating two HttpMessage Clients each running on a child thread");

	std::thread t1(
		[&]() {
		Client c1;

		std::string threadid = Utilities::Convert<std::thread::id>::toString(std::this_thread::get_id());

		c1.ConnectToServer();
		c1.sendMessage(c1.cMakeMessage(0, "message from client " + threadid, "localhost: 8080"));
		std::cout << "\n\n  sending message: \"message from client\"";

		std::vector<std::string> files = FileSystem::Directory::getFiles("../TestFiles", "*.cpp");
		for (size_t i = 0; i < 1; ++i)
		{
			Show::write("\n\n  sending file " + files[i] + "\n");
			c1.sendFile(FileSystem::Path::getFullFileSpec("../TestFiles/" + files[i]), "test");
		}
		c1.cStartListen();
	}
	);
	t1.join();

	system("pause");
}
#endif

