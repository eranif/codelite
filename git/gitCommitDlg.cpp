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

#include "ColoursAndFontsManager.h"
#include "GitDiffOutputParser.h"
#include "ai/LLMManager.hpp"
#include "clSingleChoiceDialog.h"
#include "git.h"
#include "gitentry.h"
#include "globals.h"
#include "lexer_configuration.h"

#include <wx/tokenzr.h>

namespace
{
wxString GetTempCommitFile()
{
    return wxFileName{clStandardPaths::Get().GetTempDir(), "commit-message.tmp"}.GetFullPath();
}
} // namespace

GitCommitDlg::GitCommitDlg(wxWindow* parent, GitPlugin* plugin, const wxString& workingDir)
    : GitCommitDlgBase(parent)
    , m_plugin(plugin)
    , m_workingDir(workingDir)
    , m_toggleChecks(false)
{
    m_dvListCtrlFiles->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());

    // Load persistent settings
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_splitterInner->CallAfter(&wxSplitterWindow::SetSashPosition, data.GetGitCommitDlgHSashPos(), true);
    m_splitterMain->CallAfter(&wxSplitterWindow::SetSashPosition, data.GetGitCommitDlgVSashPos(), true);
    m_checkBoxSignedOff->SetValue(data.GetFlags() & GitEntry::CheckSignedOffBy);

    LexerConf::Ptr_t diffLexer = ColoursAndFontsManager::Get().GetLexer("diff");
    if (diffLexer) {
        diffLexer->Apply(m_stcDiff);
    }

    LexerConf::Ptr_t markdown_lexer = ColoursAndFontsManager::Get().GetLexer("markdown");
    if (markdown_lexer) {
        markdown_lexer->Apply(m_stcCommitMessage);
    }

    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(XRCID("ID_CHECKALL"), _("Toggle files"), images->Add("check-all"));
    m_toolbar->AddTool(XRCID("ID_HISTORY"), _("Show commit history"), images->Add("history"));
    m_toolbar->AddTool(XRCID("ID_GENERATE"), _("Generate commit message"), images->Add("wand"));

    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &GitCommitDlg::OnToggleCheckAll, this, XRCID("ID_CHECKALL"));
    m_toolbar->Bind(wxEVT_TOOL, &GitCommitDlg::OnCommitHistory, this, XRCID("ID_HISTORY"));
    m_toolbar->Bind(wxEVT_TOOL, &GitCommitDlg::OnGenerate, this, XRCID("ID_GENERATE"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitCommitDlg::OnGenerateUI, this, XRCID("ID_GENERATE"));
    ::clSetTLWindowBestSizeAndPosition(this);
    CentreOnParent();

    wxFileName temp_commit = GetTempCommitFile();
    if (temp_commit.FileExists()) {
        wxString content;
        FileUtils::ReadFileContent(temp_commit, content);
        m_stcCommitMessage->SetText(content);
    }

    m_indicatorPanel = new IndicatorPanel(this);
    m_mainSizer->Add(m_indicatorPanel, wxSizerFlags(0).Expand());
    m_indicatorPanel->Stop(wxEmptyString);

    // set the focus to the text control
    m_stcCommitMessage->CallAfter(&wxStyledTextCtrl::SetFocus);
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
    data.EnableFlag(GitEntry::CheckSignedOffBy, m_checkBoxSignedOff->IsChecked());
    conf.WriteItem(&data);

    // if the dialog was dismissed with "OK", remove the commit file
    if (m_dismissedWithOk) {
        FileUtils::RemoveFile(GetTempCommitFile());
    } else if (!m_stcCommitMessage->IsEmpty()) {
        // otherwise, write the content to the file, we will load it later
        FileUtils::WriteFileContent(GetTempCommitFile(), m_stcCommitMessage->GetText());
    }
}

/*******************************************************************************/
void GitCommitDlg::AppendDiff(const wxString& diff)
{
    m_rawDiff = diff;
    GitDiffOutputParser diff_parser;
    diff_parser.GetDiffMap(diff, m_diffMap);
    m_dvListCtrlFiles->DeleteAllItems();
    wxVector<wxVariant> cols;
    BitmapLoader* bitmaps = clGetManager()->GetStdIcons();
    std::vector<wxString> names;
    names.reserve(m_diffMap.size());
    for (const wxStringMap_t::value_type& vt : m_diffMap) {
        names.push_back(vt.first);
    }

    std::sort(names.begin(), names.end(), [](const wxString& a, const wxString& b) { return a.CmpNoCase(b) < 0; });

    for (const wxString& filename : names) {
        cols.clear();
        cols.push_back(::MakeCheckboxVariant(filename, true, bitmaps->GetMimeImageId(filename)));
        m_dvListCtrlFiles->AppendItem(cols);
    }

    if (!names.empty()) {
        // Set a selection
        CallAfter([this]() {
            m_dvListCtrlFiles->Select(m_dvListCtrlFiles->RowToItem(0));
            wxStringMap_t::iterator it = m_diffMap.begin();
            m_stcDiff->SetText((*it).second);
            m_stcDiff->SetReadOnly(true);
        });
    }
}

