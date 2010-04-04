/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   Copyright (C) 2006-2010 by Jim Pattee <jimp03@email.com>
 *   Copyright (C) 1998-2002 by Tal Davidson
 *   <http://www.gnu.org/licenses/lgpl-3.0.html>
 *
 *   This file is a part of Artistic Style - an indentation and
 *   reformatting tool for C, C++, C# and Java source files.
 *   <http://astyle.sourceforge.net>
 *
 *   Artistic Style is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Artistic Style is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with Artistic Style.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#ifndef ASTYLE_MAIN_H
#define ASTYLE_MAIN_H

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

#include <ctime>
#include "astyle.h"

#if defined(_MSC_VER) || defined(__DMC__)
#include <sys/utime.h>
#include <sys/stat.h>
#else
#include <utime.h>
#include <sys/stat.h>
#endif                         // end compiler checks

#ifdef ASTYLE_JNI
#include <jni.h>
#ifndef ASTYLE_LIB    // ASTYLE_LIB must be defined for ASTYLE_JNI
#define ASTYLE_LIB
#endif
#endif  //  ASTYLE_JNI

// for G++ implementation of string.compare:
#if defined(__GNUC__) && __GNUC__ < 3
#error - Use GNU C compiler release 3 or higher
#endif

// for namespace problem in version 5.0
#if defined(_MSC_VER) && _MSC_VER < 1200        // check for V6.0
#error - Use Microsoft compiler version 6 or higher
#endif

//using namespace astyle;

namespace astyle
{

// return values for ASConsole
enum processReturn { CONTINUE, END_SUCCESS, END_FAILURE };

//----------------------------------------------------------------------------
// ASStreamIterator class
// typename will be istringstream for GUI and istream otherwise
// ASSourceIterator is an abstract class defined in astyle.h
//----------------------------------------------------------------------------

template<typename T>
class ASStreamIterator : public ASSourceIterator
{
	public:
		bool checkForEmptyLine;

		// function declarations
		ASStreamIterator(T *in);
		virtual ~ASStreamIterator();
		bool getLineEndChange(int lineEndFormat) const;
		string nextLine(bool emptyLineWasDeleted);
		string peekNextLine();
		void peekReset();
		void saveLastInputLine();

	private:
		T * inStream;          // pointer to the input stream
		string buffer;         // current input line
		string prevBuffer;     // previous input line
		int eolWindows;        // number of Windows line endings, CRLF
		int eolLinux;          // number of Linux line endings, LF
		int eolMacOld;         // number of old Mac line endings. CR
		char outputEOL[4];     // next output end of line char
		streamoff peekStart;   // starting position for peekNextLine
		bool prevLineDeleted;  // the previous input line was deleted

	public:	// inline functions
		bool compareToInputBuffer(const string &nextLine) const
		{ return (nextLine == prevBuffer); }
		const char* getOutputEOL() const { return outputEOL; }
		bool hasMoreLines() const { return !inStream->eof(); }
};

//----------------------------------------------------------------------------
// ASConsole class for console build
//----------------------------------------------------------------------------

class ASConsole
{
	private:	// variables
		// command line options
		bool isRecursive;                   // recursive option
		string origSuffix;                  // suffix= option
		bool noBackup;                      // suffix=none option
		bool preserveDate;                  // preserve-date option
		bool isVerbose;                     // verbose option
		bool isQuiet;                       // quiet option
		bool isFormattedOnly;               // formatted lines only option
		bool optionsFileRequired;           // options= option
		// other variables
		bool hasWildcard;                   // file name includes a wildcard
		size_t mainDirectoryLength;         // directory length to be excluded in displays
		bool filesAreIdentical;				// input and output files are identical
		bool lineEndsMixed;					// outputhas mixed line ends
		int  linesOut;                      // number of output lines
		int  filesFormatted;                // number of files formatted
		int  filesUnchanged;                // number of files unchanged
		char outputEOL[4];					// current line end
		char prevEOL[4];					// previous line end

		string optionsFileName;             // file path and name of the options file to use
		string targetDirectory;             // path to the directory being processed
		string targetFilename;              // file name being processed

		vector<string> excludeVector;       // exclude from wildcard hits
		vector<bool>   excludeHitsVector;   // exclude flags for eror reporting
		vector<string> fileNameVector;      // file paths and names from the command line
		vector<string> optionsVector;       // options from the command line
		vector<string> fileOptionsVector;   // options from the options file
		vector<string> fileName;            // files to be processed including path

