//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : replaceinfilespanel.cpp
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
#include "replaceinfilespanel.h"

#include "ColoursAndFontsManager.h"
#include "clFileSystemEvent.h"
#include "clStrings.h"
#include "cl_command_event.h"
#include "cl_editor.h"
#include "codelite_events.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "frame.h"
#include "globals.h"
#include "macros.h"
#include "manager.h"

#include <set>
#include <vector>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/renderer.h>
#include <wx/xrc/xmlres.h>

ReplaceInFilesPanel::ReplaceInFilesPanel(wxWindow* parent, wxWindowID id, const wxString& name)
    : FindResultsTab(parent, id, name, true)
{
    wxBoxSizer* horzSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* unmark = new wxButton(this, wxID_ANY, _("&Unmark All"));
    horzSizer->Add(unmark, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
    unmark->Bind(wxEVT_BUTTON, &ReplaceInFilesPanel::OnUnmarkAll, this);
    unmark->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnUnmarkAllUI, this);

    wxButton* mark = new wxButton(this, wxID_ANY, _("Mark &All"));
    horzSizer->Add(mark, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
    mark->Bind(wxEVT_BUTTON, &ReplaceInFilesPanel::OnMarkAll, this);
    mark->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnMarkAllUI, this);

    m_replaceWith = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, {});
    m_replaceWith->SetHint(_("Replace with"));
    horzSizer->Add(m_replaceWith, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    m_replaceWith->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnReplaceWithComboUI, this);

    wxButton* button_replace = new wxButton(this, wxID_ANY, _("Replace"));
    horzSizer->Add(button_replace, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
    button_replace->Bind(wxEVT_BUTTON, &ReplaceInFilesPanel::OnReplace, this);
    button_replace->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnReplaceUI, this);

    m_tb->DeleteTool(XRCID("repeat_output"));
    m_tb->DeleteTool(XRCID("recent_searches"));
    m_tb->Realize();

    // The order is: toolbar, this row, results, progress bar, status line
    m_vSizer->Insert(1, horzSizer, 0, wxEXPAND | wxTOP | wxBOTTOM);

    m_progress = new wxGauge(this, wxID_ANY, 1, wxDefaultPosition, wxSize(-1, 15), wxGA_HORIZONTAL);
    m_progress->Hide();
    m_vSizer->Insert(3, m_progress, wxSizerFlags().Expand().Border(wxALL, 5));

    m_vSizer->Layout();
    ReplaceInFilesPanel::ApplyTheme();
}

void ReplaceInFilesPanel::OnSearchStart(wxCommandEvent& e)
{
    e.Skip();
    // set the "Replace With" field with the user value
    SearchData* data = (SearchData*)e.GetClientData();
    if (data) {
        m_replaceWith->SetValue(data->GetReplaceWith());
    }
    FindResultsTab::OnSearchStart(e);

    // Make sure that the Output view & the "Replace" tab
    // are visible
    clCommandEvent event(wxEVT_SHOW_OUTPUT_TAB);
    event.SetSelected(true).SetString(REPLACE_IN_FILES);
    EventNotifier::Get()->AddPendingEvent(event);
}

void ReplaceInFilesPanel::OnSearchMatch(wxCommandEvent& e)
{
    e.Skip();
    FindResultsTab::OnSearchMatch(e);

    // With a single match, use the matched text as the replacement
    auto* model = m_view->GetResultsModel();
    if (model->GetMatchCount() != 1 || !m_replaceWith->GetValue().IsEmpty())
        return;
    m_replaceWith->SetValue(model->GetMatches().front()->result.GetFindWhat());
    m_replaceWith->SetFocus();
}

void ReplaceInFilesPanel::OnSearchEnded(wxCommandEvent& e)
{
    e.Skip();
    SearchSummary* summary = (SearchSummary*)e.GetClientData();
    CHECK_PTR_RET(summary);

    // set the "Replace With" field with the user value
    m_replaceWith->SetValue(summary->GetReplaceWith());

    FindResultsTab::OnSearchEnded(e);
    OnMarkAll(e);

    // Set the focus to the "Replace With" field
    m_replaceWith->CallAfter(&wxComboBox::SetFocus);
}

void ReplaceInFilesPanel::OnMarkAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_view->GetResultsModel()->SetAllChecked(true);
}

void ReplaceInFilesPanel::OnMarkAllUI(wxUpdateUIEvent& e)
{
    e.Enable(!m_view->GetResultsModel()->IsEmpty() && !m_searchInProgress);
}

void ReplaceInFilesPanel::OnUnmarkAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_view->GetResultsModel()->SetAllChecked(false);
}

void ReplaceInFilesPanel::OnUnmarkAllUI(wxUpdateUIEvent& e)
{
    e.Enable(!m_view->GetResultsModel()->IsEmpty() && !m_searchInProgress);
}

