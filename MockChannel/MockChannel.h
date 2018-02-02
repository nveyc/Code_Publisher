#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H
/////////////////////////////////////////////////////////////////////////
// MockChannel.h - provide interface for C++\CLI client                //
//                                                                     //
// Chao Yang, SUID: #461352435                                         //
//                                                                     //
// Reference:                                                          //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
/////////////////////////////////////////////////////////////////////////
/*
* - build as DLL to show how C++\CLI client can use native code channel
* - MockChannel reads from sendQ and writes to recvQ
*
*
* Maintenance History:
* --------------------
* ver 1.0 : May.4 2017
*/

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <string>
using Message = std::string;

struct ISendr
{
  virtual void postMessage(const Message& msg) = 0;
};

struct IRecvr
{
  virtual std::string getMessage() = 0;
};

struct IMockChannel
{
public:
  virtual void start() = 0;
  virtual void stop() = 0;
};


struct IClient
{
public:
	virtual void setClient() = 0;
	virtual void setSender(ISendr*) = 0;
	virtual void allCategories(std::string filename,std::string category) = 0;
	virtual void reCodeAnalysis() = 0;
	virtual void reAllCategory() = 0;
	virtual void reHTML()=0;
	virtual void reFileInCategory(std::string category) = 0;
	virtual void reFileWithNP(std::string category) = 0;
	virtual void downLoad(std::string filename, std::string category) = 0;
	virtual void reDelete(std::string filename, std::string category) = 0;
	virtual void demo() = 0;

};

extern "C" {
  struct ObjectFactory
  {
    DLL_DECL ISendr* createSendr();
    DLL_DECL IRecvr* createRecvr();
	DLL_DECL IClient* createClient();
    DLL_DECL IMockChannel* createMockChannel(ISendr* pISendr, IRecvr* pIRecvr,IClient* pIClient);
  };
}

#endif


