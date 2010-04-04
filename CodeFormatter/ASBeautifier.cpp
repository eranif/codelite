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

#include "astyle.h"

#include <algorithm>
#include <iostream>


namespace astyle
{
// static member variables
int ASBeautifier::beautifierFileType = 9;		// initialized with an invalid type
vector<const string*>* ASBeautifier::headers = NULL;
vector<const string*>* ASBeautifier::nonParenHeaders = NULL;
vector<const string*>* ASBeautifier::preBlockStatements;
vector<const string*>* ASBeautifier::assignmentOperators = NULL;
vector<const string*>* ASBeautifier::nonAssignmentOperators;
vector<const string*>* ASBeautifier::indentableHeaders;


/**
 * ASBeautifier's constructor
 */
ASBeautifier::ASBeautifier()
{
	waitingBeautifierStack = NULL;
	activeBeautifierStack = NULL;
	waitingBeautifierStackLengthStack = NULL;
	activeBeautifierStackLengthStack = NULL;

	headerStack  = NULL;
	tempStacks = NULL;
	blockParenDepthStack = NULL;
	blockStatementStack = NULL;
	parenStatementStack = NULL;
	bracketBlockStateStack = NULL;
	inStatementIndentStack = NULL;
	inStatementIndentStackSizeStack = NULL;
	parenIndentStack = NULL;
	sourceIterator = NULL;
	isIndentManuallySet = false;
	isMinConditionalManuallySet = false;
	isModeManuallySet = false;
	shouldForceTabIndentation = false;
	setSpaceIndentation(4);				// also sets minConditionalIndent
	setMaxInStatementIndentLength(40);
	classInitializerTabs = 1;
	setClassIndent(false);
	setSwitchIndent(false);
	setCaseIndent(false);
	setBlockIndent(false);
	setBracketIndent(false);
	setNamespaceIndent(false);
	setLabelIndent(false);
	setEmptyLineFill(false);
	setCStyle();
	setPreprocessorIndent(false);

	// initialize ASBeautifier static member vectors
	beautifierFileType = 9;		// reset to an invalid type
	initVector(headers);
	initVector(nonParenHeaders);
	initVector(assignmentOperators);
	initVector(nonAssignmentOperators);
	initVector(preBlockStatements);
	initVector(indentableHeaders);
}

/**
 * ASBeautifier's copy constructor
 * must explicitly call the base class copy constructor
 */
ASBeautifier::ASBeautifier(const ASBeautifier &other) : ASBase(other)
{
	// these don't need to copy the stack
	waitingBeautifierStack = NULL;
	activeBeautifierStack = NULL;
	waitingBeautifierStackLengthStack = NULL;
	activeBeautifierStackLengthStack = NULL;

	// vector '=' operator performs a DEEP copy of all elements in the vector

	headerStack  = new vector<const string*>;
	*headerStack = *other.headerStack;

	//tempStacks = new vector<vector<const string*>*>;
	//vector<vector<const string*>*>::iterator iter;
	//for (iter = other.tempStacks->begin();
	//        iter != other.tempStacks->end();
	//        ++iter)
	//{
	//	vector<const string*> *newVec = new vector<const string*>;
	//	*newVec = **iter;
	//	tempStacks->push_back(newVec);
	//}
	tempStacks = copyTempStacks(other);

	blockParenDepthStack = new vector<int>;
	*blockParenDepthStack = *other.blockParenDepthStack;

	blockStatementStack = new vector<bool>;
	*blockStatementStack = *other.blockStatementStack;

	parenStatementStack =  new vector<bool>;
	*parenStatementStack = *other.parenStatementStack;

	bracketBlockStateStack = new vector<bool>;
	*bracketBlockStateStack = *other.bracketBlockStateStack;

	inStatementIndentStack = new vector<int>;
	*inStatementIndentStack = *other.inStatementIndentStack;

	inStatementIndentStackSizeStack = new vector<int>;
	*inStatementIndentStackSizeStack = *other.inStatementIndentStackSizeStack;

	parenIndentStack = new vector<int>;
	*parenIndentStack = *other.parenIndentStack;

	sourceIterator = other.sourceIterator;

	// protected variables
	// variables set by ASFormatter
	// must also be updated in activeBeautifierStack
	inLineNumber = other.inLineNumber;
	horstmannIndentInStatement = other.horstmannIndentInStatement;
	nonInStatementBracket = other.nonInStatementBracket;
	lineCommentNoBeautify = other.lineCommentNoBeautify;
	isNonInStatementArray = other.isNonInStatementArray;
	isSharpAccessor = other.isSharpAccessor;
	isSharpDelegate = other.isSharpDelegate;
	isInExtern = other.isInExtern;
	isInBeautifySQL = other.isInBeautifySQL;
	isInIndentableStruct = other.isInIndentableStruct;

	// private variables
	indentString = other.indentString;
	currentHeader = other.currentHeader;
	previousLastLineHeader = other.previousLastLineHeader;
	probationHeader = other.probationHeader;
	isInQuote = other.isInQuote;
	isInVerbatimQuote = other.isInVerbatimQuote;
	haveLineContinuationChar = other.haveLineContinuationChar;
	isInAsm = other.isInAsm;
	isInAsmOneLine = other.isInAsmOneLine;
	isInAsmBlock = other.isInAsmBlock;
	isInComment = other.isInComment;
	isInHorstmannComment = other.isInHorstmannComment;
	isInCase = other.isInCase;
	isInQuestion = other.isInQuestion;
	isInStatement = other.isInStatement;
	isInHeader = other.isInHeader;
	isInTemplate = other.isInTemplate;
	isInDefine = other.isInDefine;
	isInDefineDefinition = other.isInDefineDefinition;
	classIndent = other.classIndent;
	isInClassInitializer = other.isInClassInitializer;
	isInClassHeaderTab = other.isInClassHeaderTab;
	isInEnum = other.isInEnum;
	switchIndent = other.switchIndent;
	caseIndent = other.caseIndent;
	namespaceIndent = other.namespaceIndent;
	bracketIndent = other.bracketIndent;
	blockIndent = other.blockIndent;
	labelIndent = other.labelIndent;
	preprocessorIndent = other.preprocessorIndent;
	isInConditional = other.isInConditional;
	isIndentManuallySet = other.isIndentManuallySet;
	isMinConditionalManuallySet = other.isMinConditionalManuallySet;
	isModeManuallySet = other.isModeManuallySet;
	shouldForceTabIndentation = other.shouldForceTabIndentation;
	emptyLineFill = other.emptyLineFill;
	lineOpensComment = other.lineOpensComment;
	backslashEndsPrevLine = other.backslashEndsPrevLine;
	blockCommentNoIndent = other.blockCommentNoIndent;
	blockCommentNoBeautify = other.blockCommentNoBeautify;
	previousLineProbationTab = other.previousLineProbationTab;
	fileType = other.fileType;
	minConditionalIndent = other.minConditionalIndent;
	parenDepth = other.parenDepth;
	indentLength = other.indentLength;
	blockTabCount = other.blockTabCount;
	maxInStatementIndent = other.maxInStatementIndent;
	classInitializerTabs = other.classInitializerTabs;
	templateDepth = other.templateDepth;
	prevFinalLineSpaceTabCount = other.prevFinalLineSpaceTabCount;
	prevFinalLineTabCount = other.prevFinalLineTabCount;
	defineTabCount = other.defineTabCount;
	quoteChar = other.quoteChar;
	prevNonSpaceCh = other.prevNonSpaceCh;
	currentNonSpaceCh = other.currentNonSpaceCh;
	currentNonLegalCh = other.currentNonLegalCh;
	prevNonLegalCh = other.prevNonLegalCh;
}

/**
 * ASBeautifier's destructor
 */
ASBeautifier::~ASBeautifier()
{
	deleteContainer(waitingBeautifierStack);
	deleteContainer(activeBeautifierStack);
	deleteContainer(waitingBeautifierStackLengthStack);
	deleteContainer(activeBeautifierStackLengthStack);
	deleteContainer(headerStack);
	deleteContainer(tempStacks);
	deleteContainer(blockParenDepthStack);
	deleteContainer(blockStatementStack);
	deleteContainer(parenStatementStack);
	deleteContainer(bracketBlockStateStack);
	deleteContainer(inStatementIndentStack);
	deleteContainer(inStatementIndentStackSizeStack);
	deleteContainer(parenIndentStack);
}

/**
 * initialize the ASBeautifier.
 *
 * init() should be called every time a ABeautifier object is to start
 * beautifying a NEW source file.
 * init() recieves a pointer to a ASSourceIterator object that will be
 * used to iterate through the source code.
 *
 * @param iter     a pointer to the ASSourceIterator or ASStreamIterator object.
 */
void ASBeautifier::init(ASSourceIterator *iter)
{
	sourceIterator = iter;
	init();
}

/**
 * initialize the ASBeautifier.
 */
void ASBeautifier::init()
{
	initStatic();
	ASBase::init(getFileType());

	initContainer(waitingBeautifierStack, new vector<ASBeautifier*>);
	initContainer(activeBeautifierStack, new vector<ASBeautifier*>);

	initContainer(waitingBeautifierStackLengthStack, new vector<int>);
	initContainer(activeBeautifierStackLengthStack, new vector<int>);

	initContainer(headerStack,  new vector<const string*>);

	initContainer(tempStacks, new vector<vector<const string*>*>);
	tempStacks->push_back(new vector<const string*>);

	initContainer(blockParenDepthStack, new vector<int>);
	initContainer(blockStatementStack, new vector<bool>);
	initContainer(parenStatementStack, new vector<bool>);

	initContainer(bracketBlockStateStack, new vector<bool>);
	bracketBlockStateStack->push_back(true);

	initContainer(inStatementIndentStack, new vector<int>);
	initContainer(inStatementIndentStackSizeStack, new vector<int>);
	inStatementIndentStackSizeStack->push_back(0);
	initContainer(parenIndentStack, new vector<int>);

	previousLastLineHeader = NULL;
	currentHeader = NULL;

	isInQuote = false;
	isInVerbatimQuote = false;
	haveLineContinuationChar = false;
	isInAsm = false;
	isInAsmOneLine = false;
	isInAsmBlock = false;
	isInComment = false;
	isInHorstmannComment = false;
	isInStatement = false;
	isInCase = false;
	isInQuestion = false;
	isInClassInitializer = false;
	isInClassHeaderTab = false;
	isInEnum = false;
	isInHeader = false;
	isInTemplate = false;
	isInConditional = false;

	templateDepth = 0;
	parenDepth = 0;
	blockTabCount = 0;
	prevNonSpaceCh = '{';
	currentNonSpaceCh = '{';
	prevNonLegalCh = '{';
	currentNonLegalCh = '{';
	quoteChar = ' ';
	prevFinalLineSpaceTabCount = 0;
	prevFinalLineTabCount = 0;
	probationHeader = NULL;
	backslashEndsPrevLine = false;
	lineOpensComment = false;
	isInDefine = false;
	isInDefineDefinition = false;
	defineTabCount = 0;
	lineCommentNoBeautify = false;
	blockCommentNoIndent = false;
	blockCommentNoBeautify = false;
	previousLineProbationTab = false;
	isNonInStatementArray = false;
	isSharpAccessor = false;
	isSharpDelegate = false;
	isInExtern = false;
	isInBeautifySQL = false;
	isInIndentableStruct = false;
	inLineNumber = 0;
	horstmannIndentInStatement = 0;
	nonInStatementBracket = 0;
}

/*
 * initialize the static vars
 */
void ASBeautifier::initStatic()
{
	if (fileType == beautifierFileType)    // don't build unless necessary
		return;

	beautifierFileType = fileType;

	headers->clear();
	nonParenHeaders->clear();
	assignmentOperators->clear();
	nonAssignmentOperators->clear();
	preBlockStatements->clear();
	indentableHeaders->clear();

	ASResource::buildHeaders(headers, fileType, true);
	ASResource::buildNonParenHeaders(nonParenHeaders, fileType, true);
	ASResource::buildAssignmentOperators(assignmentOperators);
	ASResource::buildNonAssignmentOperators(nonAssignmentOperators);
	ASResource::buildPreBlockStatements(preBlockStatements, fileType);
	ASResource::buildIndentableHeaders(indentableHeaders);
}

/**
 * set indentation style to C/C++.
 */
void ASBeautifier::setCStyle()
{
	fileType = C_TYPE;
}

/**
 * set indentation style to Java.
 */
void ASBeautifier::setJavaStyle()
{
	fileType = JAVA_TYPE;
}

/**
 * set indentation style to C#.
 */
void ASBeautifier::setSharpStyle()
{
	fileType = SHARP_TYPE;
}

/**
 * set mode manually set flag
 */
void ASBeautifier::setModeManuallySet(bool state)
{
	isModeManuallySet = state;
}

/**
 * indent using one tab per indentation
 */
void ASBeautifier::setTabIndentation(int length, bool forceTabs)
{
	indentString = "\t";
	indentLength = length;
	shouldForceTabIndentation = forceTabs;

	if (!isMinConditionalManuallySet)
		minConditionalIndent = indentLength * 2;
}

/**
 * indent using a number of spaces per indentation.
 *
 * @param   length     number of spaces per indent.
 */
void ASBeautifier::setSpaceIndentation(int length)
{
	indentString = string(length, ' ');
	indentLength = length;

	if (!isMinConditionalManuallySet)
		minConditionalIndent = indentLength * 2;
}

/**
 * set indent manually set flag
 */
void ASBeautifier::setIndentManuallySet(bool state)
{
	isIndentManuallySet = state;
}

/**
 * set the maximum indentation between two lines in a multi-line statement.
 *
 * @param   max     maximum indentation length.
 */
void ASBeautifier::setMaxInStatementIndentLength(int max)
{
	maxInStatementIndent = max;
}

/**
 * set the minimum indentation between two lines in a multi-line condition.
 *
 * @param   min     minimal indentation length.
 */
void ASBeautifier::setMinConditionalIndentLength(int min)
{
	minConditionalIndent = min;
}

/**
 * set min conditional manually set flag
 */
void ASBeautifier::setMinConditionalManuallySet(bool state)
{
	isMinConditionalManuallySet = state;
}

/**
 * set the state of the bracket indentation option. If true, brackets will
 * be indented one additional indent.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setBracketIndent(bool state)
{
	bracketIndent = state;
}

/**
 * set the state of the block indentation option. If true, entire blocks
 * will be indented one additional indent, similar to the GNU indent style.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setBlockIndent(bool state)
{
	blockIndent = state;
}

/**
 * set the state of the class indentation option. If true, C++ class
 * definitions will be indented one additional indent.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setClassIndent(bool state)
{
	classIndent = state;
}

/**
 * set the state of the switch indentation option. If true, blocks of 'switch'
 * statements will be indented one additional indent.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setSwitchIndent(bool state)
{
	switchIndent = state;
}

/**
 * set the state of the case indentation option. If true, lines of 'case'
 * statements will be indented one additional indent.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setCaseIndent(bool state)
{
	caseIndent = state;
}

/**
 * set the state of the namespace indentation option.
 * If true, blocks of 'namespace' statements will be indented one
 * additional indent. Otherwise, NO indentation will be added.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setNamespaceIndent(bool state)
{
	namespaceIndent = state;
}

/**
 * set the state of the label indentation option.
 * If true, labels will be indented one indent LESS than the
 * current indentation level.
 * If false, labels will be flushed to the left with NO
 * indent at all.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setLabelIndent(bool state)
{
	labelIndent = state;
}

/**
 * set the state of the preprocessor indentation option.
 * If true, multiline #define statements will be indented.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setPreprocessorIndent(bool state)
{
	preprocessorIndent = state;
}

/**
 * set the state of the empty line fill option.
 * If true, empty lines will be filled with the whitespace.
 * of their previous lines.
 * If false, these lines will remain empty.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setEmptyLineFill(bool state)
{
	emptyLineFill = state;
}

/**
 * get the file type.
 */
int ASBeautifier::getFileType()
{
	return fileType;
}

/**
 * get the number of spaces per indent
 *
 * @return   value of indentLength option.
 */
int ASBeautifier::getIndentLength(void)
{
	return indentLength;
}

/**
 * get the char used for indentation, space or tab
 *
 * @return   the char used for indentation.
 */
string ASBeautifier::getIndentString(void)
{
	return indentString;
}

/**
 * get indent manually set flag
 */
bool ASBeautifier::getIndentManuallySet()
{
	return isIndentManuallySet;
}

/**
 * get the state of the isMinConditionalManuallySet flag
 *
 * @return   the state of isMinConditionalManuallySet.
 */
bool ASBeautifier::getMinConditionalManuallySet()
{
	return isMinConditionalManuallySet;
}

/**
 * get mode manually set flag
 */
bool ASBeautifier::getModeManuallySet()
{
	return isModeManuallySet;
}

/**
 * get the state of the force tab indentation option.
 *
 * @return   state of force tab indentation.
 */
bool ASBeautifier::getForceTabIndentation(void)
{
	return shouldForceTabIndentation;
}

/**
 * get the state of the block indentation option.
 *
 * @return   state of blockIndent option.
 */
bool ASBeautifier::getBlockIndent(void)
{
	return blockIndent;
}

/**
 * get the state of the bracket indentation option.
 *
 * @return   state of bracketIndent option.
 */
bool ASBeautifier::getBracketIndent(void)
{
	return bracketIndent;
}

/**
 * get the state of the class indentation option. If true, blocks of
 * the 'class' statement will be indented one additional indent.
 *
 * @return   state of classIndent option.
 */
bool ASBeautifier::getClassIndent(void)
{
	return classIndent;
}

/**
 * get the state of the switch indentation option. If true, blocks of
 * the 'switch' statement will be indented one additional indent.
 *
 * @return   state of switchIndent option.
 */
bool ASBeautifier::getSwitchIndent(void)
{
	return switchIndent;
}

/**
 * get the state of the case indentation option. If true, lines of 'case'
 * statements will be indented one additional indent.
 *
 * @return   state of caseIndent option.
 */
bool ASBeautifier::getCaseIndent(void)
{
	return caseIndent;
}

/**
 * get the state of the empty line fill option.
 * If true, empty lines will be filled with the whitespace.
 * of their previous lines.
 * If false, these lines will remain empty.
 *
 * @return   state of emptyLineFill option.
 */
bool ASBeautifier::getEmptyLineFill(void)
{
	return emptyLineFill;
}

/**
 * check if there are any indented lines ready to be read by nextLine()
 *
 * @return    are there any indented lines ready?
 */
bool ASBeautifier::hasMoreLines() const
{
	return sourceIterator->hasMoreLines();
}

/**
 * get the next indented line.
 *
 * @return    indented line.
 */
string ASBeautifier::nextLine()
{
	return beautify(sourceIterator->nextLine());
}

/**
 * beautify a line of source code.
 * every line of source code in a source code file should be sent
 * one after the other to the beautify method.
 *
 * @return      the indented line.
 * @param originalLine       the original unindented line.
 */
string ASBeautifier::beautify(const string &originalLine)
{
	string line;
	bool isInLineComment = false;
	bool lineStartsInComment = false;
	bool isInClass = false;
	bool isInSwitch = false;
	bool isInOperator = false;
	bool isSpecialChar = false;
	bool haveCaseIndent = false;
	bool haveAssignmentThisLine = false;
	bool lineBeginsWithBracket = false;
	bool closingBracketReached = false;
	bool shouldIndentBrackettedLine = true;
	bool previousLineProbation = (probationHeader != NULL);
	bool isInQuoteContinuation = isInVerbatimQuote | haveLineContinuationChar;
	char ch = ' ';
	char prevCh;
	char tempCh;
	int tabCount = 0;
	int spaceTabCount = 0;
	int lineOpeningBlocksNum = 0;
	int lineClosingBlocksNum = 0;
	int tabIncrementIn = 0;
	int i;
	int iPrelim;
	string outBuffer; // the newly idented line is buffered here
	const string *lastLineHeader = NULL;

	currentHeader = NULL;
	lineStartsInComment = isInComment;
	blockCommentNoBeautify = blockCommentNoIndent;
	isInAsmOneLine = false;
	lineOpensComment = false;
	previousLineProbationTab = false;
	haveLineContinuationChar = false;

	// handle and remove white spaces around the line:
	// If not in comment, first find out size of white space before line,
	// so that possible comments starting in the line continue in
	// relation to the preliminary white-space.
	if (isInQuoteContinuation)
	{
		// trim a single space added by ASFormatter, otherwise leave it alone
		if (!(originalLine.length() == 1 && originalLine[0] == ' '))
			line = originalLine;
	}
	else if (isInComment || isInBeautifySQL)
	{
		// trim the end of comment and SQL lines
		line = originalLine;
		size_t trimEnd = line.find_last_not_of(" \t");
		if (trimEnd == string::npos)
			trimEnd = 0;
		else
			trimEnd++;
		if (trimEnd < line.length())
			line.erase(trimEnd);
	}
	else
	{
		line = trim(originalLine);
		if (line.length() > 0 && line[0] == '{')
			lineBeginsWithBracket = true;

		isInHorstmannComment = false;
		size_t j = line.find_first_not_of(" \t{");
		if (j != string::npos && line.compare(j, 2, "/*") == 0)
		{
			lineOpensComment = true;
			size_t k = line.find_first_not_of(" \t");
			if (k != string::npos && line.compare(k, 1, "{") == 0)
				isInHorstmannComment = true;
		}
	}

	if (line.length() == 0)
	{
		if (backslashEndsPrevLine)  // must continue to clear variables
			line = ' ';
		else if (emptyLineFill && !isInQuoteContinuation && headerStack->size() > 0)
			return preLineWS(prevFinalLineSpaceTabCount, prevFinalLineTabCount);
		else
			return line;
	}

	// handle preprocessor commands
	// except C# region and endregion

	if (!isInComment
	        && (line[0] == '#' || backslashEndsPrevLine)
	        && line.compare(0, 7, "#region") != 0
	        && line.compare(0, 10, "#endregion") != 0)
	{
		if (line[0] == '#')
		{
			string preproc = trim(string(line.c_str() + 1));

			// When finding a multi-lined #define statement, the original beautifier
			// 1. sets its isInDefineDefinition flag
			// 2. clones a new beautifier that will be used for the actual indentation
			//    of the #define. This clone is put into the activeBeautifierStack in order
			//    to be called for the actual indentation.
			// The original beautifier will have isInDefineDefinition = true, isInDefine = false
			// The cloned beautifier will have   isInDefineDefinition = true, isInDefine = true
			if (preprocessorIndent && preproc.compare(0, 6, "define") == 0 && line[line.length() - 1] == '\\')
			{
				if (!isInDefineDefinition)
				{
					ASBeautifier *defineBeautifier;

					// this is the original beautifier
					isInDefineDefinition = true;

					// push a new beautifier into the active stack
					// this beautifier will be used for the indentation of this define
					defineBeautifier = new ASBeautifier(*this);
					activeBeautifierStack->push_back(defineBeautifier);
				}
				else
				{
					// the is the cloned beautifier that is in charge of indenting the #define.
					isInDefine = true;
				}
			}
			else if (preproc.compare(0, 2, "if") == 0)
			{
				// push a new beautifier into the stack
				waitingBeautifierStackLengthStack->push_back(waitingBeautifierStack->size());
				activeBeautifierStackLengthStack->push_back(activeBeautifierStack->size());
				waitingBeautifierStack->push_back(new ASBeautifier(*this));
			}
			else if (preproc.compare(0, 4/*2*/, "else") == 0)
			{
				if (waitingBeautifierStack && !waitingBeautifierStack->empty())
				{
					// MOVE current waiting beautifier to active stack.
					activeBeautifierStack->push_back(waitingBeautifierStack->back());
					waitingBeautifierStack->pop_back();
				}
			}
			else if (preproc.compare(0, 4, "elif") == 0)
			{
				if (waitingBeautifierStack && !waitingBeautifierStack->empty())
				{
					// append a COPY current waiting beautifier to active stack, WITHOUT deleting the original.
					activeBeautifierStack->push_back(new ASBeautifier(*(waitingBeautifierStack->back())));
				}
			}
			else if (preproc.compare(0, 5, "endif") == 0)
			{
				int stackLength;
				ASBeautifier *beautifier;

				if (waitingBeautifierStackLengthStack && !waitingBeautifierStackLengthStack->empty())
				{
					stackLength = waitingBeautifierStackLengthStack->back();
					waitingBeautifierStackLengthStack->pop_back();
					while ((int) waitingBeautifierStack->size() > stackLength)
					{
						beautifier = waitingBeautifierStack->back();
						waitingBeautifierStack->pop_back();
						delete beautifier;
					}
				}

				if (!activeBeautifierStackLengthStack->empty())
				{
					stackLength = activeBeautifierStackLengthStack->back();
					activeBeautifierStackLengthStack->pop_back();
					while ((int) activeBeautifierStack->size() > stackLength)
					{
						beautifier = activeBeautifierStack->back();
						activeBeautifierStack->pop_back();
						delete beautifier;
					}
				}
			}
		}

		// check if the last char is a backslash
		if (line.length() > 0)
			backslashEndsPrevLine = (line[line.length() - 1] == '\\');
		else
			backslashEndsPrevLine = false;

		// check if this line ends a multi-line #define
		// if so, use the #define's cloned beautifier for the line's indentation
		// and then remove it from the active beautifier stack and delete it.
		if (!backslashEndsPrevLine && isInDefineDefinition && !isInDefine)
		{
			string beautifiedLine;
			ASBeautifier *defineBeautifier;

			isInDefineDefinition = false;
			defineBeautifier = activeBeautifierStack->back();
			activeBeautifierStack->pop_back();

			beautifiedLine = defineBeautifier->beautify(line);
			delete defineBeautifier;
			return beautifiedLine;
		}

		// unless this is a multi-line #define, return this precompiler line as is.
		if (!isInDefine && !isInDefineDefinition)
			return originalLine;
	}

	// if there exists any worker beautifier in the activeBeautifierStack,
	// then use it instead of me to indent the current line.
	// variables set by ASFormatter must be updated.
	if (!isInDefine && activeBeautifierStack != NULL && !activeBeautifierStack->empty())
	{
		activeBeautifierStack->back()->inLineNumber = inLineNumber;
		activeBeautifierStack->back()->horstmannIndentInStatement = horstmannIndentInStatement;
		activeBeautifierStack->back()->nonInStatementBracket = nonInStatementBracket;
		activeBeautifierStack->back()->lineCommentNoBeautify = lineCommentNoBeautify;
		activeBeautifierStack->back()->isNonInStatementArray = isNonInStatementArray;
		activeBeautifierStack->back()->isSharpAccessor = isSharpAccessor;
		activeBeautifierStack->back()->isSharpDelegate = isSharpDelegate;
		activeBeautifierStack->back()->isInExtern = isInExtern;
		activeBeautifierStack->back()->isInBeautifySQL = isInBeautifySQL;
		activeBeautifierStack->back()->isInIndentableStruct = isInIndentableStruct;
		// must return originalLine not the trimmed line
		return activeBeautifierStack->back()->beautify(originalLine);
	}

	// calculate preliminary indentation based on data from past lines

	if (!inStatementIndentStack->empty())
		spaceTabCount = inStatementIndentStack->back();

	for (i = 0; i < (int) headerStack->size(); i++)
	{
		isInClass = false;

		if (blockIndent)
		{
			// do NOT indent opening block for these headers
			if (!((*headerStack)[i] == &AS_NAMESPACE
			        || (*headerStack)[i] == &AS_CLASS
			        || (*headerStack)[i] == &AS_STRUCT
			        || (*headerStack)[i] == &AS_UNION
			        || (*headerStack)[i] == &AS_CONST
			        || (*headerStack)[i] == &AS_INTERFACE
			        || (*headerStack)[i] == &AS_THROWS
			        || (*headerStack)[i] == &AS_STATIC))
				++tabCount;
		}
		else if (!(i > 0 && (*headerStack)[i-1] != &AS_OPEN_BRACKET
		           && (*headerStack)[i] == &AS_OPEN_BRACKET))
			++tabCount;

		if (!isJavaStyle() && !namespaceIndent && i >= 1
		        && (*headerStack)[i-1] == &AS_NAMESPACE
		        && (*headerStack)[i] == &AS_OPEN_BRACKET)
			--tabCount;

		if (isCStyle() && i >= 1
		        && (*headerStack)[i-1] == &AS_CLASS
		        && (*headerStack)[i] == &AS_OPEN_BRACKET)
		{
			if (classIndent)
				++tabCount;
			isInClass = true;
		}

		// is the switchIndent option is on, indent switch statements an additional indent.
		else if (switchIndent && i > 1
		         && (*headerStack)[i-1] == &AS_SWITCH
		         && (*headerStack)[i] == &AS_OPEN_BRACKET)
		{
			++tabCount;
			isInSwitch = true;
		}

	}	// end of for loop * end of for loop * end of for loop * end of for loop * end of for loop *

	iPrelim = i;

	if (!lineStartsInComment
	        && isCStyle()
	        && isInClass
	        && classIndent
	        && headerStack->size() >= 2
	        && (*headerStack)[headerStack->size()-2] == &AS_CLASS
	        && (*headerStack)[headerStack->size()-1] == &AS_OPEN_BRACKET
	        && line[0] == '}'
	        && bracketBlockStateStack->back() == true)
		--tabCount;

	else if (!lineStartsInComment
	         && isInSwitch
	         && switchIndent
	         && headerStack->size() >= 2
	         && (*headerStack)[headerStack->size()-2] == &AS_SWITCH
	         && (*headerStack)[headerStack->size()-1] == &AS_OPEN_BRACKET
	         && line[0] == '}')
		--tabCount;

	if (isInClassInitializer)
	{
		if (lineStartsInComment || lineOpensComment)
		{
			if (!lineBeginsWithBracket)
				tabCount--;
		}
		else if (isCStyle() && !isClassAccessModifier(line))
		{
			isInClassHeaderTab = true;
			tabCount += classInitializerTabs;
		}
		else if (blockIndent)
		{
			if (!lineBeginsWithBracket)
				tabCount++;
		}
	}
	// handle special case of indented horstmann brackets
	else if (lineStartsInComment && isInHorstmannComment && bracketIndent)
		tabCount++;

	// handle special case of horstmann comment in an indented class statement
	if (isInClass
	        && classIndent
	        && isInHorstmannComment
	        && !lineOpensComment
	        && headerStack->size() >= 2
	        && (*headerStack)[headerStack->size()-2] == &AS_CLASS)
		--tabCount;

	if (isInConditional)
	{
		--tabCount;
	}


	// parse characters in the current line.

	for (i = 0; i < (int) line.length(); i++)
	{
		outBuffer.append(1, line[i]);

		tempCh = line[i];
		prevCh = ch;
		ch = tempCh;

		if (isInBeautifySQL)
			continue;

		if (isWhiteSpace(ch))
		{
			if (ch == '\t')
				tabIncrementIn += convertTabToSpaces(i, tabIncrementIn);
			continue;
		}

		// handle special characters (i.e. backslash+character such as \n, \t, ...)

		if (isInQuote && !isInVerbatimQuote)
		{
			if (isSpecialChar)
			{
				isSpecialChar = false;
				continue;
			}
			if (line.compare(i, 2, "\\\\") == 0)
			{
				outBuffer.append(1, '\\');
				i++;
				continue;
			}
			if (ch == '\\')
			{
				if (peekNextChar(line, i) == ' ')   // is this '\' at end of line
					haveLineContinuationChar = true;
				else
					isSpecialChar = true;
				continue;
			}
		}
		else if (isInDefine && ch == '\\')
			continue;

		// handle quotes (such as 'x' and "Hello Dolly")
		if (!(isInComment || isInLineComment) && (ch == '"' || ch == '\''))
		{
			if (!isInQuote)
			{
				quoteChar = ch;
				isInQuote = true;
				if (isSharpStyle() && prevCh == '@')
					isInVerbatimQuote = true;
			}
			else if (isInVerbatimQuote && ch == '"')
			{
				if (peekNextChar(line, i) == '"')           // check consecutive quotes
				{
					outBuffer.append(1, '"');
					i++;
				}
				else
				{
					isInQuote = false;
					isInVerbatimQuote = false;
				}
			}
			else if (quoteChar == ch)
			{
				isInQuote = false;
				isInStatement = true;
				continue;
			}
		}
		if (isInQuote)
			continue;

		// handle comments

		if (!(isInComment || isInLineComment) && line.compare(i, 2, "//") == 0)
		{
			isInLineComment = true;
			outBuffer.append(1, '/');
			i++;
			continue;
		}
		else if (!(isInComment || isInLineComment) && line.compare(i, 2, "/*") == 0)
		{
			isInComment = true;
			outBuffer.append(1, '*');
			i++;
			if (!lineOpensComment)					// does line start with comment?
				blockCommentNoIndent = true;        // if no, cannot indent continuation lines
			continue;
		}
		else if ((isInComment || isInLineComment) && line.compare(i, 2, "*/") == 0)
		{
			isInComment = false;
			outBuffer.append(1, '/');
			i++;
			blockCommentNoIndent = false;           // ok to indent next comment
			continue;
		}
		// treat C# '#region' and '#endregion' statements as a line comment
		else if (isSharpStyle() &&
		         (line.compare(i, 7, "#region") == 0 || line.compare(i, 10, "#endregion") == 0))
		{
			isInLineComment = true;
		}

		if (isInComment || isInLineComment)
		{
			// append rest of the comment up to the comment end
			while (i+1 < (int) line.length()
			        && line.compare(i+1, 2, "*/") != 0)
				outBuffer.append(1, line[++i]);
			continue;
		}

		// if we have reached this far then we are NOT in a comment or string of special character...

		// SQL if formatted in ASEnhancer
		if (isInBeautifySQL)
			continue;

		if (probationHeader != NULL)
		{
			if (((probationHeader == &AS_STATIC || probationHeader == &AS_CONST) && ch == '{')
			        || (probationHeader == &AS_SYNCHRONIZED && ch == '('))
			{
				// insert the probation header as a new header
				isInHeader = true;
				headerStack->push_back(probationHeader);

				// handle the specific probation header
				isInConditional = (probationHeader == &AS_SYNCHRONIZED);

				isInStatement = false;
				// if the probation comes from the previous line, then indent by 1 tab count.
				if (previousLineProbation
				        && ch == '{'
				        && !(blockIndent
				             && (probationHeader == &AS_CONST || probationHeader == &AS_STATIC)))
				{
					tabCount++;
					previousLineProbationTab = true;
				}
				previousLineProbation = false;
			}

			// dismiss the probation header
			probationHeader = NULL;
		}

		prevNonSpaceCh = currentNonSpaceCh;
		currentNonSpaceCh = ch;
		if (!isLegalNameChar(ch) && ch != ',' && ch != ';')
		{
			prevNonLegalCh = currentNonLegalCh;
			currentNonLegalCh = ch;
		}

		if (isInHeader)
		{
			isInHeader = false;
			currentHeader = headerStack->back();
		}
		else
			currentHeader = NULL;

		if (isCStyle() && isInTemplate
		        && (ch == '<' || ch == '>')
		        &&  findOperator(line, i, nonAssignmentOperators) == NULL)
		{
			if (ch == '<')
			{
				++templateDepth;
			}
			else if (ch == '>')
			{
				if (--templateDepth <= 0)
				{
					if (isInTemplate)
						ch = ';';
					else
						ch = 't';
					isInTemplate = false;
					templateDepth = 0;
				}
			}
		}

		// handle parenthesies
		if (ch == '(' || ch == '[' || ch == ')' || ch == ']')
		{
			if (ch == '(' || ch == '[')
			{
				isInOperator = false;
				// if have a struct header, this is a declaration not a definition
				if (ch == '('
				        && (isInClassInitializer || isInClassHeaderTab)
				        && headerStack->size() > 0
				        && headerStack->back() == &AS_STRUCT)
				{
					headerStack->pop_back();
					isInClassInitializer = false;
					// -1 for isInClassInitializer, -2 for isInClassHeaderTab
					if (isInClassHeaderTab)
					{
						tabCount -= (1 + classInitializerTabs);
						isInClassHeaderTab = false;
					}
					if (tabCount < 0)
						tabCount = 0;
				}

				if (parenDepth == 0)
				{
					parenStatementStack->push_back(isInStatement);
					isInStatement = true;
				}
				parenDepth++;

				inStatementIndentStackSizeStack->push_back(inStatementIndentStack->size());

				if (currentHeader != NULL)
					registerInStatementIndent(line, i, spaceTabCount, tabIncrementIn, minConditionalIndent/*indentLength*2*/, true);
				else
					registerInStatementIndent(line, i, spaceTabCount, tabIncrementIn, 0, true);
			}
			else if (ch == ')' || ch == ']')
			{
				parenDepth--;
				if (parenDepth == 0)
				{
					if (!parenStatementStack->empty())      // in case of unmatched closing parens
					{
						isInStatement = parenStatementStack->back();
						parenStatementStack->pop_back();
					}
					ch = ' ';
					isInAsm = false;
					isInConditional = false;
				}

				if (!inStatementIndentStackSizeStack->empty())
				{
					int previousIndentStackSize = inStatementIndentStackSizeStack->back();
					inStatementIndentStackSizeStack->pop_back();
					while (previousIndentStackSize < (int) inStatementIndentStack->size())
						inStatementIndentStack->pop_back();

					if (!parenIndentStack->empty())
					{
						int poppedIndent = parenIndentStack->back();
						parenIndentStack->pop_back();

						if (i == 0)
							spaceTabCount = poppedIndent;
					}
				}
			}

			continue;
		}


		if (ch == '{')
		{
			// first, check if '{' is a block-opener or an static-array opener
			bool isBlockOpener = ((prevNonSpaceCh == '{' && bracketBlockStateStack->back())
			                      || prevNonSpaceCh == '}'
			                      || prevNonSpaceCh == ')'
			                      || prevNonSpaceCh == ';'
			                      || peekNextChar(line, i) == '{'
			                      || isInClassInitializer
			                      || isNonInStatementArray
			                      || isSharpAccessor
			                      || isSharpDelegate
			                      || isInExtern
			                      || (isInDefine &&
			                          (prevNonSpaceCh == '('
			                           || isLegalNameChar(prevNonSpaceCh))));

			// remove inStatementIndent for C++ class initializer
			if (isInClassInitializer)
			{
				if (inStatementIndentStack->size() > 0)
					inStatementIndentStack->pop_back();
				isInStatement = false;
				if (lineBeginsWithBracket)
					spaceTabCount = 0;
				isInClassInitializer = false;
			}

			if (!isBlockOpener && currentHeader != NULL)
			{
				for (size_t n = 0; n < nonParenHeaders->size(); n++)
					if (currentHeader == (*nonParenHeaders)[n])
					{
						isBlockOpener = true;
						break;
					}
			}

			bracketBlockStateStack->push_back(isBlockOpener);

			if (!isBlockOpener)
			{
				inStatementIndentStackSizeStack->push_back(inStatementIndentStack->size());
				registerInStatementIndent(line, i, spaceTabCount, tabIncrementIn, 0, true);
				parenDepth++;
				if (i == 0)
					shouldIndentBrackettedLine = false;

				continue;
			}

			// this bracket is a block opener...

			++lineOpeningBlocksNum;

			if (isInClassHeaderTab)
			{
				isInClassHeaderTab = false;
				// decrease tab count if bracket is broken
				size_t firstChar = line.find_first_not_of(" \t");
				if (firstChar != string::npos
				        && line[firstChar] == '{'
				        && (int) firstChar == i)
				{
					tabCount -= classInitializerTabs;
					// decrease one more if an empty class
					if (headerStack->size() > 0
					        && (*headerStack).back() == &AS_CLASS)
					{
						int nextChar = getNextProgramCharDistance(line, i);
						if (line[nextChar] == '}')
							tabCount--;
					}
				}
			}

			if (bracketIndent && !namespaceIndent && headerStack->size() > 0
			        && (*headerStack).back() == &AS_NAMESPACE)
			{
				shouldIndentBrackettedLine = false;
				tabCount--;
			}

			// an indentable struct is treated like a class in the header stack
			if (headerStack->size() > 0
			        && (*headerStack).back() == &AS_STRUCT
			        && isInIndentableStruct)
				(*headerStack).back() = &AS_CLASS;

			blockParenDepthStack->push_back(parenDepth);
			blockStatementStack->push_back(isInStatement);

			inStatementIndentStackSizeStack->push_back(inStatementIndentStack->size());
			if (inStatementIndentStack->size() > 0)
			{
				spaceTabCount = 0;
				inStatementIndentStack->back() = 0;
			}

			blockTabCount += isInStatement ? 1 : 0;
			parenDepth = 0;
			isInStatement = false;

			tempStacks->push_back(new vector<const string*>);
			headerStack->push_back(&AS_OPEN_BRACKET);
			lastLineHeader = &AS_OPEN_BRACKET;

			continue;
		}

		//check if a header has been reached
		bool isPotentialHeader = isCharPotentialHeader(line, i);

		if (isPotentialHeader)
		{
			const string *newHeader = findHeader(line, i, headers);

			if (newHeader != NULL)
			{
				char peekChar = peekNextChar(line, i + newHeader->length() - 1);

				// is not a header if part of a definition
				if (peekChar == ',' || peekChar == ')')
					newHeader = NULL;
				// the following accessor definitions are NOT headers
				// goto default; is NOT a header
				// default(int) keyword in C# is NOT a header
				else if ((newHeader == &AS_GET || newHeader == &AS_SET || newHeader == &AS_DEFAULT)
				         && (peekChar == ';' ||  peekChar == '('))
				{
					newHeader = NULL;
				}
			}

			if (newHeader != NULL)
			{
				// if we reached here, then this is a header...
				bool isIndentableHeader = true;

				isInHeader = true;

				vector<const string*> *lastTempStack;
				if (tempStacks->empty())
					lastTempStack = NULL;
				else
					lastTempStack = tempStacks->back();

				// if a new block is opened, push a new stack into tempStacks to hold the
				// future list of headers in the new block.

				// take care of the special case: 'else if (...)'
				if (newHeader == &AS_IF && lastLineHeader == &AS_ELSE)
				{
					headerStack->pop_back();
				}

				// take care of 'else'
				else if (newHeader == &AS_ELSE)
				{
					if (lastTempStack != NULL)
					{
						int indexOfIf = indexOf(*lastTempStack, &AS_IF);
						if (indexOfIf != -1)
						{
							// recreate the header list in headerStack up to the previous 'if'
							// from the temporary snapshot stored in lastTempStack.
							int restackSize = lastTempStack->size() - indexOfIf - 1;
							for (int r = 0; r < restackSize; r++)
							{
								headerStack->push_back(lastTempStack->back());
								lastTempStack->pop_back();
							}
							if (!closingBracketReached)
								tabCount += restackSize;
						}
						/*
						 * If the above if is not true, i.e. no 'if' before the 'else',
						 * then nothing beautiful will come out of this...
						 * I should think about inserting an Exception here to notify the caller of this...
						 */
					}
				}

				// check if 'while' closes a previous 'do'
				else if (newHeader == &AS_WHILE)
				{
					if (lastTempStack != NULL)
					{
						int indexOfDo = indexOf(*lastTempStack, &AS_DO);
						if (indexOfDo != -1)
						{
							// recreate the header list in headerStack up to the previous 'do'
							// from the temporary snapshot stored in lastTempStack.
							int restackSize = lastTempStack->size() - indexOfDo - 1;
							for (int r = 0; r < restackSize; r++)
							{
								headerStack->push_back(lastTempStack->back());
								lastTempStack->pop_back();
							}
							if (!closingBracketReached)
								tabCount += restackSize;
						}
					}
				}
				// check if 'catch' closes a previous 'try' or 'catch'
				else if (newHeader == &AS_CATCH || newHeader == &AS_FINALLY)
				{
					if (lastTempStack != NULL)
					{
						int indexOfTry = indexOf(*lastTempStack, &AS_TRY);
						if (indexOfTry == -1)
							indexOfTry = indexOf(*lastTempStack, &AS_CATCH);
						if (indexOfTry != -1)
						{
							// recreate the header list in headerStack up to the previous 'try'
							// from the temporary snapshot stored in lastTempStack.
							int restackSize = lastTempStack->size() - indexOfTry - 1;
							for (int r = 0; r < restackSize; r++)
							{
								headerStack->push_back(lastTempStack->back());
								lastTempStack->pop_back();
							}

							if (!closingBracketReached)
								tabCount += restackSize;
						}
					}
				}
				else if (newHeader == &AS_CASE)
				{
					isInCase = true;
					if (!haveCaseIndent)
					{
						haveCaseIndent = true;
						if (!lineBeginsWithBracket)
							--tabCount;
					}
				}
				else if (newHeader == &AS_DEFAULT)
				{
					isInCase = true;
					--tabCount;
				}
				else if (newHeader == &AS_STATIC
				         || newHeader == &AS_SYNCHRONIZED
				         || (newHeader == &AS_CONST && isCStyle()))
				{
					if (!headerStack->empty() &&
					        (headerStack->back() == &AS_STATIC
					         || headerStack->back() == &AS_SYNCHRONIZED
					         || headerStack->back() == &AS_CONST))
					{
						isIndentableHeader = false;
					}
					else
					{
						isIndentableHeader = false;
						probationHeader = newHeader;
					}
				}
				else if (newHeader == &AS_CONST)
				{
					isIndentableHeader = false;
				}
				else if (newHeader == &AS_TEMPLATE)
				{
					if (isCStyle())
						isInTemplate = true;
					isIndentableHeader = false;
				}

				if (isIndentableHeader)
				{
					headerStack->push_back(newHeader);
					isInStatement = false;
					if (indexOf(*nonParenHeaders, newHeader) == -1)
					{
						isInConditional = true;
					}
					lastLineHeader = newHeader;
				}
				else
					isInHeader = false;

				outBuffer.append(newHeader->substr(1));
				i += newHeader->length() - 1;

				continue;
			}  // newHeader != NULL

			if (isCStyle() && findKeyword(line, i, AS_ENUM))
				isInEnum = true;

		}   // isPotentialHeader

		if (ch == '?')
			isInQuestion = true;

		// special handling of 'case' statements
		if (ch == ':')
		{
			if ((int) line.length() > i + 1 && line[i+1] == ':') // look for ::
			{
				++i;
				outBuffer.append(1, ':');
				ch = ' ';
				continue;
			}

			else if (isInQuestion)
			{
				isInQuestion = false;
			}

			else if (isCStyle() && isInClassInitializer)
			{
				// found a 'class A : public B' definition
				// so do nothing special
			}

			else if (isCStyle()
			         && (isInAsm || isInAsmOneLine || isInAsmBlock))
			{
				// do nothing special
			}

			else if (isCStyle() && isdigit(peekNextChar(line, i)))
			{
				// found a bit field
				// so do nothing special
			}

			else if (isCStyle() && isInClass && prevNonSpaceCh != ')')
			{
				// found a 'private:' or 'public:' inside a class definition
				--tabCount;
			}

			else if (isCStyle() && prevNonSpaceCh == ')' && !isInCase)
			{
				isInClassInitializer = true;
				if (i == 0)
					tabCount += classInitializerTabs;
			}

			else if (isJavaStyle() && lastLineHeader == &AS_FOR)
			{
				// found a java for-each statement
				// so do nothing special
			}

			else
			{
				currentNonSpaceCh = ';'; // so that brackets after the ':' will appear as block-openers
				if (isInCase)
				{
					isInCase = false;
					ch = ';'; // from here on, treat char as ';'
				}
				else if (isCStyle() || (isSharpStyle() && peekNextChar(line, i) == ';'))    // is in a label (e.g. 'label1:')
				{
					if (labelIndent)
						--tabCount; // unindent label by one indent
					else if (!lineBeginsWithBracket)
						tabCount = 0; // completely flush indent to left
				}
			}
		}

		if ((ch == ';'  || (parenDepth > 0 && ch == ','))  && !inStatementIndentStackSizeStack->empty())
			while ((int) inStatementIndentStackSizeStack->back() + (parenDepth > 0 ? 1 : 0)
			        < (int) inStatementIndentStack->size())
				inStatementIndentStack->pop_back();

		// handle commas
		// previous "isInStatement" will be from an assignment operator
		if (ch == ',' && parenDepth == 0 && !isInStatement && !isNonInStatementArray)
		{
			// is comma at end of line
			size_t nextChar = line.find_first_not_of(" \t", i + 1);
			if (nextChar != string::npos)
			{
				if (line.compare(nextChar, 2, "//") == 0
				        || line.compare(nextChar, 2, "/*") == 0)
					nextChar = string::npos;
			}
			// register indent
			if (nextChar == string::npos)
			{
				// register indent at first word after the colon of a C++ class initializer
				if (isInClassInitializer)
				{
					size_t firstChar = line.find_first_not_of(" \t");
					if (firstChar != string::npos && line[firstChar] == ':')
					{
						size_t firstWord = line.find_first_not_of(" \t", firstChar + 1);
						if (firstChar != string::npos)
						{
							int inStatementIndent = firstWord + spaceTabCount + tabIncrementIn;
							inStatementIndentStack->push_back(inStatementIndent);
							isInStatement = true;
						}
					}
				}
				// register indent at previous word
				else
				{
					int prevWord = getInStatementIndentComma(line, i);
					int inStatementIndent = prevWord + spaceTabCount + tabIncrementIn;
					inStatementIndentStack->push_back(inStatementIndent);
					isInStatement = true;
				}
			}
		}

		// handle ends of statements
		if ((ch == ';' && parenDepth == 0) || ch == '}')
		{
			if (ch == '}')
			{
				// first check if this '}' closes a previous block, or a static array...
				if (!bracketBlockStateStack->empty())
				{
					bool bracketBlockState = bracketBlockStateStack->back();
					bracketBlockStateStack->pop_back();
					if (!bracketBlockState)
					{
						if (!inStatementIndentStackSizeStack->empty())
						{
							// this bracket is a static array

							int previousIndentStackSize = inStatementIndentStackSizeStack->back();
							inStatementIndentStackSizeStack->pop_back();
							while (previousIndentStackSize < (int) inStatementIndentStack->size())
								inStatementIndentStack->pop_back();
							parenDepth--;
							if (i == 0)
								shouldIndentBrackettedLine = false;

							if (!parenIndentStack->empty())
							{
								int poppedIndent = parenIndentStack->back();
								parenIndentStack->pop_back();
								if (i == 0)
									spaceTabCount = poppedIndent;
							}
						}
						continue;
					}
				}

				// this bracket is block closer...

				++lineClosingBlocksNum;

				if (!inStatementIndentStackSizeStack->empty())
					inStatementIndentStackSizeStack->pop_back();

				if (!blockParenDepthStack->empty())
				{
					parenDepth = blockParenDepthStack->back();
					blockParenDepthStack->pop_back();
					isInStatement = blockStatementStack->back();
					blockStatementStack->pop_back();

					if (isInStatement)
						blockTabCount--;
				}

				closingBracketReached = true;
				isInAsmOneLine = false;

				// added for release 1.24
				// TODO: remove at the appropriate time
				assert(isInAsm == false);
				assert(isInAsmOneLine == false);
				assert(isInQuote == false);
				isInAsm = isInAsmOneLine = isInQuote = false;
				// end remove

				int headerPlace = indexOf(*headerStack, &AS_OPEN_BRACKET);
				if (headerPlace != -1)
				{
					const string *popped = headerStack->back();
					while (popped != &AS_OPEN_BRACKET)
					{
						headerStack->pop_back();
						popped = headerStack->back();
					}
					headerStack->pop_back();

					// do not indent namespace bracket unless namespaces are indented
					if (!namespaceIndent && headerStack->size() > 0
					        && (*headerStack).back() == &AS_NAMESPACE)
						shouldIndentBrackettedLine = false;

					if (!tempStacks->empty())
					{
						vector<const string*> *temp =  tempStacks->back();
						tempStacks->pop_back();
						delete temp;
					}
				}


				ch = ' '; // needed due to cases such as '}else{', so that headers ('else' tn tih case) will be identified...
			}

			/*
			 * Create a temporary snapshot of the current block's header-list in the
			 * uppermost inner stack in tempStacks, and clear the headerStack up to
			 * the begining of the block.
			 * Thus, the next future statement will think it comes one indent past
			 * the block's '{' unless it specifically checks for a companion-header
			 * (such as a previous 'if' for an 'else' header) within the tempStacks,
			 * and recreates the temporary snapshot by manipulating the tempStacks.
			 */
			if (!tempStacks->back()->empty())
				while (!tempStacks->back()->empty())
					tempStacks->back()->pop_back();
			while (!headerStack->empty() && headerStack->back() != &AS_OPEN_BRACKET)
			{
				tempStacks->back()->push_back(headerStack->back());
				headerStack->pop_back();
			}

			if (parenDepth == 0 && ch == ';')
				isInStatement = false;

			previousLastLineHeader = NULL;
			isInClassInitializer = false;
			isInEnum = false;
			isInQuestion = false;

			continue;
		}

		if (isPotentialHeader)
		{
			// check for preBlockStatements in C/C++ ONLY if not within parenthesies
			// (otherwise 'struct XXX' statements would be wrongly interpreted...)
			if (!isInTemplate && !(isCStyle() && parenDepth > 0))
			{
				const string *newHeader = findHeader(line, i, preBlockStatements);
				if (newHeader != NULL
				        && !(isCStyle() && newHeader == &AS_CLASS && isInEnum))	// is it 'enum class'
				{
					isInClassInitializer = true;

					if (!isSharpStyle())
						headerStack->push_back(newHeader);
					// do not need 'where' in the headerStack
					// do not need second 'class' statement in a row
					else if (!(newHeader == &AS_WHERE
					           || (newHeader == &AS_CLASS
					               && headerStack->size() > 0
					               && headerStack->back() == &AS_CLASS)))
						headerStack->push_back(newHeader);

					outBuffer.append(newHeader->substr(1));
					i += newHeader->length() - 1;
					continue;
				}
			}
			const string *foundIndentableHeader = findHeader(line, i, indentableHeaders);

			if (foundIndentableHeader != NULL)
			{
				// must bypass the header before registering the in statement
				outBuffer.append(foundIndentableHeader->substr(1));
				i += foundIndentableHeader->length() - 1;
				if (!isInOperator && !isInTemplate && !isNonInStatementArray)
				{
					registerInStatementIndent(line, i, spaceTabCount, tabIncrementIn, 0, false);
					isInStatement = true;
				}
				continue;
			}

			if (isCStyle() && findKeyword(line, i, AS_OPERATOR))
				isInOperator = true;

			// "new" operator is a pointer, not a calculation
			if (findKeyword(line, i, AS_NEW))
			{
				if (prevNonSpaceCh == '=' && isInStatement && !inStatementIndentStack->empty())
					inStatementIndentStack->back() = 0;
			}

			if (isCStyle())
			{
				if (findKeyword(line, i, AS_ASM)
				        || findKeyword(line, i, AS__ASM__))
				{
					isInAsm = true;
				}
				else if (findKeyword(line, i, AS_MS_ASM)		// microsoft specific
				         || findKeyword(line, i, AS_MS__ASM))
				{
					int index = 4;
					if (peekNextChar(line, i) == '_')		// check for __asm
						index = 5;

					char peekedChar = ASBase::peekNextChar(line, i + index);
					if (peekedChar == '{' || peekedChar == ' ')
						isInAsmBlock = true;
					else
						isInAsmOneLine = true;
				}
			}

			// append the entire name for all others
			string name = getCurrentWord(line, i);
			outBuffer.append(name.substr(1));
			i += name.length() - 1;
			continue;
		}

		// Handle operators

		bool isPotentialOperator = isCharPotentialOperator(ch);

		if (isPotentialOperator)
		{
			// Check if an operator has been reached.
			const string *foundAssignmentOp = findOperator(line, i, assignmentOperators);
			const string *foundNonAssignmentOp = findOperator(line, i, nonAssignmentOperators);

			// Since findHeader's boundry checking was not used above, it is possible
			// that both an assignment op and a non-assignment op where found,
			// e.g. '>>' and '>>='. If this is the case, treat the LONGER one as the
			// found operator.
			if (foundAssignmentOp != NULL && foundNonAssignmentOp != NULL)
			{
				if (foundAssignmentOp->length() < foundNonAssignmentOp->length())
					foundAssignmentOp = NULL;
				else
					foundNonAssignmentOp = NULL;
			}

			if (foundNonAssignmentOp != NULL)
			{
				if (foundNonAssignmentOp->length() > 1)
				{
					outBuffer.append(foundNonAssignmentOp->substr(1));
					i += foundNonAssignmentOp->length() - 1;
				}

				// For C++ input/output, operator<< and >> should be
				// aligned, if we are not in a statement already and
				// also not in the "operator<<(...)" header line
				if (!isInOperator
				        && inStatementIndentStack->empty()
				        && isCStyle()
				        && (foundNonAssignmentOp == &AS_GR_GR ||
				            foundNonAssignmentOp == &AS_LS_LS))
				{
					// this will be true if the line begins with the operator
					if (i < 2 && spaceTabCount == 0)
						spaceTabCount += 2 * indentLength;
					// align to the beginning column of the operator
					registerInStatementIndent(line, i - foundNonAssignmentOp->length(), spaceTabCount, tabIncrementIn, 0, false);
				}
			}

			else if (foundAssignmentOp != NULL)
			{
				if (foundAssignmentOp->length() > 1)
				{
					outBuffer.append(foundAssignmentOp->substr(1));
					i += foundAssignmentOp->length() - 1;
				}

				if (!isInOperator && !isInTemplate && !isNonInStatementArray)
				{
					// if multiple assignments, align on the previous word
					if (foundAssignmentOp == &AS_ASSIGN
					        && prevNonSpaceCh != ']'		// an array
					        && statementEndsWithComma(line, i))
					{
						if (!haveAssignmentThisLine)		// only one assignment indent per line
						{
							// register indent at previous word
							haveAssignmentThisLine = true;
							int prevWordIndex = getInStatementIndentAssign(line, i);
							int inStatementIndent = prevWordIndex + spaceTabCount + tabIncrementIn;
							inStatementIndentStack->push_back(inStatementIndent);
						}
					}
					else
						registerInStatementIndent(line, i, spaceTabCount, tabIncrementIn, 0, false);

					isInStatement = true;
				}
			}
		}
	}	// end of for loop * end of for loop * end of for loop * end of for loop * end of for loop *

	// handle special cases of unindentation:

	/*
	 * if '{' doesn't follow an immediately previous '{' in the headerStack
	 * (but rather another header such as "for" or "if", then unindent it
	 * by one indentation relative to its block.
	 */

	if (!lineStartsInComment
	        && !blockIndent
	        && outBuffer.length() > 0
	        && outBuffer[0] == '{'
	        && !(lineOpeningBlocksNum > 0 && lineOpeningBlocksNum == lineClosingBlocksNum)
	        && !(headerStack->size() > 1 && (*headerStack)[headerStack->size()-2] == &AS_OPEN_BRACKET)
	        && shouldIndentBrackettedLine)
		--tabCount;

	// must check one less in headerStack if more than one header on a line (allow-addins)...
	else if (!lineStartsInComment
	         && (int) headerStack->size() > iPrelim + 1
	         && !blockIndent
	         && outBuffer.length() > 0
	         && outBuffer[0] == '{'
	         && !(lineOpeningBlocksNum > 0 && lineOpeningBlocksNum == lineClosingBlocksNum)
	         && !(headerStack->size() > 2 && (*headerStack)[headerStack->size()-3] == &AS_OPEN_BRACKET)
	         && shouldIndentBrackettedLine)
		--tabCount;

	// unindent a closing bracket...
	else if (!lineStartsInComment
	         && outBuffer.length() > 0
	         && outBuffer[0] == '}'
	         && shouldIndentBrackettedLine)
		--tabCount;

	// correctly indent one-line-blocks...
	else if (!lineStartsInComment
	         && outBuffer.length() > 0
	         && lineOpeningBlocksNum > 0
	         && lineOpeningBlocksNum == lineClosingBlocksNum
	         && previousLineProbationTab)
		--tabCount; //lineOpeningBlocksNum - (blockIndent ? 1 : 0);

	// correctly indent class continuation lines...
	else if (!lineStartsInComment
	         && !lineOpensComment
	         && isInClassHeaderTab
	         && !blockIndent
	         && outBuffer.length() > 0
	         && lineOpeningBlocksNum == 0
	         && lineOpeningBlocksNum == lineClosingBlocksNum
	         && (headerStack->size() > 0 && headerStack->back() == &AS_CLASS))
		--tabCount;

	if (tabCount < 0)
		tabCount = 0;

	// take care of extra bracket indentatation option...
	if (!lineStartsInComment
	        && bracketIndent
	        && shouldIndentBrackettedLine
	        && outBuffer.length() > 0
	        && (outBuffer[0] == '{' || outBuffer[0] == '}'))
		tabCount++;

	if (isInDefine)
	{
		if (outBuffer[0] == '#')
		{
			string preproc = trim(string(outBuffer.c_str() + 1));
			if (preproc.compare(0, 6, "define") == 0)
			{
				if (!inStatementIndentStack->empty()
				        && inStatementIndentStack->back() > 0)
				{
					defineTabCount = tabCount;
				}
				else
				{
					defineTabCount = tabCount - 1;
					tabCount--;
				}
			}
		}

		tabCount -= defineTabCount;
	}

	if (tabCount < 0)
		tabCount = 0;
	if (lineCommentNoBeautify || blockCommentNoBeautify || isInQuoteContinuation)
		tabCount = spaceTabCount = 0;

	// finally, insert indentations into begining of line

	if (shouldForceTabIndentation)
	{
		tabCount += spaceTabCount / indentLength;
		spaceTabCount = spaceTabCount % indentLength;
	}

	outBuffer = preLineWS(spaceTabCount, tabCount) + outBuffer;

	prevFinalLineSpaceTabCount = spaceTabCount;
	prevFinalLineTabCount = tabCount;

	if (lastLineHeader != NULL)
		previousLastLineHeader = lastLineHeader;

	return outBuffer;
}


string ASBeautifier::preLineWS(int spaceTabCount, int tabCount)
{
	string ws;

	for (int i = 0; i < tabCount; i++)
		ws += indentString;

	while ((spaceTabCount--) > 0)
		ws += string(" ");

	return ws;

}

bool ASBeautifier::isClassAccessModifier(string& line) const
{
	size_t firstChar = line.find_first_not_of(" \t");
	if (firstChar == string::npos)
		return false;
	// bypass a colon
	if (line[firstChar] == ':')
	{
		firstChar = line.find_first_not_of(" \t");
		if (firstChar == string::npos)
			return false;
	}
	if (line.compare(firstChar, 7, "public ") == 0
	        || line.compare(firstChar, 8, "private ") == 0
	        || line.compare(firstChar, 10, "protected ") == 0)
		return true;
	return false;
}

/**
 * register an in-statement indent.
 */
void ASBeautifier::registerInStatementIndent(const string &line, int i, int spaceTabCount,
        int tabIncrementIn, int minIndent, bool updateParenStack)
{
	int inStatementIndent;
	int remainingCharNum = line.length() - i;
	int nextNonWSChar = getNextProgramCharDistance(line, i);

	// if indent is around the last char in the line, indent instead one indent from the previous indent
	if (nextNonWSChar == remainingCharNum)
	{
		int previousIndent = spaceTabCount;
		if (!inStatementIndentStack->empty())
			previousIndent = inStatementIndentStack->back();
		int currIndent = /*2*/ indentLength + previousIndent;
		if (currIndent > maxInStatementIndent
		        && line[i] != '{')
			currIndent = indentLength * 2 + spaceTabCount;
		inStatementIndentStack->push_back(currIndent);
		if (updateParenStack)
			parenIndentStack->push_back(previousIndent);
		return;
	}

	if (updateParenStack)
		parenIndentStack->push_back(i + spaceTabCount - horstmannIndentInStatement);

	int tabIncrement = tabIncrementIn;

	// check for following tabs
	for (int j = i + 1; j < (i + nextNonWSChar); j++)
	{
		if (line[j] == '\t')
			tabIncrement += convertTabToSpaces(j, tabIncrement);
	}

	inStatementIndent = i + nextNonWSChar + spaceTabCount + tabIncrement;

	// check for run-in statement
	if (i > 0 && line[0] == '{')
		inStatementIndent -= indentLength;

//	if (i + nextNonWSChar < minIndent)
//		inStatementIndent = minIndent + spaceTabCount;

	if (inStatementIndent < minIndent)
		inStatementIndent = minIndent + spaceTabCount;

//	if (i + nextNonWSChar > maxInStatementIndent)
//		inStatementIndent =  indentLength * 2 + spaceTabCount;

	if (inStatementIndent > maxInStatementIndent)
		inStatementIndent = indentLength * 2 + spaceTabCount;

	if (!inStatementIndentStack->empty() &&
	        inStatementIndent < inStatementIndentStack->back())
		inStatementIndent = inStatementIndentStack->back();

	// the block opener is not indented for a NonInStatementArray
	if (isNonInStatementArray && !bracketBlockStateStack->empty() && bracketBlockStateStack->back())
		inStatementIndent = 0;

	inStatementIndentStack->push_back(inStatementIndent);
}

/**
 * get distance to the next non-white space, non-comment character in the line.
 * if no such character exists, return the length remaining to the end of the line.
 */
int ASBeautifier::getNextProgramCharDistance(const string &line, int i) const
{
	bool inComment = false;
	int  remainingCharNum = line.length() - i;
	int  charDistance;
	char ch;

	for (charDistance = 1; charDistance < remainingCharNum; charDistance++)
	{
		ch = line[i + charDistance];
		if (inComment)
		{
			if (line.compare(i + charDistance, 2, "*/") == 0)
			{
				charDistance++;
				inComment = false;
			}
			continue;
		}
		else if (isWhiteSpace(ch))
			continue;
		else if (ch == '/')
		{
			if (line.compare(i + charDistance, 2, "//") == 0)
				return remainingCharNum;
			else if (line.compare(i + charDistance, 2, "/*") == 0)
			{
				charDistance++;
				inComment = true;
			}
		}
		else
			return charDistance;
	}

	return charDistance;
}

// check if a specific line position contains a header.
const string* ASBeautifier::findHeader(const string &line, int i,
                                       const vector<const string*>* possibleHeaders) const
{
	assert(isCharPotentialHeader(line, i));
	// check the word
	size_t maxHeaders = possibleHeaders->size();
	for (size_t p = 0; p < maxHeaders; p++)
	{
		const string* header = (*possibleHeaders)[p];
		const size_t wordEnd = i + header->length();
		if (wordEnd > line.length())
			continue;
		int result = (line.compare(i, header->length(), *header));
		if (result > 0)
			continue;
		if (result < 0)
			break;
		// check that this is not part of a longer word
		if (wordEnd == line.length())
			return header;
		if (isLegalNameChar(line[wordEnd]))
			continue;
		// is not a header if part of a definition
		const char peekChar = peekNextChar(line, wordEnd - 1);
		if (peekChar == ',' || peekChar == ')')
			break;
		return header;
	}
	return NULL;
}

// check if a specific line position contains an operator.
const string* ASBeautifier::findOperator(const string &line, int i,
        const vector<const string*>* possibleOperators) const
{
	assert(isCharPotentialOperator(line[i]));
	// find the operator in the vector
	// the vector contains the LONGEST operators first
	// must loop thru the entire vector
	size_t maxOperators = possibleOperators->size();
	for (size_t p = 0; p < maxOperators; p++)
	{
		const size_t wordEnd = i + (*(*possibleOperators)[p]).length();
		if (wordEnd > line.length())
			continue;
		if (line.compare(i, (*(*possibleOperators)[p]).length(), *(*possibleOperators)[p]) == 0)
			return (*possibleOperators)[p];
	}
	return NULL;
}

/**
 * find the index number of a string element in a container of strings
 *
 * @return              the index number of element in the container. -1 if element not found.
 * @param container     a vector of strings.
 * @param element       the element to find .
 */
int ASBeautifier::indexOf(vector<const string*> &container, const string *element)
{
	vector<const string*>::const_iterator where;

	where = find(container.begin(), container.end(), element);
	if (where == container.end())
		return -1;
	else
		return (int) (where - container.begin());
}

/**
 * convert tabs to spaces.
 * i is the position of the character to convert to spaces.
 * tabIncrementIn is the increment that must be added for tab indent characters
 *     to get the correct column for the current tab.
 */
int ASBeautifier::convertTabToSpaces(int i, int tabIncrementIn) const
{
	int tabToSpacesAdjustment = indentLength - 1 - ((tabIncrementIn + i) % indentLength);
	return tabToSpacesAdjustment;
}

/**
 * trim removes the white space surrounding a line.
 *
 * @return          the trimmed line.
 * @param str       the line to trim.
 */
string ASBeautifier::trim(const string &str)
{

	int start = 0;
	int end = str.length() - 1;

	while (start < end && isWhiteSpace(str[start]))
		start++;

	while (start <= end && isWhiteSpace(str[end]))
		end--;

	string returnStr(str, start, end + 1 - start);
	return returnStr;
}

/**
 * Copy tempStacks for the copy constructor.
 * The value of the vectors must also be copied.
 */
vector<vector<const string*>*>* ASBeautifier::copyTempStacks(const ASBeautifier &other) const
{
	vector<vector<const string*>*> *tempStacksNew = new vector<vector<const string*>*>;
	vector<vector<const string*>*>::iterator iter;
	for (iter = other.tempStacks->begin();
	        iter != other.tempStacks->end();
	        ++iter)
	{
		vector<const string*> *newVec = new vector<const string*>;
		*newVec = **iter;
		tempStacksNew->push_back(newVec);
	}
	return tempStacksNew;
}

/**
 * delete a static member vector to eliminate memory leak reporting for the vector
 */
void ASBeautifier::deleteStaticVectors()
{
	beautifierFileType = 9;		// reset to an invalid type
	deleteVector(headers);
	deleteVector(nonParenHeaders);
	deleteVector(preBlockStatements);
	deleteVector(assignmentOperators);
	deleteVector(nonAssignmentOperators);
	deleteVector(indentableHeaders);
}

/**
 * delete a vector object
 * T is the type of vector
 * used for all vectors except tempStacks
 */
template<typename T>
void ASBeautifier::deleteContainer(T &container)
{
	if (container != NULL)
	{
		container->clear();
		delete (container);
		container = NULL;
	}
}

/**
 * Delete the tempStacks vector object.
 * The tempStacks is a vector of pointers to strings allocated with
 * the 'new' operator.
 * Therefore the strings have to be deleted in addition to the
 * tempStacks entries.
 */
void ASBeautifier::deleteContainer(vector<vector<const string*>*>* &container)
{
	if (container != NULL)
	{
		vector<vector<const string*>*>::iterator iter = container->begin();
		for (; iter != container->end(); iter++)
			delete *iter;
		container->clear();
		delete (container);
		container = NULL;
	}
}

/**
 * delete a vector<const string*>* object
 */
void ASBeautifier::deleteVector(vector<const string*>*& container)
{
	assert(container != NULL);
	delete container;
	container = NULL;
}

/**
 * initialize a vector object
 * T is the type of vector
 * used for all vectors
 */
template<typename T>
void ASBeautifier::initContainer(T &container, T value)
{
	// since the ASFormatter object is never deleted,
	// the existing vectors must be deleted before creating new ones
	if (container != NULL )
		deleteContainer(container);
	container = value;
}

/**
 * initialize a vector<const string*>* object
 */
void ASBeautifier::initVector(vector<const string*>*& container)
{
	assert(container == NULL);
	container = new vector<const string*>;
}

/**
 * Determine if an assignment statement ends with a comma
 *     that is not in a function argument. It ends with a
 *     comma if a comma is the last char on the line.
 *
 * @return  true if line ends with a comma, otherwise false.
 */
bool ASBeautifier::statementEndsWithComma(string &line, int index)
{
	assert(line[index] == '=');

	bool isInComment = false;
	bool isInQuote = false;
	int parenCount = 0;
	size_t lineLength = line.length();
	size_t i = 0;
	char quoteChar = ' ';

	for (i = index + 1; i < lineLength; ++i)
	{
		char ch = line[i];

		if (isInComment)
		{
			if (line.compare(i, 2, "*/") == 0)
			{
				isInComment = false;
				++i;
			}
			continue;
		}

		if (ch == '\\')
		{
			++i;
			continue;
		}

		if (isInQuote)
		{
			if (ch == quoteChar)
				isInQuote = false;
			continue;
		}

		if (ch == '"' || ch == '\'')
		{
			isInQuote = true;
			quoteChar = ch;
			continue;
		}

		if (line.compare(i, 2, "//") == 0)
			break;

		if (line.compare(i, 2, "/*") == 0)
		{
			if (isLineEndComment(line, i))
				break;
			else
			{
				isInComment = true;
				++i;
				continue;
			}
		}

		if (ch == '(')
			parenCount++;
		if (ch == ')')
			parenCount--;
	}
	if (isInComment
	        || isInQuote
	        || parenCount > 0)
		return false;

	size_t lastChar = line.find_last_not_of(" \t", i - 1);

	if (lastChar == string::npos || line[lastChar] != ',')
		return false;

	return true;
}

/**
 * check if current comment is a line-end comment
 *
 * @return     is before a line-end comment.
 */
bool ASBeautifier::isLineEndComment(string& line, int startPos) const
{
	assert(line.compare(startPos, 2, "/*") == 0);

	// comment must be closed on this line with nothing after it
	size_t endNum = line.find("*/", startPos + 2);
	if (endNum != string::npos)
	{
		size_t nextChar = line.find_first_not_of(" \t", endNum + 2);
		if (nextChar == string::npos)
			return true;
	}
	return false;
}

/**
 * get the previous word index for an assignment operator
 *
 * @return is the index to the previous word (the in statement indent).
 */
int ASBeautifier::getInStatementIndentAssign(const string& line, size_t currPos) const
{
	assert(line[currPos] == '=');

	if (currPos == 0)
		return 0;

	// get the last legal word (may be a number)
	size_t end = line.find_last_not_of(" \t", currPos-1);
	if (end == string::npos || !isLegalNameChar(line[end]))
		return 0;

	int start;          // start of the previous word
	for (start = end; start > -1; start--)
	{
		if (!isLegalNameChar(line[start]) || line[start] == '.')
			break;
	}
	start++;

	return start;
}

/**
 * get the instatement indent for a comma
 *
 * @return is the indent to the second word on the line (the in statement indent).
 */
int ASBeautifier::getInStatementIndentComma(const string& line, size_t currPos) const
{
	assert(line[currPos] == ',');

	if (currPos == 0)
		return 0;

	// get first word on a line
	size_t indent = line.find_first_not_of(" \t");
	if (indent == string::npos || !isLegalNameChar(line[indent]))
		return 0;

	// bypass first word
	for (; indent < currPos; indent++)
	{
		if (!isLegalNameChar(line[indent]))
			break;
	}
	indent++;
	if (indent >= currPos)
		return 0;

	// point to second word or assignment operator
	indent = line.find_last_not_of(" \t", indent);
	if (indent == string::npos || indent >= currPos)
		return 0;

	return indent;
}


}   // end namespace astyle

