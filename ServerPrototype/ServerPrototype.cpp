////////////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp : It is the server package runs the code publisher on  //   
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
/*
*  Package Operations:
* ---------------------
*  Package contains one class, Server, that contains a Message-Passing Communication
*  facility. It processes each message by invoking an installed callable object
*  defined by the message's command key.
*
*  Message handling runs on a child thread, so the Server main thread is free to do
*  any necessary background processing (none, so far).
*
*  Required Files:
* -----------------
*  AbstrSynTree.h, AbstrSynTree.cpp
*  CodeUtilities.h, CodeUtilities.cpp
*  Converter.h, Converter.cpp
*  Display.cpp, Display.h
*  Executive.h
 * CodeUtilities.cpp, CodeUtilities.h
 * Utilities.cpp, Utilities.h
 * Toker.cpp, Toker.h
 * Semi.cpp, Semi.h
 * DirExplorerN.cpp, DirExplorerN.h
 * FileSystem.cpp, FileSystem.h
 * StringUtilities.cpp, StringUtilities.h
 * ActionsAndRules.cpp, ActionsAndRules.h
 * AbstrSynTree.cpp, AbstrSynTree.cpp
 * Logger.cpp, Logger.h
 * Parcer.cpp, Parcer.h
 * ScopeStack.cpp, ScopeStack.h
 * Executive.h, IExecutive.h
*  ServerPrototype.h, ServerPrototype.cpp
*  Comm.h, Comm.cpp, IComm.h
*  Message.h, Message.cpp
*  FileSystem.h, FileSystem.cpp
*  Utilities.h
*
*  Maintenance History:
* ----------------------
*  ver 1.1 : 04/30/2019
*  - Added Publish function and the Send converted function to the Client folder
     Added functions to display the requirements on the console
*  ver 1.0 : 3/27/2018
*  - first release
*/
#include <iostream>   // std::cout

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>
#include <iostream>
#include "../Utilities/Utilities/Utilities.h"
#include "../Tokenizer/Toker.h"
#include "../SemiExpression/Semi.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"
#include "../Converter/Converter.h"
#include "../Logger/Logger.h"
#include <filesystem>
#include <direct.h>
#include "../FileSystem/FileSystem.h"
#include <queue>
#include <string>

using namespace Lexer;
using namespace Utilities;
using namespace Logging;  
using namespace FileSystem;
using namespace CodeAnalysis;
using namespace ExecutiveNS;
using Demo = Logging::StaticLogger<1>;
#define Util StringHelper
ProcessCmdLine::Usage customUsage();
namespace MsgPassComm = MsgPassingCommunication;
namespace fs = std::filesystem;

using namespace Repository;
namespace repo = Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;


//----< returns the files from the given path >----------------
Files Server::getFiles(const repo::SearchPath& path)
{
  return Directory::getFiles(path);
}

//----< publishes the files based on the input arguments >----------------
std::vector<std::string> Server::publishFiles(std::string arg)
{
  std::vector<std::string> conFilesList;
  std::vector<std::string> argusValue;
  size_t pos = 0;
  int argsCount = 0;
  std::string token;
  while ((pos = arg.find(' ')) != std::string::npos) {
    token = arg.substr(0, pos);
    std::cout << token << std::endl;
    argusValue.push_back(token);
    arg.erase(0, pos + 1);
    argsCount++;
  }
  char ** argv = new char*[argsCount];
  for (int i = 0; i < argsCount; i++)
  {
    std::string arg = argusValue[i];
    argv[i] = new char[argusValue[i].length() + 1];
    strcpy_s(argv[i], argusValue[i].length() + 1, arg.c_str());
  }
  fs::path serverForderPath = fs::path("..") / argv[1];
  serverForderPath = fs::canonical(serverForderPath);
  std::string serverFolPath = serverForderPath.string();
  argv[1] = new char[serverFolPath.length() + 1];
  strcpy_s(argv[1], serverFolPath.length() + 1, serverFolPath.c_str());
  Executive exe_;
  exe_.setArgument(argsCount, argv);
  exe_.publishFiles();
  conFilesList = exe_.getConvertedFilesList();
  return conFilesList;
  delete[] argv;
}

