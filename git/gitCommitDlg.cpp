//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gitCommitDlg.cpp
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

#include "gitCommitDlg.h"
#include "gitCommitEditor.h"
#include "windowattrmanager.h"
#include <wx/tokenzr.h>
#include "gitentry.h"
#include "lexer_configuration.h"
#include "editor_config.h"
#include "ColoursAndFontsManager.h"
#include "globals.h"
#include "clSingleChoiceDialog.h"

GitCommitDlg::GitCommitDlg(wxWindow* parent)
    : GitCommitDlgBase(parent)
    , m_toggleChecks(false)
{
    // read the configuration
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    m_splitterInner->SetSashPosition(data.GetGitCommitDlgHSashPos());
    m_splitterMain->SetSashPosition(data.GetGitCommitDlgVSashPos());

    LexerConf::Ptr_t diffLexer = ColoursAndFontsManager::Get().GetLexer("diff");
    if(diffLexer) {
        diffLexer->Apply(m_stcDiff);
    }

    SetName("GitCommitDlg");
    WindowAttrManager::Load(this);
    LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
    lex->Apply(m_stcCommitMessage);
}

/*******************************************************************************/
GitCommitDlg::~GitCommitDlg()
{
    // read the configuration
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    wxString message = m_stcCommitMessage->GetText();
    data.AddRecentCommit(message);
    data.SetGitCommitDlgHSashPos(m_splitterInner->GetSashPosition());
    data.SetGitCommitDlgVSashPos(m_splitterMain->GetSashPosition());
    conf.WriteItem(&data);
}

/*******************************************************************************/
void GitCommitDlg::AppendDiff(const wxString& diff)
{
    wxArrayString diffList = wxStringTokenize(diff, wxT("\n"), wxTOKEN_STRTOK);
    unsigned index = 0;
    wxString currentFile;
    while(index < diffList.GetCount()) {
        wxString line = diffList[index];
        if(line.StartsWith(wxT("diff"))) {
            line.Replace(wxT("diff --git a/"), wxT(""));
            currentFile = line.Left(line.Find(wxT(" ")));

        } else if(line.StartsWith(wxT("Binary"))) {
            m_diffMap[currentFile] = wxT("Binary diff");

        } else {
            m_diffMap[currentFile].Append(line + wxT("\n"));
        }
        ++index;
    }
    index = 0;
    for(std::map<wxString, wxString>::iterator it = m_diffMap.begin(); it != m_diffMap.end(); ++it) {
        m_listBox->Append((*it).first);
        m_listBox->Check(index++, true);
    }

    if(m_diffMap.size() != 0) {
        std::map<wxString, wxString>::iterator it = m_diffMap.begin();
        m_stcDiff->SetText((*it).second);
        m_listBox->Select(0);
        m_stcDiff->SetReadOnly(true);
    }
}

/*******************************************************************************/
wxArrayString GitCommitDlg::GetSelectedFiles()
{
    wxArrayString ret;
    for(unsigned i = 0; i < m_listBox->GetCount(); ++i) {
        if(m_listBox->IsChecked(i)) ret.Add(m_listBox->GetString(i));
    }
    return ret;
}
/*******************************************************************************/
wxString GitCommitDlg::GetCommitMessage()
{
    wxString msg = m_stcCommitMessage->GetText();
    msg.Replace(wxT("\""), wxT("\\\""));
    return msg;
}
/*******************************************************************************/
void GitCommitDlg::OnChangeFile(wxCommandEvent& e)
{
    int sel = m_listBox->GetSelection();
    wxString file = m_listBox->GetString(sel);
    m_stcDiff->SetReadOnly(false);
    m_stcDiff->SetText(m_diffMap[file]);
    ::clRecalculateSTCHScrollBar(m_stcDiff);
    m_stcDiff->SetReadOnly(true);
}

void GitCommitDlg::OnCommitOK(wxCommandEvent& event)
{
    if(m_stcCommitMessage->GetText().IsEmpty() && !IsAmending()) {
        ::wxMessageBox(_("Git requires a commit message"), "codelite", wxICON_WARNING | wxOK | wxCENTER);
        return;
    }
    EndModal(wxID_OK);
}

/*******************************************************************************/
void GitCommitDlg::OnToggleCheckAll(wxCommandEvent& event)
{
    for(size_t i = 0; i < m_listBox->GetCount(); ++i) {
        m_listBox->Check(i, m_toggleChecks);
    }
    m_toggleChecks = !m_toggleChecks;
}

void GitCommitDlg::OnClearGitCommitHistory(wxCommandEvent& event)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    data.GetRecentCommit().Clear();
    conf.WriteItem(&data);
}

void GitCommitDlg::OnCommitHistory(wxCommandEvent& event)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    
    const wxArrayString& options = data.GetRecentCommit();
    
    clSingleChoiceDialog dlg(this, options);
    dlg.SetLabel(_("Choose a commit"));
    if(dlg.ShowModal() != wxID_OK) return;
    
    m_stcCommitMessage->SetText(dlg.GetSelection());
}

void GitCommitDlg::OnCommitHistoryUI(wxUpdateUIEvent& event)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    event.Enable(!data.GetRecentCommit().IsEmpty());
}
