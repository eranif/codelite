#ifndef __includepathlocator__
#define __includepathlocator__

#include <wx/arrstr.h>
#include "imanager.h"

class IncludePathLocator : public wxEvtHandler {
	IManager *m_mgr;
public:
	IncludePathLocator(IManager *mgr);
	~IncludePathLocator();

	void Locate(wxArrayString &paths, wxArrayString &excludePaths);
};
#endif // __includepathlocator__
