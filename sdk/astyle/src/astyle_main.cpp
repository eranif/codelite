/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   astyle_main.cpp
 *
 *   This file is a part of "Artistic Style" - an indentation and
 *   reformatting tool for C, C++, C# and Java source files.
 *   http://astyle.sourceforge.net
 *
 *   The "Artistic Style" project, including all files needed to
 *   compile it, is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later
 *   version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this project; if not, write to the
 *   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA  02110-1301, USA.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include "astyle.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

#ifndef ASTYLE_LIB             // for console build only
#if defined(_MSC_VER) || defined(__DMC__)
#include <sys/utime.h>
#include <sys/stat.h>
//#define utimbuf  _utimbuf
//#define utime    _utime
#else
#include <utime.h>
#include <sys/stat.h>
#endif                         // end compiler checks
#endif                         // end ASTYLE_LIB

// for G++ implementation of string.compare:
// compare((str), (place), (length))  instead of  compare(place, length, str)
#if defined(__GNUC__) && __GNUC__ < 3
#error - Use GNU C compiler release 3 or higher
#endif

// for namespace problem in version 5.0
#if defined(_MSC_VER) && _MSC_VER < 1200        // check for V6.0
#error - Use Microsoft compiler version 6 or higher
#endif

#ifdef _WIN32
#define STDCALL __stdcall
#define EXPORT  __declspec(dllexport)
#else
#define STDCALL
#define EXPORT
#endif

#define IS_OPTION(arg,op)          ((arg).compare(op)==0)
#define IS_OPTIONS(arg,a,b)        (IS_OPTION((arg),(a)) || IS_OPTION((arg),(b)))

#define GET_PARAM(arg,op)          ((arg).substr(strlen(op)))
#define GET_PARAMS(arg,a,b) (isParamOption((arg),(a)) ? GET_PARAM((arg),(a)) : GET_PARAM((arg),(b)))

using namespace astyle;

const char* _version = "1.21";

// some compilers want this declared
bool parseOption(ASFormatter &formatter, const string &arg, const string &errorInfo);

#ifdef ASTYLE_LIB
// GUI function pointers
typedef void (STDCALL *fpError)(int, char*);       // pointer to callback error handler
typedef char* (STDCALL *fpAlloc)(unsigned long);   // pointer to callback memory allocation
// GUI variables
stringstream *_err = NULL;
#else
// console variables
ostream *_err = &cerr;
bool _purgeOrigIn = false;
bool _preserveDate = false;
string _suffix = ".orig";
stringstream _msg;          // info messages are not printed until a file is read
#endif
bool _modeManuallySet = false;


// typename will be istringstream for GUI and istream otherwise
template<typename T>
class ASStreamIterator :
			public ASSourceIterator
{
	public:
		ASStreamIterator(T *in);
		virtual ~ASStreamIterator();
		bool hasMoreLines() const;
		string nextLine();

	private:
		T * inStream;
		string buffer;
		bool inStreamEOF;
};

template<typename T>
ASStreamIterator<T>::ASStreamIterator(T *in)
{
	inStream = in;
	buffer.reserve(200);
	inStreamEOF = false;
}


template<typename T>
ASStreamIterator<T>::~ASStreamIterator()
{
}


template<typename T>
bool ASStreamIterator<T>::hasMoreLines() const
{
	return !inStreamEOF;
}


/**
 * read the input stream, delete any end of line characters,
 *     and build a string that contains the input line.
 *
 * @return        string containing the next input line minus any end of line characters
 */
template<typename T>
string ASStreamIterator<T>::nextLine()
{
	char ch;
	char LF = '\n';
	char CR = '\r';
	inStream->get(ch);
	buffer.clear();

	while (!inStream->eof() && ch != LF && ch != CR)
	{
		buffer.append(1, ch);
		inStream->get(ch);
	}

	if (inStream->eof())
	{
		inStreamEOF = true;
		return buffer;
	}

	int peekch = inStream->peek();

	if (ch == CR)		// CR+LF is windows otherwise Mac OS 9
	{
		if (peekch == LF)
		{
			inStream->get();
			eolWindows++;
		}
		else
			eolMacOld++;
	}
	else				// LF is Linux, allow for improbable LF/CR
	{
		if (peekch == CR)
		{
			inStream->get();
			eolWindows++;
		}
		else
			eolLinux++;
	}

	// set output end of line character
	if (eolWindows >= eolLinux)
		if (eolWindows >= eolMacOld)
			strcpy(outputEOL, "\r\n");  // Windows (CR+LF)
		else
			strcpy(outputEOL, "\r");    // MacOld (CR)
	else
		if (eolLinux >= eolMacOld)
			strcpy(outputEOL, "\n");    // Linux (LF)
		else
			strcpy(outputEOL, "\r");    // MacOld (CR)

	return buffer;
}


