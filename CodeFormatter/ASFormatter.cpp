/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   ASFormatter.cpp
 *
 *   Copyright (C) 2006-2011 by Jim Pattee <jimp03@email.com>
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

#include "astyle.h"

#include <algorithm>
#include <fstream>
#include <iostream>


namespace astyle
{
/**
 * Constructor of ASFormatter
 */
ASFormatter::ASFormatter()
{
	sourceIterator = NULL;
	enhancer = new ASEnhancer;
	preBracketHeaderStack = NULL;
	bracketTypeStack = NULL;
	parenStack = NULL;
	structStack = NULL;
	lineCommentNoIndent = false;
	formattingStyle = STYLE_NONE;
	bracketFormatMode = NONE_MODE;
	pointerAlignment = PTR_ALIGN_NONE;
	referenceAlignment = REF_SAME_AS_PTR;
	lineEnd = LINEEND_DEFAULT;
	shouldPadOperators = false;
	shouldPadParensOutside = false;
	shouldPadParensInside = false;
	shouldPadHeader = false;
	shouldUnPadParens = false;
	shouldAttachClosingBracket = false;
	shouldBreakOneLineBlocks = true;
	shouldBreakOneLineStatements = true;
	shouldConvertTabs = false;
	shouldIndentCol1Comments = false;
	shouldBreakBlocks = false;
	shouldBreakClosingHeaderBlocks = false;
	shouldBreakClosingHeaderBrackets = false;
	shouldDeleteEmptyLines = false;
	shouldBreakElseIfs = false;
	shouldAddBrackets = false;
	shouldAddOneLineBrackets = false;

	// initialize ASFormatter member vectors
	formatterFileType = 9;		// reset to an invalid type
	headers = new vector<const string*>;
	nonParenHeaders = new vector<const string*>;
	preDefinitionHeaders = new vector<const string*>;
	preCommandHeaders = new vector<const string*>;
	operators = new vector<const string*>;
	assignmentOperators = new vector<const string*>;
	castOperators = new vector<const string*>;

	// the following prevents warning messages with cppcheck
	// it will NOT compile if activated
//	init();
}

/**
 * Destructor of ASFormatter
 */
ASFormatter::~ASFormatter()
{
	// delete ASFormatter stack vectors
	deleteContainer(preBracketHeaderStack);
	deleteContainer(bracketTypeStack);
	deleteContainer(parenStack);
	deleteContainer(structStack);

	// delete ASFormatter member vectors
	formatterFileType = 9;		// reset to an invalid type
	delete headers;
	delete nonParenHeaders;
	delete preDefinitionHeaders;
	delete preCommandHeaders;
	delete operators;
	delete assignmentOperators;
	delete castOperators;

	// delete ASBeautifier member vectors
	// must be done when the ASFormatter object is deleted (not ASBeautifier)
	ASBeautifier::deleteBeautifierVectors();

	delete enhancer;
}

/**
 * initialize the ASFormatter.
 *
 * init() should be called every time a ASFormatter object is to start
 * formatting a NEW source file.
 * init() recieves a pointer to a ASSourceIterator object that will be
 * used to iterate through the source code.
 *
 * @param sourceIterator     a pointer to the ASSourceIterator or ASStreamIterator object.
 */
void ASFormatter::init(ASSourceIterator* si)
{
	buildLanguageVectors();
	fixOptionVariableConflicts();

	ASBeautifier::init(si);
	enhancer->init(getFileType(),
	               getIndentLength(),
	               getIndentString(),
	               getCaseIndent(),
	               getPreprocessorIndent(),
	               getEmptyLineFill());
	sourceIterator = si;

	initContainer(preBracketHeaderStack, new vector<const string*>);
	initContainer(parenStack, new vector<int>);
	initContainer(structStack, new vector<bool>);
	parenStack->push_back(0);               // parenStack must contain this default entry
	initContainer(bracketTypeStack, new vector<BracketType>);
	bracketTypeStack->push_back(NULL_TYPE); // bracketTypeStack must contain this default entry

	currentHeader = NULL;
	currentLine = "";
	readyFormattedLine = "";
	formattedLine = "";
	currentChar = ' ';
	previousChar = ' ';
	previousCommandChar = ' ';
	previousNonWSChar = ' ';
	quoteChar = '"';
	charNum = 0;
	checksumIn = 0;
	checksumOut = 0;
	leadingSpaces = 0;
	formattedLineCommentNum = 0;
	preprocBracketTypeStackSize = 0;
	spacePadNum = 0;
	nextLineSpacePadNum = 0;
	currentLineFirstBracketNum = string::npos;
	previousReadyFormattedLineLength = string::npos;
	templateDepth = 0;
	traceLineNumber = 0;
	horstmannIndentChars = 0;
	tabIncrementIn = 0;
	previousBracketType = NULL_TYPE;
	previousOperator = NULL;

	isVirgin = true;
	isInLineComment = false;
	isInComment = false;
	noTrimCommentContinuation = false;
	isInPreprocessor = false;
	isInPreprocessorBeautify = false;
	doesLineStartComment = false;
	lineEndsInCommentOnly = false;
	lineIsLineCommentOnly = false;
	lineIsEmpty = false;
	isImmediatelyPostCommentOnly = false;
	isImmediatelyPostEmptyLine = false;
	isInQuote = false;
	isInVerbatimQuote = false;
	haveLineContinuationChar = false;
	isInQuoteContinuation = false;
	isSpecialChar = false;
	isNonParenHeader = false;
	foundNamespaceHeader = false;
	foundClassHeader = false;
	foundStructHeader = false;
	foundInterfaceHeader = false;
	foundPreDefinitionHeader = false;
	foundPreCommandHeader = false;
	foundCastOperator = false;
	foundQuestionMark = false;
	isInLineBreak = false;
	endOfCodeReached = false;
	isInExecSQL = false;
	isInAsm = false;
	isInAsmOneLine = false;
	isInAsmBlock = false;
	isLineReady = false;
	isPreviousBracketBlockRelated = false;
	isInPotentialCalculation = false;
	shouldReparseCurrentChar = false;
	needHeaderOpeningBracket = false;
	shouldBreakLineAtNextChar = false;
	passedSemicolon = false;
	passedColon = false;
	isImmediatelyPostNonInStmt = false;
	isCharImmediatelyPostNonInStmt = false;
	isInTemplate = false;
	isImmediatelyPostComment = false;
	isImmediatelyPostLineComment = false;
	isImmediatelyPostEmptyBlock = false;
	isImmediatelyPostPreprocessor = false;
	isImmediatelyPostReturn = false;
	isImmediatelyPostOperator = false;
	isImmediatelyPostPointerOrReference = false;
	isCharImmediatelyPostReturn = false;
	isCharImmediatelyPostOperator = false;
	isCharImmediatelyPostComment = false;
	isPreviousCharPostComment = false;
	isCharImmediatelyPostLineComment = false;
	isCharImmediatelyPostOpenBlock = false;
	isCharImmediatelyPostCloseBlock = false;
	isCharImmediatelyPostTemplate = false;
	isCharImmediatelyPostPointerOrReference = false;
	breakCurrentOneLineBlock = false;
	isInHorstmannRunIn = false;
	currentLineBeginsWithBracket = false;
	isPrependPostBlockEmptyLineRequested = false;
	isAppendPostBlockEmptyLineRequested = false;
	prependEmptyLine = false;
	appendOpeningBracket = false;
	foundClosingHeader = false;
	isImmediatelyPostHeader = false;
	isInHeader = false;
	isInCase = false;
	isJavaStaticConstructor = false;
}

/**
 * build vectors for each programing language
 * depending on the file extension.
 */
void ASFormatter::buildLanguageVectors()
{
	if (getFileType() == formatterFileType)  // don't build unless necessary
		return;

	formatterFileType = getFileType();

	headers->clear();
	nonParenHeaders->clear();
	preDefinitionHeaders->clear();
	preCommandHeaders->clear();
	operators->clear();
	assignmentOperators->clear();
	castOperators->clear();

	ASResource::buildHeaders(headers, getFileType());
	ASResource::buildNonParenHeaders(nonParenHeaders, getFileType());
	ASResource::buildPreDefinitionHeaders(preDefinitionHeaders, getFileType());
	ASResource::buildPreCommandHeaders(preCommandHeaders, getFileType());
	if (operators->size() == 0)
		ASResource::buildOperators(operators);
	if (assignmentOperators->size() == 0)
		ASResource::buildAssignmentOperators(assignmentOperators);
	if (castOperators->size() == 0)
		ASResource::buildCastOperators(castOperators);
}

/**
 * set the variables for each preefined style.
 * this will override any previous settings.
 */
void ASFormatter::fixOptionVariableConflicts()
{
	if (formattingStyle == STYLE_ALLMAN)
	{
		setBracketFormatMode(BREAK_MODE);
	}
	else if (formattingStyle == STYLE_JAVA)
	{
		setBracketFormatMode(ATTACH_MODE);
	}
	else if (formattingStyle == STYLE_KR)
	{
		setBracketFormatMode(LINUX_MODE);
	}
	else if (formattingStyle == STYLE_STROUSTRUP)
	{
		setBracketFormatMode(STROUSTRUP_MODE);
	}
	else if (formattingStyle == STYLE_WHITESMITH)
	{
		setBracketFormatMode(BREAK_MODE);
		setBracketIndent(true);
		setClassIndent(true);
		setSwitchIndent(true);
	}
	else if (formattingStyle == STYLE_BANNER)
	{
		setBracketFormatMode(ATTACH_MODE);
		setBracketIndent(true);
		setClassIndent(true);
		setSwitchIndent(true);
	}
	else if (formattingStyle == STYLE_GNU)
	{
		setBracketFormatMode(BREAK_MODE);
		setBlockIndent(true);
	}
	else if (formattingStyle == STYLE_LINUX)
	{
		setBracketFormatMode(LINUX_MODE);
		// always for Linux style
		setMinConditionalIndentOption(MINCOND_ONEHALF);
	}
	else if (formattingStyle == STYLE_HORSTMANN)
	{
		setBracketFormatMode(RUN_IN_MODE);
		setSwitchIndent(true);
	}
	else if (formattingStyle == STYLE_1TBS)
	{
		setBracketFormatMode(LINUX_MODE);
		setAddBracketsMode(true);
	}
	else if (formattingStyle == STYLE_PICO)
	{
		setBracketFormatMode(RUN_IN_MODE);
		setAttachClosingBracket(true);
		setSwitchIndent(true);
		setBreakOneLineBlocksMode(false);
		setSingleStatementsMode(false);
		// add-brackets won't work for pico, but it could be fixed if necessary
		// both options should be set to true
		if (shouldAddBrackets)
			shouldAddOneLineBrackets = true;
	}
	else if (formattingStyle == STYLE_LISP)
	{
		setBracketFormatMode(ATTACH_MODE);
		setAttachClosingBracket(true);
		setSingleStatementsMode(false);
		// add-one-line-brackets won't work for lisp
		// only shouldAddBrackets should be set to true
		if (shouldAddOneLineBrackets)
		{
			shouldAddBrackets = true;
			shouldAddOneLineBrackets = false;
		}
	}
	setMinConditionalIndentLength();
	// add-one-line-brackets implies keep-one-line-blocks
	if (shouldAddOneLineBrackets)
		setBreakOneLineBlocksMode(false);
}

/**
 * get the next formatted line.
 *
 * @return    formatted line.
 */
string ASFormatter::nextLine()
{
	const string* newHeader;
	bool isInVirginLine = isVirgin;
	isCharImmediatelyPostComment = false;
	isPreviousCharPostComment = false;
	isCharImmediatelyPostLineComment = false;
	isCharImmediatelyPostOpenBlock = false;
	isCharImmediatelyPostCloseBlock = false;
	isCharImmediatelyPostTemplate = false;
	traceLineNumber++;

	while (!isLineReady)
	{
		if (shouldReparseCurrentChar)
			shouldReparseCurrentChar = false;
		else if (!getNextChar())
		{
			breakLine();
			continue;
		}
		else // stuff to do when reading a new character...
		{
			// make sure that a virgin '{' at the begining of the file will be treated as a block...
			if (isInVirginLine && currentChar == '{'
			        && currentLineBeginsWithBracket	// lineBeginsWith('{')
			        && previousCommandChar == ' ')
				previousCommandChar = '{';
			if (isInHorstmannRunIn)
				isInLineBreak = false;
			if (!isWhiteSpace(currentChar))
				isInHorstmannRunIn = false;
			isPreviousCharPostComment = isCharImmediatelyPostComment;
			isCharImmediatelyPostComment = false;
			isCharImmediatelyPostTemplate = false;
			isCharImmediatelyPostReturn = false;
			isCharImmediatelyPostOperator = false;
			isCharImmediatelyPostPointerOrReference = false;
			isCharImmediatelyPostOpenBlock = false;
			isCharImmediatelyPostCloseBlock = false;
		}

//		if (inLineNumber >= 7)
//			int x = 1;

		if (shouldBreakLineAtNextChar && !isWhiteSpace(currentChar))
		{
			isInLineBreak = true;
			shouldBreakLineAtNextChar = false;
		}

		if (isInExecSQL && !passedSemicolon)
		{
			if (currentChar == ';')
				passedSemicolon = true;
			appendCurrentChar();
			continue;
		}

		if (isInLineComment)
		{
			formatLineCommentBody();
			continue;
		}
		else if (isInComment)
		{
			formatCommentBody();
			continue;
		}

		// not in line comment or comment

		else if (isInQuote)
		{
			formatQuoteBody();
			continue;
		}

		if (isSequenceReached("//"))
		{
			formatLineCommentOpener();
			continue;
		}
		else if (isSequenceReached("/*"))
		{
			formatCommentOpener();
			continue;
		}
		else if (currentChar == '"' || currentChar == '\'')
		{
			formatQuoteOpener();
			continue;
		}
		// treat these preprocessor statements as a line comment
		else if (currentChar =='#')
		{
			string preproc = trim(currentLine.c_str() + charNum + 1);
			if (preproc.compare(0, 6, "region") == 0
			        || preproc.compare(0, 9, "endregion") == 0
			        || preproc.compare(0, 5, "error") == 0
			        || preproc.compare(0, 7, "warning") == 0)
			{
				// check for horstmann run-in
				if (formattedLine.length() > 0 && formattedLine[0] == '{')
				{
					isInLineBreak = true;
					isInHorstmannRunIn = false;
				}
				isInLineComment = true;
				appendCurrentChar();
				continue;
			}
		}

		if (isInPreprocessor)
		{
			appendCurrentChar();
			continue;
		}

		// handle white space - needed to simplify the rest.
		if (isWhiteSpace(currentChar))
		{
			appendCurrentChar();
			continue;
		}

		/* not in MIDDLE of quote or comment or SQL or white-space of any type ... */

		// check if in preprocessor
		// ** isInPreprocessor will be automatically reset at the begining
		//    of a new line in getnextChar()
		if (currentChar == '#')
		{
			isInPreprocessor = true;
			// check for horstmann run-in
			if (formattedLine.length() > 0 && formattedLine[0] == '{')
			{
				isInLineBreak = true;
				isInHorstmannRunIn = false;
			}
			processPreprocessor();
			//  need to fall thru here to reset the variables
		}

		/* not in preprocessor ... */

		if (isImmediatelyPostComment)
		{
			isImmediatelyPostComment = false;
			isCharImmediatelyPostComment = true;
		}

		if (isImmediatelyPostLineComment)
		{
			isImmediatelyPostLineComment = false;
			isCharImmediatelyPostLineComment = true;
		}

		if (isImmediatelyPostReturn)
		{
			isImmediatelyPostReturn = false;
			isCharImmediatelyPostReturn = true;
		}

		if (isImmediatelyPostOperator)
		{
			isImmediatelyPostOperator = false;
			isCharImmediatelyPostOperator = true;
		}
		if (isImmediatelyPostPointerOrReference)
		{
			isImmediatelyPostPointerOrReference = false;
			isCharImmediatelyPostPointerOrReference = true;
		}

		// reset isImmediatelyPostHeader information
		if (isImmediatelyPostHeader)
		{
			// should brackets be added
			if (currentChar != '{' && shouldAddBrackets)
			{
				bool bracketsAdded = addBracketsToStatement();
				if (bracketsAdded && !shouldAddOneLineBrackets)
				{
					size_t firstText = currentLine.find_first_not_of(" \t");
					assert(firstText != string::npos);
					if ((int) firstText == charNum)
						breakCurrentOneLineBlock = true;
				}
			}

			// Make sure headers are broken from their succeeding blocks
			// (e.g.
			//     if (isFoo) DoBar();
			//  should become
			//     if (isFoo)
			//         DoBar;
			// )
			// But treat else if() as a special case which should not be broken!
			if (shouldBreakOneLineStatements
			        && isOkToBreakBlock(bracketTypeStack->back()))
			{
				// if may break 'else if()'s, then simply break the line
				if (shouldBreakElseIfs)
					isInLineBreak = true;
			}

			isImmediatelyPostHeader = false;
		}

		if (passedSemicolon)    // need to break the formattedLine
		{
			passedSemicolon = false;
			if (parenStack->back() == 0 && currentChar != ';') // allow ;;
			{
				// does a one-line statement have ending comments?
				if (isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE))
				{
					size_t blockEnd = currentLine.rfind(AS_CLOSE_BRACKET);
					assert(blockEnd != string::npos);
					// move ending comments to this formattedLine
					if (isBeforeAnyLineEndComment(blockEnd))
					{
						size_t commentStart = currentLine.find_first_not_of(" \t", blockEnd + 1);
						assert(commentStart != string::npos);
						assert((currentLine.compare(commentStart, 2, "//") == 0)
						       || (currentLine.compare(commentStart, 2, "/*") == 0));
						size_t commentLength = currentLine.length() - commentStart;
						formattedLine.append(getIndentLength() - 1, ' ');
						formattedLine.append(currentLine, commentStart, commentLength);
						currentLine.erase(commentStart, commentLength);
					}
				}
				isInExecSQL = false;
				shouldReparseCurrentChar = true;
				isInLineBreak = true;
				if (needHeaderOpeningBracket)
				{
					isCharImmediatelyPostCloseBlock = true;
					needHeaderOpeningBracket = false;
				}
				continue;
			}
		}

		if (passedColon)
		{
			passedColon = false;
			if (parenStack->back() == 0 && !isBeforeAnyComment())
			{
				shouldReparseCurrentChar = true;
				isInLineBreak = true;
				continue;
			}
		}

		// Check if in template declaration, e.g. foo<bar> or foo<bar,fig>
		if (!isInTemplate && currentChar == '<')
		{
			checkIfTemplateOpener();
		}

		// handle parenthesies
		if (currentChar == '(' || currentChar == '[' || (isInTemplate && currentChar == '<'))
		{
			parenStack->back()++;
		}
		else if (currentChar == ')' || currentChar == ']' || (isInTemplate && currentChar == '>'))
		{
			foundPreCommandHeader = false;
			parenStack->back()--;
			if (isInTemplate && currentChar == '>')
			{
				templateDepth--;
				if (templateDepth == 0)
				{
					isInTemplate = false;
					isCharImmediatelyPostTemplate = true;
				}
			}

			// check if this parenthesis closes a header, e.g. if (...), while (...)
			if (isInHeader && parenStack->back() == 0)
			{
				isInHeader = false;
				isImmediatelyPostHeader = true;
				foundQuestionMark = false;
			}
			if (currentChar == ')')
			{
				foundCastOperator = false;
				if (parenStack->back() == 0)
					isInAsm = false;
			}
		}

		// handle brackets
		if (currentChar == '{' || currentChar == '}')
		{
			// if appendOpeningBracket this was already done for the original bracket
			if (currentChar == '{' && !appendOpeningBracket)
			{
				BracketType newBracketType = getBracketType();
				foundNamespaceHeader = false;
				foundClassHeader = false;
				foundStructHeader = false;
				foundInterfaceHeader = false;
				foundPreDefinitionHeader = false;
				foundPreCommandHeader = false;
				isInPotentialCalculation = false;
				isJavaStaticConstructor = false;
				isCharImmediatelyPostNonInStmt = false;
				needHeaderOpeningBracket = false;

				isPreviousBracketBlockRelated = !isBracketType(newBracketType, ARRAY_TYPE);
				bracketTypeStack->push_back(newBracketType);
				preBracketHeaderStack->push_back(currentHeader);
				currentHeader = NULL;
				structStack->push_back(isInIndentableStruct);
				if (isBracketType(newBracketType, STRUCT_TYPE) && isCStyle())
					isInIndentableStruct = isStructAccessModified(currentLine, charNum);
				else
					isInIndentableStruct = false;
			}

			// this must be done before the bracketTypeStack is popped
			BracketType bracketType = bracketTypeStack->back();
			bool isOpeningArrayBracket = (isBracketType(bracketType, ARRAY_TYPE)
			                              && bracketTypeStack->size() >= 2
			                              && !isBracketType((*bracketTypeStack)[bracketTypeStack->size()-2], ARRAY_TYPE)
			                             );

			if (currentChar == '}')
			{
				// if a request has been made to append a post block empty line,
				// but the block exists immediately before a closing bracket,
				// then there is no need for the post block empty line.
				isAppendPostBlockEmptyLineRequested = false;
				breakCurrentOneLineBlock = false;
				isInAsmBlock = false;

				// added for release 1.24
				// TODO: remove at the appropriate time
				assert(isInAsm == false);
				assert(isInAsmOneLine == false);
				assert(isInQuote == false);
				isInAsm = isInAsmOneLine = isInQuote = false;
				// end remove

				if (bracketTypeStack->size() > 1)
				{
					previousBracketType = bracketTypeStack->back();
					bracketTypeStack->pop_back();
					isPreviousBracketBlockRelated = !isBracketType(bracketType, ARRAY_TYPE);
				}
				else
				{
					previousBracketType = NULL_TYPE;
					isPreviousBracketBlockRelated = false;
				}

				if (!preBracketHeaderStack->empty())
				{
					currentHeader = preBracketHeaderStack->back();
					preBracketHeaderStack->pop_back();
				}
				else
					currentHeader = NULL;

				if (!structStack->empty())
				{
					isInIndentableStruct = structStack->back();
					structStack->pop_back();
				}
				else
					isInIndentableStruct = false;

				if (isNonInStatementArray
				        && (!isBracketType(bracketTypeStack->back(), ARRAY_TYPE)	// check previous bracket
				            || peekNextChar() == ';'))								// check for "};" added V2.01
					isImmediatelyPostNonInStmt = true;
			}

			// format brackets
			appendOpeningBracket = false;
			if (isBracketType(bracketType, ARRAY_TYPE))
			{
				formatArrayBrackets(bracketType, isOpeningArrayBracket);
			}
			else
			{
				if (currentChar == '{')
					formatOpeningBracket(bracketType);
				else
					formatClosingBracket(bracketType);
			}
			continue;
		}

		if ((((previousCommandChar == '{' && isPreviousBracketBlockRelated)
		        || ((previousCommandChar == '}'
		             && !isImmediatelyPostEmptyBlock
		             && isPreviousBracketBlockRelated
		             && !isPreviousCharPostComment       // Fixes wrongly appended newlines after '}' immediately after comments
		             && peekNextChar() != ' '
		             && !isBracketType(previousBracketType,  DEFINITION_TYPE))
		            && !isBracketType(bracketTypeStack->back(),  DEFINITION_TYPE)))
		        && isOkToBreakBlock(bracketTypeStack->back()))
		        // check for array
		        || (previousCommandChar == '{'			// added 9/30/2010
		            && isBracketType(bracketTypeStack->back(), ARRAY_TYPE)
		            && !isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE)
		            && isNonInStatementArray))
		{
			isCharImmediatelyPostOpenBlock = (previousCommandChar == '{');
			isCharImmediatelyPostCloseBlock = (previousCommandChar == '}');

			if (isCharImmediatelyPostOpenBlock
			        && !isCharImmediatelyPostComment
			        && !isCharImmediatelyPostLineComment)
			{
				previousCommandChar = ' ';

				if (bracketFormatMode == NONE_MODE)
				{
					if (shouldBreakOneLineBlocks
					        && isBracketType(bracketTypeStack->back(),  SINGLE_LINE_TYPE))
						isInLineBreak = true;
					else if (currentLineBeginsWithBracket)
						formatRunIn();
					else
						breakLine();
				}
				else if (bracketFormatMode == RUN_IN_MODE
				         && currentChar != '#')
					formatRunIn();
				else
					isInLineBreak = true;
			}
			else if (isCharImmediatelyPostCloseBlock
			         && shouldBreakOneLineStatements
			         && (isLegalNameChar(currentChar) && currentChar != '.')
			         && !isCharImmediatelyPostComment)
			{
				previousCommandChar = ' ';
				isInLineBreak = true;
			}
		}

