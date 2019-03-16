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

#include "ColoursAndFontsManager.h"
#include "clSingleChoiceDialog.h"
#include "editor_config.h"
#include "git.h"
#include "gitCommitDlg.h"
#include "gitCommitEditor.h"
#include "gitentry.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "windowattrmanager.h"
#include <wx/tokenzr.h>
#include "GitDiffOutputParser.h"

GitCommitDlg::GitCommitDlg(wxWindow* parent, GitPlugin* plugin, const wxString& workingDir)
    : GitCommitDlgBase(parent)
    , m_plugin(plugin)
    , m_workingDir(workingDir)
    , m_toggleChecks(false)
{
    m_dvListCtrlFiles->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    // read the configuration
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_splitterInner->CallAfter(&wxSplitterWindow::SetSashPosition, data.GetGitCommitDlgHSashPos(), true);
    m_splitterMain->CallAfter(&wxSplitterWindow::SetSashPosition, data.GetGitCommitDlgVSashPos(), true);

    LexerConf::Ptr_t diffLexer = ColoursAndFontsManager::Get().GetLexer("diff");
    if(diffLexer) { diffLexer->Apply(m_stcDiff); }

    m_toolbar->AddTool(XRCID("ID_CHECKALL"), _("Toggle files"), clGetManager()->GetStdIcons()->LoadBitmap("check-all"));
    m_toolbar->AddTool(XRCID("ID_HISTORY"), _("Show commit history"),
                       clGetManager()->GetStdIcons()->LoadBitmap("history"));
    m_toolbar->Realize();
    LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
    lex->Apply(m_stcCommitMessage);
    m_toolbar->Bind(wxEVT_TOOL, &GitCommitDlg::OnToggleCheckAll, this, XRCID("ID_CHECKALL"));
    m_toolbar->Bind(wxEVT_TOOL, &GitCommitDlg::OnCommitHistory, this, XRCID("ID_HISTORY"));
    m_editEventsHandlerCommitStc.Reset(new clEditEventsHandler(m_stcCommitMessage));
    m_editEventsHandlerDiffStc.Reset(new clEditEventsHandler(m_stcDiff));
    ::clSetTLWindowBestSizeAndPosition(this);
}

/*******************************************************************************/
GitCommitDlg::~GitCommitDlg()
{
    // read the configuration
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    data.SetGitCommitDlgHSashPos(m_splitterInner->GetSashPosition());
    data.SetGitCommitDlgVSashPos(m_splitterMain->GetSashPosition());
    conf.WriteItem(&data);
}

/*******************************************************************************/
void GitCommitDlg::AppendDiff(const wxString& diff)
{
    GitDiffOutputParser diff_parser;
    diff_parser.GetDiffMap(diff, m_diffMap);
    m_dvListCtrlFiles->DeleteAllItems();
    wxVector<wxVariant> cols;
    BitmapLoader* bitmaps = clGetManager()->GetStdIcons();
    for(const wxStringMap_t::value_type& vt : m_diffMap) {
        cols.clear();
        cols.push_back(::MakeCheckboxVariant(vt.first, true, bitmaps->GetMimeImageId(vt.first)));
        m_dvListCtrlFiles->AppendItem(cols);
    }

    if(!m_diffMap.empty()) {
        m_dvListCtrlFiles->Select(m_dvListCtrlFiles->RowToItem(0));
        wxStringMap_t::iterator it = m_diffMap.begin();
        m_stcDiff->SetText((*it).second);
        m_stcDiff->SetReadOnly(true);
    }
}

/*******************************************************************************/
wxArrayString GitCommitDlg::GetSelectedFiles()
{
    wxArrayString ret;
    for(size_t i = 0; i < m_dvListCtrlFiles->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrlFiles->RowToItem(i);
        if(m_dvListCtrlFiles->IsItemChecked(item, 0)) { ret.Add(m_dvListCtrlFiles->GetItemText(item, 0)); }
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
void GitCommitDlg::OnChangeFile(wxDataViewEvent& e)
{
    wxString file = m_dvListCtrlFiles->GetItemText(e.GetItem(), 0);
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
    for(size_t i = 0; i < m_dvListCtrlFiles->GetItemCount(); ++i) {
        m_dvListCtrlFiles->SetItemChecked(m_dvListCtrlFiles->RowToItem(i), m_toggleChecks, 0);
    }
    m_toggleChecks = !m_toggleChecks;
}

void GitCommitDlg::OnCommitHistory(wxCommandEvent& event)
{
    clSingleChoiceDialog dlg(this, m_history);
    dlg.SetLabel(_("Choose a commit"));
    if(dlg.ShowModal() != wxID_OK) return;

    wxString commitHash = dlg.GetSelection().BeforeFirst(' ');
    if(!commitHash.empty()) {
        wxString selectedCommit;
        m_plugin->DoExecuteCommandSync("log -1 --pretty=format:\"%B\" " + commitHash, m_workingDir, selectedCommit);
        if(!selectedCommit.empty()) { m_stcCommitMessage->SetText(selectedCommit); }
    }
}

void GitCommitDlg::OnCommitHistoryUI(wxUpdateUIEvent& event) { event.Enable(!m_history.IsEmpty()); }

void GitCommitDlg::OnAmendClicked(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        if(!m_previousCommitMessage.empty()) {
            m_stashedMessage = m_stcCommitMessage->GetText();
            m_stcCommitMessage->SetText(m_previousCommitMessage);
        }
    } else {
        if(!m_stashedMessage.empty()) { m_stcCommitMessage->SetText(m_stashedMessage); }
    }
}
