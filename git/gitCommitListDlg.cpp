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

#include "editor_config.h"
#include "gitCommitListDlg.h"
#include "gitentry.h"
#include "windowattrmanager.h"

#include "asyncprocess.h"
#include "cl_config.h"
#include "editor_config.h"
#include "git.h"
#include "gitCommitEditor.h"
#include "gitentry.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "processreaderthread.h"
#include <wx/tokenzr.h>
#include "GitDiffOutputParser.h"

static int ID_COPY_COMMIT_HASH = wxNewId();
static int ID_REVERT_COMMIT = wxNewId();

GitCommitListDlg::GitCommitListDlg(wxWindow* parent, const wxString& workingDir, GitPlugin* git)
    : GitCommitListDlgBase(parent)
    , m_git(git)
    , m_workingDir(workingDir)
    , m_skip(0)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &GitCommitListDlg::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &GitCommitListDlg::OnProcessTerminated, this);

    LexerConf::Ptr_t lex = EditorConfigST::Get()->GetLexer("diff");
    if(lex) { lex->Apply(m_stcDiff, true); }

    LexerConf::Ptr_t textLex = EditorConfigST::Get()->GetLexer("text");
    textLex->Apply(m_stcCommitMessage, true);

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_gitPath = data.GetGITExecutablePath();
    m_gitPath.Trim().Trim(false);

    if(m_gitPath.IsEmpty()) { m_gitPath = "git"; }
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

    ClearAll(false);

    m_commandOutput.Replace(wxT("\r"), wxT(""));
    m_stcCommitMessage->SetEditable(true);
    m_stcDiff->SetEditable(true);

    GitDiffOutputParser diff_parser;
    diff_parser.GetDiffMap(m_commandOutput, m_diffMap);

    for(wxStringMap_t::iterator it = m_diffMap.begin(); it != m_diffMap.end(); ++it) {
        m_fileListBox->Append((*it).first);
    }
    if(m_diffMap.size() != 0) {
        wxStringMap_t::iterator it = m_diffMap.begin();
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
    if(!event.GetItem().IsOk()) { return; }

    wxString commitID = m_dvListCtrlCommitList->GetItemText(event.GetItem());
    wxString command =
        wxString::Format(wxT("%s --no-pager show --first-parent %s"), m_gitPath.c_str(), commitID.c_str());
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

    wxString commitID = m_dvListCtrlCommitList->GetItemText(sel);
    ::CopyToClipboard(commitID);
}

void GitCommitListDlg::OnRevertCommit(wxCommandEvent& e)
{
    wxDataViewItem sel = m_dvListCtrlCommitList->GetSelection();
    CHECK_ITEM_RET(sel);

    wxString commitID = m_dvListCtrlCommitList->GetItemText(sel);

    if(::wxMessageBox(_("Are you sure you want to revert commit #") + commitID, "CodeLite",
                      wxYES_NO | wxCANCEL | wxICON_QUESTION, this) != wxYES) {
        return;
    }
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
    ClearAll();

    // hash @ subject @ author-name @ date
    wxArrayString gitList = wxStringTokenize(m_commitList, wxT("\n"), wxTOKEN_STRTOK);
    wxArrayString filters = wxStringTokenize(filter, " ");
    for(unsigned i = 0; i < gitList.GetCount(); ++i) {
        wxArrayString gitCommit = ::wxStringTokenize(gitList[i], "@");
        if(gitCommit.GetCount() >= 4) {
            wxVector<wxVariant> cols;
            cols.push_back(gitCommit.Item(0));
            cols.push_back(gitCommit.Item(1));
            cols.push_back(gitCommit.Item(2));
            cols.push_back(gitCommit.Item(3));
            m_dvListCtrlCommitList->AppendItem(cols);
        }
    }
}

void GitCommitListDlg::ClearAll(bool includingCommitlist /*=true*/)
{
    m_stcCommitMessage->SetEditable(true);
    m_stcDiff->SetEditable(true);
    m_stcCommitMessage->ClearAll();
    m_fileListBox->Clear();
    if(includingCommitlist) { m_dvListCtrlCommitList->DeleteAllItems(); }
    m_diffMap.clear();
    m_stcDiff->ClearAll();
    m_stcCommitMessage->SetEditable(false);
    m_stcDiff->SetEditable(false);
}

void GitCommitListDlg::OnSearchCommitList(wxCommandEvent& event)
{
    wxString filter = GetFilterString();
    if(filter == m_Filter) {
        return; // No change
    }

    m_Filter = filter;

    if(m_Filter.empty()) {
        wxCommandEvent e; // A search-filter has been removed, so reload
        m_git->OnCommitList(e);
        return;
    }

    ClearAll();

    m_skip = 0;
    m_history.clear();
    m_commitList.clear();

    m_git->FetchNextCommits(m_skip, m_Filter);
}

wxString GitCommitListDlg::GetFilterString() const
{
    wxString args;
    wxString filter = m_searchCtrlFilter->GetValue();

    if(filter.empty() && m_comboExtraArgs->GetValue().empty()) { return args; }

    wxArrayString searchStrings = ::wxStringTokenize(filter, " ", wxTOKEN_STRTOK);
    for(size_t i = 0; i < searchStrings.size(); ++i) {
        // Pass each search string using its own --grep field
        args << " --grep=" << searchStrings.Item(i);
    }

    if(!searchStrings.IsEmpty()) {
        //  Limit the commits output to ones that match all given --grep
        args << " --all-match";
    }

    if(m_checkBoxIgnoreCase->IsChecked()) { args << " -i"; }

    if(!m_comboExtraArgs->GetValue().empty()) { args << ' ' << m_comboExtraArgs->GetValue(); }

    return args;
}

void GitCommitListDlg::OnNext(wxCommandEvent& event)
{
    m_skip += 100;
    // Check the cache first
    if(m_history.count(m_skip)) {
        SetCommitList(m_history.find(m_skip)->second);
    } else {
        m_git->FetchNextCommits(m_skip, m_Filter);
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

void GitCommitListDlg::OnExtraArgsTextEnter(wxCommandEvent& event)
{
    // Add any text to the combobox, uniqued
    wxString extraArgs = m_comboExtraArgs->GetValue();
    if(!extraArgs.empty()) {
        int pos = m_comboExtraArgs->FindString(extraArgs);
        if(pos != 0 && pos != wxNOT_FOUND) { // 0 == already the first item in the list
            m_comboExtraArgs->Delete(pos);
        }
        m_comboExtraArgs->Insert(extraArgs, 0);
    }

    OnSearchCommitList(event);
}

void GitCommitListDlg::OnPreviousUI(wxUpdateUIEvent& event) { event.Enable(m_skip >= 100); }

void GitCommitListDlg::OnNextUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlCommitList->GetItemCount() >= 100);
}