		// reset block handling flags
		isImmediatelyPostEmptyBlock = false;

		// look for headers
		bool isPotentialHeader = isCharPotentialHeader(currentLine, charNum);

		if (isPotentialHeader && !isInTemplate)
		{
			isNonParenHeader = false;
			foundClosingHeader = false;
			newHeader = findHeader(headers);

			if (newHeader != NULL)
			{
				const string* previousHeader;

				// recognize closing headers of do..while, if..else, try..catch..finally
				if ((newHeader == &AS_ELSE && currentHeader == &AS_IF)
				        || (newHeader == &AS_WHILE && currentHeader == &AS_DO)
				        || (newHeader == &AS_CATCH && currentHeader == &AS_TRY)
				        || (newHeader == &AS_CATCH && currentHeader == &AS_CATCH)
				        || (newHeader == &AS_FINALLY && currentHeader == &AS_TRY)
				        || (newHeader == &AS_FINALLY && currentHeader == &AS_CATCH)
				        || (newHeader == &_AS_FINALLY && currentHeader == &_AS_TRY)
				        || (newHeader == &_AS_EXCEPT && currentHeader == &_AS_TRY)
				        || (newHeader == &AS_SET && currentHeader == &AS_GET)
				        || (newHeader == &AS_REMOVE && currentHeader == &AS_ADD))
					foundClosingHeader = true;

				previousHeader = currentHeader;
				currentHeader = newHeader;
				needHeaderOpeningBracket = true;

				if (foundClosingHeader && previousNonWSChar == '}')
				{
					if (isOkToBreakBlock(bracketTypeStack->back()))
						isLineBreakBeforeClosingHeader();

					// get the adjustment for a comment following the closing header
					if (isInLineBreak)
						nextLineSpacePadNum = getNextLineCommentAdjustment();
					else
						spacePadNum = getCurrentLineCommentAdjustment();
				}

				// check if the found header is non-paren header
				isNonParenHeader = findHeader(nonParenHeaders) != NULL;

				// join 'else if' statements
				if (currentHeader == &AS_IF && previousHeader == &AS_ELSE && isInLineBreak
				        && !shouldBreakElseIfs && !isCharImmediatelyPostLineComment)
				{
					// 'else' must be last thing on the line, but must not be #else
					size_t start = formattedLine.length() >= 6 ? formattedLine.length()-6 : 0;
					if (formattedLine.find("else", start) != string::npos
					        && formattedLine.find("#else", start) == string::npos)
					{
						appendSpacePad();
						isInLineBreak = false;
					}
				}

				appendSequence(*currentHeader);
				goForward(currentHeader->length() - 1);
				// if a paren-header is found add a space after it, if needed
				// this checks currentLine, appendSpacePad() checks formattedLine
				// in C# 'catch' can be either a paren or non-paren header
				if (shouldPadHeader
				        && (!isNonParenHeader
				            || (currentHeader == &AS_CASE && peekNextChar() == '(')
				            || (currentHeader == &AS_CATCH && peekNextChar() == '('))
				        && charNum < (int) currentLine.length() - 1 && !isWhiteSpace(currentLine[charNum+1]))
					appendSpacePad();

				// Signal that a header has been reached
				// *** But treat a closing while() (as in do...while)
				//     as if it were NOT a header since a closing while()
				//     should never have a block after it!
				if (!(foundClosingHeader && currentHeader == &AS_WHILE))
				{
					isInHeader = true;

					// in C# 'catch' and 'delegate' can be a paren or non-paren header
					if (isNonParenHeader && !isSharpStyleWithParen(currentHeader))
					{
						isImmediatelyPostHeader = true;
						isInHeader = false;
					}
				}

				if (shouldBreakBlocks
				        && isOkToBreakBlock(bracketTypeStack->back()))
				{
					if (previousHeader == NULL
					        && !foundClosingHeader
					        && !isCharImmediatelyPostOpenBlock
					        && !isImmediatelyPostCommentOnly)
					{
						isPrependPostBlockEmptyLineRequested = true;
					}

					if (currentHeader == &AS_ELSE
					        || currentHeader == &AS_CATCH
					        || currentHeader == &AS_FINALLY
					        || foundClosingHeader)
					{
						isPrependPostBlockEmptyLineRequested = false;
					}

					if (shouldBreakClosingHeaderBlocks
					        && isCharImmediatelyPostCloseBlock
					        && !isImmediatelyPostCommentOnly
					        && currentHeader != &AS_WHILE)    // closing do-while block
					{
						isPrependPostBlockEmptyLineRequested = true;
					}

				}

				if (currentHeader == &AS_CASE
				        || currentHeader == &AS_DEFAULT)
					isInCase = true;

				continue;
			}
			else if ((newHeader = findHeader(preDefinitionHeaders)) != NULL
			         && parenStack->back() == 0)
			{
				if (newHeader == &AS_NAMESPACE)
					foundNamespaceHeader = true;
				if (newHeader == &AS_CLASS)
					foundClassHeader = true;
				if (newHeader == &AS_STRUCT)
					foundStructHeader = true;
				if (newHeader == &AS_INTERFACE)
					foundInterfaceHeader = true;
				foundPreDefinitionHeader = true;
				appendSequence(*newHeader);
				goForward(newHeader->length() - 1);

				continue;
			}
			else if ((newHeader = findHeader(preCommandHeaders)) != NULL)
			{
				foundPreCommandHeader = true;
				// fall thru here for a 'const' that is not a precommand header
			}
			else if ((newHeader = findHeader(castOperators)) != NULL)
			{
				foundCastOperator = true;
				appendSequence(*newHeader);
				goForward(newHeader->length() - 1);
				continue;
			}
		}   // (isPotentialHeader && !isInTemplate)

		if (isInLineBreak)          // OK to break line here
		{
			breakLine();
			if (isInVirginLine)		// adjust for the first line
			{
				lineCommentNoBeautify = lineCommentNoIndent;
				lineCommentNoIndent = false;
			}
		}

		if (previousNonWSChar == '}' || currentChar == ';')
		{
			if (currentChar == ';')
			{
				if (((shouldBreakOneLineStatements
				        || isBracketType(bracketTypeStack->back(),  SINGLE_LINE_TYPE))
				        && isOkToBreakBlock(bracketTypeStack->back()))
				        && !(shouldAttachClosingBracket && peekNextChar() == '}'))
				{
					passedSemicolon = true;
				}

				// append post block empty line for unbracketed header
				if (shouldBreakBlocks
				        && currentHeader != NULL
				        && currentHeader != &AS_CASE
				        && currentHeader != &AS_DEFAULT
				        && parenStack->back() == 0)
				{
					isAppendPostBlockEmptyLineRequested = true;
				}
			}

			// end of block if a closing bracket was found
			// or an opening bracket was not found (';' closes)
			if (currentChar != ';'
			        || (needHeaderOpeningBracket && parenStack->back() == 0))
				currentHeader = NULL;

			foundQuestionMark = false;
			foundNamespaceHeader = false;
			foundClassHeader = false;
			foundStructHeader = false;
			foundInterfaceHeader = false;
			foundPreDefinitionHeader = false;
			foundPreCommandHeader = false;
			foundCastOperator = false;
			isInPotentialCalculation = false;
			isSharpAccessor = false;
			isSharpDelegate = false;
			isInExtern = false;
			nonInStatementBracket = 0;
		}

		if (currentChar == ':' && shouldBreakOneLineStatements)
		{
			if (isInCase
			        && previousChar != ':'          // not part of '::'
			        && peekNextChar() != ':')       // not part of '::'
			{
				isInCase = false;
				passedColon = true;
			}
			else if (isCStyle()                     // for C/C++ only
			         && !foundQuestionMark          // not in a ... ? ... : ... sequence
			         && !foundPreDefinitionHeader   // not in a definition block (e.g. class foo : public bar
			         && previousCommandChar != ')'  // not immediately after closing paren of a method header, e.g. ASFormatter::ASFormatter(...) : ASBeautifier(...)
			         && previousChar != ':'         // not part of '::'
			         && peekNextChar() != ':'       // not part of '::'
			         && !isDigit(peekNextChar())    // not a bit field
			         && !isInAsm                    // not in extended assembler
			         && !isInAsmOneLine             // not in extended assembler
			         && !isInAsmBlock)              // not in extended assembler
			{
				passedColon = true;
			}
		}

		if (currentChar == '?')
			foundQuestionMark = true;

		if (isPotentialHeader &&  !isInTemplate)
		{
			if (findKeyword(currentLine, charNum, AS_NEW))
				isInPotentialCalculation = false;

			if (findKeyword(currentLine, charNum, AS_RETURN))
			{
				isInPotentialCalculation = true;	// return is the same as an = sign
				isImmediatelyPostReturn = true;
			}

			if (findKeyword(currentLine, charNum, AS_OPERATOR))
				isImmediatelyPostOperator = true;

			if (isCStyle() && findKeyword(currentLine, charNum, AS_EXTERN))
				isInExtern = true;

			if (isCStyle() && isExecSQL(currentLine, charNum))
				isInExecSQL = true;

			if (isCStyle())
			{
				if (findKeyword(currentLine, charNum, AS_ASM)
				        || findKeyword(currentLine, charNum, AS__ASM__))
				{
					isInAsm = true;
				}
				else if (findKeyword(currentLine, charNum, AS_MS_ASM)		// microsoft specific
				         || findKeyword(currentLine, charNum, AS_MS__ASM))
				{
					int index = 4;
					if (peekNextChar() == '_')	// check for __asm
						index = 5;

					char peekedChar = ASBase::peekNextChar(currentLine, charNum + index);
					if (peekedChar == '{' || peekedChar == ' ')
						isInAsmBlock = true;
					else
						isInAsmOneLine = true;
				}
			}

			if (isJavaStyle()
			        && (findKeyword(currentLine, charNum, AS_STATIC)
			            && isNextCharOpeningBracket(charNum + 6)))
				isJavaStaticConstructor = true;

			if (isSharpStyle()
			        && (findKeyword(currentLine, charNum, AS_DELEGATE)
			            || findKeyword(currentLine, charNum, AS_UNCHECKED)))
				isSharpDelegate = true;

			// append the entire name
			string name = getCurrentWord(currentLine, charNum);
			appendSequence(name);
			goForward(name.length() - 1);

			continue;

		}   // (isPotentialHeader &&  !isInTemplate)

		// determine if this is a potential calculation

		bool isPotentialOperator = isCharPotentialOperator(currentChar);
		newHeader = NULL;

		if (isPotentialOperator)
		{
			newHeader = findOperator(operators);

			if (newHeader != NULL)
			{
				// correct mistake of two >> closing a template
				if (isInTemplate && (newHeader == &AS_GR_GR || newHeader == &AS_GR_GR_GR))
					newHeader = &AS_GR;

				if (!isInPotentialCalculation)
				{
					// must determine if newHeader is an assignment operator
					// do NOT use findOperator!!!
					if (find(assignmentOperators->begin(), assignmentOperators->end(), newHeader)
					        != assignmentOperators->end())
					{
						foundPreCommandHeader = false;
						char peekedChar = peekNextChar();
						isInPotentialCalculation = (!(newHeader == &AS_EQUAL && peekedChar == '*')
						                            && !(newHeader == &AS_EQUAL && peekedChar == '&'));
					}
				}
			}
		}