template<typename ITER>
bool parseOptions(ASFormatter &formatter,
                  const ITER &optionsBegin,
                  const ITER &optionsEnd,
                  const string &errorInfo)
{
	ITER option;
	bool ok = true;
	string arg, subArg;

	for (option = optionsBegin; option != optionsEnd; ++option)
	{
		arg = *option;

		if (arg.compare(0, 2, "--") == 0)
			ok &= parseOption(formatter, arg.substr(2), errorInfo);
		else if (arg[0] == '-')
		{
			size_t i;

			for (i = 1; i < arg.length(); ++i)
			{
				if (isalpha(arg[i]) && i > 1)
				{
					ok &= parseOption(formatter, subArg, errorInfo);
					subArg = "";
				}
				subArg.append(1, arg[i]);
			}
			ok &= parseOption(formatter, subArg, errorInfo);
			subArg = "";
		}
		else
		{
			ok &= parseOption(formatter, arg, errorInfo);
			subArg = "";
		}
	}
	return ok;
}

void importOptions(istream &in, vector<string> &optionsVector)
{
	char ch;
	string currentToken;

	while (in)
	{
		currentToken = "";
		do
		{
			in.get(ch);
			if (in.eof())
				break;
			// treat '#' as line comments
			if (ch == '#')
				while (in)
				{
					in.get(ch);
					if (ch == '\n')
						break;
				}

			// break options on spaces, tabs or new-lines
			if (in.eof() || ch == ' ' || ch == '\t' || ch == '\n')
				break;
			else
				currentToken.append(1, ch);

		}
		while (in);

		if (currentToken.length() != 0)
			optionsVector.push_back(currentToken);
	}
}

bool isParamOption(const string &arg, const char *option)
{
	bool retVal = arg.compare(0, strlen(option), option) == 0;
	// if comparing for short option, 2nd char of arg must be numeric
	if (retVal && strlen(option) == 1 && arg.length() > 1)
		if (!isdigit(arg[1]))
			retVal = false;
	return retVal;
}

void isOptionError(const string &arg, const string &errorInfo)
{
#ifdef ASTYLE_LIB
	if (_err->str().length() == 0)
	{
		(*_err) << errorInfo << endl;	// need main error message
		(*_err) << arg;					// output the option in error
	}
	else
		(*_err) << endl << arg;			// put endl after previous option
#else
	if (errorInfo.length() > 0)			// to avoid a compiler warning
		(*_err) << "Error in param: " << arg << endl;
#endif
}

bool isParamOption(const string &arg, const char *option1, const char *option2)
{
	return isParamOption(arg, option1) || isParamOption(arg, option2);
}


