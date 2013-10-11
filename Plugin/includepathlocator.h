//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : includepathlocator.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