		// process pointers and references
		// check newHeader to elimnate things like '&&' sequence
		if (isCStyle()
		        && (newHeader == &AS_MULT || newHeader == &AS_BIT_AND)
		        && isPointerOrReference()
		        && !isDereferenceOrAddressOf())
		{
			formatPointerOrReference();
			isImmediatelyPostPointerOrReference = true;
			continue;
		}

		if (shouldPadOperators && newHeader != NULL)
		{
			padOperators(newHeader);
			continue;
		}

		// pad commas and semi-colons
		if (currentChar == ';'
		        || (currentChar == ',' && shouldPadOperators))
		{
			char nextChar = ' ';
			if (charNum + 1 < (int) currentLine.length())
				nextChar = currentLine[charNum+1];
			if (!isWhiteSpace(nextChar)
			        && nextChar != '}'
			        && nextChar != ')'
			        && nextChar != ']'
			        && nextChar != '>'
			        && nextChar != ';'
			        && !isBeforeAnyComment()
			        /* && !(isBracketType(bracketTypeStack->back(), ARRAY_TYPE)) */
			   )
			{
				appendCurrentChar();
				appendSpaceAfter();
				continue;
			}
		}

		if ((currentChar == '(' || currentChar == ')')
		        && (shouldPadParensOutside || shouldPadParensInside || shouldUnPadParens))
		{
			padParens();
			continue;
		}

		// bypass the entire operator
		if (newHeader != NULL && newHeader->length() > 1)
		{
			appendSequence(*newHeader);
			goForward(newHeader->length() - 1);
			continue;
		}

		appendCurrentChar();

	}   // end of while loop  *  end of while loop  *  end of while loop  *  end of while loop

	// return a beautified (i.e. correctly indented) line.

	string beautifiedLine;
	size_t readyFormattedLineLength = trim(readyFormattedLine).length();

	if (prependEmptyLine                // prepend a blank line before this formatted line
	        && readyFormattedLineLength > 0
	        && previousReadyFormattedLineLength > 0)
	{
		isLineReady = true;             // signal a waiting readyFormattedLine
		beautifiedLine = beautify("");
		previousReadyFormattedLineLength = 0;
		// call the enhancer for new empty lines
		enhancer->enhance(beautifiedLine, isInPreprocessorBeautify, isInBeautifySQL);
	}
	else                                // format the current formatted line
	{
		isLineReady = false;
		horstmannIndentInStatement = horstmannIndentChars;
		beautifiedLine = beautify(readyFormattedLine);
		previousReadyFormattedLineLength = readyFormattedLineLength;
		// the enhancer is not called for no-indent line comments
		if (!lineCommentNoBeautify)
			enhancer->enhance(beautifiedLine, isInPreprocessorBeautify, isInBeautifySQL);
		horstmannIndentChars = 0;
		lineCommentNoBeautify = lineCommentNoIndent;
		lineCommentNoIndent = false;
		if (isCharImmediatelyPostNonInStmt)
		{
			isNonInStatementArray = false;
			isCharImmediatelyPostNonInStmt = false;
		}
		isInPreprocessorBeautify = isInPreprocessor;	// used by ASEnhancer
		isInBeautifySQL = isInExecSQL;					// used by ASEnhancer
	}

	prependEmptyLine = false;
	assert(computeChecksumOut(beautifiedLine));
	return beautifiedLine;
}


/**
 * check if there are any indented lines ready to be read by nextLine()
 *
 * @return    are there any indented lines ready?
 */
bool ASFormatter::hasMoreLines() const
{
	return !endOfCodeReached;
}

/**
 * comparison function for BracketType enum
 */
bool ASFormatter::isBracketType(BracketType a, BracketType b) const
{
	return ((a & b) == b);
}

/**
 * set the formatting style.
 *
 * @param mode         the formatting style.
 */
void ASFormatter::setFormattingStyle(FormatStyle style)
{
	formattingStyle = style;
}

/**
 * set the add brackets mode.
 * options:
 *    true     brackets added to headers for single line statements.
 *    false    brackets NOT added to headers for single line statements.
 *
 * @param mode         the bracket formatting mode.
 */
void ASFormatter::setAddBracketsMode(bool state)
{
	shouldAddBrackets = state;
}

/**
 * set the add one line brackets mode.
 * options:
 *    true     one line brackets added to headers for single line statements.
 *    false    one line brackets NOT added to headers for single line statements.
 *
 * @param mode         the bracket formatting mode.
 */
void ASFormatter::setAddOneLineBracketsMode(bool state)
{
	shouldAddBrackets = state;
	shouldAddOneLineBrackets = state;
}

/**
 * set the bracket formatting mode.
 * options:
 *
 * @param mode         the bracket formatting mode.
 */
void ASFormatter::setBracketFormatMode(BracketMode mode)
{
	bracketFormatMode = mode;
}

/**
 * set closing header bracket breaking mode
 * options:
 *    true     brackets just before closing headers (e.g. 'else', 'catch')
 *             will be broken, even if standard brackets are attached.
 *    false    closing header brackets will be treated as standard brackets.
 *
 * @param state         the closing header bracket breaking mode.
 */
void ASFormatter::setBreakClosingHeaderBracketsMode(bool state)
{
	shouldBreakClosingHeaderBrackets = state;
}

/**
 * set 'else if()' breaking mode
 * options:
 *    true     'else' headers will be broken from their succeeding 'if' headers.
 *    false    'else' headers will be attached to their succeeding 'if' headers.
 *
 * @param state         the 'else if()' breaking mode.
 */
void ASFormatter::setBreakElseIfsMode(bool state)
{
	shouldBreakElseIfs = state;
}

/**
 * set operator padding mode.
 * options:
 *    true     statement operators will be padded with spaces around them.
 *    false    statement operators will not be padded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setOperatorPaddingMode(bool state)
{
	shouldPadOperators = state;
}

/**
 * set parenthesis outside padding mode.
 * options:
 *    true     statement parenthesiss will be padded with spaces around them.
 *    false    statement parenthesiss will not be padded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setParensOutsidePaddingMode(bool state)
{
	shouldPadParensOutside = state;
}

/**
 * set parenthesis inside padding mode.
 * options:
 *    true     statement parenthesis will be padded with spaces around them.
 *    false    statement parenthesis will not be padded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setParensInsidePaddingMode(bool state)
{
	shouldPadParensInside = state;
}

/**
 * set header padding mode.
 * options:
 *    true     headers will be padded with spaces around them.
 *    false    headers will not be padded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setParensHeaderPaddingMode(bool state)
{
	shouldPadHeader = state;
}

/**
 * set parenthesis unpadding mode.
 * options:
 *    true     statement parenthesis will be unpadded with spaces removed around them.
 *    false    statement parenthesis will not be unpadded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setParensUnPaddingMode(bool state)
{
	shouldUnPadParens = state;
}

/**
 * set option to attach closing brackets
 *
 * @param state        true = attach, false = don't attach.
 */
void ASFormatter::setAttachClosingBracket(bool state)
{
	shouldAttachClosingBracket = state;
}

/**
 * set option to break/not break one-line blocks
 *
 * @param state        true = break, false = don't break.
 */
void ASFormatter::setBreakOneLineBlocksMode(bool state)
{
	shouldBreakOneLineBlocks = state;
}

/**
 * set option to break/not break lines consisting of multiple statements.
 *
 * @param state        true = break, false = don't break.
 */
void ASFormatter::setSingleStatementsMode(bool state)
{
	shouldBreakOneLineStatements = state;
}

/**
 * set option to convert tabs to spaces.
 *
 * @param state        true = convert, false = don't convert.
 */
void ASFormatter::setTabSpaceConversionMode(bool state)
{
	shouldConvertTabs = state;
}

/**
 * set option to indent comments in column 1.
 *
 * @param state        true = indent, false = don't indent.
 */
void ASFormatter::setIndentCol1CommentsMode(bool state)
{
	shouldIndentCol1Comments = state;
}

/**
 * set option to force all line ends to a particular style.
 *
 * @param fmt           format enum value
 */
void ASFormatter::setLineEndFormat(LineEndFormat fmt)
{
	lineEnd = fmt;
}

/**
 * set option to break unrelated blocks of code with empty lines.
 *
 * @param state        true = convert, false = don't convert.
 */
void ASFormatter::setBreakBlocksMode(bool state)
{
	shouldBreakBlocks = state;
}

/**
 * set option to break closing header blocks of code (such as 'else', 'catch', ...) with empty lines.
 *
 * @param state        true = convert, false = don't convert.
 */
void ASFormatter::setBreakClosingHeaderBlocksMode(bool state)
{
	shouldBreakClosingHeaderBlocks = state;
}

/**
 * set option to delete empty lines.
 *
 * @param state        true = delete, false = don't delete.
 */
void ASFormatter::setDeleteEmptyLinesMode(bool state)
{
	shouldDeleteEmptyLines = state;
}

/**
 * set the pointer alignment.
 * options:
 *
 * @param alignment    the pointer alignment.
 */
void ASFormatter::setPointerAlignment(PointerAlign alignment)
{
	pointerAlignment = alignment;
}

void ASFormatter::setReferenceAlignment(ReferenceAlign alignment)
{
	referenceAlignment = alignment;
}

/**
 * jump over several characters.
 *
 * @param i       the number of characters to jump over.
 */
void ASFormatter::goForward(int i)
{
	while (--i >= 0)
		getNextChar();
}

/**
 * peek at the next unread character.
 *
 * @return     the next unread character.
 */
char ASFormatter::peekNextChar() const
{
	char ch = ' ';
	size_t peekNum = currentLine.find_first_not_of(" \t", charNum + 1);

	if (peekNum == string::npos)
		return ch;

	ch = currentLine[peekNum];

	return ch;
}

/**
 * check if current placement is before a comment
 *
 * @return     is before a comment.
 */
bool ASFormatter::isBeforeComment() const
{
	bool foundComment = false;
	size_t peekNum = currentLine.find_first_not_of(" \t", charNum + 1);

	if (peekNum == string::npos)
		return foundComment;

	foundComment = (currentLine.compare(peekNum, 2, "/*") == 0);

	return foundComment;
}

/**
 * check if current placement is before a comment or line-comment
 *
 * @return     is before a comment or line-comment.
 */
bool ASFormatter::isBeforeAnyComment() const
{
	bool foundComment = false;
	size_t peekNum = currentLine.find_first_not_of(" \t", charNum + 1);

	if (peekNum == string::npos)
		return foundComment;

	foundComment = (currentLine.compare(peekNum, 2, "/*") == 0
	                || currentLine.compare(peekNum, 2, "//") == 0);

	return foundComment;
}

/**
 * check if current placement is before a comment or line-comment
 * if a block comment it must be at the end of the line
 *
 * @return     is before a comment or line-comment.
 */
bool ASFormatter::isBeforeAnyLineEndComment(int startPos) const
{
	bool foundLineEndComment = false;
	size_t peekNum = currentLine.find_first_not_of(" \t", startPos + 1);

	if (peekNum != string::npos)
	{
		if (currentLine.compare(peekNum, 2, "//") == 0)
			foundLineEndComment = true;
		else if (currentLine.compare(peekNum, 2, "/*") == 0)
		{
			// comment must be closed on this line with nothing after it
			size_t endNum = currentLine.find("*/", peekNum + 2);
			if (endNum != string::npos)
			{
				size_t nextChar = currentLine.find_first_not_of(" \t", endNum + 2);
				if (nextChar == string::npos)
					foundLineEndComment = true;
			}
		}
	}
	return foundLineEndComment;
}

/**
 * check if current placement is before a comment followed by a line-comment
 *
 * @return     is before a multiple line-end comment.
 */
bool ASFormatter::isBeforeMultipleLineEndComments(int startPos) const
{
	bool foundMultipleLineEndComment = false;
	size_t peekNum = currentLine.find_first_not_of(" \t", startPos + 1);

	if (peekNum != string::npos)
	{
		if (currentLine.compare(peekNum, 2, "/*") == 0)
		{
			// comment must be closed on this line with nothing after it
			size_t endNum = currentLine.find("*/", peekNum + 2);
			if (endNum != string::npos)
			{
				size_t nextChar = currentLine.find_first_not_of(" \t", endNum + 2);
				if (nextChar != string::npos
				        && currentLine.compare(nextChar, 2, "//") == 0)
					foundMultipleLineEndComment = true;
			}
		}
	}
	return foundMultipleLineEndComment;
}


/**
 * get the next character, increasing the current placement in the process.
 * the new character is inserted into the variable currentChar.
 *
 * @return   whether succeded to recieve the new character.
 */
bool ASFormatter::getNextChar()
{
	isInLineBreak = false;
	previousChar = currentChar;

	if (!isWhiteSpace(currentChar))
	{
		previousNonWSChar = currentChar;
		if (!isInComment && !isInLineComment && !isInQuote
		        && !isImmediatelyPostComment
		        && !isImmediatelyPostLineComment
		        && !isInPreprocessor
		        && !isSequenceReached("/*")
		        && !isSequenceReached("//"))
			previousCommandChar = currentChar;
	}

	if (charNum + 1 < (int) currentLine.length()
	        && (!isWhiteSpace(peekNextChar()) || isInComment || isInLineComment))
	{
		currentChar = currentLine[++charNum];

		if (shouldConvertTabs && currentChar == '\t')
			convertTabToSpaces();

		return true;
	}

	// end of line has been reached
	return getNextLine();
}

/**
 * get the next line of input, increasing the current placement in the process.
 *
 * @param sequence         the sequence to append.
 * @return   whether succeded in reading the next line.
 */
bool ASFormatter::getNextLine(bool emptyLineWasDeleted /*false*/)
{
	if (sourceIterator->hasMoreLines())
	{
		if (appendOpeningBracket)
			currentLine = "{";		// append bracket that was removed from the previous line
		else
		{
			currentLine = sourceIterator->nextLine(emptyLineWasDeleted);
			assert(computeChecksumIn(currentLine));
		}
		// reset variables for new line
		inLineNumber++;
		isInCase = false;
		isInAsmOneLine = false;
		isInQuoteContinuation = isInVerbatimQuote | haveLineContinuationChar;
		haveLineContinuationChar= false;
		isImmediatelyPostEmptyLine = lineIsEmpty;
		previousChar = ' ';

		if (currentLine.length() == 0)
			currentLine = string(" ");        // a null is inserted if this is not done

		// unless reading in the first line of the file, break a new line.
		if (!isVirgin)
			isInLineBreak = true;
		else
			isVirgin = false;

		if (isImmediatelyPostNonInStmt)
		{
			isCharImmediatelyPostNonInStmt = true;
			isImmediatelyPostNonInStmt = false;
		}

		// check if is in preprocessor before line trimming
		// a blank line after a \ will remove the flag
		isImmediatelyPostPreprocessor = isInPreprocessor;
		if (previousNonWSChar != '\\'
		        || isEmptyLine(currentLine))
			isInPreprocessor = false;

		if (passedSemicolon)
			isInExecSQL = false;
		initNewLine();

		currentChar = currentLine[charNum];
		if (isInHorstmannRunIn && previousNonWSChar == '{' && !isInComment)
			isInLineBreak = false;
		isInHorstmannRunIn = false;

		if (shouldConvertTabs && currentChar == '\t')
			convertTabToSpaces();

		// check for an empty line inside a command bracket.
		// if yes then read the next line (calls getNextLine recursively).
		// must be after initNewLine.
		if (shouldDeleteEmptyLines
		        && lineIsEmpty
		        && isBracketType((*bracketTypeStack)[bracketTypeStack->size()-1], COMMAND_TYPE))
		{
			if (!shouldBreakBlocks || previousNonWSChar == '{' || !commentAndHeaderFollows())
			{
				isInPreprocessor = isImmediatelyPostPreprocessor;		// restore
				lineIsEmpty = false;
				return getNextLine(true);
			}
		}
		return true;
	}
	else
	{
		endOfCodeReached = true;
		return false;
	}
}

/**
 * jump over the leading white space in the current line,
 * IF the line does not begin a comment or is in a preprocessor definition.
 */
void ASFormatter::initNewLine()
{
	size_t len = currentLine.length();
	size_t indent = getIndentLength();
	charNum = 0;

	if (isInPreprocessor || isInQuoteContinuation)
		return;

	// SQL continuation lines must be adjusted so the leading spaces
	// is equivalent to the opening EXEC SQL
	if (isInExecSQL)
	{
		// replace leading tabs with spaces
		// so that continuation indent will be spaces
		size_t tabCount_ = 0;
		size_t i;
		for (i = 0; i < currentLine.length(); i++)
		{
			if (!isWhiteSpace(currentLine[i]))		// stop at first text
				break;
			if (currentLine[i] == '\t')
			{
				size_t numSpaces = indent - ((tabCount_ + i) % indent);
				currentLine.replace(i, 1, numSpaces, ' ');
				tabCount_++;
				i += indent - 1;
			}
		}
		// this will correct the format if EXEC SQL is not a hanging indent
		trimContinuationLine();
		return;
	}

	// comment continuation lines must be adjusted so the leading spaces
	// is equivalent to the opening comment
	if (isInComment)
	{
		if (noTrimCommentContinuation)
			leadingSpaces = tabIncrementIn = 0;
		trimContinuationLine();
		return;
	}

	// compute leading spaces
	isImmediatelyPostCommentOnly = lineIsLineCommentOnly || lineEndsInCommentOnly;
	lineIsLineCommentOnly = false;
	lineEndsInCommentOnly = false;
	doesLineStartComment = false;
	currentLineBeginsWithBracket = false;
	lineIsEmpty = false;
	currentLineFirstBracketNum = string::npos;
	tabIncrementIn = 0;

	for (charNum = 0; isWhiteSpace(currentLine[charNum]) && charNum + 1 < (int) len; charNum++)
	{
		if (currentLine[charNum] == '\t')
			tabIncrementIn += indent - 1 - ((tabIncrementIn + charNum) % indent);
	}
	leadingSpaces = charNum + tabIncrementIn;

	if (isSequenceReached("/*"))
	{
		doesLineStartComment = true;
	}
	else if (isSequenceReached("//"))
	{
		lineIsLineCommentOnly = true;
	}
	else if (isSequenceReached("{"))
	{
		currentLineBeginsWithBracket = true;
		currentLineFirstBracketNum = charNum;
		size_t firstText = currentLine.find_first_not_of(" \t", charNum + 1);
		if (firstText != string::npos)
		{
			if (currentLine.compare(firstText, 2, "//") == 0)
				lineIsLineCommentOnly = true;
			else if (currentLine.compare(firstText, 2, "/*") == 0
			         || isExecSQL(currentLine, firstText))
			{
				// get the extra adjustment
				size_t j;
				for (j = charNum + 1; isWhiteSpace(currentLine[j]) && j < firstText; j++)
				{
					if (currentLine[j] == '\t')
						tabIncrementIn += indent - 1 - ((tabIncrementIn + j) % indent);
				}
				leadingSpaces = j + tabIncrementIn;
				if (currentLine.compare(firstText, 2, "/*") == 0)
					doesLineStartComment = true;
			}
		}
	}
	else if (isWhiteSpace(currentLine[charNum]) && !(charNum + 1 < (int) currentLine.length()))
	{
		lineIsEmpty = true;
	}
}

