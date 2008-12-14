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
#include "StyleContext.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

typedef int (*GCC_COLOUR_FUNC_PTR)(int, const char*, size_t&, size_t&);

static GCC_COLOUR_FUNC_PTR s_gccColourFunc = NULL;

void SetGccColourFunction(GCC_COLOUR_FUNC_PTR func){
	s_gccColourFunc = func;
}

int ColourGccLine(int startLine, const char *line, size_t &fileNameStart, size_t &fileNameLen)
{
	if(s_gccColourFunc) {
		return s_gccColourFunc(startLine, line, fileNameStart, fileNameLen);
	}else{
		return 0;
	}
}

static inline bool AtEOL(Accessor &styler, unsigned int i)
{
	return (styler[i] == '\n') ||
	       ((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n'));
}

static void ColouriseGccDoc(
    unsigned int pos,
    int length,
    int /*initStyle*/,
    WordList * /*keywordlists*/[],  
    Accessor &styler)
{
    std::string line;
    line.reserve(2048);

	styler.StartAt(pos);
	styler.StartSegment(pos);
	
	for (; length > 0; pos++, length--) {
        line += styler[pos];
		if (AtEOL(styler, pos)) {
			// End of line met, colourise it
			size_t start(0);
			size_t len(0);
            int startLine = pos-line.size()+1;
			int style = ColourGccLine(startLine, line.c_str(), start, len);
			if(len != 0) {
				styler.ColourTo(startLine + start - 1, style);
				styler.ColourTo(startLine + start + len - 1, SCLEX_GCC_FILE_LINK);
			}
            styler.ColourTo(pos, style);
            line.clear();
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
