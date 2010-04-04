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

#include "astyle_main.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <errno.h>

// includes for recursive getFileNames() function
#ifdef _WIN32
#undef UNICODE		// use ASCII windows functions
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#ifdef __VMS
#include <unixlib.h>
#include <rms.h>
#include <ssdef.h>
#include <stsdef.h>
#include <lib$routines.h>
#include <starlet.h>
#endif /* __VMS */
#endif

// turn off MinGW automatic file globbing
// this CANNOT be in the astyle namespace
int _CRT_glob = 0;

namespace astyle
{

#define IS_OPTION(arg,op)          ((arg).compare(op)==0)
#define IS_OPTIONS(arg,a,b)        (IS_OPTION((arg),(a)) || IS_OPTION((arg),(b)))

#define GET_PARAM(arg,op)          ((arg).substr(strlen(op)))
#define GET_PARAMS(arg,a,b) (isParamOption((arg),(a)) ? GET_PARAM((arg),(a)) : GET_PARAM((arg),(b)))

#ifdef _WIN32
char g_fileSeparator = '\\';
bool g_isCaseSensitive = false;
#else
char g_fileSeparator = '/';
bool g_isCaseSensitive = true;
#endif

#ifdef ASTYLE_LIB
// library build variables
stringstream* _err = NULL;
#else
// console build variables
ostream* _err = &cerr;           // direct error messages to cerr
ASConsole* g_console = NULL;     // class to encapsulate console variables
#endif

#ifdef ASTYLE_JNI
// java library build variables
JNIEnv*   g_env;
jobject   g_obj;
jmethodID g_mid;
#endif

const char* _version = "1.24";


/**
 * parse the options vector
 * ITER can be either a fileOptionsVector (options file) or an optionsVector (command line)
 *
 * @return        true if no errors, false if errors
 */
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

			// break options on spaces, tabs, commas, or new-lines
			if (in.eof() || ch == ' ' || ch == '\t' || ch == ',' || ch == '\n')
				break;
			else
				currentToken.append(1, ch);

		}
		while (in);

		if (currentToken.length() != 0)
			optionsVector.push_back(currentToken);
	}
}

void isOptionError(const string &arg, const string &errorInfo)
{
#ifdef ASTYLE_LIB
	if (_err->str().length() == 0)
	{
		(*_err) << errorInfo << endl;   // need main error message
		(*_err) << arg;                 // output the option in error
	}
	else
		(*_err) << endl << arg;         // put endl after previous option
#else
	if (errorInfo.length() > 0)         // to avoid a compiler warning
		g_console->error("Error in param: ", arg.c_str());
#endif
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

bool isParamOption(const string &arg, const char *option1, const char *option2)
{
	return isParamOption(arg, option1) || isParamOption(arg, option2);
}

bool parseOption(ASFormatter &formatter, const string &arg, const string &errorInfo)
{
	if ( IS_OPTION(arg, "style=allman") || IS_OPTION(arg, "style=ansi")  || IS_OPTION(arg, "style=bsd") )
	{
		formatter.setFormattingStyle(STYLE_ALLMAN);
	}
	else if ( IS_OPTION(arg, "style=java") )
	{
		formatter.setFormattingStyle(STYLE_JAVA);
	}
	else if ( IS_OPTION(arg, "style=k&r") || IS_OPTION(arg, "style=k/r") )
	{
		formatter.setFormattingStyle(STYLE_KandR);
	}
	else if ( IS_OPTION(arg, "style=stroustrup") )
	{
		formatter.setFormattingStyle(STYLE_STROUSTRUP);
	}
	else if ( IS_OPTION(arg, "style=whitesmith") )
	{
		formatter.setFormattingStyle(STYLE_WHITESMITH);
	}
	else if ( IS_OPTION(arg, "style=banner") )
	{
		formatter.setFormattingStyle(STYLE_BANNER);
	}
	else if ( IS_OPTION(arg, "style=gnu") )
	{
		formatter.setFormattingStyle(STYLE_GNU);
	}
	else if ( IS_OPTION(arg, "style=linux") )
	{
		formatter.setFormattingStyle(STYLE_LINUX);
	}
	else if ( IS_OPTION(arg, "style=horstmann") )
	{
		formatter.setFormattingStyle(STYLE_HORSTMANN);
	}
	else if ( IS_OPTION(arg, "style=1tbs") || IS_OPTION(arg, "style=otbs") )
	{
		formatter.setFormattingStyle(STYLE_1TBS);
	}
	else if ( isParamOption(arg, "A") )
	{
		int style = 0;
		string styleParam = GET_PARAM(arg, "A");
		if (styleParam.length() > 0)
			style = atoi(styleParam.c_str());
		if (style < 1 || style > 10)
			isOptionError(arg, errorInfo);
		else if (style == 1)
			formatter.setFormattingStyle(STYLE_ALLMAN);
		else if (style == 2)
			formatter.setFormattingStyle(STYLE_JAVA);
		else if (style == 3)
			formatter.setFormattingStyle(STYLE_KandR);
		else if (style == 4)
			formatter.setFormattingStyle(STYLE_STROUSTRUP);
		else if (style == 5)
			formatter.setFormattingStyle(STYLE_WHITESMITH);
		else if (style == 6)
			formatter.setFormattingStyle(STYLE_BANNER);
		else if (style == 7)
			formatter.setFormattingStyle(STYLE_GNU);
		else if (style == 8)
			formatter.setFormattingStyle(STYLE_LINUX);
		else if (style == 9)
			formatter.setFormattingStyle(STYLE_HORSTMANN);
		else if (style == 10)
			formatter.setFormattingStyle(STYLE_1TBS);
	}
	// must check for mode=cs before mode=c !!!
	else if ( IS_OPTION(arg, "mode=cs") )
	{
		formatter.setSharpStyle();
		formatter.setModeManuallySet(true);
	}
	else if ( IS_OPTION(arg, "mode=c") )
	{
		formatter.setCStyle();
		formatter.setModeManuallySet(true);
	}
	else if ( IS_OPTION(arg, "mode=java") )
	{
		formatter.setJavaStyle();
		formatter.setModeManuallySet(true);
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
		{
			formatter.setTabIndentation(spaceNum, false);
			formatter.setIndentManuallySet(true);
		}
	}
	else if ( IS_OPTION(arg, "indent=tab") )
	{
		formatter.setTabIndentation(4);
		// do NOT call setIndentManuallySet(true);
	}
	else if ( isParamOption(arg, "T", "indent=force-tab=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "T", "indent=force-tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
		{
			formatter.setTabIndentation(spaceNum, true);
			formatter.setIndentManuallySet(true);
		}
	}
	else if ( IS_OPTION(arg, "indent=force-tab") )
	{
		formatter.setTabIndentation(4, true);
		// do NOT call setIndentManuallySet(true);
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
		{
			formatter.setSpaceIndentation(spaceNum);
			formatter.setIndentManuallySet(true);
		}
	}
	else if ( IS_OPTION(arg, "indent=spaces") )
	{
		formatter.setSpaceIndentation(4);
		// do NOT call setIndentManuallySet(true);
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
		{
			formatter.setMinConditionalIndentLength(minIndent);
			formatter.setMinConditionalManuallySet(true);
		}
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
	else if ( IS_OPTIONS(arg, "y", "break-closing-brackets") )
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
		formatter.setBracketFormatMode(LINUX_MODE);
	}
	else if ( IS_OPTIONS(arg, "u", "brackets=stroustrup") )
	{
		formatter.setBracketFormatMode(STROUSTRUP_MODE);
	}
	else if ( IS_OPTIONS(arg, "g", "brackets=horstmann") )
	{
		formatter.setBracketFormatMode(HORSTMANN_MODE);
	}
	else if ( IS_OPTIONS(arg, "O", "keep-one-line-blocks") )
	{
		formatter.setBreakOneLineBlocksMode(false);
	}
	else if ( IS_OPTIONS(arg, "o", "keep-one-line-statements") )
	{
		formatter.setSingleStatementsMode(false);
	}
	else if ( IS_OPTIONS(arg, "P", "pad-paren") )
	{
		formatter.setParensOutsidePaddingMode(true);
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "d", "pad-paren-out") )
	{
		formatter.setParensOutsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "D", "pad-paren-in") )
	{
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "H", "pad-header") )
	{
		formatter.setParensHeaderPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "U", "unpad-paren") )
	{
		formatter.setParensUnPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "p", "pad-oper") )
	{
		formatter.setOperatorPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "x", "delete-empty-lines") )
	{
		formatter.setDeleteEmptyLinesMode(true);
	}
	else if ( IS_OPTIONS(arg, "E", "fill-empty-lines") )
	{
		formatter.setEmptyLineFill(true);
	}
	else if ( IS_OPTIONS(arg, "w", "indent-preprocessor") )
	{
		formatter.setPreprocessorIndent(true);
	}
	else if ( IS_OPTIONS(arg, "c", "convert-tabs") )
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
	else if ( IS_OPTIONS(arg, "j", "add-brackets") )
	{
		formatter.setAddBracketsMode(true);
	}
	else if ( IS_OPTIONS(arg, "J", "add-one-line-brackets") )
	{
		formatter.setAddOneLineBracketsMode(true);
	}
	else if ( IS_OPTIONS(arg, "Y", "indent-col1-comments") )
	{
		formatter.setIndentCol1CommentsMode(true);
	}
	else if ( IS_OPTION(arg, "align-pointer=type") )
	{
		formatter.setPointerAlignment(ALIGN_TYPE);
	}
	else if ( IS_OPTION(arg, "align-pointer=middle") )
	{
		formatter.setPointerAlignment(ALIGN_MIDDLE);
	}
	else if ( IS_OPTION(arg, "align-pointer=name") )
	{
		formatter.setPointerAlignment(ALIGN_NAME);
	}
	else if ( isParamOption(arg, "k") )
	{
		int align = 0;
		string styleParam = GET_PARAM(arg, "k");
		if (styleParam.length() > 0)
			align = atoi(styleParam.c_str());
		if (align < 1 || align > 3)
			isOptionError(arg, errorInfo);
		else if (align == 1)
			formatter.setPointerAlignment(ALIGN_TYPE);
		else if (align == 2)
			formatter.setPointerAlignment(ALIGN_MIDDLE);
		else if (align == 3)
			formatter.setPointerAlignment(ALIGN_NAME);
	}
	// depreciated options /////////////////////////////////////////////////////////////////////////////////////
	// depreciated in release 1.23
	// removed from documentation in release 1.24
	// may be removed at an appropriate time
