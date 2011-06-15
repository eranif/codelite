#ifndef __includepathlocator__
#define __includepathlocator__

#include <wx/arrstr.h>
#include "imanager.h"

class WXDLLIMPEXP_SDK IncludePathLocator : public wxEvtHandler {
	IManager *m_mgr;
public:
	IncludePathLocator(IManager *mgr);
	~IncludePathLocator();

	void Locate(wxArrayString &paths, wxArrayString &excludePaths, bool thirdPartyLibs = true, const wxString &tool = wxString());
};
#endif // __includepathlocator__