/**
 * append a string sequence to the current formatted line.
 * Unless disabled (via canBreakLine == false), first check if a
 * line-break has been registered, and if so break the
 * formatted line, and only then append the sequence into
 * the next formatted line.
 *
 * @param sequence         the sequence to append.
 * @param canBreakLine     if true, a registered line-break
 */
void ASFormatter::appendSequence(const string& sequence, bool canBreakLine)
{
	if (canBreakLine && isInLineBreak)
		breakLine();
	formattedLine.append(sequence);
}

/**
 * append a space to the current formattedline, UNLESS the
 * last character is already a white-space character.
 */
void ASFormatter::appendSpacePad()
{
	int len = formattedLine.length();
	if (len > 0 && !isWhiteSpace(formattedLine[len-1]))
	{
		formattedLine.append(1, ' ');
		spacePadNum++;
	}
}

/**
 * append a space to the current formattedline, UNLESS the
 * next character is already a white-space character.
 */
void ASFormatter::appendSpaceAfter()
{
	int len = currentLine.length();
	if (charNum + 1 < len && !isWhiteSpace(currentLine[charNum+1]))
	{
		formattedLine.append(1, ' ');
		spacePadNum++;
	}
}

/**
 * register a line break for the formatted line.
 */
void ASFormatter::breakLine()
{
	isLineReady = true;
	isInLineBreak = false;
	spacePadNum = nextLineSpacePadNum;
	nextLineSpacePadNum = 0;
	formattedLineCommentNum = string::npos;

	// queue an empty line prepend request if one exists
	prependEmptyLine = isPrependPostBlockEmptyLineRequested;

	if (isAppendPostBlockEmptyLineRequested)
	{
		isAppendPostBlockEmptyLineRequested = false;
		isPrependPostBlockEmptyLineRequested = true;
	}
	else
	{
		isPrependPostBlockEmptyLineRequested = false;
	}

	readyFormattedLine =  formattedLine;
	formattedLine = "";
}

/**
 * check if the currently reached open-bracket (i.e. '{')
 * opens a:
 * - a definition type block (such as a class or namespace),
 * - a command block (such as a method block)
 * - a static array
 * this method takes for granted that the current character
 * is an opening bracket.
 *
 * @return    the type of the opened block.
 */
BracketType ASFormatter::getBracketType()
{
	assert(currentChar == '{');

	BracketType returnVal;

	if ((previousNonWSChar == '='
	        || isBracketType(bracketTypeStack->back(),  ARRAY_TYPE))
	        && previousCommandChar != ')')
		returnVal = ARRAY_TYPE;
	else if (foundPreDefinitionHeader)
	{
		returnVal = DEFINITION_TYPE;
		if (foundNamespaceHeader)
			returnVal = (BracketType)(returnVal | NAMESPACE_TYPE);
		else if (foundClassHeader)
			returnVal = (BracketType)(returnVal | CLASS_TYPE);
		else if (foundStructHeader)
			returnVal = (BracketType)(returnVal | STRUCT_TYPE);
		else if (foundInterfaceHeader)
			returnVal = (BracketType)(returnVal | INTERFACE_TYPE);
	}
	else
	{
		bool isCommandType = (foundPreCommandHeader
		                      || (currentHeader != NULL && isNonParenHeader)
		                      || (previousCommandChar == ')')
		                      || (previousCommandChar == ':' && !foundQuestionMark)
		                      || (previousCommandChar == ';')
		                      || ((previousCommandChar == '{' ||  previousCommandChar == '}')
		                          && isPreviousBracketBlockRelated)
		                      || isJavaStaticConstructor
		                      || isSharpDelegate);

		// C# methods containing 'get', 'set', 'add', and 'remove' do NOT end with parens
		if (!isCommandType && isSharpStyle() && isNextWordSharpNonParenHeader(charNum + 1))
		{
			isCommandType = true;
			isSharpAccessor = true;
		}

		if (!isCommandType && isInExtern)
			returnVal = EXTERN_TYPE;
		else
			returnVal = (isCommandType ? COMMAND_TYPE : ARRAY_TYPE);
	}

	int foundOneLineBlock = isOneLineBlockReached(currentLine, charNum);
	// this assumes each array definition is on a single line
	// (foundOneLineBlock == 2) is a one line block followed by a comma
	if (foundOneLineBlock == 2 && returnVal == COMMAND_TYPE)
		returnVal = ARRAY_TYPE;

	if (foundOneLineBlock > 0)		// found one line block
		returnVal = (BracketType)(returnVal | SINGLE_LINE_TYPE);

	if (isBracketType(returnVal, ARRAY_TYPE) && isNonInStatementArrayBracket())
	{
		returnVal = (BracketType)(returnVal | ARRAY_NIS_TYPE);
		isNonInStatementArray = true;
		nonInStatementBracket = formattedLine.length() - 1;
	}

	return returnVal;
}

/**
 * check if a line is empty
 *
 * @return        whether line is empty
 */
bool ASFormatter::isEmptyLine(const string& line) const
{
	return line.find_first_not_of(" \t") == string::npos;
}

/**
 * Check if the currently reached  '*' or '&' character is
 * a pointer-or-reference symbol, or another operator.
 * A pointer dereference (*) or an "address of" character (&)
 * counts as a pointer or reference because it is not an
 * arithmetic operator.
 *
 * @return        whether current character is a reference-or-pointer
 */
bool ASFormatter::isPointerOrReference() const
{
	assert(currentChar == '*' || currentChar == '&');

	if (!isCStyle())
		return false;

	if ((currentChar == '&' && previousChar == '&')
	        || isCharImmediatelyPostOperator)
		return false;

	if (previousNonWSChar == '='
	        || previousNonWSChar == '('
	        || previousNonWSChar == '['
	        || currentHeader == &AS_CATCH
	        || isCharImmediatelyPostReturn)
		return true;

	// get the last legal word (may be a number)
	string lastWord = getPreviousWord(currentLine, charNum);
	if (lastWord.empty())
		lastWord = " ";
	char nextChar = peekNextChar();

	// check for preceding or following numeric values
	if (isDigit(lastWord[0])
	        || isDigit(nextChar)
	        || nextChar == '!')
		return false;

	// checks on operators in parens
	if (parenStack->back() > 0
	        && isLegalNameChar(lastWord[0])
	        && isLegalNameChar(nextChar))
	{
		// if followed by an assignment it is a pointer or reference
		size_t nextNum = currentLine.find_first_of("=;)]", charNum + 1);
		if (nextNum != string::npos && currentLine[nextNum] == '=')
			return true;

		// if a function definition it is a pointer or reference
		// otherwise it is an arithmetic operator
		if (!isBracketType(bracketTypeStack->back(), COMMAND_TYPE))
			return true;
		else
			return false;
	}

	// checks on operators in parens with following '('
	if (parenStack->back() > 0
	        && nextChar == '('
	        && previousNonWSChar != ','
	        && previousNonWSChar != '('
	        && previousNonWSChar != '!'
	        && previousNonWSChar != '&'
	        && previousNonWSChar != '*'
	        && previousNonWSChar != '|')
		return false;

	if (nextChar == '-'
	        || nextChar == '+')
	{
		size_t nextNum = currentLine.find_first_not_of(" \t", charNum + 1);
		if (nextNum != string::npos)
		{
			if (currentLine.compare(nextNum, 2, "++") != 0
			        && currentLine.compare(nextNum, 2, "--") != 0)
				return false;
		}
	}

	bool isPR = (!isInPotentialCalculation
	             || isBracketType(bracketTypeStack->back(), DEFINITION_TYPE)
	             || (!isLegalNameChar(previousNonWSChar)
	                 && !(previousNonWSChar == ')' && nextChar == '(')
	                 && !(previousNonWSChar == ')' && currentChar == '*' && !isImmediatelyPostCast())
	                 && previousNonWSChar != ']')
	            );

	if (!isPR)
	{
		isPR |= (!isWhiteSpace(nextChar)
		         && nextChar != '-'
		         && nextChar != '('
		         && nextChar != '['
		         && !isLegalNameChar(nextChar));
	}

	return isPR;
}

/**
 * Check if the currently reached  '*' or '&' character is
 * a dereferenced pointer or "address of" symbol.
 * NOTE: this MUST be a pointer or reference as determined by
 * the function isPointerOrReference().
 *
 * @return        whether current character is a dereference or address of
 */
bool ASFormatter::isDereferenceOrAddressOf() const
{
	assert(isPointerOrReference());

	if (previousNonWSChar == '='
	        || previousNonWSChar == ','
	        || previousNonWSChar == '.'
	        || previousNonWSChar == '{'
	        || previousNonWSChar == '>'
	        || previousNonWSChar == '<'
	        || isCharImmediatelyPostReturn)
		return true;

	// check for **
	if (currentChar == '*'
	        && (int) currentLine.length() > charNum
	        && currentLine[charNum+1] == '*')
	{
		if (previousNonWSChar == '(')
			return true;
		if ((int) currentLine.length() < charNum + 2)
			return true;
		return false;
	}

	// check first char on the line
	if (charNum == (int) currentLine.find_first_not_of(" \t"))
		return true;

	size_t nextChar = currentLine.find_first_not_of(" \t", charNum+1);
	if (nextChar != string::npos
	        && (currentLine[nextChar] == ')'
	            || currentLine[nextChar] == '>'
	            || currentLine[nextChar] == ','))
		return false;

	if (!isBracketType(bracketTypeStack->back(), COMMAND_TYPE)
	        && parenStack->back() == 0)
		return false;

	string lastWord = getPreviousWord(currentLine, charNum);
	if (lastWord == "else" || lastWord == "delete")
		return true;

	bool isDA = (!(isLegalNameChar(previousNonWSChar) || previousNonWSChar == '>')
	             || (!isLegalNameChar(peekNextChar()) && peekNextChar() != '/')
	             || (ispunct(previousNonWSChar) && previousNonWSChar != '.')
	             || isCharImmediatelyPostReturn);

	return isDA;
}

/**
 * Check if the currently reached  '*' or '&' character is
 * centered with one space on each side.
 * Only spaces are checked, not tabs.
 * If true then a space will be deleted on the output.
 *
 * @return        whether current character is centered.
 */
bool ASFormatter::isPointerOrReferenceCentered() const
{
	assert(currentLine[charNum] == '*' || currentLine[charNum] == '&');

	int prNum = charNum;
	int lineLength = (int) currentLine.length();

	// check for end of  line
	if (peekNextChar() == ' ')
		return false;

	// check space before
	if (prNum < 1
	        || currentLine[prNum-1] != ' ')
		return false;

	// check no space before that
	if (prNum < 2
	        || currentLine[prNum-2] == ' ')
		return false;

	// check for **
	if (prNum + 1 < lineLength
	        && currentLine[prNum+1] == '*')
		prNum++;

	// check space after
	if (prNum + 1 <= lineLength
	        && currentLine[prNum+1] != ' ')
		return false;

	// check no space after that
	if (prNum + 2 < lineLength
	        && currentLine[prNum+2] == ' ')
		return false;

	return true;
}

/**
 * check if the currently reached '+' or '-' character is a unary operator
 * this method takes for granted that the current character
 * is a '+' or '-'.
 *
 * @return        whether the current '+' or '-' is a unary operator.
 */
bool ASFormatter::isUnaryOperator() const
{
	assert(currentChar == '+' || currentChar == '-');

	return ((isCharImmediatelyPostReturn || !isLegalNameChar(previousCommandChar))
	        && previousCommandChar != '.'
	        && previousCommandChar != '\"'
	        && previousCommandChar != '\''
	        && previousCommandChar != ')'
	        && previousCommandChar != ']');
}


/**
 * check if the currently reached '+' or '-' character is
 * part of an exponent, i.e. 0.2E-5.
 *
 * this method takes for granted that the current character
 * is a '+' or '-'.
 *
 * @return        whether the current '+' or '-' is in an exponent.
 */
bool ASFormatter::isInExponent() const
{
	assert(currentChar == '+' || currentChar == '-');

	int formattedLineLength = formattedLine.length();
	if (formattedLineLength >= 2)
	{
		char prevPrevFormattedChar = formattedLine[formattedLineLength - 2];
		char prevFormattedChar = formattedLine[formattedLineLength - 1];

		return ((prevFormattedChar == 'e' || prevFormattedChar == 'E')
		        && (prevPrevFormattedChar == '.' || isDigit(prevPrevFormattedChar)));
	}
	else
		return false;
}

/**
 * check if an array bracket should NOT have an in-statement indent
 *
 * @return        the array is non in-statement
 */
bool ASFormatter::isNonInStatementArrayBracket() const
{
	bool returnVal = false;
	char nextChar = peekNextChar();
	// if this opening bracket begins the line there will be no inStatement indent
	if (currentLineBeginsWithBracket
	        && charNum == (int) currentLineFirstBracketNum
	        && nextChar != '}')
		returnVal = true;
	// if an opening bracket ends the line there will be no inStatement indent
	if (isWhiteSpace(nextChar)
	        || isBeforeAnyLineEndComment(charNum)
	        || nextChar == '{')
		returnVal = true;

	// Java "new Type [] {...}" IS an inStatement indent
	if (isJavaStyle() && previousNonWSChar == ']')
		returnVal = false;

	// trace
	//if (isNonInStatementArray)
	//	cout << traceLineNumber << " " << 'x' << endl;
	//else
	//	cout << traceLineNumber << " " << ' ' << endl

	return returnVal;
}

/**
 * check if a one-line bracket has been reached,
 * i.e. if the currently reached '{' character is closed
 * with a complimentry '}' elsewhere on the current line,
 *.
 * @return     0 = one-line bracket has not been reached.
 *             1 = one-line bracket has been reached.
 *             2 = one-line bracket has been reached and is followed by a comma.
 */
int ASFormatter::isOneLineBlockReached(string& line, int startChar) const
{
	assert(line[startChar] == '{');

	bool isInComment_ = false;
	bool isInQuote_ = false;
	int bracketCount = 1;
	int lineLength = line.length();
	char quoteChar_ = ' ';
	char ch = ' ';
	char prevCh = ' ';

	for (int i = startChar + 1; i < lineLength; ++i)
	{
		ch = line[i];

		if (isInComment_)
		{
			if (line.compare(i, 2, "*/") == 0)
			{
				isInComment_ = false;
				++i;
			}
			continue;
		}

		if (ch == '\\')
		{
			++i;
			continue;
		}

		if (isInQuote_)
		{
			if (ch == quoteChar_)
				isInQuote_ = false;
			continue;
		}

		if (ch == '"' || ch == '\'')
		{
			isInQuote_ = true;
			quoteChar_ = ch;
			continue;
		}

		if (line.compare(i, 2, "//") == 0)
			break;

		if (line.compare(i, 2, "/*") == 0)
		{
			isInComment_ = true;
			++i;
			continue;
		}

		if (ch == '{')
			++bracketCount;
		else if (ch == '}')
			--bracketCount;

		if (bracketCount == 0)
		{
			// is this an array?
			if (parenStack->back() == 0 && prevCh != '}')
			{
				size_t peekNum = line.find_first_not_of(" \t", i + 1);
				if (peekNum != string::npos && line[peekNum] == ',')
					return 2;
			}
			return 1;
		}
		if (!isWhiteSpace(ch))
			prevCh = ch;
	}

	return 0;
}

/**
 * peek at the next word to determine if it is a C# non-paren header.
 * will look ahead in the input file if necessary.
 *
 * @param       char position on currentLine to start the search
 * @return      true if the next word is get or set.
 */
bool ASFormatter::isNextWordSharpNonParenHeader(int startChar) const
{
	// look ahead to find the next non-comment text
	string nextText = peekNextText(currentLine.substr(startChar));
	if (nextText.length() == 0)
		return false;
	if (nextText[0] == '[')
		return true;
	if (!isCharPotentialHeader(nextText, 0))
		return false;
	if (findKeyword(nextText, 0, AS_GET) || findKeyword(nextText, 0, AS_SET)
	        || findKeyword(nextText, 0, AS_ADD) || findKeyword(nextText, 0, AS_REMOVE))
		return true;
	return false;
}

/**
 * peek at the next char to determine if it is an opening bracket.
 * will look ahead in the input file if necessary.
 * this determines a java static constructor.
 *
 * @param       char position on currentLine to start the search
 * @return      true if the next word is an opening bracket.
 */
bool ASFormatter::isNextCharOpeningBracket(int startChar) const
{
	bool retVal = false;
	string nextText = peekNextText(currentLine.substr(startChar));
	if (nextText.compare(0, 1, "{") == 0)
		retVal = true;
	return retVal;
}

/**
 * get the next non-whitespace substring on following lines, bypassing all comments.
 *
 * @param   the first line to check
 * @return  the next non-whitespace substring.
 */