//----< publishes the files based on the input arguments >----------------
std::function<Msg(Msg)> publishFiles = [](Msg msg)
{
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("publishFiles");
  std::string args = msg.value("args");
  std::vector<std::string> conFiles;
  conFiles = Server::publishFiles(args);
  size_t count = 0;
  for (auto item : conFiles)
  {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("conFile" + countStr, item);
  }
  return reply;
};

//----< sends the files to the client location >----------------
std::function<Msg(Msg)> getConFiles = [](Msg msg)
{
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getConFiles");
  reply.file(msg.attributes()["conFileName"]);
  std::string conFileName = msg.value("conFileName");
  reply.attribute("sentConFile", conFileName);
  return reply;
};

//----< sends the directory list based on the input path >----------------
Dirs Server::getDirs(const repo::SearchPath& path)
{
  return Directory::getDirectories(path);
}

template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

//----< function to echo the message >----------------
std::function<Msg(Msg)> echo = [](Msg msg) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

//----< function to get the files in the given path>----------------
std::function<Msg(Msg)> getFiles = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};

//----< function to get the directory in the given path>----------------
std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Dirs dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};

//Start of Executive functions 
// -----< initialize loggers in proper display mode >------------------------
void initializeLoggers(DisplayMode dm) {
  if (dm == DisplayMode::Debug) {
    LoggerDebug::attach(&std::cout);
    LoggerDebug::start();
  }

  else if (dm == DisplayMode::Demo) {
    LoggerDemo::attach(&std::cout);
    LoggerDemo::start();
  }
}

// -----< terminate loggers in proper display mode >------------------------
void terminateLoggers(DisplayMode dm) {
  if (dm == DisplayMode::Debug) {
    LoggerDebug::detach(&std::cout);
    LoggerDebug::detach(&std::cout);
  }

  else if (dm == DisplayMode::Demo) {
    LoggerDemo::detach(&std::cout);
    LoggerDemo::detach(&std::cout);
  }
}


// -----< Shows all the requirement on the Console window >------------------------
void Executive::showFourthPrjDemo()
{
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::start();
  this->demoReq1Prj4();
  this->demoReq2Prj4();
  this->demoReq3Prj4();
  this->demoReq4Prj4();
  this->demoReq5Prj4();
  this->demoReq6Prj4();
  this->demoReq7Prj4();
  this->demoReq8Prj4();
  LoggerDemo::stop();
  terminateLoggers(Utilities::DisplayMode::Demo);
}

// -----< Shows all the requirement on the Console window >------------------------
void Executive::showDemo()
{
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::start();
  this->demoReq1();
  this->demoReq2();
  this->demoReq3();
  this->demoReq4();
  this->demoReq5();
  this->demoReq6();
  this->demoReq7();
  LoggerDemo::stop();
  terminateLoggers(Utilities::DisplayMode::Demo);
}

// -----< Sets the argument which is necessary for the Executive package >-------------------
bool Executive::setArgument(int argc, char ** argv)
{
  if (!this->processCommandLineArgs(argc, argv)) {
    std::cout << "\n  Failed to process command line arguments. Terminating\n\n";
    this->parseExtractFiles();
    return 0;
  }
  else
  {
    this->parseExtractFiles();
    this->extractFiles();
    return 1;
  }
}

// -----< Publishes the input files to their corresponding HTML files >-------------------
void Executive::publishFiles()
{
  this->parseExtractFiles();
  this->demoReq8();
}

// -----< Returns the converted files list >-------------------
std::vector<std::string> Executive::getConvertedFilesList()
{
  std::vector<std::string> convertedFiles = files_;
  return convertedFiles;
}