//	else if ( IS_OPTION(arg, "style=kr") )
//	{
//		formatter.setFormattingStyle(STYLE_JAVA);
//	}
	else if ( isParamOption(arg, "T", "force-indent=tab=") )
	{
		// the 'T' option will already have been processed
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "T", "force-indent=tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 1 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
			formatter.setTabIndentation(spaceNum, true);
	}
	else if ( IS_OPTION(arg, "brackets=break-closing") )
	{
		formatter.setBreakClosingHeaderBracketsMode(true);
	}

	else if ( IS_OPTION(arg, "one-line=keep-blocks") )
	{
		formatter.setBreakOneLineBlocksMode(false);
	}
	else if ( IS_OPTION(arg, "one-line=keep-statements") )
	{
		formatter.setSingleStatementsMode(false);
	}
	else if ( IS_OPTION(arg, "pad=paren") )
	{
		formatter.setParensOutsidePaddingMode(true);
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTION(arg, "pad=paren-out") )
	{
		formatter.setParensOutsidePaddingMode(true);
	}
	else if ( IS_OPTION(arg, "pad=paren-in") )
	{
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTION(arg, "unpad=paren") )
	{
		formatter.setParensUnPaddingMode(true);
	}
	else if ( IS_OPTION(arg, "pad=oper") )
	{
		formatter.setOperatorPaddingMode(true);
	}
	// end depreciated options //////////////////////////////////////////////////////////////////////////////
#ifdef ASTYLE_LIB
	// End of options used by GUI
	else
		isOptionError(arg, errorInfo);
#else
	// Options used by only console
	else if ( IS_OPTIONS(arg, "n", "suffix=none") )
	{
		g_console->setNoBackup(true);
	}
	else if ( isParamOption(arg, "suffix=") )
	{
		string suffixParam = GET_PARAM(arg, "suffix=");
		if (suffixParam.length() > 0)
		{
			g_console->setOrigSuffix(suffixParam);
		}
	}
	else if ( isParamOption(arg, "exclude=") )
	{
		string suffixParam = GET_PARAM(arg, "exclude=");
		if (suffixParam.length() > 0)
			g_console->updateExcludeVector(suffixParam);
	}
	else if ( IS_OPTIONS(arg, "r", "R") || IS_OPTION(arg, "recursive") )
	{
		g_console->setIsRecursive(true);
	}
	else if ( IS_OPTIONS(arg, "Z", "preserve-date") )
	{
		g_console->setPreserveDate(true);
	}
	else if ( IS_OPTIONS(arg, "v", "verbose") )
	{
		g_console->setIsVerbose(true);
	}
	else if ( IS_OPTIONS(arg, "Q", "formatted") )
	{
		g_console->setIsFormattedOnly(true);
	}
	else if ( IS_OPTIONS(arg, "q", "quiet") )
	{
		g_console->setIsQuiet(true);
	}
	else if ( IS_OPTIONS(arg, "X", "errors-to-stdout") )
	{
		_err = &cout;
	}
	else if ( IS_OPTION(arg, "lineend=windows") )
	{
		formatter.setLineEndFormat(LINEEND_WINDOWS);
	}
	else if ( IS_OPTION(arg, "lineend=linux") )
	{
		formatter.setLineEndFormat(LINEEND_LINUX);
	}
	else if ( IS_OPTION(arg, "lineend=macold") )
	{
		formatter.setLineEndFormat(LINEEND_MACOLD);
	}
	else if ( isParamOption(arg, "z") )
	{
		int lineendType = 0;
		string lineendParam = GET_PARAM(arg, "z");
		if (lineendParam.length() > 0)
			lineendType = atoi(lineendParam.c_str());
		if (lineendType < 1 || lineendType > 3)
			isOptionError(arg, errorInfo);
		else if (lineendType == 1)
			formatter.setLineEndFormat(LINEEND_WINDOWS);
		else if (lineendType == 2)
			formatter.setLineEndFormat(LINEEND_LINUX);
		else if (lineendType == 3)
			formatter.setLineEndFormat(LINEEND_MACOLD);
	}
	else
	{
		(*_err) << errorInfo << arg << endl;
		return false; // invalid option
	}
#endif
// End of parseOption function
	return true; //o.k.
}

//--------------------------------------------------------------------------------------
// ASStreamIterator class
// typename will be istringstream for GUI and istream otherwise
//--------------------------------------------------------------------------------------

template<typename T>
ASStreamIterator<T>::ASStreamIterator(T *in)
{
	inStream = in;
	buffer.reserve(200);
	eolWindows = 0;
	eolLinux = 0;
	eolMacOld = 0;
	outputEOL[0] = '\0';
	peekStart = 0;
	prevLineDeleted = false;
	checkForEmptyLine = false;
}

template<typename T>
ASStreamIterator<T>::~ASStreamIterator()
{
}

/**
 * read the input stream, delete any end of line characters,
 *     and build a string that contains the input line.
 *
 * @return        string containing the next input line minus any end of line characters
 */
template<typename T>
string ASStreamIterator<T>::nextLine(bool emptyLineWasDeleted)
{
	// verify that the current position is correct
	assert (peekStart == 0);

	// a deleted line may be replaced if break-blocks is requested
	// this sets up the compare to check for a replaced empty line
	if (prevLineDeleted)
	{
		prevLineDeleted = false;
		checkForEmptyLine = true;
	}
	if (!emptyLineWasDeleted)
		prevBuffer = buffer;
	else
		prevLineDeleted = true;

	// read the next record
	buffer.clear();
	char ch;
	inStream->get(ch);

	while (!inStream->eof() && ch != '\n' && ch != '\r')
	{
		buffer.append(1, ch);
		inStream->get(ch);
	}

	if (inStream->eof())
	{
		return buffer;
	}

	int peekCh = inStream->peek();

	// find input end-of-line characters
	if (!inStream->eof())
	{
		if (ch == '\r')         // CR+LF is windows otherwise Mac OS 9
		{
			if (peekCh == '\n')
			{
				inStream->get();
				eolWindows++;
			}
			else
				eolMacOld++;
		}
		else                    // LF is Linux, allow for improbable LF/CR
		{
			if (peekCh == '\r')
			{
				inStream->get();
				eolWindows++;
			}
			else
				eolLinux++;
		}
	}
	else
	{
		inStream->clear();
	}

	// set output end of line characters
	if (eolWindows >= eolLinux)
	{
		if (eolWindows >= eolMacOld)
			strcpy(outputEOL, "\r\n");  // Windows (CR+LF)
		else
			strcpy(outputEOL, "\r");    // MacOld (CR)
	}
	else if (eolLinux >= eolMacOld)
		strcpy(outputEOL, "\n");    // Linux (LF)
	else
		strcpy(outputEOL, "\r");    // MacOld (CR)

	return buffer;
}

// save the current position and get the next line
// this can be called for multiple reads
// when finished peeking you MUST call peekReset()
// call this function from ASFormatter ONLY
template<typename T>
string ASStreamIterator<T>::peekNextLine()
{
	assert (hasMoreLines());
	string nextLine;
	char ch;

	if (peekStart == 0)
		peekStart = inStream->tellg();

	// read the next record
	inStream->get(ch);
	while (!inStream->eof() && ch != '\n' && ch != '\r')
	{
		nextLine.append(1, ch);
		inStream->get(ch);
	}

	if (inStream->eof())
	{
		return nextLine;
	}

	int peekCh = inStream->peek();

	// remove end-of-line characters
	if (!inStream->eof())
	{
		if ((peekCh == '\n' || peekCh == '\r') && peekCh != ch)  /////////////  changed  //////////
			inStream->get();
	}

	return nextLine;
}

