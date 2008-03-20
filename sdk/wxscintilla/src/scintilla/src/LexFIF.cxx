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

static inline bool AtEOL(Accessor &styler, unsigned int i)
{
	return (styler[i] == '\n') ||
	       ((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n'));
}

static void ColouriseFifDoc(
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
	int offset(0);
	
	for (unsigned int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
			// lineBuffer contains our line
			// End of line (or of line buffer) met, colourise it
			lineBuffer[linePos] = '\0';
			std::string str(lineBuffer);
			
			if(str.at(0) == '='){
				styler.ColourTo(i, SCLEX_FIF_DEFAULT);
			} else {
				size_t pos = str.find("(");
				if(pos != std::string::npos) {
					styler.ColourTo(startLine + pos - 1, SCLEX_FIF_FILE);
				}
			}
			
			linePos = 0;
			startLine = i + 1;
		}
	}
}

static const char * const fifWordListDesc[] = {
	"Internal Commands",
	"External Commands",
	0
};

static const char * const emptyWordListDesc[] = {
	0
};

LexerModule lmFif(SCLEX_FIF, ColouriseFifDoc, "fif", 0, fifWordListDesc);