// -----< process cmd line arguments to get info >------------------------------------
bool Executive::processCommandLineArgs(int argc, char ** argv)
{
  pcl_ = new ProcessCmdLine(argc, argv);
  pcl_->usage(customUsage());

  preface("Command Line: ");
  pcl_->showCmdLine();

  if (pcl_->parseError())
  {
    pcl_->usage();
    std::cout << "\n\n";
    return false;
  }
  dirIn_ = pcl_->path();
  return true;
}

// -----< return input directory - from PCL >----------------------------
const std::string & Executive::workingDirectory() const
{
  return dirIn_;
}

// -----< set output directory >-----------------------------------------
void Executive::outputDirectory(const std::string & dir)
{
  dirOut_ = dir;
  cconv_.outputDir(dir);
}

// -----< return output directory >--------------------------------------
const std::string & Executive::outputDirectory() const
{
  return dirOut_;
}

// -----< extract files - after processing cmd >-------------------------
bool Executive::extractFiles()
{
  DirExplorerN de(pcl_->path());

  for (auto patt : pcl_->patterns())
  {
    de.addPattern(patt);
  }

  if (pcl_->hasOption("s"))
  {
    de.recurse();
  }

  bool res = de.search() && de.match_regexes(pcl_->regexes());
  de.showStats();

  files_ = de.filesList();

  return res;
}

// -----< publish - files extracted from directory explorer >---------------
std::vector<std::string> Executive::convertToHTML()
{
  std::vector<std::string> convertedFiles_ = convertedFiles;
  std::vector<std::string> _files_ = files_;
  auto pred = [&convertedFiles_](const std::string& key) ->bool
  {
    return std::find(convertedFiles_.begin(), convertedFiles_.end(), key) != convertedFiles_.end();
  };
  _files_.erase(std::remove_if(_files_.begin(), _files_.end(), pred), _files_.end());

  return cconv_.convert(_files_);
}

// -----< publish - single file >-------------------------------------------
std::string Executive::convertToHTML(const std::string & file)
{
  return cconv_.convert(file);
}

// -----< publish - must provide list >-------------------------------------
std::vector<std::string> Executive::convertToHTML(const std::vector<std::string>& files)
{
  return cconv_.convert(files);
}

// -----< publish - must provide list >-------------------------------------
void Executive::displayHTML()
{
  std::string path = "../../../../ConvertedWebpages/";
  std::vector<std::string> conFiles;
  for (const auto & entry : std::filesystem::directory_iterator(path))
  {
    conFiles.push_back(entry.path().string());
  }
  display_.display(conFiles);
}


// -----< gets display mode as set by PCL >---------------------------------
Utilities::DisplayMode Executive::displayMode() const
{
  return pcl_->displayMode();
}

// -----< command line usage >----------------------------------------------
ProcessCmdLine::Usage customUsage()
{
  std::string usage;
  usage += "\n  Command Line: path [/option]* [/pattern]* [/regex]*";
  usage += "\n    path is relative or absolute path where processing begins";
  usage += "\n    [/option]* are one or more options of the form:";
  usage += "\n      /s     - walk directory recursively";
  usage += "\n      /demo  - run in demonstration mode (cannot coexist with /debug)";
  usage += "\n      /debug - run in debug mode (cannot coexist with /demo)";
  //usage += "\n      /h - hide empty directories";
  //usage += "\n      /a - on stopping, show all files in current directory";
  usage += "\n    [pattern]* are one or more pattern strings of the form:";
  usage += "\n      *.h *.cpp *.cs *.txt or *.*";
  usage += "\n    [regex] regular expression(s), i.e. [A-B](.*)";
  usage += "\n";
  return usage;
}

// -----< stop loggers >-----------------------------------------------------
void stopLoggers() {
  LoggerDemo::stop();
  LoggerDebug::stop();
}

