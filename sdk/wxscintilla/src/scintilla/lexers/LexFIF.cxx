// Scintilla source code edit control
/** @file LexFIF.cxx
 ** Lexers for Find In Files output format
 **/
// Copyright 1998-2001 by Eran Ifrah <eran.ifrah@gmail.com>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "ILexer.h"
#include "Platform.h"
#include "LexAccessor.h"
#include "LexerModule.h"
#include "Accessor.h"
#include "Scintilla.h"
#include "SciLexer.h"
#include <string>

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

static inline bool AtEOL(Accessor &styler, unsigned int i)
{
	return (styler[i] == '\n') ||
	       ((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n'));
}

static void ColouriseFifDoc(unsigned int pos, int length, int /*initStyle*/,
                            WordList *[], Accessor &styler)
{
	enum {
		Start,
		StateLineNum,
		StateScope,
		StateMatchString,
		StateFile,
		StateComment,
		StateHeader
	};
	
	int state = Start;
	styler.StartAt(pos);
	styler.StartSegment(pos);
	int lineFirstChar(-1);
	int lineNumCount(0);
	for (int firstchar = -1; length > 0; pos++, length--) {
        if (firstchar == -1) {
            firstchar = styler[pos]; // first char of each line
        }
		switch(state) {
		case Start:
			if(firstchar == ' ') {
				state = StateLineNum;
				
			} else if(firstchar == '=') {
				state = StateHeader;
				
			} else {
				state = StateFile;
			}
			break;
		case StateLineNum:
			if(lineNumCount == 6) {
				if(styler[pos] == '/') {
					// colour the entire line with the comment style
					styler.ColourTo(pos-1, SCLEX_FIF_LINE_NUMBER);
					state = StateComment;
					
				} else if(styler[pos] == '[') {
					// dont include the '['
					styler.ColourTo(pos-1, SCLEX_FIF_LINE_NUMBER);
					state = StateScope;
				} else {
					styler.ColourTo(pos-1, SCLEX_FIF_LINE_NUMBER);
					state = StateMatchString;
				}
			} else {
				lineNumCount++;
			}
			break;
		case StateScope:
			if(styler[pos] == ']') {
				styler.ColourTo(pos, SCLEX_FIF_SCOPE);
				state = StateMatchString;
			}
			break;
		case StateMatchString:
			if(AtEOL(styler, pos)) {
				state = Start;
				styler.ColourTo(pos, SCLEX_FIF_MATCH);
				lineNumCount = 0;
				firstchar = -1;
			}
			break;
		case StateFile:
			if(AtEOL(styler, pos)) {
				state = Start;
				styler.ColourTo(pos, SCLEX_FIF_FILE);
				lineNumCount = 0;
				firstchar = -1;
			}
			break;
		case StateComment:
			if(AtEOL(styler, pos)) {
				state = Start;
				styler.ColourTo(pos, SCLEX_FIF_MATCH_COMMENT);
				lineNumCount = 0;
				firstchar = -1;
			}
			break;
		case StateHeader:
			if(AtEOL(styler, pos)) {
				state = Start;
				styler.ColourTo(pos, SCLEX_FIF_HEADER);
				lineNumCount = 0;
				firstchar = -1;
			}
			break;
		}
	}
}

static void FoldFifDoc(unsigned int pos, int length, int,
                       WordList*[], Accessor &styler)
{
	int curLine = styler.GetLine(pos);
	int prevLevel = curLine > 0 ? styler.LevelAt(curLine-1) : SC_FOLDLEVELBASE;

    unsigned int end = pos+length;
    pos = styler.LineStart(curLine);

	do {
        int nextLevel;
        switch (styler.StyleAt(pos)) {
            case SCLEX_FIF_DEFAULT:
                nextLevel = SC_FOLDLEVELBASE;
                break;
            case SCLEX_FIF_HEADER:
                nextLevel = (SC_FOLDLEVELBASE + 1) | SC_FOLDLEVELHEADERFLAG;
                break;
            case SCLEX_FIF_FILE:
                nextLevel = (SC_FOLDLEVELBASE + 2) | SC_FOLDLEVELHEADERFLAG;
                break;
            default:
                nextLevel = prevLevel & SC_FOLDLEVELHEADERFLAG ? (prevLevel & SC_FOLDLEVELNUMBERMASK) + 1 : prevLevel;
                break;
        }
		if ((nextLevel & SC_FOLDLEVELHEADERFLAG) && nextLevel == prevLevel) {
			styler.SetLevel(curLine-1, prevLevel & ~SC_FOLDLEVELHEADERFLAG);
        }
		styler.SetLevel(curLine, nextLevel);

        curLine++;
		prevLevel = nextLevel;
		pos = styler.LineStart(curLine);
	} while (pos < end);
}

static const char * const fifWordListDesc[] = {
	"Internal Commands",
	"External Commands",
	0
};

static const char * const emptyWordListDesc[] = {
	0
};

LexerModule lmFif(SCLEX_FIF, ColouriseFifDoc, "fif", FoldFifDoc, fifWordListDesc);
