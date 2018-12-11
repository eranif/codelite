//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
#include "cl_command_event.h"
#include "macros.h"

class GitCommitEditor;
class IProcess;
class GitPlugin;

class GitDiffDlg : public GitDiffDlgBase
{
    wxStringMap_t m_diffMap;
    wxString m_workingDir;
    wxString m_gitPath;
    wxString m_commits; // Empty unless the ChooseCommits dialog is used
    wxString m_commandOutput;
    GitPlugin* m_plugin;
    IProcess* m_process;

public:
    GitDiffDlg(wxWindow* parent, const wxString& workingDir, GitPlugin* plugin);
    ~GitDiffDlg();

private:
    void OnChangeFile(wxCommandEvent& e);

    DECLARE_EVENT_TABLE();

protected:
    virtual void OnOptionsChanged(wxCommandEvent& event);
    wxString PrepareCommand() const;
    void CreateDiff();
    void SetDiff(const wxString& diff);
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);

    virtual void OnChoseCommits(wxCommandEvent& event);
    virtual void OnClose(wxCommandEvent& event);
};

#endif //__gitDiffDlg__