bool parseOption(ASFormatter &formatter, const string &arg, const string &errorInfo)
{
	if ( IS_OPTION(arg, "style=ansi") )
	{
		formatter.setSpaceIndentation(4);
		formatter.setBracketFormatMode(BREAK_MODE);
		formatter.setBracketIndent(false);
		formatter.setClassIndent(false);
		formatter.setSwitchIndent(false);
		formatter.setNamespaceIndent(false);
	}
	else if ( IS_OPTION(arg, "style=gnu") )
	{
		formatter.setSpaceIndentation(2);
		formatter.setBracketFormatMode(BREAK_MODE);
		formatter.setBlockIndent(true);
		formatter.setClassIndent(false);
		formatter.setSwitchIndent(false);
		formatter.setNamespaceIndent(false);
	}
	else if ( IS_OPTION(arg, "style=java") )
	{
//		formatter.setJavaStyle();
//		_modeManuallySet = true;
		formatter.setSpaceIndentation(4);
		formatter.setBracketFormatMode(ATTACH_MODE);
		formatter.setBracketIndent(false);
		formatter.setSwitchIndent(false);
	}
	else if ( IS_OPTION(arg, "style=kr") )
	{
		//manuallySetCStyle(formatter);
		formatter.setSpaceIndentation(4);
		formatter.setBracketFormatMode(ATTACH_MODE);
		formatter.setBracketIndent(false);
		formatter.setClassIndent(false);
		formatter.setSwitchIndent(false);
		formatter.setNamespaceIndent(false);
	}
	else if ( IS_OPTION(arg, "style=linux") )
	{
		formatter.setSpaceIndentation(8);
		formatter.setBracketFormatMode(BDAC_MODE);
		formatter.setBracketIndent(false);
		formatter.setClassIndent(false);
		formatter.setSwitchIndent(false);
		formatter.setNamespaceIndent(false);
	}
	// must check for mode=cs before mode=c !!!
	else if ( IS_OPTION(arg, "mode=cs") )
	{
		formatter.setSharpStyle();
		_modeManuallySet = true;
	}
	else if ( IS_OPTION(arg, "mode=c") )
	{
		formatter.setCStyle();
		_modeManuallySet = true;
	}
	else if ( IS_OPTION(arg, "mode=java") )
	{
		formatter.setJavaStyle();
		_modeManuallySet = true;
	}
	else if ( isParamOption(arg, "t", "indent=tab=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "t", "indent=tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
			formatter.setTabIndentation(spaceNum, false);
	}
	else if ( isParamOption(arg, "T", "force-indent=tab=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "T", "force-indent=tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
			formatter.setTabIndentation(spaceNum, true);
	}
	else if ( IS_OPTION(arg, "indent=tab") )
	{
		formatter.setTabIndentation(4);
	}
	else if ( isParamOption(arg, "s", "indent=spaces=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "s", "indent=spaces=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
			formatter.setSpaceIndentation(spaceNum);
	}
	else if ( IS_OPTION(arg, "indent=spaces") )
	{
		formatter.setSpaceIndentation(4);
	}
	else if ( isParamOption(arg, "m", "min-conditional-indent=") )
	{
		int minIndent = 8;
		string minIndentParam = GET_PARAMS(arg, "m", "min-conditional-indent=");
		if (minIndentParam.length() > 0)
			minIndent = atoi(minIndentParam.c_str());
		if (minIndent > 40)
			isOptionError(arg, errorInfo);
		else
			formatter.setMinConditionalIndentLength(minIndent);
	}
	else if ( isParamOption(arg, "M", "max-instatement-indent=") )
	{
		int maxIndent = 40;
		string maxIndentParam = GET_PARAMS(arg, "M", "max-instatement-indent=");
		if (maxIndentParam.length() > 0)
			maxIndent = atoi(maxIndentParam.c_str());
		if (maxIndent > 80)
			isOptionError(arg, errorInfo);
		else
			formatter.setMaxInStatementIndentLength(maxIndent);
	}
	else if ( IS_OPTIONS(arg, "B", "indent-brackets") )
	{
		formatter.setBracketIndent(true);
	}
	else if ( IS_OPTIONS(arg, "G", "indent-blocks") )
	{
		formatter.setBlockIndent(true);
	}
	else if ( IS_OPTIONS(arg, "N", "indent-namespaces") )
	{
		formatter.setNamespaceIndent(true);
	}
	else if ( IS_OPTIONS(arg, "C", "indent-classes") )
	{
		formatter.setClassIndent(true);
	}
	else if ( IS_OPTIONS(arg, "S", "indent-switches") )
	{
		formatter.setSwitchIndent(true);
	}
	else if ( IS_OPTIONS(arg, "K", "indent-cases") )
	{
		formatter.setCaseIndent(true);
	}
	else if ( IS_OPTIONS(arg, "L", "indent-labels") )
	{
		formatter.setLabelIndent(true);
	}
	else if ( IS_OPTIONS(arg, "y", "brackets=break-closing") )
	{
		formatter.setBreakClosingHeaderBracketsMode(true);
	}
	else if ( IS_OPTIONS(arg, "b", "brackets=break") )
	{
		formatter.setBracketFormatMode(BREAK_MODE);
	}
	else if ( IS_OPTIONS(arg, "a", "brackets=attach") )
	{
		formatter.setBracketFormatMode(ATTACH_MODE);
	}
	else if ( IS_OPTIONS(arg, "l", "brackets=linux") )
	{
		formatter.setBracketFormatMode(BDAC_MODE);
	}
	else if ( IS_OPTIONS(arg, "O", "one-line=keep-blocks") )
	{
		formatter.setBreakOneLineBlocksMode(false);
	}
	else if ( IS_OPTIONS(arg, "o", "one-line=keep-statements") )
	{
		formatter.setSingleStatementsMode(false);
	}
	else if ( IS_OPTIONS(arg, "P", "pad=paren") )
	{
		formatter.setParensOutsidePaddingMode(true);
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "d", "pad=paren-out") )
	{
		formatter.setParensOutsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "D", "pad=paren-in") )
	{
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "U", "unpad=paren") )
	{
		formatter.setParensUnPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "p", "pad=oper") )
	{
		formatter.setOperatorPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "E", "fill-empty-lines") )
	{
		formatter.setEmptyLineFill(true);
	}
	else if ( IS_OPTIONS(arg, "w", "indent-preprocessor") )
	{
		formatter.setPreprocessorIndent(true);
	}
	else if ( IS_OPTIONS(arg, "V", "convert-tabs") )
	{
		formatter.setTabSpaceConversionMode(true);
	}
	else if ( IS_OPTIONS(arg, "F", "break-blocks=all") )
	{
		formatter.setBreakBlocksMode(true);
		formatter.setBreakClosingHeaderBlocksMode(true);
	}
	else if ( IS_OPTIONS(arg, "f", "break-blocks") )
	{
		formatter.setBreakBlocksMode(true);
	}
	else if ( IS_OPTIONS(arg, "e", "break-elseifs") )
	{
		formatter.setBreakElseIfsMode(true);
	}
