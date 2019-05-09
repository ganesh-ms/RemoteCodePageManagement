/////////////////////////////////////////////////////////////////////
// Converter.cpp : Converts the input source code file to its HTML //
//                 equivalent file. In addition to this this       //
//                 package provides with three other buttons for   //
//                 hiding and displaying the functions classes and //
//                 comments                                        //
// ver			     : 1.1                                             //
// Author		     : Ganesh Mamatha Sheshappa                        //
// Language:     : Visual C++, Visual Studio 2017                  //
// Platform		   : Mac running Windows 10                          //
// Application   : Prototype for CSE687 - OOD Project-2            //
// Source		     : Ammar Salman, Syracuse University               //
//                 313/788-4694, assalman@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 *  Converts the input source code file to its HTML 
 *  equivalent file. In addition to this this      
 *  package provides with three other buttons for  
 *  hiding and displaying the functions classes and
 *  comments
 *
 * Required Files:
 * ---------------
 * FileSystem.cpp, FileSystem.h
 * Logger.cpp, Logger.h
 * DependencyTable.cpp, DependencyTable.h

 * Converter.h
 *
 * Maintenance History
 * -------------------
 * ver 1.1 : 05 Mar 2019 (Second Release)
 * - Modification of the project to convert the file by accepting the
	 file name, or file list or the dependency table.
	 Modified the code to add the Styling formates
	 Integrated the Logger
 * ver 1.0 : 05 Feb 2019
 * - first release
 */
#include "Converter.h"
#include "../Logger/Logger.h"
#include "../FileSystem/FileSystem.h"

using namespace FileSystem;
using namespace Logging;

// -----< default ctor >--------------------------------------------------
CodeConverter::CodeConverter()
{
}

// -----< ctor to set dep table >-----------------------------------------
CodeConverter::CodeConverter(const DependencyTable & dt) : dt_(dt)
{
  LoggerDebug::write("\n  CodeConverter initialized with DependencyTable\n");
}

// -----< set dependency table function >---------------------------------
void CodeConverter::setDepTable(const DependencyTable & dt)
{
  dt_ = dt;
}

// -----< function to handle the Single Line Comments Section >--------------------
void CodeConverter::handleComments(std::string& line)
{
	size_t pos = line.find('/');
	if (pos == line.npos) return;
	if (pos == line.size() - 1) return;
	if (line[pos + 1] == '/')
	{
		line.replace(pos, 1, "<div class=\"comments\">/");
		line.append("</div>");
	}
}

// -----< function to handle the Multi Line Comments Section >--------------------
void CodeConverter::handleMultilineComment(std::string & line)
{
	if (inMultilineComments)
	{
		size_t pos = line.find('*');
		if (pos == line.npos) return;
		if (pos == line.size() - 1) return;
		if (line[pos + 1] == '/')
		{
			line.replace(pos+1, 1, "/</div>");
			inMultilineComments = false;
		}
	}
	else
	{
		size_t pos = line.find('/');
		if (pos == line.npos) return;
		if (pos == line.size() - 1) return;
		if (line[pos + 1] == '*')
		{
			line.replace(pos, 1, "<div class=\"comments\">/");
			inMultilineComments = true;
		}
	}

}

// -----< function to handle the Classes and Functions >--------------------
void CodeConverter::handleClassAndFun(std::string & line, size_t count,
	std::vector < std::vector<std::string>> parserDetailsIn2D)
{
	for (size_t i = 0; i < parserDetailsIn2D.size(); i++)
	{
		if (count == atoi(parserDetailsIn2D[i][2].c_str()) + atoi(parserDetailsIn2D[i][1].c_str()) - 2)
		{
			if (parserDetailsIn2D[i][0] == "function" || parserDetailsIn2D[i][0] == "class")
			{
				line.append(" </div>");
				break;
			}
		}
		else if (count == atoi(parserDetailsIn2D[i][1].c_str()))
		{
			if (parserDetailsIn2D[i][0] == "function" || parserDetailsIn2D[i][0] == "class")
			{
				line.append(" <div class =\"" + parserDetailsIn2D[i][0] + "\">");
				break;
			}
		}
	}
}