void ReplaceInFilesPanel::DoSaveResults(wxStyledTextCtrl* sci, const std::vector<FindResultsNode*>& fileMatches)
{
    if (!sci || fileMatches.empty())
        return;
    bool ok = true;
    const wxString fileName = fileMatches.front()->result.GetFileName();
    if (dynamic_cast<clEditor*>(sci) == nullptr) {
        // it's a temp editor, check if we have any changes to save
        wxCSConv fontEncConv(EditorConfigST::Get()->GetOptions()->GetFileFontEncoding());
        if (sci->GetModify() && !FileUtils::WriteFileContent(fileName, sci->GetText(), fontEncConv)) {
            wxMessageBox(_("Failed to save file:\n") + fileName, _("CodeLite - Replace"), wxICON_ERROR | wxOK);
            clDEBUG() << "Replace: Failed to write file" << fileName;
            ok = false;
        }

        if (sci && ok) {
            // Keep the modified file name
            m_filesModified.Add(fileName);
        }

        delete sci;
    }

    // The matches that were selected and were not marked as failed already, are done (or failed to be saved)
    for (FindResultsNode* node : fileMatches) {
        if (node->checked && node->state == FindResultsNode::State::kPending) {
            node->state = ok ? FindResultsNode::State::kDone : FindResultsNode::State::kFailed;
            m_view->GetResultsModel()->NodeChanged(node);
        }
    }
}

wxStyledTextCtrl* ReplaceInFilesPanel::DoGetEditor(const wxString& fileName)
{
    // look for open editor first
    wxStyledTextCtrl* sci = clMainFrame::Get()->GetMainBook()->FindEditor(fileName);
    if (sci) {
        // FIXME: if editor is already modified, the found locations may not be accurate
        return sci;
    }

    // not open for editing, so make our own temp editor
    wxString content;
    if (!ReadFileWithConversion(fileName, content)) {
        wxMessageBox(_("Failed to open file:\n") + fileName, _("CodeLite - Replace"), wxICON_ERROR | wxOK);
        clDEBUG() << "Replace: Failed to read file" << fileName;
        return nullptr;
    }

    sci = new wxStyledTextCtrl(this);
    sci->Hide();
    sci->SetText(content);
    return sci;
}

wxString ReplaceInFilesPanel::DoGetReplaceWith(const SearchResult& res) const
{
    wxString replaceWith = m_replaceWith->GetValue();
    if (!(res.GetFlags() & wxSD_REGULAREXPRESSION)) {
        // not a regular expression search, do nothing
        return replaceWith;
    }
    // replace regular expression backrefs
    wxString s;
    for (size_t i = 0; i < replaceWith.length(); ++i) {
        wxUniChar ch0 = replaceWith[i];
        wxUniChar ch1 = '\0';
        if (i + 1 < replaceWith.length()) {
            ch1 = replaceWith[i + 1];
        }
        if (ch0 == '\\' && isdigit(ch1)) {
            // regex backref (\0 ~ \9)
            int backref = wxAtoi(ch1);
            s << res.GetRegexCapture(backref);
            ++i;
        } else if (ch0 == '\\' && ch1 == '\\') {
            // escaped backslash (\\)
            s << '\\';
            ++i;
        } else {
            s << ch0;
        }
    }
    return s;
}

