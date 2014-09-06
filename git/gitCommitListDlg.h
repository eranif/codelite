//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : gitCommitListDlg.h
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

#ifndef __gitCommitListDlg__
#define __gitCommitListDlg__

#include <map>
#include "gitui.h"
#include "macros.h"

class GitCommitEditor;
class IProcess;
class GitPlugin;
class GitCommitListDlg : public GitCommitListDlgBase
{
    GitPlugin* m_git;
    wxStringMap_t m_diffMap;
    wxString m_workingDir;
    wxString m_commandOutput;
    IProcess* m_process;
    wxString m_gitPath;

public:
    GitCommitListDlg(wxWindow* parent, const wxString& workingDir, GitPlugin* git);
    ~GitCommitListDlg();

    void SetCommitList(const wxString& commits);

private:
    void OnChangeFile(wxCommandEvent& e);
    DECLARE_EVENT_TABLE()

    // Event handlers
    void OnProcessTerminated(wxCommandEvent& event);
    void OnProcessOutput(wxCommandEvent& event);

protected:
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnContextMenu(wxDataViewEvent& event);
    virtual void OnSelectionChanged(wxDataViewEvent& event);
    void OnRevertCommit(wxCommandEvent& e);
    void OnCopyCommitHashToClipboard(wxCommandEvent& e);
};

#endif //__gitCommitListDlg__