string ASFormatter::peekNextText(const string& firstLine, bool endOnEmptyLine /*false*/, bool shouldReset /*false*/) const
{
	bool isFirstLine = true;
	bool needReset = shouldReset;
	string nextLine_ = firstLine;
	size_t firstChar= string::npos;

	// find the first non-blank text, bypassing all comments.
	bool isInComment_ = false;
	while (sourceIterator->hasMoreLines())
	{
		if (isFirstLine)
			isFirstLine = false;
		else
		{
			nextLine_ = sourceIterator->peekNextLine();
			needReset = true;
		}

		firstChar = nextLine_.find_first_not_of(" \t");
		if (firstChar == string::npos)
		{
			if (endOnEmptyLine && !isInComment_)
				break;
			continue;
		}

		if (nextLine_.compare(firstChar, 2, "/*") == 0)
		{
			firstChar += 2;
			isInComment_ = true;
		}

		if (isInComment_)
		{
			firstChar = nextLine_.find("*/", firstChar);
			if (firstChar == string::npos)
				continue;
			firstChar += 2;
			isInComment_ = false;
			firstChar = nextLine_.find_first_not_of(" \t", firstChar);
			if (firstChar == string::npos)
				continue;
		}

		if (nextLine_.compare(firstChar, 2, "//") == 0)
			continue;

		// found the next text
		break;
	}

	if (needReset)
		sourceIterator->peekReset();
	if (firstChar == string::npos)
		nextLine_ = "";
	else
		nextLine_ = nextLine_.substr(firstChar);
	return nextLine_;
}

/**
 * adjust comment position because of adding or deleting spaces
 * the spaces are added or deleted to formattedLine
 * spacePadNum contains the adjustment
 */
void ASFormatter::adjustComments(void)
{
	assert(spacePadNum != 0);
	assert(currentLine.compare(charNum, 2, "//") == 0
	       || currentLine.compare(charNum, 2, "/*") == 0);


	// block comment must be closed on this line with nothing after it
	if (currentLine.compare(charNum, 2, "/*") == 0)
	{
		size_t endNum = currentLine.find("*/", charNum + 2);
		if (endNum == string::npos)
			return;
		if (currentLine.find_first_not_of(" \t", endNum + 2) != string::npos)
			return;
	}

	size_t len = formattedLine.length();
	// don't adjust a tab
	if (formattedLine[len-1] == '\t')
		return;
	// if spaces were removed, need to add spaces before the comment
	if (spacePadNum < 0)
	{
		int adjust = -spacePadNum;          // make the number positive
		formattedLine.append(adjust, ' ');
	}
	// if spaces were added, need to delete extra spaces before the comment
	// if cannot be done put the comment one space after the last text
	else if (spacePadNum > 0)
	{
		int adjust = spacePadNum;
		size_t lastText = formattedLine.find_last_not_of(' ');
		if (lastText != string::npos
		        && lastText < len - adjust - 1)
			formattedLine.resize(len - adjust);
		else if (len > lastText + 2)
			formattedLine.resize(lastText + 2);
		else if (len < lastText + 2)
			formattedLine.append(len - lastText, ' ');
	}
}

/**
 * append the current bracket inside the end of line comments
 * currentChar contains the bracket, it will be appended to formattedLine
 * formattedLineCommentNum is the comment location on formattedLine
 */
void ASFormatter::appendCharInsideComments(void)
{
	if (formattedLineCommentNum == string::npos)    // does the comment start on the previous line?
	{
		appendCurrentChar();                        // don't attach
		return;
	}
	assert(formattedLine.compare(formattedLineCommentNum, 2, "//") == 0
	       || formattedLine.compare(formattedLineCommentNum, 2, "/*") == 0);

	// find the previous non space char
	size_t end = formattedLineCommentNum;
	size_t beg = formattedLine.find_last_not_of(" \t", end-1);
	if (beg == string::npos)
	{
		appendCurrentChar();                // don't attach
		return;
	}
	beg++;

	// insert the bracket
	if (end - beg < 3)                      // is there room to insert?
		formattedLine.insert(beg, 3-end+beg, ' ');
	if (formattedLine[beg] == '\t')         // don't pad with a tab
		formattedLine.insert(beg, 1, ' ');
	formattedLine[beg+1] = currentChar;

	if (isBeforeComment())
		breakLine();
	else if (isCharImmediatelyPostLineComment)
		shouldBreakLineAtNextChar = true;
	return;
}

/**
 * add or remove space padding to operators
 * currentChar contains the paren
 * the operators and necessary padding will be appended to formattedLine
 * the calling function should have a continue statement after calling this method
 *
 * @param *newOperator     the operator to be padded
 */
void ASFormatter::padOperators(const string* newOperator)
{
	assert(newOperator != NULL);

	bool shouldPad = (newOperator != &AS_COLON_COLON
	                  && newOperator != &AS_PLUS_PLUS
	                  && newOperator != &AS_MINUS_MINUS
	                  && newOperator != &AS_NOT
	                  && newOperator != &AS_BIT_NOT
	                  && newOperator != &AS_ARROW
	                  && !(newOperator == &AS_MINUS && isInExponent())
	                  && !((newOperator == &AS_PLUS || newOperator == &AS_MINUS)  // check for unary plus or minus
	                       && (previousNonWSChar == '('
	                           || previousNonWSChar == '['
	                           || previousNonWSChar == '='
	                           || previousNonWSChar == ','))
	                  && !(newOperator == &AS_PLUS && isInExponent())
	                  && !isCharImmediatelyPostOperator
	                  && !((newOperator == &AS_MULT || newOperator == &AS_BIT_AND)
	                       && isPointerOrReference())
	                  && !(newOperator == &AS_MULT
	                       && (previousNonWSChar == '.'
	                           || previousNonWSChar == '>'))    // check for ->
	                  && !((isInTemplate || isCharImmediatelyPostTemplate)
	                       && (newOperator == &AS_LS || newOperator == &AS_GR))
	                  && !(newOperator == &AS_GCC_MIN_ASSIGN
	                       && ASBase::peekNextChar(currentLine, charNum+1) == '>')
	                  && !(newOperator == &AS_GR && previousNonWSChar == '?')
	                  && !isInCase
	                  && !isInAsm
	                  && !isInAsmOneLine
	                  && !isInAsmBlock
	                 );

	// pad before operator
	if (shouldPad
	        && !(newOperator == &AS_COLON && !foundQuestionMark)
	        && !(newOperator == &AS_QUESTION && isSharpStyle() // check for C# nullable type (e.g. int?)
	             && currentLine.find(':', charNum+1) == string::npos)
	   )
		appendSpacePad();
	appendSequence(*newOperator);
	goForward(newOperator->length() - 1);

	currentChar = (*newOperator)[newOperator->length() - 1];
	// pad after operator
	// but do not pad after a '-' that is a unary-minus.
	if (shouldPad
	        && !isBeforeAnyComment()
	        && !(newOperator == &AS_PLUS && isUnaryOperator())
	        && !(newOperator == &AS_MINUS && isUnaryOperator())
	        && !(currentLine.compare(charNum + 1, 1,  ";") == 0)
	        && !(currentLine.compare(charNum + 1, 2, "::") == 0)
	        && !(newOperator == &AS_QUESTION && isSharpStyle() // check for C# nullable type (e.g. int?)
	             && peekNextChar() == '[')
	   )
		appendSpaceAfter();

	previousOperator = newOperator;
	return;
}

/**
 * format pointer or reference
 * currentChar contains the pointer or reference
 * the symbol and necessary padding will be appended to formattedLine
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatPointerOrReference(void)
{
	assert(currentChar == '*' || currentChar == '&');
	assert(isCStyle());

	int pa = pointerAlignment;
	int ra = referenceAlignment;
	int itemAlignment = (currentChar == '*') ? pa : ((ra == REF_SAME_AS_PTR) ? pa : ra);

	// check for cast
	char peekedChar = peekNextChar();
	if (currentChar == '*'
	        && (int) currentLine.length() > charNum + 1
	        && currentLine[charNum+1] == '*')
	{
		size_t nextChar = currentLine.find_first_not_of(" \t", charNum+2);
		if (nextChar == string::npos)
			peekedChar = ' ';
		else
			peekedChar = currentLine[nextChar];
	}
	if (peekedChar == ')' || peekedChar =='>' || peekedChar ==',')
	{
		formatPointerOrReferenceCast();
		return;
	}

	// check for a padded space and remove it
	if (charNum > 0
	        && !isWhiteSpace(currentLine[charNum-1])
	        && formattedLine.length() > 0
	        && isWhiteSpace(formattedLine[formattedLine.length()-1]))
		formattedLine.erase(formattedLine.length()-1);

	// do this before bumping charNum
	bool isOldPRCentered = isPointerOrReferenceCentered();

	if (itemAlignment == PTR_ALIGN_TYPE)
	{
		size_t prevCh = formattedLine.find_last_not_of(" \t");
		if (prevCh == string::npos)
			prevCh = 0;
		if (formattedLine.length() == 0 || prevCh == formattedLine.length() - 1)
			appendCurrentChar();
		else
		{
			// exchange * or & with character following the type
			// this may not work every time with a tab character
			string charSave = formattedLine.substr(prevCh+1, 1);
			formattedLine[prevCh+1] = currentChar;
			formattedLine.append(charSave);
		}
		if (isSequenceReached("**"))
		{
			formattedLine.insert(prevCh+2, "*");
			goForward(1);
		}
		// if no space after * then add one
		if (charNum < (int) currentLine.length() - 1
		        && !isWhiteSpace(currentLine[charNum+1])
		        && currentLine[charNum+1] != ')')
			appendSpacePad();
		// if old pointer or reference is centered, remove a space
		if (isOldPRCentered
		        && isWhiteSpace(formattedLine[formattedLine.length()-1]))
		{
			formattedLine.erase(formattedLine.length()-1, 1);
			spacePadNum--;
		}
	}
	else if (itemAlignment == PTR_ALIGN_MIDDLE)
	{
		// compute current whitespace before
		size_t wsBefore = currentLine.find_last_not_of(" \t", charNum - 1);
		if (wsBefore == string::npos)
			wsBefore = 0;
		else
			wsBefore = charNum - wsBefore - 1;
		// adjust for **
		string sequenceToInsert = currentChar == '*' ? "*" : "&";
		if (isSequenceReached("**"))
		{
			sequenceToInsert = "**";
			goForward(1);
		}
		bool isAfterScopeResolution = previousNonWSChar == ':';		// check for ::
		size_t charNumSave = charNum;
		// if a comment follows don't align, just space pad
		if (isBeforeAnyComment())
		{
			appendSpacePad();
			formattedLine.append(sequenceToInsert);
			appendSpaceAfter();
			return;
		}
		// if this is not the last thing on the line
		if ((int) currentLine.find_first_not_of(" \t", charNum + 1) > charNum)
		{
			// goForward() to convert tabs to spaces, if necessary,
			// and move following characters to preceding characters
			// this may not work every time with tab characters
			for (size_t i = charNum+1; i < currentLine.length() && isWhiteSpace(currentLine[i]); i++)
			{
				goForward(1);
				formattedLine.append(1, currentLine[i]);
			}
		}
		// find space padding after
		size_t wsAfter = currentLine.find_first_not_of(" \t", charNumSave + 1);
		if (wsAfter == string::npos || isBeforeAnyComment())
			wsAfter = 0;
		else
			wsAfter = wsAfter - charNumSave - 1;
		// don't pad before scope resolution operator, but pad after
		if (isAfterScopeResolution)
		{
			size_t lastText = formattedLine.find_last_not_of(" \t");
			formattedLine.insert(lastText + 1, sequenceToInsert);
			appendSpacePad();
		}
		// whitespace should be at least 2 chars to center
		else
		{
			if (wsBefore + wsAfter < 2)
			{
				size_t charsToAppend = (2 - (wsBefore + wsAfter));
				formattedLine.append(charsToAppend, ' ');
				spacePadNum += charsToAppend;
				if (wsBefore == 0) wsBefore++;
				if (wsAfter == 0) wsAfter++;
			}
			// insert the pointer or reference char
			size_t padAfter = (wsBefore + wsAfter) / 2;
			formattedLine.insert(formattedLine.length() - padAfter, sequenceToInsert);
		}
	}
	else if (itemAlignment == PTR_ALIGN_NAME)
	{
		size_t startNum = formattedLine.find_last_not_of(" \t");
		string sequenceToInsert = currentChar == '*' ? "*" : "&";
		if (isSequenceReached("**"))
		{
			sequenceToInsert = "**";
			goForward(1);
		}
		bool isAfterScopeResolution = previousNonWSChar == ':';		// check for ::
		// if this is not the last thing on the line
		if (!isBeforeAnyComment()
		        &&  (int) currentLine.find_first_not_of(" \t", charNum + 1) > charNum)
		{
			// goForward() to convert tabs to spaces, if necessary,
			// and move following characters to preceding characters
			// this may not work every time with tab characters
			for (size_t i = charNum+1; i < currentLine.length() && isWhiteSpace(currentLine[i]); i++)
			{
				// if a padded paren follows don't move
				if (shouldPadParensOutside && peekedChar == '(' && !isOldPRCentered)
					break;
				goForward(1);
				formattedLine.append(1, currentLine[i]);
			}
		}
		// don't pad before scope resolution operator
		if (startNum != string::npos && isAfterScopeResolution)
		{
			size_t lastText = formattedLine.find_last_not_of(" \t");
			if (lastText + 1 < formattedLine.length())
				formattedLine.erase(lastText + 1);
		}
		// if no space before * then add one
		else if (formattedLine.length() <= startNum + 1
		         || !isWhiteSpace(formattedLine[startNum+1]))
		{
			formattedLine.insert(startNum+1 , 1, ' ');
			spacePadNum++;
		}
		appendSequence(sequenceToInsert, false);
		// if old pointer or reference is centered, remove a space
		if (isOldPRCentered
		        && formattedLine.length() > startNum+1
		        && isWhiteSpace(formattedLine[startNum+1])
		        && !isBeforeAnyComment())
		{
			formattedLine.erase(startNum+1, 1);
			spacePadNum--;
		}
	}
	else	// pointerAlignment == PTR_ALIGN_NONE
	{
		appendCurrentChar();
	}
	return;
}

/**
 * format pointer or reference cast
 * currentChar contains the pointer or reference
 * NOTE: the pointers and references in function definitions
 *       are processed as a cast (e.g. void foo(void*, void*))
 *       is processed here.
 */
void ASFormatter::formatPointerOrReferenceCast(void)
{
	assert(currentChar == '*' || currentChar == '&');
	assert(isCStyle());

	int pa = pointerAlignment;
	int ra = referenceAlignment;
	int itemAlignment = (currentChar == '*') ? pa : ((ra == REF_SAME_AS_PTR) ? pa : ra);

	string sequenceToInsert = currentChar == '*' ? "*" : "&";
	if (isSequenceReached("**"))
	{
		sequenceToInsert = "**";
		goForward(1);
	}
	if (itemAlignment == PTR_ALIGN_NONE)
	{
		appendSequence(sequenceToInsert, false);
		return;
	}
	// remove trailing whitespace
	size_t prevCh = formattedLine.find_last_not_of(" \t");
	if (prevCh == string::npos)
		prevCh = 0;
	if (prevCh + 1 < formattedLine.length()
	        && isWhiteSpace(formattedLine[prevCh+1]))
	{
		spacePadNum -= (formattedLine.length() - 1 - prevCh);
		formattedLine.erase(prevCh+1);
	}
	if (itemAlignment == PTR_ALIGN_MIDDLE
	        || itemAlignment == PTR_ALIGN_NAME)
	{
		appendSpacePad();
		appendSequence(sequenceToInsert, false);
	}
	else
		appendSequence(sequenceToInsert, false);
}

