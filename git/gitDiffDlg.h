//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : gitDiffDlg.h
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

#ifndef __gitDiffDlg__
#define __gitDiffDlg__

#include <map>
#include "gitui.h"

class GitCommitEditor;
class IProcess;

class GitDiffDlg : public GitDiffDlgBase
{
    std::map<wxString, wxString> m_diffMap;
    wxString m_workingDir;
    wxString m_gitPath;

public:
    GitDiffDlg(wxWindow* parent, const wxString& workingDir);
    ~GitDiffDlg();

    void SetDiff(const wxString& diff);

private:
    void OnChangeFile(wxCommandEvent& e);

    DECLARE_EVENT_TABLE();
};

#endif //__gitDiffDlg__