// -----< convert pre-set dependency table >------------------------------
/* returns the list of converted files*/
std::vector<std::string> CodeConverter::convert()
{
  if (!createOutpurDir()) {
    LoggerDebug::write("\n\n  Failed to start conversion due to inability to create output directory");
    LoggerDebug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
    return std::vector<std::string>();
  }

  LoggerDemo::write("\n\n  Converting files to webpages");
  LoggerDebug::write("\n\n  Converting files to webpages");

  LoggerDebug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
  LoggerDemo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));

  for (auto entry : dt_) {
    if (convertFile(entry.first)) {
      LoggerDemo::write("\n  -- Converted: ");
      LoggerDebug::write("\n  -- Converted: ");
    }
    else {
      LoggerDemo::write("\n  -- Failed:    ");
      LoggerDebug::write("\n  -- Failed:    ");
    }

    std::string filename = Path::getName(entry.first);
    LoggerDebug::write(filename);
    LoggerDemo::write(filename);
  }
  LoggerDebug::write("\n");
  LoggerDemo::write("\n");
  return convertedFiles_;
}

// -----< convert single file given path >----------------------------------
/* returns the name of the converted file*/
std::string CodeConverter::convert(const std::string & filepath)
{
  if (!createOutpurDir()) {
    LoggerDebug::write("\n\n  Failed to start conversion due to inability to create output directory");
    LoggerDebug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
    return "";
  }

  LoggerDemo::write("\n\n  Converting files to webpages");
  LoggerDebug::write("\n\n  Converting files to webpages");

  LoggerDebug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
  LoggerDemo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));

  if (convertFile(filepath)) {
    LoggerDemo::write("\n  -- Converted: ");
    LoggerDebug::write("\n  -- Converted: ");
  }
  else {
    LoggerDemo::write("\n  -- Failed:    ");
    LoggerDebug::write("\n  -- Failed:    ");
  }

  std::string filename = Path::getName(filepath);
  LoggerDebug::write(filename);
  LoggerDemo::write(filename);

  return filename + ".html";
}

// -----< convert single file given path >----------------------------------
/* returns the list of the converted files*/
std::vector<std::string> CodeConverter::convert(const std::vector<std::string>& files)
{
  clear();
  dt_ = DependencyTable(files);
  return convert();
}

// -----< get output directory >--------------------------------------------
/* returns the output file path*/
const std::string & CodeConverter::outputDir() const
{
  return outputDir_;
}

// -----< set output directory >--------------------------------------------
void CodeConverter::outputDir(const std::string & dir)
{
  outputDir_ = dir;
}

// -----< create output directory >-----------------------------------------
/* returns success of creation. it could fail if dir is sys protected */
bool CodeConverter::createOutpurDir()
{
  if (!Directory::exists(outputDir_)) {
    LoggerDebug::write("\n  Creating output directory. Path: " + Path::getFullFileSpec(outputDir_));
    return Directory::create(outputDir_);
  }
  LoggerDebug::write("\n  Output directory already exists. Proceeding...");
  return true;
}

// -----< get list of converted files >-------------------------------------
/* returns the list of converted files*/
const std::vector<std::string> CodeConverter::convertedFiles() const
{
  return convertedFiles_;
}

// -----< Set the Static Dicationary variable which store the parser information >-------
void CodeConverter::setDictOfFileAndScope(std::map<std::string, std::string> receivedDictionary)
{
	setPrivateDictOfFileAndScope(receivedDictionary);
}

// -----< Syntax to initialize the static variable >-------------------------------------
std::map<std::string, std::string> CodeConverter::privateDictFileAndScope;

// -----< Set the Static Dicationary variable which store the parser information >-------
void CodeConverter::setPrivateDictOfFileAndScope(std::map<std::string, std::string> receivedDictionary)
{
	privateDictFileAndScope = receivedDictionary;
}

// -----< Clears the list of the converted files variable >-------------------------------------
void CodeConverter::clear()
{
  convertedFiles_.clear();
  dt_.clear();
}

// -----< Split funciton to split the list when the delimiter is hit >--------------------
/*Returns the Vector of string which holdes the strings after split function*/
std::vector<std::string> split(std::string str, std::string token) {
	std::vector<std::string>result;
	while (str.size()) {
		int index = str.find(token);
		if (index != std::string::npos) {
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
			if (str.size() == 0)result.push_back(str);
		}
		else {
			result.push_back(str);
			str = "";
		}
	}
	return result;
}