// reset current position and EOF for peekNextLine()
template<typename T>
void ASStreamIterator<T>::peekReset()
{
	assert(peekStart != 0);
	inStream->clear();
	inStream->seekg(peekStart);
	peekStart = 0;
}

// save the last input line after input has reached EOF
template<typename T>
void ASStreamIterator<T>::saveLastInputLine()
{
	assert(inStream->eof());
	prevBuffer = buffer;
}

// check for a change in line ends
template<typename T>
bool ASStreamIterator<T>::getLineEndChange(int lineEndFormat) const
{
	assert(lineEndFormat == LINEEND_DEFAULT
	       || lineEndFormat == LINEEND_WINDOWS
	       || lineEndFormat == LINEEND_LINUX
	       || lineEndFormat == LINEEND_MACOLD);

	bool lineEndChange = false;
	if (lineEndFormat == LINEEND_WINDOWS)
		lineEndChange = (eolLinux + eolMacOld != 0);
	else if (lineEndFormat == LINEEND_LINUX)
		lineEndChange = (eolWindows + eolMacOld != 0);
	else if (lineEndFormat == LINEEND_MACOLD)
		lineEndChange = (eolWindows + eolLinux != 0);
	else
	{
		if (eolWindows > 0)
			lineEndChange = (eolLinux + eolMacOld != 0);
		else if (eolLinux > 0)
			lineEndChange = (eolWindows + eolMacOld != 0);
		else if (eolMacOld > 0)
			lineEndChange = (eolWindows + eolLinux != 0);
	}
	return lineEndChange;
}

#ifndef ASTYLE_LIB
//--------------------------------------------------------------------------------------
// ASConsole class
// main function will be included only in the console build
//--------------------------------------------------------------------------------------

// rewrite a stringstream converting the line ends
void ASConsole::convertLineEnds(ostringstream& out, int lineEnd)
{
	assert(lineEnd == LINEEND_WINDOWS || lineEnd == LINEEND_LINUX || lineEnd == LINEEND_MACOLD);
	const string& inStr = out.str();	// avoids strange looking syntax
	string outStr;						// the converted ouput
	int inLength = inStr.length();
	for (int pos = 0; pos < inLength; pos++)
	{
		if (inStr[pos] == '\r')
		{
			if (inStr[pos+ 1] == '\n')
			{
				// CRLF
				if (lineEnd == LINEEND_CR)
				{
					outStr += inStr[pos];		// Delete the LF
					pos++;
					continue;
				}
				else if (lineEnd == LINEEND_LF)
				{
					outStr += inStr[pos+1];		// Delete the CR
					pos++;
					continue;
				}
				else
				{
					outStr += inStr[pos];		// Do not change
					outStr += inStr[pos+1];
					pos++;
					continue;
				}
			}
			else
			{
				// CR
				if (lineEnd == LINEEND_CRLF)
				{
					outStr += inStr[pos];		// Insert the CR
					outStr += '\n';				// Insert the LF
					continue;
				}
				else if (lineEnd == LINEEND_LF)
				{
					outStr += '\n';				// Insert the LF
					continue;
				}
				else
				{
					outStr += inStr[pos];		// Do not change
					continue;
				}
			}
		}
		else if (inStr[pos] == '\n')
		{
			// LF
			if (lineEnd == LINEEND_CRLF)
			{
				outStr += '\r';				// Insert the CR
				outStr += inStr[pos];		// Insert the LF
				continue;
			}
			else if (lineEnd == LINEEND_CR)
			{
				outStr += '\r';				// Insert the CR
				continue;
			}
			else
			{
				outStr += inStr[pos];		// Do not change
				continue;
			}
		}
		else
		{
			outStr += inStr[pos];		// Write the current char
		}
	}
	// replace the stream
	out.str(outStr);
}

void ASConsole::correctMixedLineEnds(ostringstream& out)
{
	LineEndFormat lineEndFormat = LINEEND_DEFAULT;
	if (strcmp(outputEOL, "\r\n") == 0)
		lineEndFormat = LINEEND_WINDOWS;
	if (strcmp(outputEOL, "\n") == 0)
		lineEndFormat = LINEEND_LINUX;
	if (strcmp(outputEOL, "\r") == 0)
		lineEndFormat = LINEEND_MACOLD;
	convertLineEnds(out, lineEndFormat);
}

void ASConsole::error(const char *why, const char* what) const
{
	(*_err) << why << ' ' << what << '\n' << endl;
	(*_err) << "Artistic Style has terminated!" << endl;
	exit(EXIT_FAILURE);
}

/**
 * If no files have been given, use cin for input and cout for output.
 *
 * This is used to format text for text editors like TextWrangler (Mac).
 * Do NOT display any console messages when this function is used.
 */
void ASConsole::formatCinToCout(ASFormatter& formatter) const
{
	ASStreamIterator<istream> streamIterator(&cin);     // create iterator for cin
	formatter.init(&streamIterator);

	while (formatter.hasMoreLines())
	{
		cout << formatter.nextLine();
		if (formatter.hasMoreLines())
			cout << streamIterator.getOutputEOL();
	}
	cout.flush();
}

/**
 * Open input file, format it, and close the output.
 *
 * @param fileName      The path and name of the file to be processed.
 * @param formatter     The formatter object.
 */
void ASConsole::formatFile(const string &fileName, ASFormatter &formatter)
{
	bool isFormatted = false;

	// open input file
	ifstream in(fileName.c_str(), ios::binary);
	if (!in)
		error("Could not open input file", fileName.c_str());

	ostringstream out;

	// Unless a specific language mode has been set, set the language mode
	// according to the file's suffix.
	if (!formatter.getModeManuallySet())
	{
		if (stringEndsWith(fileName, string(".java")))
			formatter.setJavaStyle();
		else if (stringEndsWith(fileName, string(".cs")))
			formatter.setSharpStyle();
		else
			formatter.setCStyle();
	}

	ASStreamIterator<istream> streamIterator(&in);
	formatter.init(&streamIterator);

	// make sure encoding is 8 bit
	// if not set the eofbit so the file is not formatted
	FileEncoding encoding = getFileEncoding(in);
	if (encoding != ENCODING_OK)
		in.setstate(ios::eofbit);

	// set line end format
	string nextLine;				// next output line
	filesAreIdentical = true;		// input and output files are identical
	LineEndFormat lineEndFormat = formatter.getLineEndFormat();
	initializeOutputEOL(lineEndFormat);

	// format the file
	while (formatter.hasMoreLines())
	{
		nextLine = formatter.nextLine();
		out << nextLine;
		linesOut++;
		if (formatter.hasMoreLines())
		{
			setOutputEOL(lineEndFormat, streamIterator.getOutputEOL());
			out << outputEOL;
		}
		else
			streamIterator.saveLastInputLine();     // to compare the last input line

		if (filesAreIdentical)
		{
			if (streamIterator.checkForEmptyLine)
			{
				if (nextLine.find_first_not_of(" \t") != string::npos)
					filesAreIdentical = false;
			}
			else if (!streamIterator.compareToInputBuffer(nextLine))
				filesAreIdentical = false;
			streamIterator.checkForEmptyLine = false;
		}
	}
	// correct for mixed line ends
	if (lineEndsMixed)
	{
		correctMixedLineEnds(out);
		filesAreIdentical = false;
	}
	in.close();

	// if file has changed, write the new file
	if (!filesAreIdentical || streamIterator.getLineEndChange(lineEndFormat))
	{
		writeOutputFile(fileName, out);
		isFormatted = true;
		filesFormatted++;
	}
	else
		filesUnchanged++;

	if (encoding != ENCODING_OK)
		printBadEncoding(encoding);

	// remove targetDirectory from filename if required by print
	string displayName;
	if (hasWildcard)
		displayName = fileName.substr(targetDirectory.length() + 1);
	else
		displayName = fileName;

	if (isFormatted)
		printMsg("formatted  " + displayName);
	else
	{
		if (!isFormattedOnly || encoding != ENCODING_OK)
			printMsg("unchanged* " + displayName);
	}
}

// for unit testing
vector<bool> ASConsole::getExcludeHitsVector()
{
	return excludeHitsVector;
}

// for unit testing
vector<string> ASConsole::getExcludeVector()
{ return excludeVector; }

// for unit testing
vector<string> ASConsole::getFileName()
{ return fileName; }

// for unit testing
vector<string> ASConsole::getFileNameVector()
{ return fileNameVector; }

// for unit testing
vector<string> ASConsole::getFileOptionsVector()
{ return fileOptionsVector; }

int ASConsole::getFilesUnchanged()
{ return filesUnchanged; }

int ASConsole::getFilesFormatted()
{ return filesFormatted; }

bool ASConsole::getIsFormattedOnly()
{ return isFormattedOnly; }

bool ASConsole::getIsQuiet()
{ return isQuiet; }

bool ASConsole::getIsRecursive()
{ return isRecursive; }

bool ASConsole::getIsVerbose()
{ return isVerbose; }

bool ASConsole::getLineEndsMixed()
{ return lineEndsMixed; }

bool ASConsole::getNoBackup()
{ return noBackup; }