/*******************************************************************************/
wxArrayString GitCommitDlg::GetSelectedFiles()
{
    wxArrayString ret;
    for (size_t i = 0; i < m_dvListCtrlFiles->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrlFiles->RowToItem(i);
        if (m_dvListCtrlFiles->IsItemChecked(item, 0)) {
            ret.Add(m_dvListCtrlFiles->GetItemText(item, 0));
        }
    }
    return ret;
}

/*******************************************************************************/
wxString GitCommitDlg::GetCommitMessage()
{
    wxString msg = m_stcCommitMessage->GetText();
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
    if (m_stcCommitMessage->GetText().IsEmpty() && !IsAmending()) {
        ::wxMessageBox(_("Git requires a commit message"), "codelite", wxICON_WARNING | wxOK | wxCENTER);
        return;
    }
    m_dismissedWithOk = true;
    EndModal(wxID_OK);
}

/*******************************************************************************/
void GitCommitDlg::OnToggleCheckAll(wxCommandEvent& event)
{
    for (size_t i = 0; i < m_dvListCtrlFiles->GetItemCount(); ++i) {
        m_dvListCtrlFiles->SetItemChecked(m_dvListCtrlFiles->RowToItem(i), m_toggleChecks, 0);
    }
    m_toggleChecks = !m_toggleChecks;
}

void GitCommitDlg::OnCommitHistory(wxCommandEvent& event)
{
    clSingleChoiceDialog dlg(this, m_history);
    dlg.SetLabel(_("Choose a commit"));
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString commitHash = dlg.GetSelection().BeforeFirst(' ');
    if (!commitHash.empty()) {
        wxString selectedCommit;
        m_plugin->DoExecuteCommandSync("log -1 --pretty=format:\"%B\" " + commitHash, &selectedCommit);
        if (!selectedCommit.empty()) {
            m_stcCommitMessage->SetText(selectedCommit);
        }
    }
}

void GitCommitDlg::OnAmendClicked(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        if (!m_previousCommitMessage.empty()) {
            m_stashedMessage = m_stcCommitMessage->GetText();
            m_stcCommitMessage->SetText(m_previousCommitMessage);
        }
    } else {
        if (!m_stashedMessage.empty()) {
            m_stcCommitMessage->SetText(m_stashedMessage);
        }
    }
}

void GitCommitDlg::OnGenerate(wxCommandEvent& event)
{
    wxString prompt;
    prompt << "Given the below git diff, generate a commit message. Append a footer with the following text at the end "
              "of the generated commit message:"
           << "\"** Generated by ChatAI Plugin. ** \"."
           << "Use bullets.\n"
           << "Git diff:\n"
           << "```diff\n"
           << m_rawDiff << "\n"
           << "```\n";

    auto generate_id = m_plugin->GenerateCommitMessage(prompt);
    if (!generate_id.has_value()) {
        ::wxMessageBox(_("Failed to generate commit message"), "CodeLite", wxICON_WARNING | wxOK | wxCENTER);
        return;
    }

    m_indicatorPanel->Start(_("Generating commit message..."));
    m_generate_id = generate_id;
}

void GitCommitDlg::OnGenerateUI(wxUpdateUIEvent& event)
{
    bool is_generation_active = m_generate_id.has_value();
    event.Enable(llm::Manager::GetInstance().IsAvailable() && !is_generation_active && !m_rawDiff.empty());
}

void GitCommitDlg::ClearCommitMessage() { m_stcCommitMessage->ClearAll(); }

void GitCommitDlg::SetCommitMessageGenerationCompleted()
{
    m_generate_id.reset();
    m_indicatorPanel->Stop(wxEmptyString);
}

void GitCommitDlg::AppendCommitMessage(const wxString& message)
{
    // Update the commit message
    m_stcCommitMessage->AppendText(message);
}

void GitCommitDlg::SetIndicatorMessage(const wxString& message)
{
    if (m_indicatorPanel->IsRunning()) {
        m_indicatorPanel->SetMessage(message);
    } else {
        m_indicatorPanel->Start(message);
    }
}