// -----< Splits the individual string from the vector of strings with the delimiter >--------------------
/*Returns the Vector of Vector of strings which holdes the strings after split function*/
std::vector<std::vector<std::string>> create2Dvector(std::vector<std::string> vecStr, std::string token = ",") {
	std::vector<std::vector<std::string>> result;
	for (size_t i = 0; i < vecStr.size(); i++)
	{
		result.push_back(split(vecStr[i], token));
	}
	return result;
}

// -----< Inplace deletion of the unnecessary strings in the input string>--------------------
void erase(std::string &str)
{
	while (true)
	{
		if (str[0] == '\n')
			str.erase(0, 1);
		else if (str[0] == ' ')
			str.erase(0, 1);
		else if (str[str.size() - 1] == ' ')
			str.erase(str.size() - 1);
		else if (str[str.size() - 1] == '"')
			str.erase(str.size() - 1);
		else if (str[str.size() - 1] == ')')
			str.erase(str.size() - 1);
		else
			break;
	}
	str.erase(0, 1);
}

// -----< private - read file and create webpage >--------------------------
bool CodeConverter::convertFile(std::string file)
{
  std::ifstream in(file);
  if (!in.is_open() || !in.good()) {
    LoggerDebug::write("Error -- unable to read file, path may be invalid.");
    in.close();
    return false;
  }
  std::string filename = Path::getName(file);
  std::string outputPath = outputDir_ + filename + ".html";
  out_.open(outputPath, std::ofstream::out);
  if (!out_.is_open() || !out_.good()) {
    LoggerDebug::write("Error -- unable to open output file for writing.");
    in.close();
    return false;
  }
  std::string str;
  auto it = privateDictFileAndScope.find(file); //don't pass fruit.c_str()
  if (it != privateDictFileAndScope.end())
	  str = it->second;
  else
	  std::cout << ( "File not found in the map") << std::endl;

  std::vector<std::string> parserDetails;
  parserDetails = split(str, ")\n");
  for (size_t i = 0; i < parserDetails.size(); i++)
	  erase(parserDetails[i]);
  std::vector < std::vector<std::string>> parserDetailsIn2D = create2Dvector(parserDetails);
  addPreCodeHTML(filename);
  addDependencyLinks(file);
  addPreTag();
  addVisibilityButtons();
  size_t count = 0;
  while (in.good()){
	  std::string line;
	  while (std::getline(in, line)){
		  count++;
		  skipSpecialChars(line);
		  handleComments(line);
		  handleMultilineComment(line);
		  handleClassAndFun(line, count, parserDetailsIn2D);
		  out_ << line << std::endl;
	  }
  }
  addClosingTags();
  out_.close();
  convertedFiles_.push_back(outputPath);
  return true;
}

// -----< private - add generic HTML preliminary markup >-------------------
void CodeConverter::addPreCodeHTML(const std::string& title)
{
  out_ << "<DOCTYPE !HTML>" << std::endl;
  out_ << "<html>" << std::endl;
  out_ << "  <head>" << std::endl;
  out_ << "    <Title>" << title << "</Title>" << std::endl;
  out_ << "    <style>" << std::endl;
  out_ << "      body {" << std::endl;
  out_ << "        padding:15px 40px;" << std::endl;
  out_ << "        font-family: Consolas;" << std::endl;
  out_ << "        font-size: 1.25em;" << std::endl;
  out_ << "        font-weight: normal;" << std::endl;
  out_ << "      }" << std::endl;
  out_ << "      #buttons{" << std::endl;
  out_ << "        margin-left: 75%;" << std::endl;
  out_ << "        position: fixed;" << std::endl;
  out_ << "        }" << std::endl;
  out_ << "        .nav-button{" << std::endl;
  out_ << "        display: inline-block;" << std::endl;
  out_ << "        border: 2px solid black;" << std::endl;
  out_ << "        padding: 5px;" << std::endl;
  out_ << "        background-color: white;" << std::endl;
  out_ << "        color: black;" << std::endl;
  out_ << "        border: 2px solid #008CBA;" << std::endl;
  out_ << "        text-align: center;" << std::endl;
  out_ << "        margin-bottom: 3%;" << std::endl;
  out_ << "        }" << std::endl;
  out_ << "      .nav-button:hover{" << std::endl;
  out_ << "        display: inline-block;" << std::endl;
  out_ << "        border: 2px solid black;" << std::endl;
  out_ << "        padding: 5px;" << std::endl;
  out_ << "        background-color: #008CBA;" << std::endl;
  out_ << "        color: white;" << std::endl;
  out_ << "        text-align: center;" << std::endl;
  out_ << "        cursor: pointer;" << std::endl;
  out_ << "        }" << std::endl;
  out_ << "      .comments{" << std::endl;
  out_ << "        color: #ff6868;" << std::endl;
  out_ << "        }" << std::endl;
  out_ << "      .function{" << std::endl;
  out_ << "        color: #4150af;" << std::endl;
  out_ << "        }" << std::endl;
  out_ << "      </style>" << std::endl;
  out_ << "      <script src=\"../script.js\"></script>" << std::endl;
  out_ << "  </head>" << std::endl << std::endl;
  out_ << "  <body>" << std::endl;
}