// -----< Returns the directory in the input path >-----------------------------------------------------
std::string get_directories(const std::string& s)
{
  std::string r;
  for (auto& p : std::filesystem::directory_iterator(s))
    if (p.status().type() == std::filesystem::file_type::directory)
      r += p.path().string() + "\n";
  return r;
}

// -----< Returns the files in the input path >-----------------------------------------------------
std::string get_files(const std::string& s)
{
  std::string r;
  for (auto& p : std::filesystem::directory_iterator(s))
    if (p.status().type() == std::filesystem::file_type::regular)
      r += p.path().string() + "\n";
  return r;
}

// -----< Writes the selected set of lines from the file on to the consolve  >------------------
std::string printLinesFromFile(std::string ipFileName, int from, int to)
{
  std::ifstream ipFl(ipFileName);
  int k = 1;
  std::string outputFileText;
  outputFileText += "\nFile Displayed: " + ipFileName.substr(ipFileName.find_last_of("/\\") + 1) + "\n";
  for (std::string line; std::getline(ipFl, line);)
  {
    if (k >= from && k <= to)
    {
      outputFileText += to_string(k) + line + "\n";
    }
    k++;
  }
  return outputFileText;
}

#include <windows.h>

// -----< demonstrate requirement 1 >----------------------------------------------
void Executive::demoReq1Prj4() {
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #1 ");
  LoggerDemo::write("\n Use Visual Studio 2017 and its C++ Windows Console Projects");
  LoggerDemo::write("\n  Demonstrating by viewing the solution (.sln) file in project directory hierarchy\n");
  LoggerDemo::write(get_files("../../../../"));
}

// -----< demonstrate requirement 2 >----------------------------------------------
void Executive::demoReq2Prj4() {
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #2 ");
  LoggerDemo::write("\n Windows Presentation Foundation (WPF) for the Client's user display");
  LoggerDemo::write("\n  Demonstrating by viewing the GUI\n");
}

// -----< demonstrate requirement 3 >----------------------------------------------
void Executive::demoReq3Prj4() {
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #3 ");
  LoggerDemo::write("\n Build a asynchronous Message passing communication channel and integrate prj1,prj2,prj3");
  LoggerDemo::write("\n  Demonstrating by viewing the GUI output\n");
}


// -----< demonstrate requirement 4 >----------------------------------------------
void Executive::demoReq4Prj4() {
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #4 ");
  LoggerDemo::write("\n Graphical User Interface (GUI) for the client that supports navigating remote directories ");
  LoggerDemo::write("\n  Demonstrating by viewing the GUI output\n");
}

// -----< demonstrate requirement 5 >----------------------------------------------
void Executive::demoReq5Prj4() {
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #5 ");
  LoggerDemo::write("\n Provided message designs appropriate for this application. All messages are instances ");
  LoggerDemo::write("\n  of the same Message class, but have a specified set of attributes and body contents suited for the intended task. \n");
  LoggerDemo::write("\n  Demonstrating by viewing the project in the project file\n");
}

// -----< demonstrate requirement 6 >----------------------------------------------
void Executive::demoReq6Prj4() {
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #6 ");
  LoggerDemo::write("\n Supports converting source code in the server and, with a separate request, transferring one or more converted ");
  LoggerDemo::write("\n  files back to the local client, using the communication channel.\n");
  LoggerDemo::write("\n  Demonstrating by the output of the GUI\n");
}

// -----< demonstrate requirement 7 >----------------------------------------------
void Executive::demoReq7Prj4() {
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #7 ");
  LoggerDemo::write("\n Demonstrate correct operations for two or more concurrent clients.");
  LoggerDemo::write("\n  Demonstrating by the output of the GUI with two GUI windows\n");
}

// -----< demonstrate requirement 8 >----------------------------------------------
void Executive::demoReq8Prj4() {
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #8 ");
  LoggerDemo::write("\n Automatied test unit");
  LoggerDemo::write("\n  Demonstrating by giving /demo as the argument, Demonstration provided by default run.bat file\n");
}



