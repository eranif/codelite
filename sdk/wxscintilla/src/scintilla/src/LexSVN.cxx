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
#include <string>

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

static void ColouriseSvnDoc(
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

			// Trim
			str.erase(0, str.find_first_not_of(" \t\r\n\v"));

			if (str.length() > 2) {
				if (str.at(0) == 'U' && str.at(1) == ' ') {
					styler.ColourTo(i, SCLEX_SVN_UPDATED);

				} else if (str.at(0) == '_' && str.at(1) == 'U' && str.at(1) == ' ') {
					styler.ColourTo(i, SCLEX_SVN_UPDATED);

				} else if (str.at(0) == 'U' && str.at(1) == 'U' && str.at(1) == ' ') {
					styler.ColourTo(i, SCLEX_SVN_UPDATED);

				} else if (str.at(0) == 'C' && str.at(1) == ' ') {
					styler.ColourTo(i, SCLEX_SVN_CONFLICT);

				} else if (str.at(0) == 'G' && str.at(1) == ' ') {
					styler.ColourTo(i, SCLEX_SVN_MERGED);

				} else if (str.at(0) == 'M' && str.at(1) == ' ') {
					styler.ColourTo(i, SCLEX_SVN_MERGED);

				} else if (str.at(0) == 'A' && str.at(1) == ' ') {
					styler.ColourTo(i, SCLEX_SVN_ADDED);

				} else if (str.at(0) == 'D' && str.at(1) == ' ') {
					styler.ColourTo(i, SCLEX_SVN_DELETED);

				} else if (str.at(0) == '=' || str.at(0) == '-') {
					styler.ColourTo(i, SCLEX_SVN_INFO);
				} else {
					styler.ColourTo(i, SCLEX_SVN_DEFAULT);
				}
			}else{
				styler.ColourTo(i, SCLEX_SVN_DEFAULT);
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

LexerModule lmSvn(SCLEX_SVN, ColouriseSvnDoc, "svn", 0, gccWordListDesc);