/**
 * add or remove space padding to parens
 * currentChar contains the paren
 * the parens and necessary padding will be appended to formattedLine
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::padParens(void)
{
	assert(currentChar == '(' || currentChar == ')');

	int spacesOutsideToDelete = 0;
	int spacesInsideToDelete = 0;

	if (currentChar == '(')
	{
		spacesOutsideToDelete = formattedLine.length() - 1;
		spacesInsideToDelete = 0;

		// compute spaces outside the opening paren to delete
		if (shouldUnPadParens)
		{
			char lastChar = ' ';
			bool prevIsParenHeader = false;
			size_t i = formattedLine.find_last_not_of(" \t");
			if (i != string::npos)
			{
				// if last char is a bracket the previous whitespace is an indent
				if (formattedLine[i] == '{')
					spacesOutsideToDelete = 0;
				else if (isCharImmediatelyPostPointerOrReference)
					spacesOutsideToDelete = 0;
				else
				{
					spacesOutsideToDelete -= i;
					lastChar = formattedLine[i];
					// if previous word is a header, it will be a paren header
					string prevWord = getPreviousWord(formattedLine, formattedLine.length());
					const string* prevWordH = NULL;
					if (shouldPadHeader
					        && prevWord.length() > 0
					        && isCharPotentialHeader(prevWord, 0))
						prevWordH = ASBeautifier::findHeader(prevWord, 0, headers);
					if (prevWordH != NULL)
					{
						prevIsParenHeader = true;
						// trace
						//cout << traceLineNumber << " " << *prevWordH << endl;
					}
					else if (prevWord == "return")  // don't unpad return statements
					{
						prevIsParenHeader = true;
						// trace
						//cout << traceLineNumber << " " << prevWord << endl;
					}
					// don't unpad variables
					else if (prevWord == "bool"
					         || prevWord ==  "int"
					         || prevWord ==  "void"
					         || prevWord ==  "void*"
					         || (prevWord.length() >= 6     // check end of word for _t
					             && prevWord.compare(prevWord.length()-2, 2, "_t") == 0)
					         || prevWord ==  "BOOL"
					         || prevWord ==  "DWORD"
					         || prevWord ==  "HWND"
					         || prevWord ==  "INT"
					         || prevWord ==  "LPSTR"
					         || prevWord ==  "VOID"
					         || prevWord ==  "LPVOID"
					        )
					{
						prevIsParenHeader = true;
						// trace
						//cout << traceLineNumber << " " << prevWord << endl;
					}
				}
			}
			// do not unpad operators, but leave them if already padded
			if (shouldPadParensOutside || prevIsParenHeader)
				spacesOutsideToDelete--;
			else if (lastChar == '|'          // check for ||
			         || lastChar == '&'       // check for &&
			         || lastChar == ','
			         || (lastChar == '(' && shouldPadParensInside)
			         || (lastChar == '>' && !foundCastOperator)
			         || lastChar == '<'
			         || lastChar == '?'
			         || lastChar == ':'
			         || lastChar == ';'
			         || lastChar == '='
			         || lastChar == '+'
			         || lastChar == '-'
			         || lastChar == '*'
			         || lastChar == '/'
			         || lastChar == '%'
			         || lastChar == '^'
			        )
				spacesOutsideToDelete--;

			if (spacesOutsideToDelete > 0)
			{
				formattedLine.erase(i + 1, spacesOutsideToDelete);
				spacePadNum -= spacesOutsideToDelete;
			}
		}

		// pad open paren outside
		char peekedCharOutside = peekNextChar();
		if (shouldPadParensOutside)
			if (!(currentChar == '(' && peekedCharOutside == ')'))
				appendSpacePad();

		appendCurrentChar();

		// unpad open paren inside
		if (shouldUnPadParens)
		{
			size_t j = currentLine.find_first_not_of(" \t", charNum + 1);
			if (j != string::npos)
				spacesInsideToDelete = j - charNum - 1;
			if (shouldPadParensInside)
				spacesInsideToDelete--;
			if (spacesInsideToDelete > 0)
			{
				currentLine.erase(charNum + 1, spacesInsideToDelete);
				spacePadNum -= spacesInsideToDelete;
			}
			// convert tab to space if requested
			if (shouldConvertTabs
			        && (int)currentLine.length() > charNum + 1
			        && currentLine[charNum+1] == '\t')
				currentLine[charNum+1] = ' ';

		}

		// pad open paren inside
		char peekedCharInside = peekNextChar();
		if (shouldPadParensInside)
			if (!(currentChar == '(' && peekedCharInside == ')'))
				appendSpaceAfter();
		// trace
		//if(spacesOutsideToDelete > 0 || spacesInsideToDelete > 0)
		//    cout << traceLineNumber << " " << spacesOutsideToDelete << '(' << spacesInsideToDelete << endl;
	}
	else if (currentChar == ')')
	{
		spacesOutsideToDelete = 0;
		spacesInsideToDelete = formattedLine.length();

		// unpad close paren inside
		if (shouldUnPadParens)
		{
			size_t i = formattedLine.find_last_not_of(" \t");
			if (i != string::npos)
				spacesInsideToDelete = formattedLine.length() - 1 - i;
			if (shouldPadParensInside)
				spacesInsideToDelete--;
			if (spacesInsideToDelete > 0)
			{
				formattedLine.erase(i + 1, spacesInsideToDelete);
				spacePadNum -= spacesInsideToDelete;
			}
		}

		// pad close paren inside
		if (shouldPadParensInside)
			if (!(previousChar == '(' && currentChar == ')'))
				appendSpacePad();

		appendCurrentChar();

		// unpad close paren outside
		// close parens outside are left unchanged
		if (shouldUnPadParens)
		{
			//size_t j = currentLine.find_first_not_of(" \t", charNum + 1);
			//if (j != string::npos)
			//	spacesOutsideToDelete = j - charNum - 1;
			//if (shouldPadParensOutside)
			//	spacesOutsideToDelete--;

			//if (spacesOutsideToDelete > 0)
			//{
			//	currentLine.erase(charNum + 1, spacesOutsideToDelete);
			//	spacePadNum -= spacesOutsideToDelete;
			//}
		}

		// pad close paren outside
		char peekedCharOutside = peekNextChar();
		if (shouldPadParensOutside)
			if (peekedCharOutside != ';'
			        && peekedCharOutside != ','
			        && peekedCharOutside != '.'
			        && peekedCharOutside != '-'     // check for ->
			        && peekedCharOutside != ']')
				appendSpaceAfter();

		// trace
		//if(spacesInsideToDelete > 0)
		//	cout << traceLineNumber << " " << spacesInsideToDelete << ')' << 0 << endl;
	}
	return;
}

/**
 * format opening bracket as attached or broken
 * currentChar contains the bracket
 * the brackets will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 *
 * @param bracketType    the type of bracket to be formatted.
 */
void ASFormatter::formatOpeningBracket(BracketType bracketType)
{
	assert(!isBracketType(bracketType, ARRAY_TYPE));
	assert(currentChar == '{');

	parenStack->push_back(0);

	bool breakBracket = isCurrentBracketBroken();

	if (breakBracket)
	{
		if (isBeforeAnyComment() && isOkToBreakBlock(bracketType))
		{
			// if comment is at line end leave the comment on this line
			if (isBeforeAnyLineEndComment(charNum) && !currentLineBeginsWithBracket)	// lineBeginsWith('{')
			{
				currentChar = ' ';              // remove bracket from current line
				if (parenStack->size() > 1)
					parenStack->pop_back();
				currentLine[charNum] = currentChar;
				appendOpeningBracket = true;    // append bracket to following line
			}
			// else put comment after the bracket
			else if (!isBeforeMultipleLineEndComments(charNum))
				breakLine();
		}
		else if (!isBracketType(bracketType, SINGLE_LINE_TYPE))
			breakLine();
		else if (shouldBreakOneLineBlocks && peekNextChar() != '}')
			breakLine();
		else if (!isInLineBreak)
			appendSpacePad();

		appendCurrentChar();

		// should a following comment break from the bracket?
		// must break the line AFTER the bracket
		if (isBeforeComment()
		        && formattedLine.length() > 0
		        && formattedLine[0] == '{'
		        && isOkToBreakBlock(bracketType)
		        && (bracketFormatMode == BREAK_MODE
		            || bracketFormatMode == LINUX_MODE
		            || bracketFormatMode == STROUSTRUP_MODE))
		{
			shouldBreakLineAtNextChar = true;
		}

	}
	else    // attach bracket
	{
		// are there comments before the bracket?
		if (isCharImmediatelyPostComment || isCharImmediatelyPostLineComment)
		{
			if (isOkToBreakBlock(bracketType)
			        && !(isCharImmediatelyPostComment && isCharImmediatelyPostLineComment)	// don't attach if two comments on the line
			        && !isImmediatelyPostPreprocessor
			        && peekNextChar() != '}'        // don't attach { }
			        && previousCommandChar != '{'   // don't attach { {
			        && previousCommandChar != '}'   // don't attach } {
			        && previousCommandChar != ';')  // don't attach ; {
			{
				appendCharInsideComments();
			}
			else
			{
				appendCurrentChar();            // don't attach
			}
		}
		else if (previousCommandChar == '{'
		         || previousCommandChar == '}'
		         || previousCommandChar == ';')  // '}' , ';' chars added for proper handling of '{' immediately after a '}' or ';'
		{
			appendCurrentChar();                // don't attach
		}
		else
		{
			// if a blank line preceeds this don't attach
			if (isEmptyLine(formattedLine))
				appendCurrentChar();            // don't attach
			else if (isOkToBreakBlock(bracketType)
			         && !(isImmediatelyPostPreprocessor
			              && currentLineBeginsWithBracket))		// lineBeginsWith('{')
			{
				if (peekNextChar() != '}')
				{
					appendSpacePad();
					appendCurrentChar(false);       // OK to attach
					// should a following comment attach with the bracket?
					// insert spaces to reposition the comment
					if (isBeforeComment()
					        && !isBeforeMultipleLineEndComments(charNum)
					        && (!isBeforeAnyLineEndComment(charNum)	|| currentLineBeginsWithBracket))	// lineBeginsWith('{')
					{
						breakLine();
						currentLine.insert(charNum+1, charNum+1, ' ');
					}
				}
				else
				{
					appendSpacePad();
					appendCurrentChar();
				}
			}
			else
			{
				if (!isInLineBreak)
					appendSpacePad();
				appendCurrentChar();            // don't attach
			}
		}
	}
}

/**
 * format closing bracket
 * currentChar contains the bracket
 * the calling function should have a continue statement after calling this method
 *
 * @param bracketType    the type of the opening bracket for this closing bracket.
 */
void ASFormatter::formatClosingBracket(BracketType bracketType)
{
	assert(!isBracketType(bracketType, ARRAY_TYPE));
	assert(currentChar == '}');

	// parenStack must contain one entry
	if (parenStack->size() > 1)
		parenStack->pop_back();

	// mark state of immediately after empty block
	// this state will be used for locating brackets that appear immedately AFTER an empty block (e.g. '{} \n}').
	if (previousCommandChar == '{')
		isImmediatelyPostEmptyBlock = true;

	if (shouldAttachClosingBracket)
	{
		// for now, namespaces and classes will be attached. Uncomment the lines below to break.
		if ((isEmptyLine(formattedLine)			// if a blank line preceeds this
		        || isCharImmediatelyPostLineComment
		        || isCharImmediatelyPostComment
		        || (isImmediatelyPostPreprocessor && (int) currentLine.find_first_not_of(" \t") == charNum)
//		        || (isBracketType(bracketType, CLASS_TYPE) && isOkToBreakBlock(bracketType) && previousNonWSChar != '{')
//		        || (isBracketType(bracketType, NAMESPACE_TYPE) && isOkToBreakBlock(bracketType) && previousNonWSChar != '{')
		    )
		        && (!isBracketType(bracketType, SINGLE_LINE_TYPE) || isOkToBreakBlock(bracketType)))
		{
			breakLine();
			appendCurrentChar();				// don't attach
		}
		else
		{
			if (previousNonWSChar != '{'
			        && (!isBracketType(bracketType, SINGLE_LINE_TYPE) || isOkToBreakBlock(bracketType)))
				appendSpacePad();
			appendCurrentChar(false);			// attach
		}
	}
	else if ((!(previousCommandChar == '{' && isPreviousBracketBlockRelated))	// this '{' does not close an empty block
	         && isOkToBreakBlock(bracketType))									// astyle is allowed to break one line blocks
//	        && !isImmediatelyPostEmptyBlock)    /* removed 9/5/10 */			// this '}' does not immediately follow an empty block
	{
		breakLine();
		appendCurrentChar();
	}
	else
	{
		appendCurrentChar();
	}

	// if a declaration follows a definition, space pad
	if (isLegalNameChar(peekNextChar()))
		appendSpaceAfter();

	if (shouldBreakBlocks && currentHeader != NULL && parenStack->back() == 0)
	{
		if (currentHeader == &AS_CASE || currentHeader == &AS_DEFAULT)
		{
			// do not yet insert a line if "break" statement is outside the brackets
			string nextText = peekNextText(currentLine.substr(charNum+1));
			if (nextText.substr(0, 5) != "break")
				isAppendPostBlockEmptyLineRequested = true;
		}
		else
			isAppendPostBlockEmptyLineRequested = true;
	}
}

/**
 * format array brackets as attached or broken
 * determine if the brackets can have an inStatement indent
 * currentChar contains the bracket
 * the brackets will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 *
 * @param bracketType            the type of bracket to be formatted, must be an ARRAY_TYPE.
 * @param isOpeningArrayBracket  indicates if this is the opening bracket for the array block.
 */
void ASFormatter::formatArrayBrackets(BracketType bracketType, bool isOpeningArrayBracket)
{
	assert(isBracketType(bracketType, ARRAY_TYPE));
	assert(currentChar == '{' || currentChar == '}');

	if (currentChar == '{')
	{
		// is this the first opening bracket in the array?
		if (isOpeningArrayBracket)
		{
			if (bracketFormatMode == ATTACH_MODE
			        || bracketFormatMode == LINUX_MODE
			        || bracketFormatMode == STROUSTRUP_MODE)
			{
				// don't attach to a preprocessor directive
				if (isImmediatelyPostPreprocessor && currentLineBeginsWithBracket)	// lineBeginsWith('{')
				{
					isInLineBreak = true;
					appendCurrentChar();                // don't attach
				}
				else if (isCharImmediatelyPostComment)
				{
					// TODO: attach bracket to line-end comment
					appendCurrentChar();                // don't attach
				}
				else if (isCharImmediatelyPostLineComment && !isBracketType(bracketType, SINGLE_LINE_TYPE))
				{
					appendCharInsideComments();
				}
				else
				{
					// if a blank line preceeds this don't attach
					if (isEmptyLine(formattedLine))
						appendCurrentChar();            // don't attach
					else
					{
						// if bracket is broken or not an assignment
						if (currentLineBeginsWithBracket 	// lineBeginsWith('{')
						        && !isBracketType(bracketType, SINGLE_LINE_TYPE))
						{
							appendSpacePad();
							appendCurrentChar(false);       // OK to attach

							if (currentLineBeginsWithBracket
							        && (int)currentLineFirstBracketNum == charNum)
								shouldBreakLineAtNextChar = true;
						}
						else
						{
							appendSpacePad();
							appendCurrentChar();
						}
					}
				}
			}
			else if (bracketFormatMode == BREAK_MODE)
			{
				if (isWhiteSpace(peekNextChar()))
					breakLine();
				else if (isBeforeAnyComment())
				{
					// do not break unless comment is at line end
					if (isBeforeAnyLineEndComment(charNum) && !currentLineBeginsWithBracket)
					{
						currentChar = ' ';              // remove bracket from current line
						appendOpeningBracket = true;    // append bracket to following line
					}
				}
				if (!isInLineBreak)
					appendSpacePad();
				appendCurrentChar();

				if (currentLineBeginsWithBracket
				        && (int)currentLineFirstBracketNum == charNum
				        && !isBracketType(bracketType, SINGLE_LINE_TYPE))
					shouldBreakLineAtNextChar = true;
			}
			else if (bracketFormatMode == RUN_IN_MODE)
			{
				if (isWhiteSpace(peekNextChar()))
					breakLine();
				else if (isBeforeAnyComment())
				{
					// do not break unless comment is at line end
					if (isBeforeAnyLineEndComment(charNum) && !currentLineBeginsWithBracket)	// lineBeginsWith('{')
					{
						currentChar = ' ';              // remove bracket from current line
						appendOpeningBracket = true;    // append bracket to following line
					}
				}
				if (!isInLineBreak)
					appendSpacePad();
				appendCurrentChar();
			}
			else if (bracketFormatMode == NONE_MODE)
			{
				if (currentLineBeginsWithBracket)       // lineBeginsWith('{')
				{
					appendCurrentChar();                // don't attach
				}
				else
				{
					appendSpacePad();
					appendCurrentChar(false);           // OK to attach
				}
			}
		}
		else	     // not the first opening bracket
		{
			if (bracketFormatMode == RUN_IN_MODE)
			{
				if (previousNonWSChar == '{'
				        && bracketTypeStack->size() > 2
				        && !isBracketType((*bracketTypeStack)[bracketTypeStack->size()-2], SINGLE_LINE_TYPE))
					formatArrayRunIn();
			}
			else if (!isInLineBreak
			         && !isWhiteSpace(peekNextChar())
			         && previousNonWSChar == '{'
			         && bracketTypeStack->size() > 2
			         && !isBracketType((*bracketTypeStack)[bracketTypeStack->size()-2], SINGLE_LINE_TYPE))
				formatArrayRunIn();

			appendCurrentChar();
		}
	}
	else if (currentChar == '}')
	{
		if (shouldAttachClosingBracket)
		{
			if (isEmptyLine(formattedLine)			// if a blank line preceeds this
			        || isImmediatelyPostPreprocessor
			        || isCharImmediatelyPostLineComment
			        || isCharImmediatelyPostComment)
				appendCurrentChar();				// don't attach
			else
			{
				appendSpacePad();
				appendCurrentChar(false);			// attach
			}
		}
		else
		{
			// does this close the first opening bracket in the array?
			// must check if the block is still a single line because of anonymous statements
			if (!isBracketType(bracketType, SINGLE_LINE_TYPE)
			        || formattedLine.find('{') == string::npos)
				breakLine();
			appendCurrentChar();
		}

		// if a declaration follows an enum definition, space pad
		char peekedChar = peekNextChar();
		if (isLegalNameChar(peekedChar)
		        || peekedChar == '[')
			appendSpaceAfter();
	}
}

/**
 * determine if a run-in can be attached.
 * if it can insert the indents in formattedLine and reset the current line break.
 */
void ASFormatter::formatRunIn()
{
	assert(bracketFormatMode == RUN_IN_MODE || bracketFormatMode == NONE_MODE);

	// keep one line blocks returns true without indenting the run-in
	if (!isOkToBreakBlock(bracketTypeStack->back()))
		return; // true;

	// make sure the line begins with a bracket
	size_t lastText = formattedLine.find_last_not_of(" \t");
	if (lastText == string::npos || formattedLine[lastText] != '{')
		return; // false;

	// make sure the bracket is broken
	if (formattedLine.find_first_not_of(" \t{") != string::npos)
		return; // false;

	if (isBracketType(bracketTypeStack->back(), NAMESPACE_TYPE))
		return; // false;

	bool extraIndent = false;
	isInLineBreak = true;

	// cannot attach a class modifier without indent-classes
	if (isCStyle()
	        && isCharPotentialHeader(currentLine, charNum)
	        && (isBracketType(bracketTypeStack->back(), CLASS_TYPE)
	            || (isBracketType(bracketTypeStack->back(), STRUCT_TYPE)
	                && isInIndentableStruct)))
	{
		if (findKeyword(currentLine, charNum, AS_PUBLIC)
		        || findKeyword(currentLine, charNum, AS_PRIVATE)
		        || findKeyword(currentLine, charNum, AS_PROTECTED))
		{
			if (!getClassIndent())
				return; // false;
		}
		else if (getClassIndent())
			extraIndent = true;
	}

	// cannot attach a 'case' statement without indent-switches
	if (!getSwitchIndent()
	        && isCharPotentialHeader(currentLine, charNum)
	        && (findKeyword(currentLine, charNum, AS_CASE)
	            || findKeyword(currentLine, charNum, AS_DEFAULT)))
		return; // false;

	// extra indent for switch statements
	if (getSwitchIndent()
	        && !preBracketHeaderStack->empty()
	        && preBracketHeaderStack->back() == &AS_SWITCH
	        && ((isLegalNameChar(currentChar)
	             && !findKeyword(currentLine, charNum, AS_CASE))
	            || isSequenceReached("//")
	            || isSequenceReached("/*")))
		extraIndent = true;

	isInLineBreak = false;
	// remove for extra whitespace
	if (formattedLine.length() > lastText+1
	        && formattedLine.find_first_not_of(" \t", lastText+1) == string::npos)
		formattedLine.erase(lastText+1);

	if (getIndentString() == "\t")
	{
		appendChar('\t', false);
		horstmannIndentChars = 2;	// one for { and one for tab
		if (extraIndent)
		{
			appendChar('\t', false);
			horstmannIndentChars++;
		}
	}
	else
	{
		int indent = getIndentLength();
		formattedLine.append(indent-1, ' ');
		horstmannIndentChars = indent;
		if (extraIndent)
		{
			formattedLine.append(indent, ' ');
			horstmannIndentChars += indent;
		}
	}
	isInHorstmannRunIn = true;
}