// -----< demonstrate requirement 1 >----------------------------------------------
void Executive::demoReq1() {
  initializeLoggers(Utilities::DisplayMode::Demo);
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #1 ");
  LoggerDemo::write("\n Use Visual Studio 2017 and its C++ Windows Console Projects");
  LoggerDemo::write("\n  Demonstrating by viewing the solution (.sln) file in project directory hierarchy\n");
  LoggerDemo::write(get_files("../../../../"));
}


// -----< demonstrate requirement 2 >----------------------------------------------
void Executive::demoReq2() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #2 ");
  LoggerDemo::write("\n Windows Presentation Foundation (WPF) and C# to implement the Client GUI\n");
  LoggerDemo::write("\n The project uses WPF and C# implementation - can be verified by viewing the GUI");
}

// -----< demonstrate requirement 2 >----------------------------------------------
void demoReq2() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #2 -  see line 63 and 185 respectively ");
  LoggerDemo::write("\n Use the standard streams libraries for all I/O\n");
  LoggerDemo::write(printLinesFromFile("../Utilities/Utilities.h", 62, 66));
  LoggerDemo::write(printLinesFromFile("../Parser/ActionsAndRules.h", 185, 188));
}

// -----< demonstrate requirement 3 >----------------------------------------------
void Executive::demoReq3() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #3 - see lines #42 to #64");
  LoggerDemo::write("\n  Shall provide an interface for the Executive developed in Project #2");
  LoggerDemo::write("\n  Demonstrating by viewing the code in  IExecutive.cpp\n");
  LoggerDemo::write(printLinesFromFile("../../../../Project2Starter/IExecutive.h", 41, 64));
}

// -----< demonstrate requirement 3 >----------------------------------------------
void demoReq3() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #3 - see line 311");
  LoggerDemo::write("\n  The Executive package accepts command line, a path containing files to be converted");
  LoggerDemo::write("\n  Demonstrating by viewing the code in  Executive.cpp\n");
  LoggerDemo::write(printLinesFromFile("../Project2Starter/Executive.cpp", 299, 312));
}


// -----< demonstrate requirement 4 >----------------------------------------------
void demoReq4() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #4 ");
  LoggerDemo::write("\n Provides Executive / Loader (Project2Starter), Converter, Dependencies, and Display packages.");
  LoggerDemo::write("\n  Demonstrating by viewing files in project directory hierarchy\n");
  LoggerDemo::write(get_directories("../"));
}

// -----< demonstrate requirement 4 >----------------------------------------------
void Executive::demoReq4() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #4 ");
  LoggerDemo::write("\n Shall build the Publisher as a dynamic link library ");
  LoggerDemo::write("\n  Demonstrating by viewing the dll file in the Debug folder \n");
  //LoggerDemo::write(get_files("../../../../Debug/"));
}

// -----< demonstrate requirement 5 >----------------------------------------------
void Executive::demoReq5() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #5");
  LoggerDemo::write("\n Shall provide a C++\\CLI translater that delegates calls from the Client GUI \n");
  LoggerDemo::write("\n Demonstrating Requirement #5 -  see line 16 and 29 respectively \n");
  LoggerDemo::write(printLinesFromFile("../../../../Shim/Shim.h", 16, 29));
}

// -----< demonstrate requirement 6 >----------------------------------------------
void Executive::demoReq6() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #6 ");
  LoggerDemo::write("\n Shall provide a Client package that implements a WPF GUI with a least two views\n");
  LoggerDemo::write("\n Demonstrating by viewing the GUI");
}