string ASConsole::getOptionsFileName()
{ return optionsFileName; }

bool ASConsole::getOptionsFileRequired()
{ return optionsFileRequired; }

// for unit testing
vector<string> ASConsole::getOptionsVector()
{ return optionsVector; }

string ASConsole::getOrigSuffix()
{ return origSuffix; }

bool ASConsole::getPreserveDate()
{ return preserveDate; }

// initialize output end of line
void ASConsole::initializeOutputEOL(LineEndFormat lineEndFormat)
{
	assert(lineEndFormat == LINEEND_DEFAULT
	       || lineEndFormat == LINEEND_WINDOWS
	       || lineEndFormat == LINEEND_LINUX
	       || lineEndFormat == LINEEND_MACOLD);

	outputEOL[0] = '\0';		// current line end
	prevEOL[0] = '\0';			// previous line end
	lineEndsMixed = false;		// output has mixed line ends, LINEEND_DEFAULT only

	if (lineEndFormat == LINEEND_WINDOWS)
		strcpy(outputEOL, "\r\n");
	else if (lineEndFormat == LINEEND_LINUX)
		strcpy(outputEOL, "\n");
	else if (lineEndFormat == LINEEND_MACOLD)
		strcpy(outputEOL, "\r");
	else
		outputEOL[0] = '\0';
}

void ASConsole::printBadEncoding(FileEncoding encoding) const
{
	string msg = "********** following file unchanged: ";
	if (encoding == UTF_16BE)
		msg += "UTF-16BE encoding";
	else if (encoding == UTF_16LE)
		msg += "UTF-16LE encoding";
	else if (encoding == UTF_32BE)
		msg += "UTF-32BE encoding";
	else if (encoding == UTF_32LE)
		msg += "UTF-32LE encoding";
	else
		msg += "???????? encoding";
	printMsg(msg);
}

void ASConsole::setIsFormattedOnly(bool state)
{ isFormattedOnly = state; }

void ASConsole::setIsQuiet(bool state)
{ isQuiet = state; }

void ASConsole::setIsRecursive(bool state)
{ isRecursive = state; }

void ASConsole::setIsVerbose(bool state)
{ isVerbose = state; }

void ASConsole::setNoBackup(bool state)
{ noBackup = state; }

void ASConsole::setOptionsFileName(string name)
{ optionsFileName = name; }

void ASConsole::setOptionsFileRequired(bool state)
{ optionsFileRequired = state; }

void ASConsole::setOrigSuffix(string suffix)
{ origSuffix = suffix; }

void ASConsole::setPreserveDate(bool state)
{ preserveDate = state; }

// set outputEOL variable
void ASConsole::setOutputEOL(LineEndFormat lineEndFormat, const char* currentEOL)
{
	if (lineEndFormat == LINEEND_DEFAULT)
	{
		strcpy(outputEOL, currentEOL);
		if (strlen(prevEOL) == 0)
			strcpy(prevEOL, outputEOL);
		if (strcmp(prevEOL, outputEOL) != 0)
		{
			lineEndsMixed = true;
			filesAreIdentical = false;
			strcpy(prevEOL, outputEOL);
		}
	}
	else
	{
		strcpy(prevEOL, currentEOL);
		if (strcmp(prevEOL, outputEOL) != 0)
			filesAreIdentical = false;
	}
}

#ifdef _WIN32  // Windows specific

/**
 * WINDOWS function to get the current directory.
 * NOTE: getenv("CD") does not work for Windows Vista.
 *        The Windows function GetCurrentDirectory is used instead.
 *
 * @return              The path of the current directory
 */
string ASConsole::getCurrentDirectory(const string &fileName) const
{
	char currdir[MAX_PATH];
	currdir[0] = '\0';
	if (!GetCurrentDirectory(sizeof(currdir), currdir))
		error("Cannot find file", fileName.c_str());
	return string(currdir);
}

/**
 * WINDOWS function to resolve wildcards and recurse into sub directories.
 * The fileName vector is filled with the path and names of files to process.
 *
 * @param directory     The path of the directory to be processed.
 * @param wildcard      The wildcard to be processed (e.g. *.cpp).
 * @param fileName      An empty vector which will be filled with the path and names of files to process.
 */
