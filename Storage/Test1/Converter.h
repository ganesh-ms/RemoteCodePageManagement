#pragma once
/////////////////////////////////////////////////////////////////////
// Converter.h : Converts the input source code file to its HTML   //
//                 equivalent file. In addition to this this       //
//                 package provides with three other buttons for   //
//                 hiding and displaying the functions classes and //
//                 comments                                        //
// ver		    	 : 1.1                                             //
// Author		     : Ganesh Mamatha Sheshappa                        //
// Language:     : Visual C++, Visual Studio 2017                  //
// Platform		   : Mac running Windows 10                          //
// Application   : Prototype for CSE687 - OOD Project-2            //
// Source		     : Ammar Salman, Syracuse University               //
//                 313/788-4694, assalman@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "../DependencyTable/DependencyTable.h"
#include <vector>
#include <string>
#include <fstream>
#include <map>

class CodeConverter
{
public:
  CodeConverter();
  CodeConverter(const DependencyTable& dt);
  const std::string& outputDir() const;
  void outputDir(const std::string& dir);
  bool createOutpurDir();
  void setDepTable(const DependencyTable& dt);
  void handleComments(std::string&);
  void handleMultilineComment(std::string&);
  void handleClassAndFun(std::string&, size_t count,
  std::vector < std::vector<std::string>>);
  std::vector<std::string> convert();
  std::string convert(const std::string& filepath);
  std::vector<std::string> convert(const std::vector<std::string>& files);
  const std::vector<std::string> convertedFiles() const;
  void setDictOfFileAndScope(std::map<std::string, std::string>);
  void clear();

private:
  bool convertFile(std::string file);
  void addPreCodeHTML(const std::string& title);
  void addPreTag();
  void addVisibilityButtons();
  void addDependencyLinks(std::string file);
  void addClosingTags();
  void skipSpecialChars(std::string& line);
  void setPrivateDictOfFileAndScope(std::map<std::string, std::string> receivedDictionary);
  static std::map<std::string, std::string> privateDictFileAndScope;

private:
  DependencyTable dt_;
  bool inMultilineComments = false;
  std::string outputDir_ = "..\\..\\..\\..\\ConvertedWebpages\\";
  std::vector<std::string> convertedFiles_;
  std::ofstream out_;
};