/**
 * remove whitepace and add indentation for an array run-in.
 */
void ASFormatter::formatArrayRunIn()
{
	assert(isBracketType(bracketTypeStack->back(), ARRAY_TYPE));

	// make sure the bracket is broken
	if (formattedLine.find_first_not_of(" \t{") != string::npos)
		return;

	size_t lastText = formattedLine.find_last_not_of(" \t");
	if (lastText == string::npos || formattedLine[lastText] != '{')
		return;

	// check for extra whitespace
	if (formattedLine.length() > lastText+1
	        && formattedLine.find_first_not_of(" \t", lastText+1) == string::npos)
		formattedLine.erase(lastText+1);

	if (getIndentString() == "\t")
	{
		appendChar('\t', false);
		horstmannIndentChars = 2;	// one for { and one for tab
	}
	else
	{
		int indent = getIndentLength();
		formattedLine.append(indent-1, ' ');
		horstmannIndentChars = indent;
	}
	isInHorstmannRunIn = true;
	isInLineBreak = false;
}

/**
 * delete a bracketTypeStack vector object
 * BracketTypeStack did not work with the DeleteContainer template
 */
void ASFormatter::deleteContainer(vector<BracketType>* &container)
{
	if (container != NULL)
	{
		container->clear();
		delete (container);
		container = NULL;
	}
}

/**
 * delete a vector object
 * T is the type of vector
 * used for all vectors except bracketTypeStack
 */
template<typename T>
void ASFormatter::deleteContainer(T& container)
{
	if (container != NULL)
	{
		container->clear();
		delete (container);
		container = NULL;
	}
}

/**
 * initialize a BracketType vector object
 * BracketType did not work with the DeleteContainer template
 */
void ASFormatter::initContainer(vector<BracketType>* &container, vector<BracketType>* value)
{
	if (container != NULL)
		deleteContainer(container);
	container = value;
}

/**
 * initialize a vector object
 * T is the type of vector
 * used for all vectors except bracketTypeStack
 */
template<typename T>
void ASFormatter::initContainer(T& container, T value)
{
	// since the ASFormatter object is never deleted,
	// the existing vectors must be deleted before creating new ones
	if (container != NULL)
		deleteContainer(container);
	container = value;
}

/**
 * convert a tab to spaces.
 * charNum points to the current character to convert to spaces.
 * tabIncrementIn is the increment that must be added for tab indent characters
 *     to get the correct column for the current tab.
 * replaces the tab in currentLine with the required number of spaces.
 * replaces the value of currentChar.
 */
void ASFormatter::convertTabToSpaces()
{
	assert(currentLine[charNum] == '\t');

	// do NOT replace if in quotes
	if (isInQuote || isInQuoteContinuation)
		return;

	size_t indent = getIndentLength();
	size_t numSpaces = indent - ((tabIncrementIn + charNum) % indent);
	currentLine.replace(charNum, 1, numSpaces, ' ');
	currentChar = currentLine[charNum];
}

/**
* is it ok to break this block?
*/
bool ASFormatter::isOkToBreakBlock(BracketType bracketType) const
{
	// Actually, there should not be an ARRAY_TYPE bracket here.
	// But this will avoid breaking a one line block when there is.
	// Otherwise they will be formatted differently on consecutive runs.
	if (isBracketType(bracketType, ARRAY_TYPE)
	        && isBracketType(bracketType, SINGLE_LINE_TYPE))
		return false;
	if (!isBracketType(bracketType, SINGLE_LINE_TYPE)
	        || shouldBreakOneLineBlocks
	        || breakCurrentOneLineBlock)
		return true;
	return false;
}

/**
* check if a sharp header is a paren or nonparen header
*/
bool ASFormatter::isSharpStyleWithParen(const string* header) const
{
	if (isSharpStyle() && peekNextChar() == '('
	        && (header == &AS_CATCH
	            || header == &AS_DELEGATE))
		return true;
	return false;
}

/**
 * check for a following header when a comment is reached.
 * if a header follows, the comments are kept as part of the header block.
 * firstLine must contain the start of the comment.
 */
void ASFormatter::checkForHeaderFollowingComment(const string& firstLine)
{
	assert(isInComment || isInLineComment);
	// this is called ONLY IF shouldBreakBlocks is TRUE.
	assert(shouldBreakBlocks);
	// look ahead to find the next non-comment text
	string nextText = peekNextText(firstLine, true);

	if (nextText.length() == 0 || !isCharPotentialHeader(nextText, 0))
		return;

	const string* newHeader = ASBeautifier::findHeader(nextText, 0, headers);

	if (newHeader == NULL)
		return;

	// if a closing header, reset break unless break is requested
	if (isClosingHeader(newHeader))
	{
		if (!shouldBreakClosingHeaderBlocks)
			isPrependPostBlockEmptyLineRequested = false;
	}
	// if an opening header, break before the comment
	else
	{
		isPrependPostBlockEmptyLineRequested = true;
	}
}

/**
 * process preprocessor statements.
 * charNum should be the index of the preprocessor directive.
 *
 * delete bracketTypeStack entries added by #if if a #else is found.
 * prevents double entries in the bracketTypeStack.
 */
void ASFormatter::processPreprocessor()
{
	assert(currentChar == '#');

	const int preproc = charNum + 1;

	if (currentLine.compare(preproc, 2, "if") == 0)
	{
		preprocBracketTypeStackSize = bracketTypeStack->size();
	}
	else if (currentLine.compare(preproc, 4, "else") == 0)
	{
		// delete stack entries added in #if
		// should be replaced by #else
		if (preprocBracketTypeStackSize > 0)
		{
			int addedPreproc = bracketTypeStack->size() - preprocBracketTypeStackSize;
			for (int i=0; i < addedPreproc; i++)
				bracketTypeStack->pop_back();
		}
	}
}

/**
 * determine if the next line starts a comment
 * and a header follows the comment or comments.
 */
bool ASFormatter::commentAndHeaderFollows()
{
	// called ONLY IF shouldDeleteEmptyLines and shouldBreakBlocks are TRUE.
	assert(shouldDeleteEmptyLines && shouldBreakBlocks);

	// is the next line a comment
	if (!sourceIterator->hasMoreLines())
		return false;
	string nextLine_ = sourceIterator->peekNextLine();
	size_t firstChar = nextLine_.find_first_not_of(" \t");
	if (firstChar == string::npos
	        || !(nextLine_.compare(firstChar, 2, "//") == 0
	             || nextLine_.compare(firstChar, 2, "/*") == 0))
	{
		sourceIterator->peekReset();
		return false;
	}

	// find the next non-comment text, and reset
	string nextText = peekNextText(nextLine_, false, true);
	if (nextText.length() == 0 || !isCharPotentialHeader(nextText, 0))
		return false;

	const string* newHeader = ASBeautifier::findHeader(nextText, 0, headers);

	if (newHeader == NULL)
		return false;

	// if a closing header, reset break unless break is requested
	if (isClosingHeader(newHeader) && !shouldBreakClosingHeaderBlocks)
	{
		isAppendPostBlockEmptyLineRequested = false;
		return false;
	}

	return true;
}

/**
 * determine if a bracket should be attached or broken
 * uses brackets in the bracketTypeStack
 * the last bracket in the bracketTypeStack is the one being formatted
 * returns true if the bracket should be broken
 */
bool ASFormatter::isCurrentBracketBroken() const
{
	assert(bracketTypeStack->size() > 1);

	bool breakBracket = false;
	size_t bracketTypeStackEnd = bracketTypeStack->size()-1;

	if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd], EXTERN_TYPE))
	{
		if (currentLineBeginsWithBracket
		        || bracketFormatMode == RUN_IN_MODE)
			breakBracket = true;
	}
	else if (bracketFormatMode == NONE_MODE)
	{
		if (currentLineBeginsWithBracket
		        && (int)currentLineFirstBracketNum == charNum)		// lineBeginsWith('{')
			breakBracket = true;
	}
	else if (bracketFormatMode == BREAK_MODE || bracketFormatMode == RUN_IN_MODE)
	{
		breakBracket = true;
	}
	else if (bracketFormatMode == LINUX_MODE || bracketFormatMode == STROUSTRUP_MODE)
	{
		// break a class if Linux
		if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd], CLASS_TYPE))
		{
			if (bracketFormatMode == LINUX_MODE)
				breakBracket = true;
		}
		// break a namespace or interface if Linux
		else if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd], NAMESPACE_TYPE)
		         || isBracketType((*bracketTypeStack)[bracketTypeStackEnd], INTERFACE_TYPE))
		{
			if (bracketFormatMode == LINUX_MODE)
				breakBracket = true;
		}
		// break the first bracket if a function
		else if (bracketTypeStackEnd == 1
		         && isBracketType((*bracketTypeStack)[bracketTypeStackEnd], COMMAND_TYPE))
		{
			breakBracket = true;
		}
		else if (bracketTypeStackEnd > 1)
		{
			// break the first bracket after a namespace or extern if a function
			if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], NAMESPACE_TYPE)
			        || isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], EXTERN_TYPE))
			{
				if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd], COMMAND_TYPE))
					breakBracket = true;
			}
			// if not C style then break the first bracket after a class if a function
			else if (!isCStyle())
			{
				if ((isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], CLASS_TYPE)
				        || isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], ARRAY_TYPE)
				        || isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], STRUCT_TYPE))
				        && isBracketType((*bracketTypeStack)[bracketTypeStackEnd], COMMAND_TYPE))
					breakBracket = true;
			}
		}
	}
	return breakBracket;
}

/**
 * format comment body
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatCommentBody()
{
	assert(isInComment);

	if (isSequenceReached("*/"))
	{
		isInComment = false;
		noTrimCommentContinuation = false;
		isImmediatelyPostComment = true;
		appendSequence(AS_CLOSE_COMMENT);
		goForward(1);
		if (doesLineStartComment
		        && (currentLine.find_first_not_of(" \t", charNum+1) == string::npos))
			lineEndsInCommentOnly = true;
		if (peekNextChar() == '}'
		        && previousCommandChar != ';'
		        && !isBracketType(bracketTypeStack->back(),  ARRAY_TYPE)
		        && !isInPreprocessor
		        && isOkToBreakBlock(bracketTypeStack->back()))
		{
			isInLineBreak = true;
			shouldBreakLineAtNextChar = true;
		}
	}
	else
	{
		appendCurrentChar();
		// append the comment up to the next tab or comment end
		// tabs must be checked for convert-tabs before appending
		while (charNum + 1 < (int) currentLine.length()
		        && currentLine[charNum+1] != '\t'
		        && currentLine.compare(charNum+1, 2, "*/") != 0)
		{
			currentChar = currentLine[++charNum];
			appendCurrentChar();
		}
	}
}

/**
 * format a comment opener
 * the comment opener will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatCommentOpener()
{
	assert(isSequenceReached("/*"));

	isInComment = true;
	isImmediatelyPostLineComment = false;

	if (spacePadNum != 0 && !isInLineBreak)
		adjustComments();
	formattedLineCommentNum = formattedLine.length();

	// must be done BEFORE appendSequence
	if (previousCommandChar == '{'
	        && !isImmediatelyPostComment
	        && !isImmediatelyPostLineComment)
	{
		if (bracketFormatMode == NONE_MODE)
		{
			// should a run-in statement be attached?
			if (currentLineBeginsWithBracket)
				formatRunIn();
		}
		else if (bracketFormatMode == ATTACH_MODE)
		{
			// if the bracket was not attached?
			if (formattedLine.length() > 0 && formattedLine[0] == '{'
			        && !isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE))
				isInLineBreak = true;
		}
		else if (bracketFormatMode == RUN_IN_MODE)
		{
			// should a run-in statement be attached?
			if (formattedLine.length() > 0 && formattedLine[0] == '{')
				formatRunIn();
		}
	}
	else if (!doesLineStartComment)
		noTrimCommentContinuation = true;

	// appendSequence will write the previous line
	appendSequence(AS_OPEN_COMMENT);
	goForward(1);

	// must be done AFTER appendSequence
	if (shouldBreakBlocks)
	{
		// break before the comment if a header follows the comment
		// for speed, do not check if previous line is empty,
		//     if previous line is a line comment or if previous line is '{'
		if (doesLineStartComment
		        && !isImmediatelyPostEmptyLine
		        && !isImmediatelyPostCommentOnly
		        && previousCommandChar != '{')
		{
			checkForHeaderFollowingComment(currentLine.substr(charNum-1));
		}
	}

	if (previousCommandChar == '}')
		currentHeader = NULL;
}

/**
 * format a line comment body
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatLineCommentBody()
{
	assert(isInLineComment);

	appendCurrentChar();
	// append the comment up to the next tab
	// tabs must be checked for convert-tabs before appending
	while (charNum + 1 < (int) currentLine.length()
	        && currentLine[charNum+1] != '\t')
	{
		currentChar = currentLine[++charNum];
		appendCurrentChar();
	}

	// explicitely break a line when a line comment's end is found.
	if (charNum + 1 == (int) currentLine.length())
	{
		isInLineBreak = true;
		isInLineComment = false;
		isImmediatelyPostLineComment = true;
		currentChar = 0;  //make sure it is a neutral char.
	}
}

/**
 * format a line comment opener
 * the line comment opener will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatLineCommentOpener()
{
	assert(isSequenceReached("//"));

	if ((int)currentLine.length() > charNum + 2
	        && currentLine[charNum+2] == '\xf2')       // check for windows line marker
		isAppendPostBlockEmptyLineRequested = false;

	isInLineComment = true;
	isCharImmediatelyPostComment = false;

	// do not indent if in column 1 or 2
	if (!shouldIndentCol1Comments && !lineCommentNoIndent)
	{
		if (charNum == 0)
			lineCommentNoIndent = true;
		else if (charNum == 1 && currentLine[0] == ' ')
			lineCommentNoIndent = true;
	}
	// move comment if spaces were added or deleted
	if (lineCommentNoIndent == false && spacePadNum != 0 && !isInLineBreak)
		adjustComments();
	formattedLineCommentNum = formattedLine.length();

	// must be done BEFORE appendSequence
	// check for run-in statement
	if (previousCommandChar == '{'
	        && !isImmediatelyPostComment
	        && !isImmediatelyPostLineComment)
	{
		if (bracketFormatMode == NONE_MODE)
		{
			if (currentLineBeginsWithBracket)
				formatRunIn();
		}
		else if (bracketFormatMode == RUN_IN_MODE)
		{
			if (!lineCommentNoIndent)
				formatRunIn();
			else
				isInLineBreak = true;
		}
		else if (bracketFormatMode == BREAK_MODE)
		{
			if (formattedLine.length() > 0 && formattedLine[0] == '{')
				isInLineBreak = true;
		}
		else
		{
			if (currentLineBeginsWithBracket)
				isInLineBreak = true;
		}
	}

	// appendSequence will write the previous line
	appendSequence(AS_OPEN_LINE_COMMENT);
	goForward(1);

	if (formattedLine.compare(0, 2, "//") == 0)
		lineIsLineCommentOnly = true;

	// must be done AFTER appendSequence
	if (shouldBreakBlocks)
	{
		// break before the comment if a header follows the line comment
		// for speed, do not check if previous line is empty,
		//     if previous line is a comment or if previous line is '{'
		if (lineIsLineCommentOnly
		        && previousCommandChar != '{'
		        && !isImmediatelyPostEmptyLine
		        && !isImmediatelyPostCommentOnly)
		{
			checkForHeaderFollowingComment(currentLine.substr(charNum-1));
		}
	}

	if (previousCommandChar == '}')
		currentHeader = NULL;

	// if tabbed input don't convert the immediately following tabs to spaces
	if (getIndentString() == "\t" && lineCommentNoIndent)
	{
		while (charNum + 1 < (int) currentLine.length()
		        && currentLine[charNum+1] == '\t')
		{
			currentChar = currentLine[++charNum];
			appendCurrentChar();
		}
	}

	// explicitely break a line when a line comment's end is found.
	if (charNum + 1 == (int) currentLine.length())
	{
		isInLineBreak = true;
		isInLineComment = false;
		isImmediatelyPostLineComment = true;
		currentChar = 0;  //make sure it is a neutral char.
	}
}

/**
 * format quote body
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatQuoteBody()
{
	assert(isInQuote);

	if (isSpecialChar)
	{
		isSpecialChar = false;
	}
	else if (currentChar == '\\' && !isInVerbatimQuote)
	{
		if (peekNextChar() == ' ')              // is this '\' at end of line
			haveLineContinuationChar = true;
		else
			isSpecialChar = true;
	}
	else if (isInVerbatimQuote && currentChar == '"')
	{
		if (peekNextChar() == '"')              // check consecutive quotes
		{
			appendSequence("\"\"");
			goForward(1);
			return;
		}
		else
		{
			isInQuote = false;
			isInVerbatimQuote = false;
		}
	}
	else if (quoteChar == currentChar)
	{
		isInQuote = false;
	}

	appendCurrentChar();

	// append the text to the ending quoteChar or an escape sequence
	// tabs in quotes are NOT changed by convert-tabs
	if (isInQuote && currentChar != '\\')
	{
		while (charNum + 1 < (int) currentLine.length()
		        && currentLine[charNum+1] != quoteChar
		        && currentLine[charNum+1] != '\\')
		{
			currentChar = currentLine[++charNum];
			appendCurrentChar();
		}
	}
}

/**
 * format a quote opener
 * the quote opener will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatQuoteOpener()
{
	assert(currentChar == '"' || currentChar == '\'');

	isInQuote = true;
	quoteChar = currentChar;
	if (isSharpStyle() && previousChar == '@')
		isInVerbatimQuote = true;

	// a quote following a bracket is an array
	if (previousCommandChar == '{'
	        && !isImmediatelyPostComment
	        && !isImmediatelyPostLineComment
	        && isNonInStatementArray
	        && !isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE)
	        && !isWhiteSpace(peekNextChar()))
	{
		if (bracketFormatMode == NONE_MODE)
		{
			if (currentLineBeginsWithBracket)
				formatRunIn();
		}
		else if (bracketFormatMode == RUN_IN_MODE)
		{
			formatRunIn();
		}
		else if (bracketFormatMode == BREAK_MODE)
		{
			if (formattedLine.length() > 0 && formattedLine[0] == '{')
				isInLineBreak = true;
		}
		else
		{
			if (currentLineBeginsWithBracket)
				isInLineBreak = true;
		}
	}
	previousCommandChar = ' ';
	appendCurrentChar();
}

/**
 * get the next line comment adjustment that results from breaking a closing bracket.
 * the bracket must be on the same line as the closing header.
 * i.e "} else" changed to "} \n else".
 */