void ASConsole::getFileNames(const string &directory, const string &wildcard)
{
	vector<string> subDirectory;    // sub directories of directory
	WIN32_FIND_DATA FindFileData;   // for FindFirstFile and FindNextFile

	// Find the first file in the directory
	string firstFile = directory + "\\*";
	HANDLE hFind = FindFirstFile(firstFile.c_str(), &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		error("Cannot open directory", directory.c_str());

	// save files and sub directories
	do
	{
		// skip hidden or read only
		if (FindFileData.cFileName[0] == '.'
		        || (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		        || (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
			continue;

		// if a sub directory and recursive, save sub directory
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && isRecursive)
		{
			string subDirectoryPath = directory + g_fileSeparator + FindFileData.cFileName;
			if (isPathExclued(subDirectoryPath))
				printMsg("exclude " + subDirectoryPath.substr(mainDirectoryLength));
			else
				subDirectory.push_back(subDirectoryPath);
			continue;
		}

		// save the file name
		string filePathName = directory + g_fileSeparator + FindFileData.cFileName;
		// check exclude before wildcmp to avoid "unmatched exclude" error
		bool isExcluded = isPathExclued(filePathName);
		// save file name if wildcard match
		if (wildcmp(wildcard.c_str(), FindFileData.cFileName))
		{
			if (isExcluded)
				printMsg("exclude " + filePathName.substr(mainDirectoryLength));
			else
				fileName.push_back(filePathName);
		}
	}
	while (FindNextFile(hFind, &FindFileData) != 0);

	// check for processing error
	FindClose(hFind);
	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
		error("Error processing directory", directory.c_str());

	// recurse into sub directories
	// if not doing recursive subDirectory is empty
	for (unsigned i = 0; i < subDirectory.size(); i++)
	{
		getFileNames(subDirectory[i], wildcard);
	}

	return;
}

#else  // not _WIN32

/**
 * LINUX function to get the current directory.
 * This is done if the fileName does not contain a path.
 * It is probably from an editor sending a single file.
 *
 * @param fileName      The filename is used only for  the error message.
 * @return              The path of the current directory
 */
string ASConsole::getCurrentDirectory(const string &fileName) const
{
	char *currdir = getenv("PWD");
	if (currdir == NULL)
		error("Cannot find file", fileName.c_str());
	return string(currdir);
}

/**
 * LINUX function to resolve wildcards and recurse into sub directories.
 * The fileName vector is filled with the path and names of files to process.
 *
 * @param directory     The path of the directory to be processed.
 * @param wildcard      The wildcard to be processed (e.g. *.cpp).
 * @param fileName      An empty vector which will be filled with the path and names of files to process.
 */
void ASConsole::getFileNames(const string &directory, const string &wildcard)
{
	struct dirent *entry;           // entry from readdir()
	struct stat statbuf;            // entry from stat()
	vector<string> subDirectory;    // sub directories of this directory

	// errno is defined in <errno.h> and is set for errors in opendir, readdir, or stat
	errno = 0;

	DIR *dp = opendir(directory.c_str());
	if (dp == NULL)
		error("Cannot open directory", directory.c_str());

	// save the first fileName entry for this recursion
	const unsigned firstEntry = fileName.size();

	// save files and sub directories
	while ((entry = readdir(dp)) != NULL)
	{
		// get file status
		string entryFilepath = directory + g_fileSeparator + entry->d_name;
		if (stat(entryFilepath.c_str(), &statbuf) != 0)
		{
			if (errno == EOVERFLOW)         // file over 2 GB is OK
			{
				errno = 0;
				continue;
			}
			perror("errno message");
			error("Error getting file status in directory", directory.c_str());
		}
		// skip hidden or read only
		if (entry->d_name[0] == '.' || !(statbuf.st_mode & S_IWUSR))
			continue;
		// if a sub directory and recursive, save sub directory
		if (S_ISDIR(statbuf.st_mode) && isRecursive)
		{
			if (isPathExclued(entryFilepath))
				printMsg("exclude " + entryFilepath.substr(mainDirectoryLength));
			else
				subDirectory.push_back(entryFilepath);
			continue;
		}

		// if a file, save file name
		if (S_ISREG(statbuf.st_mode))
		{
			// check exclude before wildcmp to avoid "unmatched exclude" error
			bool isExcluded = isPathExclued(entryFilepath);
			// save file name if wildcard match
			if (wildcmp(wildcard.c_str(), entry->d_name))
			{
				if (isExcluded)
					printMsg("exclude " + entryFilepath.substr(mainDirectoryLength));
				else
					fileName.push_back(entryFilepath);
			}
		}
	}

	if (closedir(dp) != 0)
	{
		perror("errno message");
		error("Error reading directory", directory.c_str());
	}

	// sort the current entries for fileName
	if (firstEntry < fileName.size())
		sort(&fileName[firstEntry], &fileName[fileName.size()]);

	// recurse into sub directories
	// if not doing recursive, subDirectory is empty
	if (subDirectory.size() > 1)
		sort(subDirectory.begin(), subDirectory.end());
	for (unsigned i = 0; i < subDirectory.size(); i++)
	{
		getFileNames(subDirectory[i], wildcard);
	}

	return;
}

#endif  // _WIN32

// check files for 16 or 32 bit encoding
// the file must have a Byte Order Mark (BOM)
// NOTE: some string functions don't work with NULLs (e.g. length())
FileEncoding ASConsole::getFileEncoding(ifstream& in) const
{
	// BOM max is 4 bytes
	char buff[4] = {'\0'};
	in.read(buff, 4);
	in.seekg(0);

	FileEncoding encoding = ENCODING_OK;

	if (memcmp(buff, "\x00\x00\xFE\xFF", 4) == 0)
		encoding = UTF_32BE;
	else if (memcmp(buff, "\xFF\xFE\x00\x00", 4) == 0)
		encoding = UTF_32LE;
	else if (memcmp(buff, "\xFE\xFF", 2) == 0)
		encoding = UTF_16BE;
	else if (memcmp(buff, "\xFF\xFE", 2) == 0)
		encoding = UTF_16LE;

	return encoding;
}

// get individual file names from the command-line file path
void ASConsole::getFilePaths(string &filePath)
{
	fileName.clear();
	targetDirectory = string();
	targetFilename = string();

	// separate directory and file name
	size_t separator = filePath.find_last_of(g_fileSeparator);
	if (separator == string::npos)
	{
		// if no directory is present, use the currently active directory
		targetDirectory = getCurrentDirectory(filePath);
		targetFilename  = filePath;
		mainDirectoryLength = targetDirectory.length() + 1;    // +1 includes trailing separator
	}
	else
	{
		targetDirectory = filePath.substr(0, separator);
		targetFilename  = filePath.substr(separator+1);
		mainDirectoryLength = targetDirectory.length() + 1;    // +1 includes trailing separator
	}

	if (targetFilename.length() == 0)
		error("Missing filename in", filePath.c_str());

	// check filename for wildcards
	hasWildcard = false;
	if (targetFilename.find_first_of( "*?") != string::npos)
		hasWildcard = true;

	// clear exclude hits vector
	for (size_t ix = 0; ix < excludeHitsVector.size(); ix++)
		excludeHitsVector[ix] = false;

	// display directory name for wildcard processing
	if (hasWildcard)
	{
		printMsg("--------------------------------------------------");
		printMsg("directory " + targetDirectory + g_fileSeparator + targetFilename);
	}

	// create a vector of paths and file names to process
	if (hasWildcard || isRecursive)
		getFileNames(targetDirectory, targetFilename);
	else
		fileName.push_back(targetDirectory + g_fileSeparator + targetFilename);

	if (hasWildcard)
		printMsg("--------------------------------------------------");

	// check for unprocessed excludes
	bool excludeErr = false;
	for (size_t ix = 0; ix < excludeHitsVector.size(); ix++)
	{
		if (excludeHitsVector[ix] == false)
		{
			(*_err) << "Unmatched exclude " << excludeVector[ix].c_str() << endl;
			excludeErr = true;
		}
	}
#ifndef ASTYLECON_LIB
	// abort if not a test
	if (excludeErr)
		exit(EXIT_FAILURE);
#endif
	// check if files were found (probably an input error if not)
	if (fileName.size() == 0)
		(*_err) << "No file to process " << filePath.c_str() << endl;
}

bool ASConsole::fileNameVectorIsEmpty()
{
	return fileNameVector.empty();
}

// compare a path to the exclude vector
// used for both directories and filenames
// updates the g_excludeHitsVector
// return true if a match
bool ASConsole::isPathExclued(const string &subPath)
{
	bool retVal = false;

	// read the exclude vector checking for a match
	for (size_t i = 0; i < excludeVector.size(); i++)
	{
		string exclude = excludeVector[i];

		if (subPath.length() < exclude.length())
			continue;

		size_t compareStart = subPath.length() - exclude.length();
		// subPath compare must start with a directory name
		if (compareStart > 0)
		{
			char lastPathChar = subPath[compareStart - 1];
			if (lastPathChar != g_fileSeparator)
				continue;
		}

		string compare = subPath.substr(compareStart);
		if (!g_isCaseSensitive)
		{
			// make it case insensitive for Windows
			for (size_t j=0; j<compare.length(); j++)
				compare[j] = (char)tolower(compare[j]);
			for (size_t j=0; j<exclude.length(); j++)
				exclude[j] = (char)tolower(exclude[j]);
		}
		// compare sub directory to exclude data - must check them all
		if (compare == exclude)
		{
			excludeHitsVector[i] = true;
			retVal = true;
			break;
		}
	}
	return retVal;
}

void ASConsole::printHelp() const
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
	(*_err) << "Wildcards (* and ?) may be used in the filename.\n";
	(*_err) << "A \'recursive\' option can process directories recursively.\n";
	(*_err) << endl;
	(*_err) << "By default, astyle is set up to indent C/C++/C#/Java files, with four\n";
	(*_err) << "spaces per indent, a maximal indentation of 40 spaces inside continuous\n";
	(*_err) << "statements, a minimum indentation of eight spaces inside conditional\n";
	(*_err) << "statements, and NO formatting options.\n";
	(*_err) << endl;
	(*_err) << "Option's Format:\n";
	(*_err) << "----------------\n";
	(*_err) << "    Long options (starting with '--') must be written one at a time.\n";
	(*_err) << "    Short options (starting with '-') may be appended together.\n";
	(*_err) << "    Thus, -bps4 is the same as -b -p -s4.\n";
	(*_err) << endl;
	(*_err) << "Predefined Style Options:\n";
	(*_err) << "-------------------------\n";
	(*_err) << "    --style=allman  OR  --style=ansi  OR  --style=bsd  OR  -A1\n";
	(*_err) << "    Allman style formatting/indenting.\n";
	(*_err) << "    Broken brackets.\n";
	(*_err) << endl;
	(*_err) << "    --style=java  OR  -A2\n";
	(*_err) << "    Java style formatting/indenting.\n";
	(*_err) << "    Attached brackets.\n";
	(*_err) << endl;
	(*_err) << "    --style=k&r  OR  --style=k/r  OR  -A3\n";
	(*_err) << "    Kernighan & Ritchie style formatting/indenting.\n";
	(*_err) << "    Linux brackets.\n";
	(*_err) << endl;
	(*_err) << "    --style=stroustrup  OR  -A4\n";
	(*_err) << "    Stroustrup style formatting/indenting.\n";
	(*_err) << "    Stroustrup brackets.\n";
	(*_err) << endl;
	(*_err) << "    --style=whitesmith  OR  -A5\n";
	(*_err) << "    Whitesmith style formatting/indenting.\n";
	(*_err) << "    Broken, indented brackets.\n";
	(*_err) << "    Indented class blocks and switch blocks.\n";
	(*_err) << endl;
	(*_err) << "    --style=banner  OR  -A6\n";
	(*_err) << "    Banner style formatting/indenting.\n";
	(*_err) << "    Attached, indented brackets.\n";
	(*_err) << "    Indented class blocks and switch blocks.\n";
	(*_err) << endl;
	(*_err) << "    --style=gnu  OR  -A7\n";
	(*_err) << "    GNU style formatting/indenting.\n";
	(*_err) << "    Broken brackets, indented blocks, indent is 2 spaces.\n";
	(*_err) << endl;
	(*_err) << "    --style=linux  OR  -A8\n";
	(*_err) << "    GNU style formatting/indenting.\n";
	(*_err) << "    Linux brackets, indent is 8 spaces.\n";
	(*_err) << endl;
	(*_err) << "    --style=horstmann  OR  -A9\n";
	(*_err) << "    Horstmann style formatting/indenting.\n";
	(*_err) << "    Horstmann brackets, indented switches, indent is 3 spaces.\n";
	(*_err) << endl;
	(*_err) << "    --style=1tbs  OR  --style=otbs  OR  -A10\n";
	(*_err) << "    One True Brace Style formatting/indenting.\n";
	(*_err) << "    Linux brackets, add brackets to all conditionals.\n";
	(*_err) << endl;
	(*_err) << "Tab and Bracket Options:\n";
	(*_err) << "------------------------\n";
	(*_err) << "    default indent option\n";
	(*_err) << "    If no indentation option is set,\n";
	(*_err) << "    the default option of 4 spaces will be used.\n";
	(*_err) << endl;
	(*_err) << "    --indent=spaces=#  OR  -s#\n";
	(*_err) << "    Indent using # spaces per indent. Not specifying #\n";
	(*_err) << "    will result in a default of 4 spaces per indent.\n";
	(*_err) << endl;
	(*_err) << "    --indent=tab  OR  --indent=tab=#  OR  -t  OR  -t#\n";
	(*_err) << "    Indent using tab characters, assuming that each\n";
	(*_err) << "    tab is # spaces long. Not specifying # will result\n";
	(*_err) << "    in a default assumption of 4 spaces per tab.\n";
	(*_err) << endl;
	(*_err) << "    --indent=force-tab=#  OR  -T#\n";
	(*_err) << "    Indent using tab characters, assuming that each\n";
	(*_err) << "    tab is # spaces long. Force tabs to be used in areas\n";
	(*_err) << "    Astyle would prefer to use spaces.\n";
	(*_err) << endl;
	(*_err) << "    default brackets option\n";
	(*_err) << "    If no brackets option is set,\n";
	(*_err) << "    the brackets will not be changed.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=break  OR  -b\n";
	(*_err) << "    Break brackets from pre-block code (i.e. ANSI C/C++ style).\n";
	(*_err) << endl;
	(*_err) << "    --brackets=attach  OR  -a\n";
	(*_err) << "    Attach brackets to pre-block code (i.e. Java/K&R style).\n";
	(*_err) << endl;
	(*_err) << "    --brackets=linux  OR  -l\n";
	(*_err) << "    Break definition-block brackets and attach command-block\n";
	(*_err) << "    brackets.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=stroustrup  OR  -u\n";
	(*_err) << "    Attach all brackets except function definition brackets.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=horstmann  OR  -g\n";
	(*_err) << "    Break brackets from pre-block code, but allow following\n";
	(*_err) << "    run-in statements on the same line as an opening bracket.\n";
	(*_err) << endl;
	(*_err) << "Indentation options:\n";
	(*_err) << "--------------------\n";
	(*_err) << "    --indent-classes  OR  -C\n";
	(*_err) << "    Indent 'class' blocks, so that the inner 'public:',\n";
	(*_err) << "    'protected:' and 'private: headers are indented in\n";
	(*_err) << "    relation to the class block.\n";
	(*_err) << endl;
	(*_err) << "    --indent-switches  OR  -S\n";
	(*_err) << "    Indent 'switch' blocks, so that the inner 'case XXX:'\n";
	(*_err) << "    headers are indented in relation to the switch block.\n";
	(*_err) << endl;
	(*_err) << "    --indent-cases  OR  -K\n";
	(*_err) << "    Indent case blocks from the 'case XXX:' headers.\n";
	(*_err) << "    Case statements not enclosed in blocks are NOT indented.\n";
	(*_err) << endl;
	(*_err) << "    --indent-brackets  OR  -B\n";
	(*_err) << "    Add extra indentation to '{' and '}' block brackets.\n";
	(*_err) << endl;
	(*_err) << "    --indent-blocks  OR  -G\n";
	(*_err) << "    Add extra indentation entire blocks (including brackets).\n";
	(*_err) << endl;
	(*_err) << "    --indent-namespaces  OR  -N\n";
	(*_err) << "    Indent the contents of namespace blocks.\n";
	(*_err) << endl;
	(*_err) << "    --indent-labels  OR  -L\n";
	(*_err) << "    Indent labels so that they appear one indent less than\n";
	(*_err) << "    the current indentation level, rather than being\n";
	(*_err) << "    flushed completely to the left (which is the default).\n";
	(*_err) << endl;
	(*_err) << "    --indent-preprocessor  OR  -w\n";
	(*_err) << "    Indent multi-line #define statements.\n";
	(*_err) << endl;
	(*_err) << "    --indent-col1-comments  OR  -Y\n";
	(*_err) << "    Indent line comments that start in column one.\n";
	(*_err) << endl;
	(*_err) << "    --max-instatement-indent=#  OR  -M#\n";
	(*_err) << "    Indent a maximal # spaces in a continuous statement,\n";
	(*_err) << "    relative to the previous line.\n";
	(*_err) << endl;
	(*_err) << "    --min-conditional-indent=#  OR  -m#\n";
	(*_err) << "    Indent a minimal # spaces in a continuous conditional\n";
	(*_err) << "    belonging to a conditional header.\n";
	(*_err) << endl;
	(*_err) << "Padding options:\n";
	(*_err) << "--------------------\n";
	(*_err) << "    --break-blocks  OR  -f\n";
	(*_err) << "    Insert empty lines around unrelated blocks, labels, classes, ...\n";
	(*_err) << endl;
	(*_err) << "    --break-blocks=all  OR  -F\n";
	(*_err) << "    Like --break-blocks, except also insert empty lines \n";
	(*_err) << "    around closing headers (e.g. 'else', 'catch', ...).\n";
	(*_err) << endl;
	(*_err) << "    --pad-oper  OR  -p\n";
	(*_err) << "    Insert space paddings around operators.\n";
	(*_err) << endl;
	(*_err) << "    --pad-paren  OR  -P\n";
	(*_err) << "    Insert space padding around parenthesis on both the outside\n";
	(*_err) << "    and the inside.\n";
	(*_err) << endl;
	(*_err) << "    --pad-paren-out  OR  -d\n";
	(*_err) << "    Insert space padding around parenthesis on the outside only.\n";
	(*_err) << endl;
	(*_err) << "    --pad-paren-in  OR  -D\n";
	(*_err) << "    Insert space padding around parenthesis on the inside only.\n";
	(*_err) << endl;
	(*_err) << "    --pad-header  OR  -H\n";
	(*_err) << "    Insert space padding after paren headers (e.g. 'if', 'for'...).\n";
	(*_err) << endl;
	(*_err) << "    --unpad-paren  OR  -U\n";
	(*_err) << "    Remove unnecessary space padding around parenthesis.  This\n";
	(*_err) << "    can be used in combination with the 'pad' options above.\n";
	(*_err) << endl;
	(*_err) << "    --delete-empty-lines  OR  -x\n";
	(*_err) << "    Delete empty lines within a function or method.\n";
	(*_err) << "    It will NOT delete lines added by the break-blocks options.\n";
	(*_err) << endl;
	(*_err) << "    --fill-empty-lines  OR  -E\n";
	(*_err) << "    Fill empty lines with the white space of their\n";
	(*_err) << "    previous lines.\n";
	(*_err) << endl;
	(*_err) << "Formatting options:\n";
	(*_err) << "-------------------\n";
	(*_err) << "    --break-closing-brackets  OR  -y\n";
	(*_err) << "    Break brackets before closing headers (e.g. 'else', 'catch', ...).\n";
	(*_err) << "    Use with --brackets=attach, --brackets=linux, \n";
	(*_err) << "    or --brackets=stroustrup.\n";
	(*_err) << endl;
	(*_err) << "    --break-elseifs  OR  -e\n";
	(*_err) << "    Break 'else if()' statements into two different lines.\n";
	(*_err) << endl;
	(*_err) << "    --add-brackets  OR  -j\n";
	(*_err) << "    Add brackets to unbracketed one line conditional statements.\n";
	(*_err) << endl;
	(*_err) << "    --add-one-line-brackets  OR  -J\n";
	(*_err) << "    Add one line brackets to unbracketed one line conditional\n";
	(*_err) << "    statements.\n";
	(*_err) << endl;
	(*_err) << "    --keep-one-line-blocks  OR  -O\n";
	(*_err) << "    Don't break blocks residing completely on one line.\n";
	(*_err) << endl;
	(*_err) << "    --keep-one-line-statements  OR  -o\n";
	(*_err) << "    Don't break lines containing multiple statements into\n";
	(*_err) << "    multiple single-statement lines.\n";
	(*_err) << endl;
	(*_err) << "    --convert-tabs  OR  -c\n";
	(*_err) << "    Convert tabs to the appropriate number of spaces.\n";
	(*_err) << endl;
	(*_err) << "    --align-pointer=type    OR  -k1\n";
	(*_err) << "    --align-pointer=middle  OR  -k2\n";
	(*_err) << "    --align-pointer=name    OR  -k3\n";
	(*_err) << "    Attach a pointer or reference operator (* or &) to either\n";
	(*_err) << "    the operator type (left), middle, or operator name (right).\n";
	(*_err) << endl;
	(*_err) << "    --mode=c\n";
	(*_err) << "    Indent a C or C++ source file (this is the default).\n";
	(*_err) << endl;
	(*_err) << "    --mode=java\n";
	(*_err) << "    Indent a Java source file.\n";
	(*_err) << endl;
	(*_err) << "    --mode=cs\n";
	(*_err) << "    Indent a C# source file.\n";
	(*_err) << endl;
	(*_err) << "Other options:\n";
	(*_err) << "--------------\n";
	(*_err) << "    --suffix=####\n";
	(*_err) << "    Append the suffix #### instead of '.orig' to original filename.\n";
	(*_err) << endl;
	(*_err) << "    --suffix=none  OR  -n\n";
	(*_err) << "    Do not retain a backup of the original file.\n";
	(*_err) << endl;
	(*_err) << "    --options=####\n";
	(*_err) << "    Specify an options file #### to read and use.\n";
	(*_err) << endl;
	(*_err) << "    --options=none\n";
	(*_err) << "    Disable the default options file.\n";
	(*_err) << "    Only the command-line parameters will be used.\n";
	(*_err) << endl;
	(*_err) << "    --recursive  OR  -r  OR  -R\n";
	(*_err) << "    Process subdirectories recursively.\n";
	(*_err) << endl;
	(*_err) << "    --exclude=####\n";
	(*_err) << "    Specify a file or directory #### to be excluded from processing.\n";
	(*_err) << endl;
	(*_err) << "    --errors-to-stdout  OR  -X\n";
	(*_err) << "    Print errors and help information to standard-output rather than\n";
	(*_err) << "    to standard-error.\n";
	(*_err) << endl;
	(*_err) << "    --preserve-date  OR  -Z\n";
	(*_err) << "    The date and time modified will not be changed in the formatted file.\n";
	(*_err) << endl;
	(*_err) << "    --verbose  OR  -v\n";
	(*_err) << "    Verbose mode. Extra informational messages will be displayed.\n";
	(*_err) << endl;
	(*_err) << "    --formatted  OR  -Q\n";
	(*_err) << "    Formatted display mode. Display only the files that have been formatted.\n";
	(*_err) << endl;
	(*_err) << "    --quiet  OR  -q\n";
	(*_err) << "    Quiet mode. Suppress all output except error messages.\n";
	(*_err) << endl;
	(*_err) << "    --lineend=windows  OR  -z1\n";
	(*_err) << "    --lineend=linux    OR  -z2\n";
	(*_err) << "    --lineend=macold   OR  -z3\n";
	(*_err) << "    Force use of the specified line end style. Valid options\n";
	(*_err) << "    are windows (CRLF), linux (LF), and macold (CR).\n";
	(*_err) << endl;
	(*_err) << "    --version  OR  -V\n";
	(*_err) << "    Print version number.\n";
	(*_err) << endl;
	(*_err) << "    --help  OR  -h  OR  -?\n";
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


/**
 * Process files in the fileNameVector.
 *
 * @param formatter     The formatter object.
 */
void ASConsole::processFiles(ASFormatter &formatter)
{
	if (isVerbose)
		printVerboseHeader();

	clock_t startTime = clock();     // start time of file formatting

	// loop thru input fileNameVector and process the files
	for (size_t i = 0; i < fileNameVector.size(); i++)
	{
		getFilePaths(fileNameVector[i]);

		// loop thru fileName vector formatting the files
		for (size_t j = 0; j < fileName.size(); j++)
			formatFile(fileName[j], formatter);
	}

	// files are processed, display stats
	if (isVerbose)
		printVerboseStats(startTime);

}

// process options from the command line and options file
// build the vectors fileNameVector, excludeVector, optionsVector, and fileOptionsVector
processReturn ASConsole::processOptions(int argc, char** argv, ASFormatter &formatter)
{
	string arg;
	bool ok = true;
	bool shouldParseOptionsFile = true;

	// get command line options
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
			optionsFileRequired = true;
			if (optionsFileName.compare("") == 0)
				setOptionsFileName(" ");
		}
		else if ( IS_OPTION(arg, "-h")
		          || IS_OPTION(arg, "--help")
		          || IS_OPTION(arg, "-?") )
		{
			printHelp();
			return(END_SUCCESS);
		}
		else if ( IS_OPTION(arg, "-V" )
		          || IS_OPTION(arg, "--version") )
		{
			(*_err) << "Artistic Style Version " << _version << endl;
			return(END_SUCCESS);
		}
		else if (arg[0] == '-')
		{
			optionsVector.push_back(arg);
		}
		else // file-name
		{
			standardizePath(arg);
			fileNameVector.push_back(arg);
		}
	}

	// get options file path and name
	if (shouldParseOptionsFile)
	{
		if (optionsFileName.compare("") == 0)
		{
			char* env = getenv("ARTISTIC_STYLE_OPTIONS");
			if (env != NULL)
				setOptionsFileName(env);
		}
		if (optionsFileName.compare("") == 0)
		{
			char* env = getenv("HOME");
			if (env != NULL)
				setOptionsFileName(string(env) + "/.astylerc");
		}
		if (optionsFileName.compare("") == 0)
		{
			char* env = getenv("USERPROFILE");
			if (env != NULL)
				setOptionsFileName(string(env) + "/astylerc");
		}
		if (optionsFileName.compare("") != 0)
			standardizePath(optionsFileName);
	}

	// create the options file vector and parse the options for errors
	if (optionsFileName.compare("") != 0)
	{
		ifstream optionsIn(optionsFileName.c_str());
		if (optionsIn)
		{
			importOptions(optionsIn, fileOptionsVector);
			ok = parseOptions(formatter,
			                  fileOptionsVector.begin(),
			                  fileOptionsVector.end(),
			                  string("Invalid option in default options file: "));
		}
		else
		{
			if (optionsFileRequired)
			{
				(*_err) << "Could not open options file: " << optionsFileName.c_str() << endl;
				return (END_FAILURE);
			}
			optionsFileName.clear();
		}
		optionsIn.close();
	}
	if (!ok)
	{
		(*_err) << "For help on options, type 'astyle -h' " << endl;
		return(END_FAILURE);
	}

	// parse the command line options vector for errors
	ok = parseOptions(formatter,
	                  optionsVector.begin(),
	                  optionsVector.end(),
	                  string("Invalid command line option: "));
	if (!ok)
	{
		(*_err) << "For help on options, type 'astyle -h' \n" << endl;
		return(END_FAILURE);
	}
	return(CONTINUE);
}

// remove a file and check for an error
void ASConsole::removeFile(const char* fileName, const char* errMsg) const
{
	remove(fileName);
	if (errno == ENOENT)        // no file is OK
		errno = 0;
	if (errno)
	{
		perror("errno message");
		error(errMsg, fileName);
	}
}

// rename a file and check for an error
void ASConsole::renameFile(const char* oldFileName, const char* newFileName, const char* errMsg) const
{
	rename(oldFileName, newFileName);
	// if file still exists the remove needs more time - retry
	if (errno == EEXIST)
	{
		errno = 0;
		waitForRemove(newFileName);
		rename(oldFileName, newFileName);
	}
	if (errno)
	{
		perror("errno message");
		error(errMsg, oldFileName);
	}
}

// make sure file separators are correct type (Windows or Linux)
// remove ending file separator
// remove beginning file separator if requested and NOT a complete file path
void ASConsole::standardizePath(string &path, bool removeBeginningSeparator /*false*/) const
{
#ifdef __VMS
	struct FAB fab;
	struct NAML naml;
	char less[NAML$C_MAXRSS];
	char sess[NAM$C_MAXRSS];
	int r0_status;

	// If we are on a VMS system, translate VMS style filenames to unix
	// style.
	fab = cc$rms_fab;
	fab.fab$l_fna = (char *)-1;
	fab.fab$b_fns = 0;
	fab.fab$l_naml = &naml;
	naml = cc$rms_naml;
	strcpy (sess, path.c_str());
	naml.naml$l_long_filename = (char *)sess;
	naml.naml$l_long_filename_size = path.length();
	naml.naml$l_long_expand = less;
	naml.naml$l_long_expand_alloc = sizeof (less);
	naml.naml$l_esa = sess;
	naml.naml$b_ess = sizeof (sess);
	naml.naml$v_no_short_upcase = 1;
	r0_status = sys$parse (&fab);
	if (r0_status == RMS$_SYN)
	{
		error("File syntax error", path.c_str());
	}
	else
	{
		if (!$VMS_STATUS_SUCCESS(r0_status))
		{
			(void)lib$signal (r0_status);
		}
	}
	less[naml.naml$l_long_expand_size - naml.naml$b_ver] = '\0';
	sess[naml.naml$b_esl - naml.naml$b_ver] = '\0';
	if (naml.naml$l_long_expand_size > naml.naml$b_esl)
	{
		path = decc$translate_vms (less);
	}
	else
	{
		path = decc$translate_vms (sess);
	}
#endif /* __VMS */

	// make sure separators are correct type (Windows or Linux)
	for (size_t i = 0; i < path.length(); i++)
	{
		i = path.find_first_of("/\\", i);
		if (i == string::npos)
			break;
		path[i] = g_fileSeparator;
	}
	// remove separator from the end
	if (path[path.length()-1] == g_fileSeparator)
		path.erase(path.length()-1, 1);
	// remove beginning separator if requested
	if (removeBeginningSeparator && (path[0] == g_fileSeparator))
		path.erase(0, 1);
}

void ASConsole::printMsg(const string &msg) const
{
	if (isQuiet)
		return;
	cout << msg << endl;
}

void ASConsole::printVerboseHeader() const
{
	assert(isVerbose);
	if (isQuiet)
		return;
	cout << "Artistic Style " << _version << endl;
	if (optionsFileName.compare("") != 0)
		cout << "Using default options file " << optionsFileName << endl;
}

void ASConsole::printVerboseStats(clock_t startTime) const
{
	assert(isVerbose);
	if (isQuiet)
		return;
	if (hasWildcard)
		cout << "--------------------------------------------------" << endl;
	cout << filesFormatted << " formatted, ";
	cout << filesUnchanged << " unchanged, ";

	// show processing time
	clock_t stopTime = clock();
	float secs = float ((stopTime - startTime) / CLOCKS_PER_SEC);
	if (secs < 60)
	{
		// show tenths of a second if time is less than 20 seconds
		cout.precision(2);
		if (secs >= 10 && secs < 20)
			cout.precision(3);
		cout << secs << " seconds, ";
		cout.precision(0);
	}
	else
	{
		// show minutes and seconds if time is greater than one minute
		int min = (int) secs / 60;
		secs -= min * 60;
		int minsec = int (secs + .5);
		cout << min << " min " << minsec << " sec, ";
	}

	cout << linesOut << " lines" << endl;
}

bool ASConsole::stringEndsWith(const string &str, const string &suffix) const
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
	// suffix longer than string
	if (strIndex < 0 && suffixIndex >= 0)
		return false;
	return true;
}

