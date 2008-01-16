#ifndef VARIABLELEXER_H_INCLUDED
#define VARIABLELEXER_H_INCLUDED

#include "wx/string.h"
#include "wx/arrstr.h"

#include <map>
//#include <pair>

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif // WXDLLIMPEXP_LE_SDK


class WXDLLIMPEXP_LE_SDK VariableLexer
{
	public:
		VariableLexer(wxString path);
		wxArrayString getResult();
		void DebugMessage(const wxString& msg);
	
	private:
		wxArrayString m_output;
		wxArrayString m_unmatched;
		wxArrayString m_error;
		std::map<wxString, wxString> m_tokens;
};

#endif // VARIABLELEXER_H_INCLUDED
