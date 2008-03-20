// Scintilla source code edit control
/** @file LexOthers.cxx
 ** Lexers for batch files, diff results, properties files, make files and error lists.
 ** Also lexer for LaTeX documents.
 **/
// Copyright 1998-2001 by Eran Ifrah <eran.ifrah@gmail.com>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include <wx/string.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

extern int ColourGccLine(int startLine, const char *line, size_t &fileNameStart, size_t &fileNameLen);

static inline bool AtEOL(Accessor &styler, unsigned int i)
{
	return (styler[i] == '\n') ||
	       ((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n'));
}

static void ColouriseGccDoc(
    unsigned int startPos,
    int length,
    int /*initStyle*/,
    WordList *keywordlists[],  
    Accessor &styler)
{
	char lineBuffer[2048];

	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	unsigned int startLine = startPos;
	for (unsigned int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
			// lineBuffer contains our line
			// End of line (or of line buffer) met, colourise it
			lineBuffer[linePos] = '\0';
			
			size_t start(0);
			size_t len(0);
			int style = ColourGccLine(startLine, lineBuffer, start, len);
			
			
			if(len != 0) {
				styler.ColourTo(startLine + start - 1, style);
				styler.ColourTo(startLine + start + len - 1, SCLEX_GCC_FILE_LINK);
				styler.ColourTo(i, style);
			} else {
				styler.ColourTo(i, style);
			}
			
			linePos = 0;
			startLine = i + 1;
		}
	}
}

static const char * const gccWordListDesc[] = {
	"Internal Commands",
	"External Commands",
	0
};

static const char * const emptyWordListDesc[] = {
	0
};

LexerModule lmGcc(SCLEX_GCC, ColouriseGccDoc, "gcc", 0, gccWordListDesc);