void ASConsole::updateExcludeVector(string suffixParam)
{
	excludeVector.push_back(suffixParam);
	standardizePath(excludeVector.back(), true);
	excludeHitsVector.push_back(false);
}

void ASConsole::sleep(int seconds) const
{
	clock_t endwait;
	endwait = clock_t (clock () + seconds * CLOCKS_PER_SEC);
	while (clock() < endwait) {}
}

int ASConsole::waitForRemove(const char* newFileName) const
{
	struct stat stBuf;
	int seconds;
	// sleep a max of 20 seconds for the remove
	for (seconds = 0; seconds < 20; seconds++)
	{
		sleep(1);
		if (stat(newFileName, &stBuf) != 0)
			break;
	}
	errno = 0;
	return seconds;
}

// From The Code Project http://www.codeproject.com/string/wildcmp.asp
// Written by Jack Handy - jakkhandy@hotmail.com
// Modified to compare case insensitive for Windows
int ASConsole::wildcmp(const char *wild, const char *data) const
{
	const char *cp = NULL, *mp = NULL;
	bool cmpval;

	while ((*data) && (*wild != '*'))
	{
		if (!g_isCaseSensitive)
			cmpval = (tolower(*wild) != tolower(*data)) && (*wild != '?');
		else
			cmpval = (*wild != *data) && (*wild != '?');

		if (cmpval)
		{
			return 0;
		}
		wild++;
		data++;
	}

	while (*data)
	{
		if (*wild == '*')
		{
			if (!*++wild)
			{
				return 1;
			}
			mp = wild;
			cp = data+1;
		}
		else
		{
			if (!g_isCaseSensitive)
				cmpval = (tolower(*wild) == tolower(*data) || (*wild == '?'));
			else
				cmpval = (*wild == *data) || (*wild == '?');

			if (cmpval)
			{
				wild++;
				data++;
			}
			else
			{
				wild = mp;
				data = cp++;
			}
		}
	}

	while (*wild == '*')
	{
		wild++;
	}
	return !*wild;
}

