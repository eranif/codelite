#ifndef TARGET_H_INCLUDED
#define TARGET_H_INCLUDED

#include "wx/string.h"
#include "wx/arrstr.h"

#include <vector>

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif // WXDLLIMPEXP_LE_SDK

class WXDLLIMPEXP_LE_SDK Target
{
	public:
		Target(wxString name, wxArrayString deps, wxArrayString actions);
		void stripWhitespace();
		void splitDepencencies();
		
		wxString getName();
		wxArrayString getDeps();
		wxArrayString getActions();
	
	private:
	
		wxString m_name;
		wxArrayString m_deps;
		wxArrayString m_actions;
};

typedef std::vector<Target> Targets;

#endif // TARGET_H_INCLUDED
