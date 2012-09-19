#ifndef MARKUP_H__
#define MARKUP_H__

#ifndef WXDLLIMPEXP_SDK

#ifdef WXMAKINGDLL_SDK
#    define WXDLLIMPEXP_SDK __declspec(dllexport)
#elif defined(WXUSINGDLL_SDK)
#    define WXDLLIMPEXP_SDK __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_SDK
#endif

#endif

#include <wx/string.h>

#define BOLD_START      270
#define BOLD_END        271
#define HORIZONTAL_LINE 272
#define NEW_LINE        273
#define CODE_START      274
#define CODE_END        275
#define COLOR_START     276
#define COLOR_END       277

extern WXDLLIMPEXP_SDK bool setMarkupLexerInput(const wxString &in);
extern WXDLLIMPEXP_SDK int MarkupLex();
extern WXDLLIMPEXP_SDK wxString MarkupText();
extern WXDLLIMPEXP_SDK void MarkupClean();

#endif // MARKUP_H__
