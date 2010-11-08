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
#include <string>
#include <wx/string.h>
#include "ILexer.h"
#include "Platform.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "Scintilla.h"
#include "SciLexer.h"
#include "StyleContext.h"
#include "LexerModule.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

#ifdef WXDLLIMPEXP_SCI
#undef WXDLLIMPEXP_SCI
#endif

#include <wx/string.h>
#ifdef WXMAKINGDLL_SCI
	#define WXDLLIMPEXP_SCI WXEXPORT
#elif defined(WXUSINGDLL_SCI)
	#define WXDLLIMPEXP_SCI WXIMPORT
#else // not making nor using DLL
	#define WXDLLIMPEXP_SCI
#endif

typedef int (*CPPCHECK_COLOUR_FUNC_PTR)(int, const char*, size_t&, size_t&);

static CPPCHECK_COLOUR_FUNC_PTR s_cppcheckColourFunc = NULL;

WXDLLIMPEXP_SCI void SetCppCheckColourFunction(CPPCHECK_COLOUR_FUNC_PTR func)
{
	s_cppcheckColourFunc = func;
}

static inline bool AtEOL(Accessor &styler, unsigned int i)
{
	return (styler[i] == '\n') ||
	       ((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n'));
}

static void ColouriseCppCheckDoc(
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
            int style = SCLEX_GCC_OUTPUT;
            if (s_cppcheckColourFunc) {
                size_t start(0);
                size_t len(0);
                int startLine = pos-line.size()+1;
                style = s_cppcheckColourFunc(startLine, line.c_str(), start, len);
                if(len != 0) {
                    styler.ColourTo(startLine + start - 1, style);
                    styler.ColourTo(startLine + start + len - 1, SCLEX_GCC_FILE_LINK);
                }
            }
            styler.ColourTo(pos, style);
            line.clear();
		}
	}
}

static void FoldCppCheckDoc(unsigned int pos, int length, int,
                       WordList*[], Accessor &styler)
{
	int curLine = styler.GetLine(pos);
	int prevLevel = curLine > 0 ? styler.LevelAt(curLine-1) : SC_FOLDLEVELBASE;

    unsigned int end = pos+length;
    pos = styler.LineStart(curLine);

	do {
        int nextLevel;
        switch (styler.StyleAt(pos)) {
            case SCLEX_GCC_BUILDING:
                nextLevel = SC_FOLDLEVELBASE | SC_FOLDLEVELHEADERFLAG;
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

static const char * const cppcheckWordListDesc[] = {
	"Internal Commands",
	"External Commands",
	0
};

static const char * const emptyWordListDesc[] = {
	0
};

LexerModule lmCppCheck(SCLEX_CPPCHECK, ColouriseCppCheckDoc, "cppcheck", FoldCppCheckDoc, cppcheckWordListDesc);
