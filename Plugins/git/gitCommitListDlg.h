//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#include "DataViewTypeHelper.h"
#include "cl_command_event.h"
#include "gitui.hpp"
#include "macros.h"

#include <map>

class IProcess;
class GitPlugin;
class GitCommitListDlg : public GitCommitListDlgBase
{
public:
    GitCommitListDlg(wxWindow* parent, const wxString& workingDir, GitPlugin* git);
    virtual ~GitCommitListDlg();

    void SetCommitList(const wxString& commits);
    void Display();

protected:
    void OnClose(wxCloseEvent& event) override;
    void OnContextMenu(wxDataViewEvent& event) override;
    void OnSelectionChanged(wxDataViewEvent& event) override;
    void OnRevertCommit(wxCommandEvent& e);
    void OnCopyCommitHashToClipboard(wxCommandEvent& e);
    void OnBtnClose(wxCommandEvent& event) override;
    void OnNextUpdateUI(wxUpdateUIEvent& event) override;
    void OnNext(wxCommandEvent& event) override;
    void OnPrevious(wxCommandEvent& event) override;
    void OnPreviousUI(wxUpdateUIEvent& event) override;
    void OnSearchCommitList(wxCommandEvent& event) override;
    void DoLoadCommits(const wxString& filter);
    void ClearAll(bool includingCommitlist = true);
    wxString GetFilterString() const;
    bool Show(bool show = true) override { return wxDialog::Show(show); }
    void DoShow() { wxDialog::Show(); }

private:
    void OnChangeFile(wxCommandEvent& e) override;
    // Event handlers
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void UpdateSelection(const wxDataViewItem& item = {});

    GitPlugin* m_git;
    wxStringMap_t m_diffMap;
    wxString m_workingDir;
    wxString m_commandOutput;
    IProcess* m_process;
    wxString m_commitList;
    wxString m_Filter;
    int m_skip;
    std::map<int, wxString> m_history;
    std::unique_ptr<DataViewTypeHelper> m_typeHelper;
};

#endif //__gitCommitListDlg__