#ifdef ASTYLE_LIB
	// End of options used by GUI
	else
		isOptionError(arg, errorInfo);
#else
	// Options used by only console
	else if ( IS_OPTIONS(arg, "n", "suffix=none") )
	{
		_purgeOrigIn = true;
	}
	else if ( isParamOption(arg, "suffix=") )
	{
		string suffixParam = GET_PARAM(arg, "suffix=");
		if (suffixParam.length() > 0)
		{
			_suffix = suffixParam;
//			if (_suffix[0] != '.')
//				_suffix = '.' + _suffix;
		}
	}
	else if ( IS_OPTIONS(arg, "Z", "preserve-date") )
	{
		_preserveDate =true;
	}
	else if ( IS_OPTIONS(arg, "X", "errors-to-stdout") )
	{
		_err = &cout;
	}
	else if ( IS_OPTIONS(arg, "v", "version") )
	{
		(*_err) << "Artistic Style " << _version << endl;
		exit(0);
	}
	else
	{
		(*_err) << errorInfo << arg << endl;
		return false; // unknown option
	}
#endif
// End of parseOption function
	return true; //o.k.
}


#ifdef ASTYLE_LIB
// *************************   GUI functions   *****************************************************
/*
 * IMPORTANT VC DLL linker must have the parameter  /EXPORT:AStyleMain=_AStyleMain@16
 *                                                  /EXPORT:AStyleGetVersion=_AStyleGetVersion@0
 * For Dll only - "warning C4702: unreachable code" in the <vector> header
 *                is caused by using the Optimization options.
 *                /O2   Maximize speed
 *                /O1   Minimize size
 *                /Ob2  Inline Expansion
 *                This is a bug in the Microsoft compiler.  The program runs about twice as fast
 *                with the options set.  There haven't been any problems so far.
*/
extern "C" EXPORT char* STDCALL
	AStyleMain(const char* pSourceIn,          // pointer to the source to be formatted
	           const char* pOptions,           // pointer to AStyle options, separated by \n
	           fpError fpErrorHandler,         // pointer to error handler function
	           fpAlloc fpMemoryAlloc)          // pointer to memory allocation function
{
	if (fpErrorHandler == NULL)			// cannot display a message if no error handler
		return NULL;

	if (pSourceIn == NULL)
	{
		fpErrorHandler(101, "No pointer to source input");
		return NULL;
	}
	if (pOptions == NULL)
	{
		fpErrorHandler(102, "No pointer to AStyle options");
		return NULL;
	}
	if (fpMemoryAlloc == NULL)
	{
		fpErrorHandler(103, "No pointer to memory allocation function");
		return NULL;
	}

	ASFormatter formatter;

	string arg;
	vector<string> optionsVector;
	istringstream opt(pOptions);
	_err = new stringstream;
	_modeManuallySet = false;

	importOptions(opt, optionsVector);

	parseOptions(formatter,
	             optionsVector.begin(),
	             optionsVector.end(),
	             "Unknown Artistic Style options\n"
	             "The following options were not processed:");

	if (_err->str().length() > 0)
		fpErrorHandler(210, (char*) _err->str().c_str());

	delete _err;
	_err = NULL;

	istringstream in(pSourceIn);
	ASStreamIterator<istringstream> streamIterator(&in);
	ostringstream out;
	formatter.init(&streamIterator);

	while (formatter.hasMoreLines())
	{
		out << formatter.nextLine();
		out << streamIterator.outputEOL;
	}

	unsigned long textSizeOut = out.str().length();
	char* pTextOut = fpMemoryAlloc(textSizeOut + 1);     // call memory allocation function
//    pTextOut = NULL;           // for testing
	if (pTextOut == NULL)
	{
		fpErrorHandler(110, "Allocation failure on output");
		return NULL;
	}

	strcpy(pTextOut, out.str().c_str());

	return pTextOut;
}

extern "C" EXPORT const char* STDCALL AStyleGetVersion (void)
{
	return _version;
}

#else

