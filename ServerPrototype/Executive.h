#pragma once
/////////////////////////////////////////////////////////////////////
// Executive.h   : It is responsible to invoke all the functions   //
//                 in different project and display all the        //  
//                 requirements                                    //
// ver			     : 1.2                                             //
// Author		     : Ganesh Mamatha Sheshappa                        //
// Language:     : Visual C++, Visual Studio 2017                  //
// Platform		   : Mac running Windows 10                          //
// Application   : Prototype for CSE687 - OOD Project-3            //
// Source		     : Ammar Salman, InstructorCode for Prj-1,         // 
//                 Syracuse University                             //
//                 313/788-4694, assalman@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include "../Converter/Converter.h"
#include <algorithm>
#include "../Display/Display.h"
#include "../DirExplorer-Naive/DirExplorerN.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../ScopeStack/ScopeStack.h"
#include <map>

using namespace CodeAnalysis;
using namespace std;
namespace ExecutiveNS
{
  class Executive
  {
  public:
    bool setArgument(int argc, char** argv);
    void publishFiles();
    std::vector<std::string> getConvertedFilesList();
    void showDemo();
    void showFourthPrjDemo();

    bool processCommandLineArgs(int argc, char ** argv);
    const std::string& workingDirectory() const;
    void outputDirectory(const std::string& dir);
    const std::string& outputDirectory() const;
    bool extractFiles();
    std::vector<std::string> convertToHTML();
    std::string convertToHTML(const std::string& file);
    vector<string> convertToHTML(const vector<string>&);
    void displayHTML();
    std::vector<std::string> parseExtractFiles();
    void addToConvertedFiles(std::vector<std::string>);
    void setOutputFilePath();
    Utilities::DisplayMode displayMode() const;
    bool addDependencyFilesToDT(ASTNode*, std::string, DependencyTable&, std::vector<std::string>);
    void sendDictToConverter(map<string, string>, CodeConverter&);
    void demoReq1();
    void demoReq2();
    void demoReq3();
    void demoReq4();
    void demoReq5();
    void demoReq6();
    void demoReq7();
    void demoReq1Prj4();
    void demoReq2Prj4();
    void demoReq3Prj4();
    void demoReq4Prj4();
    void demoReq5Prj4();
    void demoReq6Prj4();
    void demoReq7Prj4();
    void demoReq8Prj4();
    std::vector<std::string> demoReq8();
  private:
    Utilities::ProcessCmdLine *pcl_;
    CodeConverter cconv_;
    Display display_;
    std::string dirIn_;
    std::string dirOut_;
    std::vector<std::string> filterdFiles;
    std::vector<std::string> files_;
    std::vector<std::string> convertedFiles;
    std::vector<std::string> convertedHTMLFiles;
    map<string, string> dictFileAndScope;
  };
}