// -----< demonstrate requirement 7 >----------------------------------------------
void Executive::demoReq7() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #7 ");
  LoggerDemo::write("\n Shall include an Automated test unit (ATU) \n");
  LoggerDemo::write("\n DDemonstrating by viewing the code -  see line 94 and 105 respectively");
  LoggerDemo::write(printLinesFromFile("../../../../PrototypeGui-OOD-Pr3/MainWindow.xaml.cs", 94, 105));

  LoggerDemo::write("\n\n All Requirements are met! \n");
  LoggerDemo::write("-----------------------------");
}




// -----< demonstrate requirements 5 >---------------------------------------
int demoReq5(Executive& cp, int argc, char ** argv) {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirements #5 ");
  LoggerDemo::write("Support finding and loading all the files for conversion, starting at a path specified by the Executive.");
  LoggerDemo::write(customUsage());
  if (argc < 2) return 1; // even before processing, if no path, just reject it
  LoggerDemo::write("\n  Changing cmd path to ../debug to test Requirement 5\n");
  // cmd has the path always as first arg... we so change it to find no files
  char * tmp = new char[sizeof(argv[1])];
  strcpy_s(tmp, sizeof(tmp), argv[1]);
  argv[1] = new char[9];
  strcpy_s(argv[1], 9, "../debug"); // no files will be found matching this
  if (!cp.processCommandLineArgs(argc, argv)) return 1; // just in case

  if (!cp.extractFiles()) {
    LoggerDemo::write("\n\n  No files found in the current directory. We will ");
    LoggerDemo::write("\n  continue since we deliberately changed argv[1] to ");
    LoggerDemo::write("\n  test Requirement 5. After restoring argv[1], if there ");
    LoggerDemo::write("\n  are no files to process, the program will terminate. \n");
  }

  LoggerDemo::write("\n\n  Changing back cmd path to original to continue demonstartion.\n");
  delete argv[1];
  argv[1] = tmp;

  if (!cp.processCommandLineArgs(argc, argv)) return 1; // just in case
  if (!cp.extractFiles()) { // if this fails, then there are no files to process at all
    return 2; // the demo will end after this
  }

  return 0;
}

// -----< demonstrate requirement 6 >----------------------------------------------
void demoReq6() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #6 ");
  LoggerDemo::write("\n Translating the \"<\" and \">\" characters into their corresponding escape sequences.");
  LoggerDemo::write("\n  Demonstrating by viewing the code in  Converter.cpp\n");
  LoggerDemo::write(printLinesFromFile("../Converter/Converter.cpp", 463, 471));
}

// -----< demonstrate requirement 7 >----------------------------------------------
void demoReq7() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #7 ");
  LoggerDemo::write("\n Provide links to all the files in the directory tree, on which it depends");
  LoggerDemo::write("\n  Demonstrating by viewing the Converter HTML files\n");
}

// -----< demonstrate requirements 8 >------------------------------
std::vector<std::string> Executive::demoReq8() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirements #8 - Convert and Display");
  return this->convertToHTML();
}

// -----< demonstrate requirement 9 >-------------------------------------
void demoReq9() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #9 - Automated Test Unit");
  LoggerDemo::write("\n All the above Requirements are displayed from automated test unit.");
  LoggerDemo::write("\n\n  All Requirements met. ");
}

// -----< splits the string with the delimiter provided as argument >-----------
std::vector<std::string> split(const std::string& str, const std::string& delim)
{
  std::vector<std::string> tokens;
  size_t prev = 0, pos = 0;
  do
  {
    pos = str.find(delim, prev);
    if (pos == std::string::npos) pos = str.length();
    std::string token = str.substr(prev, pos - prev);
    if (!token.empty()) tokens.push_back(token);
    prev = pos + delim.length();
  } while (pos < str.length() && prev < str.length());
  return tokens;
}