	public:
		ASConsole() {
			// command line options
			isRecursive = false;
			origSuffix = ".orig";
			noBackup = false;
			preserveDate = false;
			isVerbose = false;
			isQuiet = false;
			isFormattedOnly = false;
			optionsFileRequired = false;
			// other variables
			hasWildcard = false;
			filesAreIdentical = true;
			lineEndsMixed = false;
			outputEOL[0] = '\0';
			prevEOL[0] = '\0';
			mainDirectoryLength = 0;
			filesFormatted = 0;
			filesUnchanged = 0;
			linesOut = 0;
		}

		// functions
		void convertLineEnds(ostringstream& out, int lineEnd);
		void error(const char *why, const char* what) const;
		void formatCinToCout(ASFormatter& formatter) const;
		FileEncoding getFileEncoding(ifstream& in) const;
		bool fileNameVectorIsEmpty();
		int  getFilesFormatted();
		int  getFilesUnchanged();
		bool getIsFormattedOnly();
		bool getIsQuiet();
		bool getIsRecursive();
		bool getIsVerbose();
		bool getLineEndsMixed();
		bool getNoBackup();
		string getOptionsFileName();
		bool getOptionsFileRequired();
		string getOrigSuffix();
		bool getPreserveDate();
		void processFiles(ASFormatter &formatter);
		processReturn processOptions(int argc, char** argv, ASFormatter &formatter);
		void setIsFormattedOnly(bool state);
		void setIsQuiet(bool state);
		void setIsRecursive(bool state);
		void setIsVerbose(bool state);
		void setNoBackup(bool state);
		void setOptionsFileName(string name);
		void setOptionsFileRequired(bool state);
		void setOrigSuffix(string suffix);
		void setPreserveDate(bool state);
		void standardizePath(string &path, bool removeBeginningSeparator=false) const;
		bool stringEndsWith(const string &str, const string &suffix) const;
		void updateExcludeVector(string suffixParam);

		// for unit testing
		vector<string> getExcludeVector();
		vector<bool>   getExcludeHitsVector();
		vector<string> getFileNameVector();
		vector<string> getOptionsVector();
		vector<string> getFileOptionsVector();
		vector<string> getFileName();

	private:
		void correctMixedLineEnds(ostringstream& out);
		void formatFile(const string &fileName, ASFormatter &formatter);
		string getCurrentDirectory(const string &fileName) const;
		void getFileNames(const string &directory, const string &wildcard);
		void getFilePaths(string &filePath);
		void initializeOutputEOL(LineEndFormat lineEndFormat);
		bool isPathExclued(const string &subPath);
		void printBadEncoding(FileEncoding encoding) const;
		void printHelp() const;
		void printMsg(const string &msg) const;
		void printVerboseHeader() const;
		void printVerboseStats(clock_t startTime) const;
		void removeFile(const char* fileName, const char* errMsg) const;
		void renameFile(const char* oldFileName, const char* newFileName, const char* errMsg) const;
		void setOutputEOL(LineEndFormat lineEndFormat, const char* currentEOL);
		void sleep(int seconds) const;
		int  waitForRemove(const char* oldFileName) const;
		int  wildcmp(const char *wild, const char *data) const;
		void writeOutputFile(const string &fileName, ostringstream &out) const;
};


//----------------------------------------------------------------------------
// global function declarations
// used by both console and library builds
//----------------------------------------------------------------------------

void importOptions(istream &in, vector<string> &optionsVector);
void isOptionError(const string &arg, const string &errorInfo);
bool isParamOption(const string &arg, const char *option);
bool isParamOption(const string &arg, const char *option1, const char *option2);
bool parseOption(astyle::ASFormatter &formatter, const string &arg, const string &errorInfo);

template<typename ITER>
bool parseOptions(astyle::ASFormatter &formatter, const ITER &optionsBegin,
                  const ITER &optionsEnd, const string &errorInfo);

}   // end of namespace astyle

//----------------------------------------------------------------------------
// declarations for java native interface (JNI) build
// global because they are called externally and are NOT part of the namespace
//----------------------------------------------------------------------------

#ifdef ASTYLE_JNI
void  STDCALL javaErrorHandler(int errorNumber, char* errorMessage);
char* STDCALL javaMemoryAlloc(unsigned long memoryNeeded);
// the following function names are constructed from method names in the calling java program
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleGetVersion(JNIEnv* env, jclass);
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleMain
(JNIEnv* env, jobject obj, jstring textInJava, jstring optionsJava);
#endif //  ASTYLE_JNI


#endif // closes ASTYLE_MAIN_H