void ASConsole::writeOutputFile(const string &fileName, ostringstream &out) const
{
	// save date accessed and date modified of original file
	struct stat stBuf;
	bool statErr = false;
	if (stat(fileName.c_str(), &stBuf) == -1)
		statErr = true;

	// create a backup
	if (!noBackup)
	{
		string origFileName = fileName + origSuffix;
		removeFile(origFileName.c_str(), "Could not remove pre-existing backup file");
		renameFile(fileName.c_str(), origFileName.c_str(), "Could not create backup file");
	}

	// write the output file
	ofstream fout(fileName.c_str(), ios::binary | ios::trunc);
	if (!fout)
		error("Could not open output file", fileName.c_str());
	fout << out.str();
	fout.close();

	// change date modified to original file date
	if (preserveDate)
	{
		if (!statErr)
		{
			struct utimbuf outBuf;
			outBuf.actime = stBuf.st_atime;
			// add ticks so 'make' will recoginze a change
			// Visual Studio 2008 needs more than 1
			outBuf.modtime = stBuf.st_mtime + 10;
			if (utime(fileName.c_str(), &outBuf) == -1)
				statErr = true;
		}
		if (statErr)
			(*_err) << "*********  could not preserve following file date" << endl;
	}
}

#endif
// *******************   end of console functions   ***********************************************

}   // end of namespace astyle