// -----< private - add pre tag >------------------------------------------
/* seperated into seperate function to allow for dependencies addition
*  before the actual code of the file */
void CodeConverter::addPreTag()
{
  out_ << "    <pre>" << std::endl;
}

// -----< private - add markup for the toggling buttons >-------------------
void CodeConverter::addVisibilityButtons()
{
	out_ << "<div id = \"buttons\">" << std::endl;
	out_ << "	<button class = \"nav-button\" onclick = \"toggleVisibility('comments');\" >Hide / Show Comments</button>" << std::endl;
	out_ << "	<button class = \"nav-button\" onclick = \"toggleVisibility('function');\">Hide / Show Functions</button> "<< std::endl;
	out_ << "	<button class = \"nav-button\" onclick = \"toggleVisibility('class');\">Hide / Show Classes</button> "<< std::endl;
	out_ << "</div> "<< std::endl;
}

// -----< private - add depedency links markup code >----------------------
void CodeConverter::addDependencyLinks(std::string file)
{
  std::string filename = Path::getName(file);
  if (!dt_.has(file)) { // in case of single file conversion
    LoggerDebug::write("\n    No entry found in DependencyTable for [" + filename + "]. Skipping dependency links..");
    return;
  }

  if (dt_[file].size() == 0) { // in case the file has no dependencies
    LoggerDebug::write("\n    No dependencies found for [" + filename + "]. Skipping dependency links..");
    return;
  }

  out_ << "    <h3>Dependencies: " << std::endl;
  for (auto dep : dt_[file]) {
    out_ << "      <a href=\"" << Path::getName(dep) << ".html\">" << Path::getName(dep) << "</a>" << std::endl;
  }
  out_ << "    </h3>";
}

// -----< private - add generic HTML markup closing tags >-----------------
void CodeConverter::addClosingTags()
{
  out_ << "    </pre>" << std::endl;
  out_ << "  </body>" << std::endl;
  out_ << "</html>" << std::endl;
}

// -----< private - replace HTML special chars >---------------------------
/* note: the function uses while loop to make sure ALL special characters
*  are replaced instead of just the first encounter. */
void CodeConverter::skipSpecialChars(std::string & line)
{
  size_t pos = line.npos;
  while((pos = line.find('<')) != line.npos) 
    line.replace(pos, 1, "&lt;");

  while ((pos = line.find('>')) != line.npos)
    line.replace(pos, 1, "&gt;");
}


#ifdef TEST_CONVERTER

// -----< test stub for converter class >-----------------------------------
int main() {

	// -----< Attach the files stream function to the Logger >-----------------------------------
	LoggerDebug::attach(&std::cout);

	// -----< Start the Logger in Debug mode>-----------------------------------
	LoggerDebug::start();

	// -----< Set the Title to display in the Logger Debug mode>-----------------------------------
	LoggerDebug::title("Testing CodeConverter functions");

	LoggerDebug::stop(); // do not log DependencyTable logs

	// -----< Create the dependency table object>-----------------------------------
	DependencyTable dt;

	// -----< Adding the dependency of the files>-----------------------------------
	dt.addDependency("Converter.h", "Converter.cpp");
	dt.addDependency("Converter.cpp", "Converter.h");
	dt.addDependency("A.h", "A.h");

	LoggerDebug::start(); // log from now on

	CodeConverter cc(dt);

	// -----< Display the Dependency on the screen>-----------------------------------
	dt.display();
	LoggerDebug::write("\n");

	// -----< Convert the files already in the files list>-----------------------------------
	cc.convert();

	// -----< Stop the logger >-----------------------------------
	LoggerDebug::stop();
	return 0;
}

#endif