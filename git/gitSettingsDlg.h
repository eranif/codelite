//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : gitSettingsDlg.h
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitSettingsDlg__
#define __gitSettingsDlg__

#include "gitui.h"
#include "git.h"

class GitSettingsDlg : public GitSettingsDlgBase
{
    wxString m_localRepoPath;
public:
    GitSettingsDlg(wxWindow* parent, const wxString& localRepoPath);
    virtual ~GitSettingsDlg();

protected:
    virtual void OnLocalRepoUI(wxUpdateUIEvent& event);
    virtual void OnOK(wxCommandEvent& event);
};

#endif //__gitSettingsDlg__
