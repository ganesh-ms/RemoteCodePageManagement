#pragma once
/////////////////////////////////////////////////////////////////////
// Display.h     :  It is responsible to Display the converted     //
//                  HTML files in the Default Browser using        //
//                  child process                                  //
// ver			     : 1.1                                             //
// Author		     : Ganesh Mamatha Sheshappa                        //
// Language:     : Visual C++, Visual Studio 2017                  //
// Platform		   : Mac running Windows 10                          //
// Application   : Prototype for CSE687 - OOD Project-2            //
// Source		     : Ammar Salman, InstructorCode for Prj-1,         // 
//                 Syracuse University                             //
//                 313/788-4694, assalman@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <unordered_map>

class Display
{
public:
  Display();
  void display(const std::string& file);
  void display(const std::vector<std::string>& files);
};