void preserveFileDate(const char *oldFileName, const char *newFileName)
{
	struct stat stBuf;
	bool statErr = false;
	if (stat (oldFileName, &stBuf) == -1)
		statErr = true;
	else
	{
		struct utimbuf outBuf;
		outBuf.actime = stBuf.st_atime;
		// add 1 so RCS will recoginze a change
		outBuf.modtime = stBuf.st_mtime + 1;
		if (utime (newFileName, &outBuf) == -1)
			statErr = true;
	}
	if (statErr)
		(*_err) << "    Could not preserve file date" << endl;
}

bool stringEndsWith(const string &str, const string &suffix)
{
	int strIndex = (int) str.length() - 1;
	int suffixIndex = (int) suffix.length() - 1;

	while (strIndex >= 0 && suffixIndex >= 0)
	{
		if (tolower(str[strIndex]) != tolower(suffix[suffixIndex]))
			return false;

		--strIndex;
		--suffixIndex;
	}

	return true;
}


void error(const char *why, const char* what)
{
	(*_err) << why << ' ' << what << '\n' << endl;
	exit(1);
}


void printHelp()
{
	(*_err) << endl;
	(*_err) << "                            Artistic Style " << _version << endl;
	(*_err) << "                         Maintained by: Jim Pattee\n";
	(*_err) << "                       Original Author: Tal Davidson\n";
	(*_err) << endl;
	(*_err) << "Usage  :  astyle [options] Source1.cpp Source2.cpp  [...]\n";
	(*_err) << "          astyle [options] < Original > Beautified\n";
	(*_err) << endl;
	(*_err) << "When indenting a specific file, the resulting indented file RETAINS the\n";
	(*_err) << "original file-name. The original pre-indented file is renamed, with a\n";
	(*_err) << "suffix of \".orig\" added to the original filename.\n";
	(*_err) << endl;
	(*_err) << "By default, astyle is set up to indent C/C++/C#/Java files, with 4 spaces\n";
	(*_err) << "per indent, a maximal indentation of 40 spaces inside continuous statements,\n";
	(*_err) << "and NO formatting.\n";
	(*_err) << endl;
	(*_err) << "Option's Format:\n";
	(*_err) << "----------------\n";
	(*_err) << "    Long options (starting with '--') must be written one at a time.\n";
	(*_err) << "    Short options (starting with '-') may be appended together.\n";
	(*_err) << "    Thus, -bps4 is the same as -b -p -s4.\n";
	(*_err) << endl;
	(*_err) << "Predefined Style Options:\n";
	(*_err) << "-------------------------\n";
	(*_err) << "    --style=ansi\n";
	(*_err) << "    ANSI style formatting/indenting.\n";
	(*_err) << endl;
	(*_err) << "    --style=gnu\n";
	(*_err) << "    GNU style formatting/indenting.\n";
	(*_err) << endl;
	(*_err) << "    --style=kr\n";
	(*_err) << "    Kernighan&Ritchie style formatting/indenting.\n";
	(*_err) << endl;
	(*_err) << "    --style=linux\n";
	(*_err) << "    Linux mode (8 spaces per indent, break definition-block\n";
	(*_err) << "    brackets but attach command-block brackets).\n";
	(*_err) << endl;
	(*_err) << "    --style=java\n";
	(*_err) << "    Java mode, with standard java style formatting/indenting.\n";
	(*_err) << endl;
	(*_err) << "Tab and Bracket Options:\n";
	(*_err) << "------------------------\n";
	(*_err) << "    default indent option\n";
	(*_err) << "    If no indentation option is set,\n";
	(*_err) << "    the default option of 4 spaces will be used.\n";
	(*_err) << endl;
	(*_err) << "    --indent=spaces=#   OR   -s#\n";
	(*_err) << "    Indent using # spaces per indent. Not specifying #\n";
	(*_err) << "    will result in a default of 4 spaces per indent.\n";
	(*_err) << endl;
	(*_err) << "    --indent=tab   OR   --indent=tab=#   OR   -t   OR   -t#\n";
	(*_err) << "    Indent using tab characters, assuming that each\n";
	(*_err) << "    tab is # spaces long. Not specifying # will result\n";
	(*_err) << "    in a default assumption of 4 spaces per tab.\n";
	(*_err) << endl;
	(*_err) << "    --force-indent=tab=#   OR   -T#\n";
	(*_err) << "    Indent using tab characters, assuming that each\n";
	(*_err) << "    tab is # spaces long. Force tabs to be used in areas\n";
	(*_err) << "    Astyle would prefer to use spaces.\n";
	(*_err) << endl;
	(*_err) << "    default brackets option\n";
	(*_err) << "    If no brackets option is set,\n";
	(*_err) << "    the brackets will not be changed.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=break   OR   -b\n";
	(*_err) << "    Break brackets from pre-block code (i.e. ANSI C/C++ style).\n";
	(*_err) << endl;
	(*_err) << "    --brackets=attach   OR   -a\n";
	(*_err) << "    Attach brackets to pre-block code (i.e. Java/K&R style).\n";
	(*_err) << endl;
	(*_err) << "    --brackets=linux   OR   -l\n";
	(*_err) << "    Break definition-block brackets and attach command-block\n";
	(*_err) << "    brackets.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=break-closing   OR   -y\n";
	(*_err) << "    Break brackets before closing headers (e.g. 'else', 'catch', ...).\n";
	(*_err) << "    Should be appended to --brackets=attach or --brackets=linux.\n";
	(*_err) << endl;
	(*_err) << "Indentation options:\n";
	(*_err) << "--------------------\n";
	(*_err) << "    --indent-classes   OR   -C\n";
	(*_err) << "    Indent 'class' blocks, so that the inner 'public:',\n";
	(*_err) << "    'protected:' and 'private: headers are indented in\n";
	(*_err) << "    relation to the class block.\n";
	(*_err) << endl;
	(*_err) << "    --indent-switches   OR   -S\n";
	(*_err) << "    Indent 'switch' blocks, so that the inner 'case XXX:'\n";
	(*_err) << "    headers are indented in relation to the switch block.\n";
	(*_err) << endl;
	(*_err) << "    --indent-cases   OR   -K\n";
	(*_err) << "    Indent case blocks from the 'case XXX:' headers.\n";
	(*_err) << "    Case statements not enclosed in blocks are NOT indented.\n";
	(*_err) << endl;
	(*_err) << "    --indent-brackets   OR   -B\n";
	(*_err) << "    Add extra indentation to '{' and '}' block brackets.\n";
	(*_err) << endl;
	(*_err) << "    --indent-blocks   OR   -G\n";
	(*_err) << "    Add extra indentation entire blocks (including brackets).\n";
	(*_err) << endl;
	(*_err) << "    --indent-namespaces   OR   -N\n";
	(*_err) << "    Indent the contents of namespace blocks.\n";
	(*_err) << endl;
	(*_err) << "    --indent-labels   OR   -L\n";
	(*_err) << "    Indent labels so that they appear one indent less than\n";
	(*_err) << "    the current indentation level, rather than being\n";
	(*_err) << "    flushed completely to the left (which is the default).\n";
	(*_err) << endl;
	(*_err) << "    --indent-preprocessor   OR   -w\n";
	(*_err) << "    Indent multi-line #define statements.\n";
	(*_err) << endl;
	(*_err) << "    --max-instatement-indent=#   OR   -M#\n";
	(*_err) << "    Indent a maximal # spaces in a continuous statement,\n";
	(*_err) << "    relative to the previous line.\n";
	(*_err) << endl;
	(*_err) << "    --min-conditional-indent=#   OR   -m#\n";
	(*_err) << "    Indent a minimal # spaces in a continuous conditional\n";
	(*_err) << "    belonging to a conditional header.\n";
	(*_err) << endl;
	(*_err) << "Formatting options:\n";
	(*_err) << "-------------------\n";
	(*_err) << "    --break-blocks   OR   -f\n";
	(*_err) << "    Insert empty lines around unrelated blocks, labels, classes, ...\n";
	(*_err) << endl;
	(*_err) << "    --break-blocks=all   OR   -F\n";
	(*_err) << "    Like --break-blocks, except also insert empty lines \n";
	(*_err) << "    around closing headers (e.g. 'else', 'catch', ...).\n";
	(*_err) << endl;
	(*_err) << "    --break-elseifs   OR   -e\n";
	(*_err) << "    Break 'else if()' statements into two different lines.\n";
	(*_err) << endl;
	(*_err) << "    --pad=oper   OR   -p\n";
	(*_err) << "    Insert space paddings around operators.\n";
	(*_err) << endl;
	(*_err) << "    --pad=paren   OR   -P\n";
	(*_err) << "    Insert space padding around parenthesis on both the outside\n";
	(*_err) << "    and the inside.\n";
	(*_err) << endl;
	(*_err) << "    --pad=paren-out   OR   -d\n";
	(*_err) << "    Insert space padding around parenthesis on the outside only.\n";
	(*_err) << endl;
	(*_err) << "    --pad=paren-in   OR   -D\n";
	(*_err) << "    Insert space padding around parenthesis on the inside only.\n";
	(*_err) << endl;
	(*_err) << "    --unpad=paren   OR   -U\n";
	(*_err) << "    Remove unnecessary space padding around parenthesis.  This\n";
	(*_err) << "    can be used in combination with the 'pad' options above.\n";
	(*_err) << endl;
	(*_err) << "    --one-line=keep-statements   OR   -o\n";
	(*_err) << "    Don't break lines containing multiple statements into\n";
	(*_err) << "    multiple single-statement lines.\n";
	(*_err) << endl;
	(*_err) << "    --one-line=keep-blocks   OR   -O\n";
	(*_err) << "    Don't break blocks residing completely on one line.\n";
	(*_err) << endl;
	(*_err) << "    --convert-tabs   OR   -V\n";
	(*_err) << "    Convert tabs to spaces.\n";
	(*_err) << endl;
	(*_err) << "    --fill-empty-lines   OR   -E\n";
	(*_err) << "    Fill empty lines with the white space of their\n";
	(*_err) << "    previous lines.\n";
	(*_err) << endl;
	(*_err) << "    --mode=c   OR   -c\n";
	(*_err) << "    Indent a C, C++ or C# source file (this is the default).\n";
	(*_err) << endl;
	(*_err) << "    --mode=java   OR   -j\n";
	(*_err) << "    Indent a Java(TM) source file.\n";
	(*_err) << endl;
	(*_err) << "Other options:\n";
	(*_err) << "--------------\n";
	(*_err) << "    --suffix=####\n";
	(*_err) << "    Append the suffix #### instead of '.orig' to original filename.\n";
	(*_err) << endl;
	(*_err) << "    --suffix=none   OR   -n\n";
	(*_err) << "    Do not retain a backup of the original file.\n";
	(*_err) << endl;
	(*_err) << "    --options=####\n";
	(*_err) << "    Specify an options file #### to read and use.\n";
	(*_err) << endl;
	(*_err) << "    --options=none\n";
	(*_err) << "    Disable the default options file.\n";
	(*_err) << "    Only the command-line parameters will be used.\n";
	(*_err) << endl;
	(*_err) << "    --preserve-date   OR   -Z\n";
	(*_err) << "    The date and time modified will not be changed in the formatted file.\n";
	(*_err) << endl;
	(*_err) << "    --errors-to-stdout   OR   -X\n";
	(*_err) << "    Print errors and help information to standard-output rather than\n";
	(*_err) << "    to standard-error.\n";
	(*_err) << endl;
	(*_err) << "    --version   OR   -v\n";
	(*_err) << "    Print version number.\n";
	(*_err) << endl;
	(*_err) << "    --help   OR   -h   OR   -?\n";
	(*_err) << "    Print this help message.\n";
	(*_err) << endl;
	(*_err) << "Default options file:\n";
	(*_err) << "---------------------\n";
	(*_err) << "    Artistic Style looks for a default options file in the\n";
	(*_err) << "    following order:\n";
	(*_err) << "    1. The contents of the ARTISTIC_STYLE_OPTIONS environment\n";
	(*_err) << "       variable if it exists.\n";
	(*_err) << "    2. The file called .astylerc in the directory pointed to by the\n";
	(*_err) << "       HOME environment variable ( i.e. $HOME/.astylerc ).\n";
	(*_err) << "    3. The file called astylerc in the directory pointed to by the\n";
	(*_err) << "       USERPROFILE environment variable ( i.e. %USERPROFILE%\\astylerc ).\n";
	(*_err) << "    If a default options file is found, the options in this file\n";
	(*_err) << "    will be parsed BEFORE the command-line options.\n";
	(*_err) << "    Long options within the default option file may be written without\n";
	(*_err) << "    the preliminary '--'.\n";
	(*_err) << endl;
}

