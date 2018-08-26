//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gitDiffDlg.cpp
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

#include "gitDiffDlg.h"
#include "gitentry.h"
#include "editor_config.h"
#include "windowattrmanager.h"

#include <wx/tokenzr.h>
#include "gitCommitEditor.h"
#include "asyncprocess.h"
#include "processreaderthread.h"
#include "cl_config.h"
#include "gitdiffchoosecommitishdlg.h"
#include "git.h"

BEGIN_EVENT_TABLE(GitDiffDlg, wxDialog)

END_EVENT_TABLE()

GitDiffDlg::GitDiffDlg(wxWindow* parent, const wxString& workingDir, GitPlugin* plugin)
    : GitDiffDlgBase(parent)
    , m_workingDir(workingDir)
    , m_plugin(plugin)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_gitPath = data.GetGITExecutablePath();
    
    SetName("GitDiffDlg");
    WindowAttrManager::Load(this);
    m_splitter->SetSashPosition(data.GetGitDiffDlgSashPos());

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &GitDiffDlg::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &GitDiffDlg::OnProcessTerminated, this);

    CreateDiff();
}

/*******************************************************************************/
GitDiffDlg::~GitDiffDlg()
{
    
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    data.SetGitDiffDlgSashPos(m_splitter->GetSashPosition());
    conf.WriteItem(&data);
}

/*******************************************************************************/

void GitDiffDlg::CreateDiff()
{
    m_commandOutput.Empty(); // There might be stale contents from a previous run

    wxString command = PrepareCommand();
    m_plugin->DisplayMessage("GitDiff: " + command);
    m_process = CreateAsyncProcess(this, command, IProcessCreateDefault, m_plugin->GetRepositoryDirectory());
}

wxString GitDiffDlg::PrepareCommand() const
{
    wxString commitsString = m_commits;
    if (commitsString.empty()) {
        // Standard diff of changes against HEAD, but which sort?
        switch(m_radioBoxStaged->GetSelection()) {
            case 0: commitsString = ""; break; // Unstaged only
            case 1: commitsString = "--cached "; break; // Staged only
             default: commitsString = "HEAD "; // Both
        }
    }

    wxString command(" --no-pager diff ");
    if (m_checkIgnoreSpace->GetValue()) {
        command << "--ignore-all-space "; // -w
    }

    return m_gitPath + command + commitsString;
}

void GitDiffDlg::SetDiff(const wxString& diff)
{
    wxString commandOutput = diff;
    m_fileListBox->Clear();
    m_diffMap.clear();
    commandOutput.Replace(wxT("\r"), wxT(""));
    wxArrayString diffList = wxStringTokenize(commandOutput, wxT("\n"));

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
        } else if(foundFirstDiff) {
            m_diffMap[currentFile].Append(line + wxT("\n"));
        }
        ++index;
    }
    for(std::map<wxString, wxString>::iterator it = m_diffMap.begin(); it != m_diffMap.end(); ++it) {
        m_fileListBox->Append((*it).first);
    }
    m_editor->SetReadOnly(false);
    m_editor->SetText(wxT(""));

    if(m_diffMap.size() != 0) {
        std::map<wxString, wxString>::iterator it = m_diffMap.begin();
        m_editor->SetText((*it).second);
        m_fileListBox->Select(0);
        m_editor->SetReadOnly(true);
    }
}

/*******************************************************************************/
void GitDiffDlg::OnChangeFile(wxCommandEvent& e)
{
    int sel = m_fileListBox->GetSelection();
    wxString file = m_fileListBox->GetString(sel);
    m_editor->SetReadOnly(false);
    m_editor->SetText(m_diffMap[file]);
    m_editor->SetReadOnly(true);
}

void GitDiffDlg::OnChoseCommits(wxCommandEvent& event)
{
    GitDiffChooseCommitishDlg dlg(this, m_plugin);
    if (dlg.ShowModal() == wxID_OK) {
        wxString commit1 = dlg.GetFirstCommit();
        wxString joiner = dlg.GetJoiner(); // May be ' ' or '...'
        wxString commit2 = dlg.GetSecondCommit();
        m_commits = commit1 + joiner + commit2;

        CreateDiff();
    }
}

void GitDiffDlg::OnProcessOutput(clProcessEvent& event)
{
    m_commandOutput.Append(event.GetOutput());
}

void GitDiffDlg::OnProcessTerminated(clProcessEvent& event)
{
    wxUnusedVar(event);
    wxDELETE(m_process);

    SetDiff(m_commandOutput);
}

void GitDiffDlg::OnOptionsChanged(wxCommandEvent& event)
{
    CreateDiff();
}

void GitDiffDlg::OnClose(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}
