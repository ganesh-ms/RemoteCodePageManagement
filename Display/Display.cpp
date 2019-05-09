/////////////////////////////////////////////////////////////////////
// Display.cpp   :  It is responsible to Display the converted     //
//                  HTML files in the Default Browser using        //
//                  child process                                  //
// ver			 : 1.1                                             //
// Author		 : Ganesh Mamatha Sheshappa                        //
// Language:     : Visual C++, Visual Studio 2017                  //
// Platform		 : Mac running Windows 10                          //
// Application   : Prototype for CSE687 - OOD Project-2            //
// Source		 : Ammar Salman, InstructorCode for Prj-1,         // 
//                 Syracuse University                             //
//                 313/788-4694, assalman@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines Display class which accepts a list of files as a
*  vector<string> and uses the default internet browser to display them
*  one by one. Please note that the functionality has limiations:
*   1) Opera/MS Edge: will pop-up all tabs instantly.
*   2) Chrome/Firefox: will pop-up windows separately only if no
*      already existing Chrome/Firefox window is opened (all must be
*      closed before running this).
*
*
*  Required Files:
* =======================
*  Display.h, Display.cpp 
*  Process.h, Process.cpp
*  Logger.h, Logger.cpp
*  FileSystem.h, FileSystem.cpp
 *
 * Maintenance History
 * -------------------
 * ver 1.1 : 05 Mar 2019 (Second Release)
 * - Modified the code to display the converted files one by one 
     in the default browser
	 Modified the code with the 2 Loggers (Demo and Debug Mode)
 * ver 1.0 : 05 Feb 2019
 * - first release
 */

#include "Display.h"
#include "../Process/Process.h"
#include "../Logger/Logger.h"
#include "../FileSystem/FileSystem.h"

using namespace Logging;

// -----< default ctor >--------------------------------------------------
Display::Display() { }

// -----< display single file >-------------------------------------------
void Display::display(const std::string & file)
{
  LoggerDebug::write("\n  Displaying file: " + FileSystem::Path::getName(file) + " in browser");
  LoggerDemo::write("\n  Displaying file: " + FileSystem::Path::getName(file) + " in browser");

  CBP callback = []() {
    LoggerDebug::write("\n  --- child browser exited with this message ---");
    LoggerDemo::write("\n  --- child browser exited with this message ---");
  };

  Process p;
  p.application("C:\\Windows\\System32\\cmd.exe");
  std::string cmd = "/C " + FileSystem::Path::getFullFileSpec(file);

  p.commandLine(cmd);
  p.create();
  p.setCallBackProcessing(callback);
  p.registerCallback();
  WaitForSingleObject(p.getProcessHandle(), INFINITE);
}

// -----< display multiple files  >---------------------------------------
void Display::display(const std::vector<std::string>& files)
{
  for (auto file : files) 
    display(file);
}

#ifdef TEST_DISPLAY

#include <iostream>

int main() {
	// -----< Attach the files stream function to the Logger >-----------------------------------
	LoggerDebug::attach(&std::cout);

	// -----< Start the Logger in Debug mode>-----------------------------------
	LoggerDebug::start();

	std::vector<std::string> files;

	// -----< Adding converted files to the file string variable>-----------------------------------
	files.push_back("..\\ConvertedWebpages\\Converter.h.html");
	files.push_back("..\\ConvertedWebpages\\Converter.cpp.html");

	Display d;

	// -----< Displaying the converted files in the default browser>-----------------------------------
	d.display(files);

	// -----< Stop the Logger in Debug mode>-----------------------------------
	LoggerDebug::stop();
}

#endif