// *******************   end of astyle namespace    ***********************************************

using namespace astyle;

#ifdef ASTYLE_JNI
// *************************   JNI functions   *****************************************************
// called by a java program to get the version number
// the function name is constructed from method names in the calling java program
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleGetVersion(JNIEnv* env, jclass)
{
	return env->NewStringUTF(_version);
}

// called by a java program to format the source code
// the function name is constructed from method names in the calling java program
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleMain(JNIEnv* env,
        jobject obj,
        jstring textInJava,
        jstring optionsJava)
{
	g_env = env;                                // make object available globally
	g_obj = obj;                                // make object available globally

	jstring textErr = env->NewStringUTF("");    // zero length text returned if an error occurs

	// get the method ID
	jclass cls = env->GetObjectClass(obj);
	g_mid = env->GetMethodID(cls, "ErrorHandler","(ILjava/lang/String;)V");
	if (g_mid == 0)
	{
		cout << "Cannot find java method ErrorHandler" << endl;
		return textErr;
	}

	// convert jstring to char*
	const char* textIn = env->GetStringUTFChars(textInJava, NULL);
	const char* options = env->GetStringUTFChars(optionsJava, NULL);

	// call the C++ formatting function
	char* textOut = AStyleMain(textIn, options, javaErrorHandler, javaMemoryAlloc);
	// if an error message occurred it was displayed by errorHandler
	if (textOut == NULL)
		return textErr;

	// release memory
	jstring textOutJava = env->NewStringUTF(textOut);
	delete [] textOut;
	env->ReleaseStringUTFChars(textInJava, textIn);
	env->ReleaseStringUTFChars(optionsJava, options);

	return textOutJava;
}

// Call the Java error handler
void STDCALL javaErrorHandler(int errorNumber, char* errorMessage)
{
	jstring errorMessageJava = g_env->NewStringUTF(errorMessage);
	g_env->CallVoidMethod(g_obj, g_mid, errorNumber, errorMessageJava);
}

// Allocate memory for the formatted text
char* STDCALL javaMemoryAlloc(unsigned long memoryNeeded)
{
	// error condition is checked after return from AStyleMain
	char* buffer = new(nothrow) char [memoryNeeded];
	return buffer;
}
#endif

#ifdef ASTYLE_LIB
// *************************   GUI functions   ****************************************************
/*
 * IMPORTANT VC DLL linker must have the parameter  /EXPORT:AStyleMain=_AStyleMain@16
 *                                                  /EXPORT:AStyleGetVersion=_AStyleGetVersion@0
 */
extern "C" EXPORT char* STDCALL
AStyleMain(const char* pSourceIn,          // pointer to the source to be formatted
           const char* pOptions,           // pointer to AStyle options, separated by \n
           fpError fpErrorHandler,         // pointer to error handler function
           fpAlloc fpMemoryAlloc)          // pointer to memory allocation function
{
	if (fpErrorHandler == NULL)         // cannot display a message if no error handler
		return NULL;

	if (pSourceIn == NULL)
	{
		fpErrorHandler(101, (char*)"No pointer to source input.");
		return NULL;
	}
	if (pOptions == NULL)
	{
		fpErrorHandler(102, (char*)"No pointer to AStyle options.");
		return NULL;
	}
	if (fpMemoryAlloc == NULL)
	{
		fpErrorHandler(103, (char*)"No pointer to memory allocation function.");
		return NULL;
	}

	ASFormatter formatter;

	vector<string> optionsVector;
	istringstream opt(pOptions);
	_err = new stringstream;

	importOptions(opt, optionsVector);

	parseOptions(formatter,
	             optionsVector.begin(),
	             optionsVector.end(),
	             "Invalid Artistic Style options.\n"
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
		if (formatter.hasMoreLines())
			out << streamIterator.getOutputEOL();
	}

	unsigned long textSizeOut = out.str().length();
	char* pTextOut = fpMemoryAlloc(textSizeOut + 1);     // call memory allocation function
//    pTextOut = NULL;           // for testing
	if (pTextOut == NULL)
	{
		fpErrorHandler(110, (char*)"Allocation failure on output.");
		return NULL;
	}

	strcpy(pTextOut, out.str().c_str());

	return pTextOut;
}

extern "C" EXPORT const char* STDCALL AStyleGetVersion (void)
{
	return _version;
}

// ASTYLECON_LIB is defined to exclude "main" from the test programs
#elif !defined(ASTYLECON_LIB)

// **************************   main function   ***************************************************

int main(int argc, char** argv)
{
	ASFormatter formatter;
	g_console = new ASConsole;

	// process command line and options file
	// build the vectors fileNameVector, optionsVector, and fileOptionsVector
	processReturn returnValue = g_console->processOptions(argc, argv, formatter);

	// check for end of processing
	if (returnValue == END_SUCCESS)
		return EXIT_SUCCESS;
	if (returnValue == END_FAILURE)
	{
		(*_err) << "Artistic Style has terminated!" << endl;
		return EXIT_FAILURE;
	}

	// if no files have been given, use cin for input and cout for output
	if (g_console->fileNameVectorIsEmpty())
	{
		g_console->formatCinToCout(formatter);
		return EXIT_SUCCESS;
	}

	// process entries in the fileNameVector
	g_console->processFiles(formatter);

	delete g_console;
	return EXIT_SUCCESS;
}

#endif
