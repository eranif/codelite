//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gitCommitListDlg.cpp
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

#include "gitCommitListDlg.h"
#include "gitentry.h"
#include "editor_config.h"
#include "windowattrmanager.h"

#include <wx/tokenzr.h>
#include "gitCommitEditor.h"
#include "asyncprocess.h"
#include "processreaderthread.h"
#include "cl_config.h"
#include "gitentry.h"
#include "editor_config.h"
#include "lexer_configuration.h"
#include "globals.h"
#include "git.h"

static int ID_COPY_COMMIT_HASH = wxNewId();
static int ID_REVERT_COMMIT = wxNewId();

GitCommitListDlg::GitCommitListDlg(wxWindow* parent, const wxString& workingDir, GitPlugin* git)
    : GitCommitListDlgBase(parent)
    , m_git(git)
    , m_workingDir(workingDir)
    , m_skip(100)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &GitCommitListDlg::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &GitCommitListDlg::OnProcessTerminated, this);

    LexerConf::Ptr_t lex = EditorConfigST::Get()->GetLexer("diff");
    if(lex) {
        lex->Apply(m_stcDiff, true);
    }

    LexerConf::Ptr_t textLex = EditorConfigST::Get()->GetLexer("text");
    textLex->Apply(m_stcCommitMessage, true);

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_gitPath = data.GetGITExecutablePath();
    m_gitPath.Trim().Trim(false);

    if(m_gitPath.IsEmpty()) {
        m_gitPath = "git";
    }
    SetName("GitCommitListDlg");
    WindowAttrManager::Load(this);

    m_dvListCtrlCommitList->Connect(ID_COPY_COMMIT_HASH, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(GitCommitListDlg::OnCopyCommitHashToClipboard), NULL, this);
    m_dvListCtrlCommitList->Connect(ID_REVERT_COMMIT, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(GitCommitListDlg::OnRevertCommit), NULL, this);
}

/*******************************************************************************/
GitCommitListDlg::~GitCommitListDlg() { m_git->m_commitListDlg = NULL; }

/*******************************************************************************/
void GitCommitListDlg::SetCommitList(const wxString& commits)
{
    m_commitList = commits;
    m_history.insert(std::make_pair(m_skip, m_commitList));
    // Load all commits, un-filtered
    DoLoadCommits("");
}

/*******************************************************************************/
void GitCommitListDlg::OnChangeFile(wxCommandEvent& e)
{
    int sel = m_fileListBox->GetSelection();
    wxString file = m_fileListBox->GetString(sel);
    m_stcDiff->SetReadOnly(false);
    m_stcDiff->SetText(m_diffMap[file]);
    m_stcDiff->SetReadOnly(true);
}

/*******************************************************************************/
void GitCommitListDlg::OnProcessTerminated(clProcessEvent& event)
{
    wxUnusedVar(event);
    wxDELETE(m_process);

    m_stcCommitMessage->SetEditable(true);
    m_stcDiff->SetEditable(true);
    m_stcCommitMessage->ClearAll();
    m_fileListBox->Clear();
    m_diffMap.clear();
    m_commandOutput.Replace(wxT("\r"), wxT(""));
    wxArrayString diffList = wxStringTokenize(m_commandOutput, wxT("\n"));

    bool foundFirstDiff = false;
    unsigned index = 0;
    wxString currentFile;
    while(index < diffList.GetCount()) {

        wxString line = diffList[index];
        if(line.StartsWith(wxT("diff"))) {
            line.Replace(wxT("diff --git a/"), wxT(""));
            currentFile = line.Left(line.Find(wxT(" ")));
            foundFirstDiff = true;

        } else if(line.StartsWith(wxT("Binary"))) {
            m_diffMap[currentFile] = wxT("Binary diff");

        } else if(!foundFirstDiff) {
            m_stcCommitMessage->AppendText(line + wxT("\n"));

        } else {
            m_diffMap[currentFile].Append(line + wxT("\n"));
        }
        ++index;
    }
    for(std::map<wxString, wxString>::iterator it = m_diffMap.begin(); it != m_diffMap.end(); ++it) {
        m_fileListBox->Append((*it).first);
    }

    m_stcDiff->ClearAll();

    if(m_diffMap.size() != 0) {
        std::map<wxString, wxString>::iterator it = m_diffMap.begin();
        m_stcDiff->SetText((*it).second);
        m_fileListBox->Select(0);
    }

    m_stcDiff->SetEditable(false);
    m_commandOutput.Clear();
    m_stcCommitMessage->SetEditable(false);
}
/*******************************************************************************/
void GitCommitListDlg::OnProcessOutput(clProcessEvent& event) { m_commandOutput.Append(event.GetOutput()); }

