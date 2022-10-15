//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gitCommitDlg.h
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

#ifndef __gitCommitDlg__
#define __gitCommitDlg__

#include "clEditorEditEventsHandler.h"
#include "gitui.h"
#include "macros.h"

#include <map>
#include <wx/tokenzr.h>

class GitPlugin;

class GitCommitDlg : public GitCommitDlgBase
{
    GitPlugin* m_plugin;
    wxString m_workingDir;
    wxStringMap_t m_diffMap;
    bool m_toggleChecks;
    clEditEventsHandler::Ptr_t m_editEventsHandlerDiffStc;
    wxString m_previousCommitMessage;
    wxArrayString m_history;
    wxString m_stashedMessage;

public:
    GitCommitDlg(wxWindow* parent, GitPlugin* plugin, const wxString& workingDir);
    virtual ~GitCommitDlg();

    void AppendDiff(const wxString& diff);

    wxArrayString GetSelectedFiles();
    wxString GetCommitMessage();
    void SetPreviousCommitMessage(const wxString& previous) { m_previousCommitMessage = previous; }
    void SetHistory(const wxString& history) { m_history = wxStringTokenize(history, "\n"); }
    bool IsAmending() const { return m_checkBoxAmend->IsChecked(); }

private:
    void OnChangeFile(wxDataViewEvent& e);

protected:
    virtual void OnAmendClicked(wxCommandEvent& event);
    virtual void OnCommitHistory(wxCommandEvent& event);
    virtual void OnCommitHistoryUI(wxUpdateUIEvent& event);
    virtual void OnToggleCheckAll(wxCommandEvent& event);
    virtual void OnCommitOK(wxCommandEvent& event);
};

#endif