// -----< combines two path and returns the combined path >-----------
std::string getCombinedPath(std::string pathFirst, std::string pathSecond)
{
  std::string combinedPath;
  std::vector<std::string> individualDirsInFirstPath = split(pathFirst, "\\");
  individualDirsInFirstPath.pop_back();
  while (pathSecond[0] == '.' && pathSecond[1] == '.')
  {
    individualDirsInFirstPath.pop_back();
    pathSecond.erase(pathSecond.begin(), pathSecond.begin() + 3);
  }
  std::vector<std::string> individualDirsInSecondPath = split(pathSecond, "/");
  for (auto indDir : individualDirsInFirstPath)
    combinedPath.append(indDir + '\\');
  for (auto indDir : individualDirsInSecondPath)
    combinedPath.append(indDir + '\\');
  combinedPath.pop_back();
  return combinedPath;
}

// -----< adds the dependency files to the Dependency table >-----------
bool Executive::addDependencyFilesToDT(ASTNode *pGlobalScope, std::string fileSpec, DependencyTable &dt, std::vector<std::string> files)
{
  auto iter2 = pGlobalScope->statements_.begin();
  std::vector<std::string> dependencyFiles;
  std::string dependencyFileName;
  bool addedDependency = false;
  while (iter2 != pGlobalScope->statements_.end()) {
    //std::cout << "\n " << std::string(2 * 4, ' ') << (*iter2)->ToString();
    dependencyFiles = split((*iter2)->ToString(), " ");
    if (dependencyFiles.size() > 2)
    {
      if (dependencyFiles[2][0] == '"' && dependencyFiles[2].size() > 2)
      {
        dependencyFileName = dependencyFiles[2].substr(1, dependencyFiles[2].size() - 2);
        dependencyFileName = getCombinedPath(fileSpec, dependencyFileName);
        if (std::find(files.begin(), files.end(), dependencyFileName) != files.end())
        {
          std::cout << "Dependency files : " << dependencyFileName << std::endl;
          dt.addDependency(fileSpec, dependencyFileName);
          addedDependency = true;
        }
      }
    }
    ++iter2;
  }
  return addedDependency;
}

// -----< sends the dictionary with the parser info to the converter package >-----------
void Executive::sendDictToConverter(map<string, string> dictFileAndScope_, CodeConverter& CC)
{
  CC.setDictOfFileAndScope(dictFileAndScope_);
}

// -----< Extracts the parser information and determines the dependency >-----------
std::vector<std::string> Executive::parseExtractFiles() {
  std::string parserInfo;
  Util::Title("Testing Project2Starter");
  putline();
  DependencyTable dt;
  Executive exec;
  std::vector<std::string> convertedDTFiles;
  std::string fileSpec;
  for (size_t i = 0; i < files_.size(); ++i) {
    fileSpec = FileSystem::Path::getFullFileSpec(files_[i]);
    std::string msg = "Processing file" + fileSpec;
    Util::title(msg);
    ConfigParseForCodeAnal configure;
    Parser* pParser = configure.Build();
    std::string name;
    try {
      if (pParser) {
        name = FileSystem::Path::getName(files_[i]);
        if (!configure.Attach(fileSpec)) {
          std::cout << "\n  could not open file " << name << std::endl;
          continue;
        }
      }
      else
        std::cout << "\n\n  Parser not built\n\n";
      CodeAnalysis::Repository* pRepo = CodeAnalysis::Repository::getInstance();
      pRepo->package() = name;
      while (pParser->next()) {
        pParser->parse();
      }
      ASTNode* pGlobalScope = pRepo->getGlobalScope();
      if (exec.addDependencyFilesToDT(pGlobalScope, fileSpec, dt, files_))
        convertedDTFiles.push_back(fileSpec);
      complexityEval(pGlobalScope);
      parserInfo = "";
      TreeWalkReturnStr(pGlobalScope, parserInfo);
      dictFileAndScope.insert(make_pair(fileSpec, parserInfo));
    }
    catch (std::exception& ex) {
      std::cout << "\n\n    " << ex.what() << "\\n\nn  exception caught at line " << __LINE__ << " ";
      std::cout << "\n  in package \"" << name << "\"";
    }
    std::cout << "\n";
  }
  LoggerDebug::start(); // log from now on
  CodeConverter cc(dt);
  exec.sendDictToConverter(dictFileAndScope, cc);
  dt.display();
  std::vector<std::string>tempConverter = cc.convert();
  convertedHTMLFiles.insert(convertedHTMLFiles.end(), tempConverter.begin(), tempConverter.end());
  convertedFiles.insert(convertedFiles.end(), convertedDTFiles.begin(), convertedDTFiles.end());
  LoggerDebug::stop();
  std::cout << "\n";
  return convertedFiles;
}