void GitCommitListDlg::OnSelectionChanged(wxDataViewEvent& event)
{
    wxVariant v;
    if(!event.GetItem().IsOk()) {
        return;
    }

    m_dvListCtrlCommitList->GetValue(v, m_dvListCtrlCommitList->ItemToRow(event.GetItem()), 0);
    wxString commitID = v.GetString();
    wxString command = wxString::Format(wxT("%s --no-pager show %s"), m_gitPath.c_str(), commitID.c_str());
    m_process = CreateAsyncProcess(this, command, IProcessCreateDefault, m_workingDir);
}

void GitCommitListDlg::OnContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(ID_COPY_COMMIT_HASH, _("Copy commit hash to clipboard"));
    menu.Append(ID_REVERT_COMMIT, _("Revert this commit"));
    m_dvListCtrlCommitList->PopupMenu(&menu);
}

void GitCommitListDlg::OnCopyCommitHashToClipboard(wxCommandEvent& e)
{
    wxDataViewItem sel = m_dvListCtrlCommitList->GetSelection();
    CHECK_ITEM_RET(sel);

    wxVariant v;
    m_dvListCtrlCommitList->GetValue(v, m_dvListCtrlCommitList->ItemToRow(sel), 0);
    wxString commitID = v.GetString();

    ::CopyToClipboard(commitID);
}

void GitCommitListDlg::OnRevertCommit(wxCommandEvent& e)
{
    wxDataViewItem sel = m_dvListCtrlCommitList->GetSelection();
    CHECK_ITEM_RET(sel);

    wxVariant v;
    m_dvListCtrlCommitList->GetValue(v, m_dvListCtrlCommitList->ItemToRow(sel), 0);
    wxString commitID = v.GetString();

    m_git->CallAfter(&GitPlugin::RevertCommit, commitID);
}

void GitCommitListDlg::OnClose(wxCloseEvent& event)
{
    event.Skip();
    Destroy();
}

void GitCommitListDlg::OnOK(wxCommandEvent& event) { Destroy(); }

void GitCommitListDlg::DoLoadCommits(const wxString& filter)
{
    m_stcDiff->SetEditable(true);
    m_stcCommitMessage->SetEditable(true);

    m_dvListCtrlCommitList->DeleteAllItems();
    m_stcCommitMessage->ClearAll();
    m_fileListBox->Clear();
    m_stcDiff->ClearAll();

    m_stcCommitMessage->SetEditable(false);
    m_stcDiff->SetEditable(false);

    // hash @ subject @ author-name @ date
    wxArrayString gitList = wxStringTokenize(m_commitList, wxT("\n"), wxTOKEN_STRTOK);
    wxArrayString filters = wxStringTokenize(filter, " ");
    for(unsigned i = 0; i < gitList.GetCount(); ++i) {
        wxArrayString gitCommit = ::wxStringTokenize(gitList[i], "@");
        if(gitCommit.GetCount() >= 4) {
            if(IsMatchFilter(filters, gitCommit)) {
                wxVector<wxVariant> cols;
                cols.push_back(gitCommit.Item(0));
                cols.push_back(gitCommit.Item(1));
                cols.push_back(gitCommit.Item(2));
                cols.push_back(gitCommit.Item(3));
                m_dvListCtrlCommitList->AppendItem(cols);
            }
        }
    }
}

void GitCommitListDlg::OnSearchCommitList(wxCommandEvent& event)
{
    // load commits with filter
    DoLoadCommits(m_searchCtrlFilter->GetValue());
}

bool GitCommitListDlg::IsMatchFilter(const wxArrayString& filters, const wxArrayString& columns)
{
    if(filters.IsEmpty()) return true;

    bool match = true;
    for(size_t i = 0; i < filters.GetCount() && match; ++i) {
        wxString filter = filters.Item(i).Lower();
        wxString col1, col2, col3, col4;
        col1 = columns.Item(0).Lower();
        col2 = columns.Item(1).Lower();
        col3 = columns.Item(2).Lower();
        col4 = columns.Item(3).Lower();
        match = (col1.Contains(filter) || col2.Contains(filter) || col3.Contains(filter) || col4.Contains(filter));
    }
    return match;
}
void GitCommitListDlg::OnNext(wxCommandEvent& event)
{
    m_skip += 100;
    // Check the cache first
    if(m_history.count(m_skip)) {
        SetCommitList(m_history.find(m_skip)->second);
    } else {
        m_git->FetchNextCommits(m_skip);
    }
}

void GitCommitListDlg::OnPrevious(wxCommandEvent& event)
{
    int skip = m_skip - 100;
    if(m_history.count(skip)) {
        m_skip -= 100;
        SetCommitList(m_history.find(m_skip)->second);
    }
}

void GitCommitListDlg::OnPreviousUI(wxUpdateUIEvent& event) { event.Enable(m_skip > 100); }
