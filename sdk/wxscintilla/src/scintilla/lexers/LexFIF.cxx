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
	styler.StartAt(pos);
	styler.StartSegment(pos);
	int lineFirstChar(-1);
	for (int firstchar = -1; length > 0; pos++, length--) {
        if (firstchar == -1) {
            firstchar = styler[pos]; // first char of each line
        }
		
		if(lineFirstChar == -1) {
			lineFirstChar = styler[pos];
		}
		
        if (styler[pos] == '|' && firstchar == ' ') {
            if (length > 1 && styler[pos+1] == ' ') {
                // include the following space
                pos++;
                length--;
            }
            styler.ColourTo(pos, SCLEX_FIF_FILE_SHORT);
            firstchar = '|'; 
            if (length > 1 && styler[pos+1] == '[') {
                firstchar = '[';
            }
			
		} else if (styler[pos] == '|' && firstchar == '.') {
			if (length > 1 && styler[pos+1] == ' ') {
				// include the following space
				pos++;
				length--;
			}
			styler.ColourTo(pos, SCLEX_FIF_FILE_SHORT);
			if (length > 1 && styler[pos+1] == '[') {
				firstchar = '[';
			}
		} else if (styler[pos] == ']' && firstchar == '['){
            if (length > 1 && styler[pos+1] == ' ') {
                // include the following space
                pos++;
                length--;
            }
			styler.ColourTo(pos, SCLEX_FIF_SCOPE);
            firstchar = ']'; // first ']' only
		} else if (AtEOL(styler, pos)) {
			if(lineFirstChar == '.') {
				styler.ColourTo(pos, SCLEX_FIF_MATCH_COMMENT);
			} else {
				switch (firstchar) {
					case ' ':
					case '|':
					case '[':
					case ']':
						styler.ColourTo(pos, SCLEX_FIF_MATCH);
						break;
					case '=':
						styler.ColourTo(pos, SCLEX_FIF_DEFAULT);
						break;
					case '-':
						styler.ColourTo(pos, SCLEX_FIF_PROJECT);
						break;
					default:
						styler.ColourTo(pos, SCLEX_FIF_FILE);
						break;
				}
				
			}
            firstchar     = -1;
			lineFirstChar = -1;
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
            case SCLEX_FIF_PROJECT:
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