void ReplaceInFilesPanel::OnReplace(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if (!m_progress->IsShown()) {
        m_progress->Show();
        GetSizer()->Layout();
    }

    m_filesModified.clear();
    // FIX bug#2770561
    int lineNumber = 0;
    clEditor* activeEditor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if (activeEditor) {
        lineNumber = activeEditor->GetCurrentLine();
    }

    if (m_replaceWith->FindString(m_replaceWith->GetValue(), true) == wxNOT_FOUND) {
        m_replaceWith->Append(m_replaceWith->GetValue());
    }

    // Step 1: apply selected replacements

    wxStyledTextCtrl* sci = nullptr; // file that is being altered by replacements

    wxString lastFile; // track offsets of pending substitutions caused by previous substitutions
    long lastLine = 0;
    long delta = 0, deltaInChars = 0;

    auto* model = m_view->GetResultsModel();
    const std::vector<FindResultsNode*> matches = model->GetMatches();

    // the matches of the file being updated
    std::vector<FindResultsNode*> fileMatches;

    m_progress->SetRange(matches.size());

    // Disable the 'buffer limit' feature during replace
    clMainFrame::Get()->GetMainBook()->SetUseBuffereLimit(false);
    for (FindResultsNode* node : matches) {
        SearchResult& res = node->result;
        m_progress->SetValue(m_progress->GetValue() + 1);
        m_progress->Update();

        if (res.GetFileName() != lastFile) {
            // about to start a different file, save current results
            DoSaveResults(sci, fileMatches);
            fileMatches.clear();
            lastFile = res.GetFileName();
            lastLine = 0;
            sci = nullptr;
        }
        fileMatches.push_back(node);

        if (res.GetLineNumber() == lastLine) {
            // prior substitutions affected the location of this one
            res.SetColumn(res.GetColumn() + delta);
            res.SetColumnInChars(res.GetColumnInChars() + deltaInChars);
        } else {
            delta = deltaInChars = 0;
        }
        if (!node->checked) {
            // not selected for application
            continue;
        }

        wxString replaceText = DoGetReplaceWith(res);
        int replaceLenInChars = (int)replaceText.Len();
        int replaceLen = (int)StringUtils::UTF8Length(replaceText.ToStdWstring().c_str(), replaceLenInChars);

        // extract originally matched text for safety check later
        wxString text = res.GetPattern().Mid(res.GetColumnInChars() - deltaInChars, res.GetLenInChars());
        if (text == replaceText)
            continue; // no change needed

        // need an editor for this file (try only once per file though)
        if (!sci && lastLine == 0) {
            sci = DoGetEditor(res.GetFileName());
            lastLine = res.GetLineNumber();
        }
        if (!sci) {
            // couldn't open file
            node->state = FindResultsNode::State::kFailed;
            continue;
        }

        long pos = sci->PositionFromLine(res.GetLineNumber() - 1);
        if (pos < 0) {
            // invalid line number
            node->state = FindResultsNode::State::kFailed;
            continue;
        }
        pos += res.GetColumn();

        sci->SetSelection(pos, pos + res.GetLen());
        if (sci->GetSelectedText() != text) {
            // couldn't locate the original match (file may have been modified)
            node->state = FindResultsNode::State::kFailed;
            continue;
        }
        sci->ReplaceSelection(replaceText);

        delta += replaceLen - res.GetLen();
        deltaInChars += replaceLenInChars - res.GetLenInChars();
        lastLine = res.GetLineNumber();

        res.SetLen(replaceLen);
        res.SetLenInChars(replaceLenInChars);
    }

    // hide the progress bar
    m_progress->SetValue(0);
    m_progress->Hide();
    GetSizer()->Layout();

    DoSaveResults(sci, fileMatches);

    // Disable the 'buffer limit' feature during replace
    clMainFrame::Get()->GetMainBook()->SetUseBuffereLimit(true);

    // Step 2: Update the Replace pane: remove the matches that were replaced

    std::set<wxString> updatedEditors;
    std::vector<FindResultsNode*> done;
    for (FindResultsNode* node : matches) {
        if (node->state != FindResultsNode::State::kDone) {
            continue;
        }
        const wxString& fileName = node->result.GetFileName();
        clEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(fileName);
        if (editor && editor->GetModify()) {
            updatedEditors.insert(fileName);
        }
        done.push_back(node);
    }

    // `matches` and `done` must not be used after the nodes are removed
    m_currentMatch = nullptr;
    model->RemoveMatches(done);
    model->SetAllChecked(false);
    m_replaceWith->SetValue(wxEmptyString);
    if (model->IsEmpty()) {
        Clear();
    }

    // Step 3: Notify user of changes to already opened files, ask to save
    std::vector<std::pair<wxFileName, bool>> filesToSave;
    for (const auto& editorPath : updatedEditors) {
        filesToSave.push_back(std::make_pair(wxFileName(editorPath), true));
    }
    if (!filesToSave.empty() && clMainFrame::Get()->GetMainBook()->UserSelectFiles(
                                    filesToSave,
                                    _("Save Modified Files"),
                                    _("Some files are modified.\nChoose the files you would like to save."),
                                    true)) {
        for (size_t i = 0; i < filesToSave.size(); i++) {
            if (filesToSave[i].second) {
                clEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(filesToSave[i].first.GetFullPath());
                if (editor) {
                    editor->SaveFile();
                }
            }
        }
    }

    // FIX bug#2770561
    if (activeEditor) {

        clMainFrame::Get()->GetMainBook()->SelectPage(activeEditor);

        // restore the line
        activeEditor->GotoLine(lineNumber);
    }

    if (!m_filesModified.IsEmpty()) {
        // Some files were modified directly on the file system, notify about it to the plugins
        clFileSystemEvent event(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES);
        event.SetStrings(m_filesModified);
        EventNotifier::Get()->AddPendingEvent(event);
        m_filesModified.clear();
    }
}

void ReplaceInFilesPanel::OnReplaceUI(wxUpdateUIEvent& e)
{
    e.Enable(!m_view->GetResultsModel()->IsEmpty() && !m_searchInProgress);
}

void ReplaceInFilesPanel::OnReplaceWithComboUI(wxUpdateUIEvent& e)
{
    e.Enable(!m_view->GetResultsModel()->IsEmpty() && !m_searchInProgress);
}

void ReplaceInFilesPanel::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
    int sel = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetSelection();
    if (clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) {
        return;
    }

    if (EditorConfigST::Get()->GetOptions()->GetHideOutputPaneOnUserClick()) {
        e.Enable(true);
        e.Check(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfReplace());

    } else {
        e.Enable(false);
        e.Check(false);
    }
}

void ReplaceInFilesPanel::ApplyTheme()
{
    FindResultsTab::ApplyTheme();

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if (!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("text");
    }
    if (lexer && m_replaceWith != nullptr) {
        lexer->ApplyFont(m_replaceWith);
    }
}
