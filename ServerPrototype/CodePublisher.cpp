///////////////////////////////////////////////////////////////////////////
// Executive.cpp : defines facade/executive for OOD Pr1 S19          //
// ver 1.0                                                               //
//                                                                       // 
// Application   : OOD-S19 Instructor Solution                           //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Author        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
///////////////////////////////////////////////////////////////////////////

#include "Executive.h"
#include "../Logger/Logger.h"

using namespace Utilities;
using namespace Logging; 
using namespace FileSystem;

//ProcessCmdLine::Usage customUsage();
//
//// -----< process cmd to get info >------------------------------------
//bool Executive::processCommandLineArgs(int argc, char ** argv)
//{
//  pcl_ = new ProcessCmdLine(argc, argv); 
//  pcl_->usage(customUsage());
//
//  preface("Command Line: ");
//  pcl_->showCmdLine();
//
//  if (pcl_->parseError())
//  {
//    pcl_->usage();
//    std::cout << "\n\n";
//    return false;
//  }
//  dirIn_ = pcl_->path();
//  return true;
//}
//
//// -----< return input directory - from PCL >----------------------------
//const std::string & Executive::workingDirectory() const
//{
//  return dirIn_;
//}
//
//// -----< set output directory >-----------------------------------------
//void Executive::outputDirectory(const std::string & dir)
//{
//  dirOut_ = dir; 
//  cconv_.outputDir(dir);
//}
//
//// -----< return output directory >--------------------------------------
//const std::string & Executive::outputDirectory() const
//{
//  return dirOut_;
//}
//
//// -----< extract files - after processing cmd >-------------------------
//bool Executive::extractFiles()
//{
//  DirExplorerN de(pcl_->path());
//
//  for (auto patt : pcl_->patterns())
//  {
//    de.addPattern(patt);
//  }
//
//  if (pcl_->hasOption("s"))
//  {
//    de.recurse();
//  }
//
//  bool res = de.search() && de.match_regexes(pcl_->regexes());
//  de.showStats();
//
//  files_ = de.filesList();
//
//  return res;
//}
//
//
//// -----< publish - files extracted from directory explorer >---------------
//void Executive::publish()
//{
//  display_.display(cconv_.convert(files_));
//}
//
//// -----< publish - single file >-------------------------------------------
//void Executive::publish(const std::string & file)
//{
//  display_.display(cconv_.convert(file));
//}
//
//// -----< publish - must provide list >-------------------------------------
//void Executive::publish(const std::vector<std::string>& files)
//{
//  display_.display(cconv_.convert(files));
//}
//
//// -----< gets display mode as set by PCL >---------------------------------
//Utilities::DisplayMode Executive::displayMode() const
//{
//  return pcl_->displayMode();
//}
//
//
//// -----< command line usage >----------------------------------------------
//ProcessCmdLine::Usage customUsage()
//{
//  std::string usage;
//  usage += "\n  Command Line: path [/option]* [/pattern]* [/regex]*";
//  usage += "\n    path is relative or absolute path where processing begins";
//  usage += "\n    [/option]* are one or more options of the form:";
//  usage += "\n      /s     - walk directory recursively";
//  usage += "\n      /demo  - run in demonstration mode (cannot coexist with /debug)";
//  usage += "\n      /debug - run in debug mode (cannot coexist with /demo)";
//  //usage += "\n      /h - hide empty directories";
//  //usage += "\n      /a - on stopping, show all files in current directory";
//  usage += "\n    [pattern]* are one or more pattern strings of the form:";
//  usage += "\n      *.h *.cpp *.cs *.txt or *.*";
//  usage += "\n    [regex] regular expression(s), i.e. [A-B](.*)";
//  usage += "\n";
//  return usage;
//}
//
//// -----< initialize loggers in proper display mode >------------------------
//void initializeLoggers(DisplayMode dm) {
//  if (dm == DisplayMode::Debug) {
//    LoggerDebug::attach(&std::cout);
//    LoggerDebug::start();
//  }
//
//  else if (dm == DisplayMode::Demo) {
//    LoggerDemo::attach(&std::cout);
//    LoggerDemo::start();
//  }
//}
//
//// -----< stop loggers >-----------------------------------------------------
//void stopLoggers() {
//  LoggerDemo::stop();
//  LoggerDebug::stop();
//}
//
//
//
//#ifdef DEMO_CP
//
//#include <iostream>
//
//// -----< demonstrate requirement 3 >----------------------------------------------
//void demoReq3() {
//  LoggerDemo::write("\n");
//  LoggerDemo::title("Demonstrating Requirement #3 - Packages");
//  LoggerDemo::write("\n  Converter & Display packages are available. Executive was replaced with ");
//  LoggerDemo::write("\n  Executive which also acts as facade for the entire project.\n");
//}
//
//
//// -----< demonstrate requirements 4 and 5 >---------------------------------------
//int demoReq45(Executive& cp, int argc, char ** argv) {
//  LoggerDemo::write("\n");
//  LoggerDemo::title("Demonstrating Requirements #4 & #5 - command line arguments");
//  LoggerDemo::write(customUsage());
//
//  if (argc < 2) return 1; // even before processing, if no path, just reject it
//
//  LoggerDemo::write("\n  Changing cmd path to ../debug to test Requirement 5\n");
//
//  // cmd has the path always as first arg... we so change it to find no files
//  char * tmp = new char[sizeof(argv[1])];
//  strcpy_s(tmp, sizeof(tmp), argv[1]);
//  argv[1] = new char[9];
//  strcpy_s(argv[1], 9, "../debug"); // no files will be found matching this
//  
//  if (!cp.processCommandLineArgs(argc, argv)) return 1; // just in case
//
//  if (!cp.extractFiles()) {
//    LoggerDemo::write("\n\n  No files found in the current directory. We will ");
//    LoggerDemo::write("\n  continue since we deliberately changed argv[1] to ");
//    LoggerDemo::write("\n  test Requirement 5. After restoring argv[1], if there ");
//    LoggerDemo::write("\n  are no files to process, the program will terminate. \n");
//  }
//
//  LoggerDemo::write("\n\n  Changing back cmd path to original to continue demonstartion.\n");
//  delete argv[1];
//  argv[1] = tmp;
//
//  if (!cp.processCommandLineArgs(argc, argv)) return 1; // just in case
//  if (!cp.extractFiles()) { // if this fails, then there are no files to process at all
//    return 2; // the demo will end after this
//  }
//
//  return 0;
//}
//
//// -----< demonstrate requirements 6 and 7 >------------------------------
//void demoReq8(Executive& cp) {
//  LoggerDemo::write("\n");
//  LoggerDemo::title("Demonstrating Requirements #6 & #7 - Convert and Display");
//  cp.publish();
//}
//
//
//// -----< demonstrate requirement 8 >-------------------------------------
//void demoReq8() {
//  LoggerDemo::write("\n");
//  LoggerDemo::title("Demonstrating Requirement #8 - Automated Test Unit");
//  LoggerDemo::write("\n  Well, this is the automated test unit.");
//  LoggerDemo::write("\n\n  All Requirements met. ");
//
//}
//
//
//// -----< DEMONSTRATION MAIN >---------------------------------------------
//int main(int argc, char ** argv) {
//  // the following processing of cmd is not official, it's only
//  // done to allow for demo/debug modes BEFORE Executive does
//  // its own processing of cmds..
//
//  initializeLoggers(Utilities::DisplayMode::Debug); // by default go debug
//  for (int i = 0; i < argc; i++) {
//    std::string arg = argv[i];
//    if (arg == "/demo") { // only go demo once told 
//      stopLoggers();
//      initializeLoggers(Utilities::DisplayMode::Demo);
//      break;
//    }
//  }
//
//  demoReq3();
//  
//  Executive cp;
//
//  int err = demoReq45(cp, argc, argv);
//  if (err == 1) {
//    LoggerDemo::write("\n  Invalid command line args.\n  Ending demonstration...\n");
//    return 1;
//  }
//  else if (err == 2) {
//    LoggerDemo::write("\n  The given path has no files matching patterns and/or regexes.");
//    LoggerDemo::write("\n  Ending demonstration...\n");
//    return 2;
//  }
//  
//  demoReq8(cp);
//  demoReq8();
//
//  stopLoggers();
//  return 0;
//}
//
//#endif

#ifdef USE_CP

#include <iostream>

// -----< NORMAL USE MAIN > ------------------------------------------
int main(int argc, char ** argv) {
  Executive cp;
  if (!cp.processCommandLineArgs(argc, argv)) {
    std::cout << "\n  Failed to process command line arguments. Terminating\n\n";
    return 1;
  }

  initializeLoggers(cp.displayMode());

  if (!cp.extractFiles()) {
    stopLoggers();
    std::cout << "\n  Found no files to process. Terminating.\n\n";
    return 0; 
  }

  cp.publish();
  stopLoggers();
  return 0;
}

#endif