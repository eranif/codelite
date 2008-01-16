#ifndef REGEX_PROCESSOR_H
#define REGEX_PROCESSOR_H

#include "wx/string.h"

class wxRegEx;

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif // WXDLLIMPEXP_LE_SDK

class WXDLLIMPEXP_LE_SDK RegexProcessor {
	wxRegEx *m_re;
public:
	RegexProcessor(const wxString &reStr);
	virtual ~RegexProcessor();
	bool GetGroup(const wxString &str, int grp, wxString &out);
};

#endif // REGEX_PROCESSOR_H
