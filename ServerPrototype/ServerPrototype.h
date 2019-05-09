#pragma once
////////////////////////////////////////////////////////////////////////////////
// ServerPrototype.h : It is the server package runs the code publisher on  //   
//                       the server                                           //
//                       It is responsible to invoke all the functions        //
//                       in different project and display all the             //  
//                       requirements                                         //
// ver			           : 1.1                                                  //
// Author		           : Ganesh Mamatha Sheshappa                             //
// Language:           : Visual C++, Visual Studio 2017                       //
// Platform		         : Mac running Windows 10                               //
// Application         : Prototype for CSE687 - OOD Project-3                 //
// Source		           : Jim Fawcett, CSE687 - Object Oriented Design,        // 
//                       Syracuse University                                  //
//                       313/788-4694, assalman@syr.edu                       //
////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include "../CppCommWithFileXfer/Message/Message.h"
#include "../CppCommWithFileXfer/MsgPassingComm/Comm.h"
#include <windows.h>
#include <tchar.h>
#include <filesystem>
#include "Executive.h"

namespace Repository
{

  //Start of Code Related to The Executive part
  //End of Code Related to The Executive part
  using File = std::string;
  using Files = std::vector<File>;
  using Dir = std::string;
  using Dirs = std::vector<Dir>;
  using SearchPath = std::string;
  using Key = std::string;
  using Msg = MsgPassingCommunication::Message;
  using ServerProc = std::function<Msg(Msg)>;
  using MsgDispatcher = std::unordered_map<Key,ServerProc>;
  const SearchPath storageRoot = "../Storage";  // root for all server file storage
  //const MsgPassingCommunication::EndPoint serverEndPoint("localhost", 8080);  // listening endpoint
  MsgPassingCommunication::EndPoint serverEndPoint;  // listening endpoint

  class Server
  {
  public:
    Server(MsgPassingCommunication::EndPoint ep, const std::string& name);
    void start();
    void stop();
    void addMsgProc(Key key, ServerProc proc);
    void processMessages();
    void postMessage(MsgPassingCommunication::Message msg);
    MsgPassingCommunication::Message getMessage();
    static Dirs getDirs(const SearchPath& path = storageRoot);
    static Files getFiles(const SearchPath& path = storageRoot);
    static std::vector<std::string> publishFiles(std::string);
  private:
    MsgPassingCommunication::Comm comm_;
    MsgDispatcher dispatcher_;
    std::thread msgProcThrd_; 
  };
  //----< initialize server endpoint and give server a name >----------

  inline Server::Server(MsgPassingCommunication::EndPoint ep, const std::string& name)
    : comm_(ep, name) {}

  //----< start server's instance of Comm >----------------------------

  inline void Server::start()
  {
    comm_.start("../ConvertedWebPages", "../ConvertedWebPages");
  }
  //----< stop Comm instance >-----------------------------------------

  inline void Server::stop()
  {
    if(msgProcThrd_.joinable())
      msgProcThrd_.join();
    comm_.stop();
  }
  //----< pass message to Comm for sending >---------------------------

  inline void Server::postMessage(MsgPassingCommunication::Message msg)
  {
    comm_.postMessage(msg);
  }
  //----< get message from Comm >--------------------------------------

  inline MsgPassingCommunication::Message Server::getMessage()
  {
    Msg msg = comm_.getMessage();
    return msg;
  }
  //----< add ServerProc callable object to server's dispatcher >------

  inline void Server::addMsgProc(Key key, ServerProc proc)
  {
    dispatcher_[key] = proc;
  }
  //----< start processing messages on child thread >------------------

  inline void Server::processMessages()
  {
    auto proc = [&]()
    {
      if (dispatcher_.size() == 0)
      {
        std::cout << "\n  no server procs to call";
        return;
      }
      while (true)
      {
        Msg msg = getMessage();
        std::cout << "\n  received message: " << msg.command() << " from " << msg.from().toString();
        if (msg.containsKey("verbose"))
        {
          std::cout << "\n";
          msg.show();
        }
        if (msg.command() == "serverQuit")
          break;
        if (dispatcher_.find(msg.command()) != dispatcher_.end())
        {
          Msg reply = dispatcher_[msg.command()](msg);
          if (msg.to().port != msg.from().port)  // avoid infinite message loop
          {
            postMessage(reply);
            msg.show();
            reply.show();
          }
          else
            std::cout << "\n  server attempting to post to self";
        }
      }
      std::cout << "\n  server message processing thread is shutting down";
    };
    std::thread t(proc);
    //SetThreadPriority(t.native_handle(), THREAD_PRIORITY_HIGHEST);
    std::cout << "\n  starting server thread to process messages";
    msgProcThrd_ = std::move(t);
  }
}