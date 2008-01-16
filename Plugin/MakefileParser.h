#ifndef MAKEFILEPARSER_H_INCLUDED
#define MAKEFILEPARSER_H_INCLUDED

#include "LineTypes.h"

#include "wx/string.h"
#include "wx/arrstr.h"

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif // WXDLLIMPEXP_LE_SDK

class WXDLLIMPEXP_LE_SDK MakefileParser
{
	public:
		MakefileParser(wxArrayString lines);
		TypedStrings getResult();
	
	private:
		void Lex();
		LINETYPES::LineType Deduct();
		
		wxArrayString m_lines;
		TypedStrings m_result;
		
		size_t m_current;
		bool m_continued;
};

#endif // MAKEFILEPARSER_H_INCLUDED