int main(int argc, char *argv[])
{
	ASFormatter formatter;
	vector<string> fileNameVector;
	vector<string> optionsVector;
	string optionsFileName = "";
	string arg;
	bool ok = true;
	bool shouldPrintHelp = false;
	bool shouldParseOptionsFile = true;

	_modeManuallySet = false;

	_msg << "\nArtistic Style " << _version << endl;
	// manage flags
	for (int i = 1; i < argc; i++)
	{
		arg = string(argv[i]);

		if ( IS_OPTION(arg, "--options=none") )
		{
			shouldParseOptionsFile = false;
		}
		else if ( isParamOption(arg, "--options=") )
		{
			optionsFileName = GET_PARAM(arg, "--options=");
		}
		else if ( IS_OPTION(arg, "-h")
		          || IS_OPTION(arg, "--help")
		          || IS_OPTION(arg, "-?") )
		{
			shouldPrintHelp = true;
		}
		else if (arg[0] == '-')
		{
			optionsVector.push_back(arg);
		}
		else // file-name
		{
			fileNameVector.push_back(arg);
		}
	}

	// parse options file
	if (shouldParseOptionsFile)
	{
		if (optionsFileName.compare("") == 0)
		{
			char* env = getenv("ARTISTIC_STYLE_OPTIONS");
			if (env != NULL)
				optionsFileName = string(env);
		}
		if (optionsFileName.compare("") == 0)
		{
			char* env = getenv("HOME");
			if (env != NULL)
				optionsFileName = string(env) + string("/.astylerc");
		}
		if (optionsFileName.compare("") == 0)
		{
			char* env = getenv("USERPROFILE");
			if (env != NULL)
				optionsFileName = string(env) + string("/astylerc");
		}

		if (optionsFileName.compare("") != 0)
		{
			ifstream optionsIn(optionsFileName.c_str());
			if (optionsIn)
			{
				_msg << "Using default options file " << optionsFileName << endl;
				vector<string> fileOptionsVector;
				importOptions(optionsIn, fileOptionsVector);
				ok = parseOptions(formatter,
				                  fileOptionsVector.begin(),
				                  fileOptionsVector.end(),
				                  string("Unknown option in default options file: "));
			}

			optionsIn.close();
			if (!ok)
			{
				(*_err) << "For help on options, type 'astyle -h' " << endl;
			}
		}
	}

	// parse options from command line

	ok = parseOptions(formatter,
	                  optionsVector.begin(),
	                  optionsVector.end(),
	                  string("Unknown command line option: "));
	if (!ok)
	{
		(*_err) << "For help on options, type 'astyle -h' \n" << endl;
		exit(1);
	}

	if (shouldPrintHelp)
	{
		printHelp();
		exit(0);
	}

	// if no files have been given, use cin for input and cout for output
	// this is used to format text for text editors like TextWrangler
	// do NOT display any console messages when this branch is used
	if (fileNameVector.empty())
	{
		ASStreamIterator<istream> streamIterator(&cin);

		formatter.init(&streamIterator);

		while (formatter.hasMoreLines())
		{
			cout << formatter.nextLine();
			if (formatter.hasMoreLines())
				cout << streamIterator.outputEOL;
		}
		cout.flush();
	}
	else
	{
		// indent the given files
		cout << _msg.str().c_str();
		clock_t startTime = clock();
		for (size_t i = 0; i < fileNameVector.size(); i++)
		{
			string originalFileName = fileNameVector[i];
			string inFileName = originalFileName + _suffix;

			remove(inFileName.c_str());     // remove the old .orig if present

			// check if the file is present before rename
			ifstream inCheck(originalFileName.c_str());
			if (!inCheck)
				error("Could not open input file", originalFileName.c_str());
			inCheck.close();

			if (rename(originalFileName.c_str(), inFileName.c_str()) < 0)
				error("Could not rename ", string(originalFileName + " to " + inFileName).c_str());

			ifstream in(inFileName.c_str(), ios::binary);
			if (!in)
				error("Could not open input file", inFileName.c_str());

			ofstream out(originalFileName.c_str(), ios::binary);
			if (!out)
				error("Could not open output file", originalFileName.c_str());

			// Unless a specific language mode has been, set the language mode
			// according to the file's suffix.
			if (!_modeManuallySet)
			{
				if (stringEndsWith(originalFileName, string(".java")))
					formatter.setJavaStyle();
				else if (stringEndsWith(originalFileName, string(".cs")))
					formatter.setSharpStyle();
				else
					formatter.setCStyle();
			}
			// display file formatting message and save the filename
			cout << "formatting " << originalFileName.c_str() << endl;
			size_t fname = originalFileName.find_last_of("/\\");
			if (fname == string::npos)
				fname = 0;
			else
				fname +=1;
			formatter.fileName = originalFileName.substr(fname);

			ASStreamIterator<istream> streamIterator(&in);
			formatter.init(&streamIterator);

			while (formatter.hasMoreLines())
			{
				out << formatter.nextLine();
				// the last line does not get an eol
				if (formatter.hasMoreLines())
					out << streamIterator.outputEOL;
			}
			out.flush();
			out.close();
			in.close();

			// change date modified to original file date
			if (_preserveDate)
				preserveFileDate(inFileName.c_str(), originalFileName.c_str());

			if (_purgeOrigIn)
				remove(inFileName.c_str());
		}
		// all files formatted
		clock_t stopTime = clock();
		float secs = (float) (stopTime - startTime) / CLOCKS_PER_SEC;
		// show tenths of a second if time is less than 20 seconds
		cout.precision(2);
		if (secs >= 100 || (secs >= 10 && secs < 20))
			cout.precision(3);
		cout << "total time " << secs << " seconds" << endl;
		cout.precision(0);
		cout << endl;
	}
	return 0;
}

#endif
// *************************   end of console functions   *****************************************