int ASFormatter::getNextLineCommentAdjustment()
{
	assert(foundClosingHeader && previousNonWSChar == '}');
	if (charNum < 1)			// "else" is in column 1
		return 0;
	size_t lastBracket = currentLine.rfind('}', charNum - 1);
	if (lastBracket != string::npos)
		return (lastBracket - charNum);	// return a negative number
	return 0;
}

// for console build only
LineEndFormat ASFormatter::getLineEndFormat() const
{
	return lineEnd;
}

/**
 * get the current line comment adjustment that results from attaching
 * a closing header to a closing bracket.
 * the bracket must be on the line previous to the closing header.
 * the adjustment is 2 chars, one for the bracket and one for the space.
 * i.e "} \n else" changed to "} else".
 */
int ASFormatter::getCurrentLineCommentAdjustment()
{
	assert(foundClosingHeader && previousNonWSChar == '}');
	if (charNum < 1)
		return 2;
	size_t lastBracket = currentLine.rfind('}', charNum - 1);
	if (lastBracket == string::npos)
		return 2;
	return 0;
}

/**
 * get the previous word on a line
 * the argument 'currPos' must point to the current position.
 *
 * @return is the previous word or an empty string if none found.
 */
string ASFormatter::getPreviousWord(const string& line, int currPos) const
{
	// get the last legal word (may be a number)
	if (currPos == 0)
		return string();

	size_t end = line.find_last_not_of(" \t", currPos-1);
	if (end == string::npos || !isLegalNameChar(line[end]))
		return string();

	int start;          // start of the previous word
	for (start = end; start > -1; start--)
	{
		if (!isLegalNameChar(line[start]) || line[start] == '.')
			break;
	}
	start++;

	return (line.substr(start, end-start+1));
}

/**
 * check if a line break is needed when a closing bracket
 * is followed by a closing header.
 * the break depends on the bracketFormatMode and other factors.
 */
void ASFormatter::isLineBreakBeforeClosingHeader()
{
	assert(foundClosingHeader && previousNonWSChar == '}');
	if (bracketFormatMode == BREAK_MODE
	        || bracketFormatMode == RUN_IN_MODE
	        || shouldAttachClosingBracket)
	{
		isInLineBreak = true;
	}
	else if (bracketFormatMode == NONE_MODE)
	{
		if (shouldBreakClosingHeaderBrackets
		        || getBracketIndent() || getBlockIndent())
		{
			isInLineBreak = true;
		}
		else
		{
			appendSpacePad();
			// is closing bracket broken?
			size_t i = currentLine.find_first_not_of(" \t");
			if (i != string::npos && currentLine[i] == '}')
				isInLineBreak = false;

			if (shouldBreakBlocks)
				isAppendPostBlockEmptyLineRequested = false;
		}
	}
	// bracketFormatMode == ATTACH_MODE, LINUX_MODE, STROUSTRUP_MODE
	else
	{
		if (shouldBreakClosingHeaderBrackets
		        || getBracketIndent() || getBlockIndent())
		{
			isInLineBreak = true;
		}
		else
		{
			// if a blank line does not preceed this
			// or last line is not a one line block, attach header
			bool previousLineIsEmpty = isEmptyLine(formattedLine);
			int previousLineIsOneLineBlock = 0;
			size_t firstBracket = findNextChar(formattedLine, '{');
			if (firstBracket != string::npos)
				previousLineIsOneLineBlock = isOneLineBlockReached(formattedLine, firstBracket);
			if (!previousLineIsEmpty
			        && previousLineIsOneLineBlock == 0)
			{
				isInLineBreak = false;
				appendSpacePad();
				spacePadNum = 0;	// don't count as comment padding
			}

			if (shouldBreakBlocks)
				isAppendPostBlockEmptyLineRequested = false;
		}
	}
}

/**
 * Add brackets to a single line statement following a header.
 * Brackets are not added if the proper conditions are not met.
 * Brackets are added to the currentLine.
 */
bool ASFormatter::addBracketsToStatement()
{
	assert(isImmediatelyPostHeader);

	if (currentHeader != &AS_IF
	        && currentHeader != &AS_ELSE
	        && currentHeader != &AS_FOR
	        && currentHeader != &AS_WHILE
	        && currentHeader != &AS_DO
	        && currentHeader != &AS_FOREACH)
		return false;

	if (currentHeader == &AS_WHILE && foundClosingHeader)	// do-while
		return false;

	// do not bracket an empty statement
	if (currentChar == ';')
		return false;

	// do not add if a header follows (i.e. else if)
	if (isCharPotentialHeader(currentLine, charNum))
		if (findHeader(headers) != NULL)
			return false;

	// find the next semi-colon
	size_t nextSemiColon = charNum;
	if (currentChar != ';')
		nextSemiColon = findNextChar(currentLine, ';', charNum+1);
	if (nextSemiColon == string::npos)
		return false;

	// add closing bracket before changing the line length
	if (nextSemiColon == currentLine.length() - 1)
		currentLine.append(" }");
	else
		currentLine.insert(nextSemiColon + 1, " }");
	// add opening bracket
	currentLine.insert(charNum, "{ ");
	assert(computeChecksumIn("{}"));
	currentChar = '{';
	// remove extra spaces
	if (!shouldAddOneLineBrackets)
	{
		size_t lastText = formattedLine.find_last_not_of(" \t");
		if ((formattedLine.length() - 1) - lastText > 1)
			formattedLine.erase(lastText + 1);
	}
	return true;
}

/**
 * Find the next character that is not in quotes or a comment.
 *
 * @param line         the line to be searched.
 * @param searchChar   the char to find.
 * @param searchStart  the start position on the line (default is 0).
 * @return the position on the line or string::npos if not found.
 */
size_t ASFormatter::findNextChar(string& line, char searchChar, int searchStart /*0*/)
{
	// find the next searchChar
	size_t i;
	for (i = searchStart; i < line.length(); i++)
	{
		if (line.compare(i, 2, "//") == 0)
			return string::npos;
		if (line.compare(i, 2, "/*") == 0)
		{
			size_t endComment = line.find("*/", i+2);
			if (endComment == string::npos)
				return string::npos;
			i = endComment + 2;
			if (i >= line.length())
				return string::npos;
		}
		if (line[i] == '\'' || line[i] == '\"')
		{
			char quote = line[i];
			while (i < line.length())
			{
				size_t endQuote = line.find(quote, i+1);
				if (endQuote == string::npos)
					return string::npos;
				i = endQuote;
				if (line[endQuote-1] != '\\')	// check for '\"'
					break;
				if (line[endQuote-2] == '\\')	// check for '\\'
					break;
			}
		}

		if (line[i] == searchChar)
			break;

		// for now don't process C# 'delegate' brackets
		// do this last in case the search char is a '{'
		if (line[i] == '{')
			return string::npos;
	}
	if (i >= line.length())	// didn't find searchChar
		return string::npos;

	return i;
}

/**
 * Look ahead in the file to see if a struct has access modifiers.
 *
 * @param line          a reference to the line to indent.
 * @param index         the current line index.
 * @return              true if the struct has access modifiers.
 */
bool ASFormatter::isStructAccessModified(string&  firstLine, size_t index) const
{
	assert(firstLine[index] == '{');
	assert(isCStyle());

	bool isFirstLine = true;
	bool needReset = false;
	size_t bracketCount = 1;
	string nextLine_ = firstLine.substr(index + 1);

	// find the first non-blank text, bypassing all comments and quotes.
	bool isInComment_ = false;
	bool isInQuote_ = false;
	char quoteChar_ = ' ';
	while (sourceIterator->hasMoreLines())
	{
		if (isFirstLine)
			isFirstLine = false;
		else
		{
			nextLine_ = sourceIterator->peekNextLine();
			needReset = true;
		}
		// parse the line
		for (size_t i = 0; i < nextLine_.length(); i++)
		{
			if (isWhiteSpace(nextLine_[i]))
				continue;
			if (nextLine_.compare(i, 2, "/*") == 0)
				isInComment_ = true;
			if (isInComment_)
			{
				if (nextLine_.compare(i, 2, "*/") == 0)
				{
					isInComment_ = false;
					++i;
				}
				continue;
			}
			if (nextLine_[i] == '\\')
			{
				++i;
				continue;
			}

			if (isInQuote_)
			{
				if (nextLine_[i] == quoteChar_)
					isInQuote_ = false;
				continue;
			}

			if (nextLine_[i] == '"' || nextLine_[i] == '\'')
			{
				isInQuote_ = true;
				quoteChar_ = nextLine_[i];
				continue;
			}
			if (nextLine_.compare(i, 2, "//") == 0)
			{
				i = nextLine_.length();
				continue;
			}
			// handle brackets
			if (nextLine_[i] == '{')
				++bracketCount;
			if (nextLine_[i] == '}')
				--bracketCount;
			if (bracketCount == 0)
			{
				if (needReset)
					sourceIterator->peekReset();
				return false;
			}
			// check for access modifiers
			if (isCharPotentialHeader(nextLine_, i))
			{
				if (findKeyword(nextLine_, i, AS_PUBLIC)
				        || findKeyword(nextLine_, i, AS_PRIVATE)
				        || findKeyword(nextLine_, i, AS_PROTECTED))
				{
					if (needReset)
						sourceIterator->peekReset();
					return true;
				}
				string name = getCurrentWord(nextLine_, i);
				i += name.length() - 1;
			}
		}	// end of for loop
	}	// end of while loop

	if (needReset)
		sourceIterator->peekReset();
	return false;
}

/**
 * Check to see if this is an EXEC SQL statement.
 *
 * @param line          a reference to the line to indent.
 * @param index         the current line index.
 * @return              true if the statement is EXEC SQL.
 */
bool ASFormatter::isExecSQL(string&  line, size_t index) const
{
	if (line[index] != 'e' && line[index] != 'E')	// quick check to reject most
		return false;
	string word;
	if (isCharPotentialHeader(line, index))
		word = getCurrentWord(line, index);
	for (size_t i = 0; i < word.length(); i++)
		word[i] = (char) toupper(word[i]);
	if (word != "EXEC")
		return false;
	size_t index2 = index + word.length();
	index2 = line.find_first_not_of(" \t", index2);
	if (index2 == string::npos)
		return false;
	word.erase();
	if (isCharPotentialHeader(line, index2))
		word = getCurrentWord(line, index2);
	for (size_t i = 0; i < word.length(); i++)
		word[i] = (char) toupper(word[i]);
	if (word != "SQL")
		return false;
	return true;
}

/**
 * The continuation lines must be adjusted so the leading spaces
 *     is equivalent to the text on the opening line.
 *
 * Updates currentLine and charNum.
 */
void ASFormatter::trimContinuationLine()
{
	size_t len = currentLine.length();
	size_t indent = getIndentLength();
	charNum = 0;

	if (leadingSpaces > 0 && len > 0)
	{
		size_t i;
		size_t continuationIncrementIn = 0;
		for (i = 0; (i < len) && (i + continuationIncrementIn < leadingSpaces); i++)
		{
			if (!isWhiteSpace(currentLine[i]))		// don't delete any text
			{
				if (i < continuationIncrementIn)
					leadingSpaces = i + tabIncrementIn;
				continuationIncrementIn = tabIncrementIn;
				break;
			}
			if (currentLine[i] == '\t')
				continuationIncrementIn += indent - 1 - ((continuationIncrementIn + i) % indent);
		}

		if ((int) continuationIncrementIn == tabIncrementIn)
			charNum = i;
		else
		{
			// build a new line with the equivalent leading chars
			string newLine;
			int leadingChars = 0;
			if ((int) leadingSpaces > tabIncrementIn)
				leadingChars = leadingSpaces - tabIncrementIn;
			newLine.append(leadingChars, ' ');
			newLine.append(currentLine, i, len-i);
			currentLine = newLine;
			charNum = leadingChars;
			if (currentLine.length() == 0)
				currentLine = string(" ");        // a null is inserted if this is not done
		}
		if (i >= len)
			charNum = 0;
	}
	return;
}

/**
 * Determine if a header is a closing header
 *
 * @return      true if the header is a closing header.
 */
bool ASFormatter::isClosingHeader(const string* header) const
{
	return (header == &AS_ELSE
	        || header == &AS_CATCH
	        || header == &AS_FINALLY);
}

/**
 * Determine if a * following a closing paren is immediately.
 * after a cast. If so it is a dereference and not a multiply.
 * e.g. "(int*) *ptr" is a dereference.
 */
bool ASFormatter::isImmediatelyPostCast() const
{
	assert(previousNonWSChar == ')' && currentChar == '*');
	// find preceeding closing paren
	size_t paren = currentLine.rfind(")", charNum);
	if (paren == string::npos || paren == 0)
		return false;
	// find character preceeding the closing paren
	size_t lastChar = currentLine.find_last_not_of(" \t", paren-1);
	if (lastChar == string::npos)
		return false;
	// check for pointer cast
	if (currentLine[lastChar] == '*')
		return true;
	return false;
}

/**
 * Determine if a < is a template definition or instantiation.
 * Sets the class variables isInTemplate and templateDepth.
 */
void ASFormatter::checkIfTemplateOpener()
{
	assert(!isInTemplate && currentChar == '<');

	int parenDepth_ = 0;
	int maxTemplateDepth = 0;
	templateDepth = 0;
	for (size_t i = charNum; i < currentLine.length(); i++)
	{
		char currentChar_ = currentLine[i];

		if (isWhiteSpace(currentChar_))
			continue;

		if (currentChar_ == '<')
		{
			templateDepth++;
			maxTemplateDepth++;
		}
		else if (currentChar_ == '>')
		{
			templateDepth--;
			if (templateDepth == 0)
			{
				if (parenDepth_ == 0)
				{
					// this is a template!
					isInTemplate = true;
					templateDepth = maxTemplateDepth;
				}
				return;
			}
		}
		else if (currentChar_ == '(' || currentChar_ == ')')
		{
			if (currentChar_ == '(')
				parenDepth_++;
			else
				parenDepth_--;
			continue;
		}
		else if (currentLine.compare(i, 2, "&&") == 0
		         || currentLine.compare(i, 2, "||") == 0)
		{
			// this is not a template -> leave...
			isInTemplate = false;
			return;
		}
		else if (currentChar_ == ','       // comma,     e.g. A<int, char>
		         || currentChar_ == '&'    // reference, e.g. A<int&>
		         || currentChar_ == '*'    // pointer,   e.g. A<int*>
		         || currentChar_ == ':'    // ::,        e.g. std::string
		         || currentChar_ == '='    // assign     e.g. default parameter
		         || currentChar_ == '['    // []         e.g. string[]
		         || currentChar_ == ']'    // []         e.g. string[]
		         || currentChar_ == '('    // (...)      e.g. function definition
		         || currentChar_ == ')')   // (...)      e.g. function definition
		{
			continue;
		}
		else if (!isLegalNameChar(currentChar_))
		{
			// this is not a template -> leave...
			isInTemplate = false;
			return;
		}
	}
}

/**
 * Compute the input checksum.
 * This is called as an assert so it for is debug config only
 */
bool ASFormatter::computeChecksumIn(const string& currentLine_)
{
	for (size_t i = 0; i < currentLine_.length(); i++)
		if (!isWhiteSpace(currentLine_[i]))
			checksumIn += currentLine_[i];
	return true;
}

/**
 * get the value of checksumIn for unit testing
 *
 * @return   checksumIn.
 */
size_t ASFormatter::getChecksumIn() const
{
	return checksumIn;
}

/**
 * Compute the output checksum.
 * This is called as an assert so it is for debug config only
 */
bool ASFormatter::computeChecksumOut(const string& beautifiedLine)
{
	for (size_t i = 0; i < beautifiedLine.length(); i++)
		if (!isWhiteSpace(beautifiedLine[i]))
			checksumOut += beautifiedLine[i];
	return true;
}

/**
 * Return isLineReady for the final check at end of file.
 */
bool ASFormatter::getIsLineReady() const
{
	return isLineReady;
}

/**
 * get the value of checksumOut for unit testing
 *
 * @return   checksumOut.
 */
size_t ASFormatter::getChecksumOut() const
{
	return checksumOut;
}

/**
 * Return the difference in checksums.
 * If zero all is okay.
 */
int ASFormatter::getChecksumDiff() const
{
	return checksumOut - checksumIn;
}

// for unit testing
int ASFormatter::getFormatterFileType() const
{ return formatterFileType; }


}   // end namespace astyle