// -----< adds the newly converted file list to the present converted files list>-----------
void Executive::addToConvertedFiles(std::vector<std::string> newFiles)
{
  convertedFiles.insert(convertedFiles.end(), newFiles.begin(), newFiles.end());
}

// -----< gets the output path deletes and create the output directory>-----------
void Executive::setOutputFilePath()
{
  std::string outputPath = cconv_.outputDir();
  std::filesystem::remove_all(outputPath);
  _mkdir(outputPath.c_str());
}

// -----< set the mode to demo or debug>-----------
void setDemoOrDebug(int argCount, char **argValue)
{
  initializeLoggers(Utilities::DisplayMode::Debug); // by default go debug
  for (int i = 0; i < argCount; i++) {
    std::string arg = argValue[i];
    if (arg == "/demo") { // only go demo once told 
      stopLoggers();
      initializeLoggers(Utilities::DisplayMode::Demo);
      break;
    }
  }
  LoggerDemo::write("Demonstrating Project #2 - Source Code Publisher For Projects Requirements");
  LoggerDemo::write("--------------------------------------------------------------------------");
}

int main(int argc, char** argv)
{
  Executive ep; ep.showFourthPrjDemo();
  try{
    if (argc > 1){
      size_t pos = 0;
      std::string serverURL = argv[1];
      pos = serverURL.find(':');
      MsgPassingCommunication::EndPoint::Address address = serverURL.substr(0, pos);
      serverURL.erase(0, pos + 1);
      MsgPassingCommunication::EndPoint::Port serverPort = stoi(serverURL.substr(0, serverURL.find(':')));
      MsgPassingCommunication::EndPoint SEP(address, serverPort);
      serverEndPoint = SEP;}
    std::cout << "\n  Testing Server Prototype\n ==========================\n";
    Server server(serverEndPoint, "ServerPrototype");
    server.start();std::cout << "\n  testing getFiles and getDirs methods\n --------------------------------------";
    Files files = server.getFiles();
    show(files, "Files:");Dirs dirs = server.getDirs();
    show(dirs, "Dirs:");
    std::cout << "\n\n  testing message processing\n ----------------------------";
    server.addMsgProc("echo", echo);
    server.addMsgProc("getFiles", getFiles);
    server.addMsgProc("getDirs", getDirs);
    server.addMsgProc("serverQuit", echo);
    server.addMsgProc("publishFiles", publishFiles);
    server.addMsgProc("getConFiles", getConFiles);
    server.processMessages();
    Msg msg(serverEndPoint, serverEndPoint);  // send to self
    msg.name("msgToSelf");
    msg.command("echo");
    msg.attribute("verbose", "show me");
    server.postMessage(msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    msg.command("getFiles");
    msg.remove("verbose");
    msg.attributes()["path"] = storageRoot;
    server.postMessage(msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    msg.command("getDirs");
    msg.attributes()["path"] = storageRoot;
    server.postMessage(msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "\n  press enter to exit";
    std::cin.get();
    std::cout << "\n";
    msg.command("serverQuit");
    server.postMessage(msg);
    server.stop();
    return 0;
  }
  catch (const std::exception& error){std::cout << "std::exception caught\n\n" << std::endl << error.what();}
}

