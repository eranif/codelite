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
#include "clThemedButton.h"
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

#include <vector>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/progdlg.h>
#include <wx/renderer.h>
#include <wx/xrc/xmlres.h>

ReplaceInFilesPanel::ReplaceInFilesPanel(wxWindow* parent, int id, const wxString& name)
    : FindResultsTab(parent, id, name)
{
    Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnHoldOpenUpdateUI, this, XRCID("hold_pane_open"));
    wxBoxSizer* horzSizer = new wxBoxSizer(wxHORIZONTAL);

    clThemedButton* unmark = new clThemedButton(this, wxID_ANY, _("&Unmark All"));
    horzSizer->Add(unmark, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
    unmark->Bind(wxEVT_BUTTON, &ReplaceInFilesPanel::OnUnmarkAll, this);
    unmark->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnUnmarkAllUI, this);

    clThemedButton* mark = new clThemedButton(this, wxID_ANY, _("Mark &All"));
    horzSizer->Add(mark, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
    mark->Bind(wxEVT_BUTTON, &ReplaceInFilesPanel::OnMarkAll, this);
    mark->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnMarkAllUI, this);

    m_replaceWithText = new wxStaticText(this, wxID_ANY, _("Replace With:"));
    horzSizer->Add(m_replaceWithText, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT, 5);
    m_replaceWithText->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnReplaceWithComboUI, this);

    m_replaceWith = new wxComboBox(this, wxID_ANY);
    horzSizer->Add(m_replaceWith, 2, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT, 5);
    m_replaceWith->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnReplaceWithComboUI, this);

    clThemedButton* repl = new clThemedButton(this, wxID_ANY, _("&Replace Marked"));
    horzSizer->Add(repl, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
    repl->Bind(wxEVT_BUTTON, &ReplaceInFilesPanel::OnReplace, this);
    repl->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnReplaceUI, this);

    m_progress = new wxGauge(this, wxID_ANY, 1, wxDefaultPosition, wxSize(-1, 15), wxGA_HORIZONTAL);
    horzSizer->Add(m_progress, 1, wxALIGN_CENTER_VERTICAL | wxALL | wxGA_SMOOTH, 5);

    wxBoxSizer* vertSizer = new wxBoxSizer(wxVERTICAL);
    vertSizer->Add(horzSizer, 0, wxEXPAND | wxTOP | wxBOTTOM);

    // grab the base class scintilla and put our sizer in its place
    wxSizer* mainSizer = m_vSizer;
    mainSizer->Detach(m_sci);
    vertSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);

    m_tb->DeleteById(XRCID("repeat_output"));
    m_tb->DeleteById(XRCID("recent_searches"));
    m_tb->Realize();

#ifdef __WXMAC__
    mainSizer->Insert(0, vertSizer, 1, wxEXPAND);
#else
    mainSizer->Add(vertSizer, 1, wxEXPAND);
#endif
    mainSizer->Layout();
}

ReplaceInFilesPanel::~ReplaceInFilesPanel() {}

void ReplaceInFilesPanel::OnSearchStart(wxCommandEvent& e)
{
    e.Skip();
    // set the "Replace With" field with the user value
    SearchData* data = (SearchData*)e.GetClientData();
    m_replaceWith->ChangeValue(data->GetReplaceWith());
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
    if(m_matchInfo.size() != 1 || !m_replaceWith->GetValue().IsEmpty())
        return;
    m_replaceWith->SetValue(m_matchInfo.begin()->second.GetFindWhat());
    m_replaceWith->SetFocus();
}

void ReplaceInFilesPanel::OnSearchEnded(wxCommandEvent& e)
{
    e.Skip();
    SearchSummary* summary = (SearchSummary*)e.GetClientData();
    CHECK_PTR_RET(summary);

    // set the "Replace With" field with the user value
    m_replaceWith->ChangeValue(summary->GetReplaceWith());

    FindResultsTab::OnSearchEnded(e);
    OnMarkAll(e);

    // Set the focus to the "Replace With" field
    m_replaceWith->CallAfter(&wxComboBox::SetFocus);
}

void ReplaceInFilesPanel::OnMarginClick(wxStyledTextEvent& e)
{
    int line = m_sci->LineFromPosition(e.GetPosition());
    if(m_matchInfo.find(line) == m_matchInfo.end()) {
        FindResultsTab::OnMarginClick(e);

    } else if(m_sci->MarkerGet(line) & 7 << 0x7) {
        m_sci->MarkerDelete(line, 0x7);
    } else {
        m_sci->MarkerAdd(line, 0x7);
    }
}

void ReplaceInFilesPanel::OnMarkAll(wxCommandEvent& e)
{
    MatchInfo_t::const_iterator i = m_matchInfo.begin();
    for(; i != m_matchInfo.end(); ++i) {
        if(m_sci->MarkerGet(i->first) & 7 << 0x7)
            continue;
        m_sci->MarkerAdd(i->first, 0x7);
    }
}

void ReplaceInFilesPanel::OnMarkAllUI(wxUpdateUIEvent& e) { e.Enable((m_sci->GetLength() > 0) && !m_searchInProgress); }
void ReplaceInFilesPanel::OnUnmarkAll(wxCommandEvent& e) { m_sci->MarkerDeleteAll(0x7); }
void ReplaceInFilesPanel::OnUnmarkAllUI(wxUpdateUIEvent& e)
{
    e.Enable((m_sci->GetLength() > 0) && !m_searchInProgress);
}

void ReplaceInFilesPanel::DoSaveResults(wxStyledTextCtrl* sci, std::map<int, SearchResult>::iterator begin,
                                        std::map<int, SearchResult>::iterator end)
{
    if(!sci || begin == end)
        return;
    bool ok = true;
    if(dynamic_cast<clEditor*>(sci) == NULL) {
        // it's a temp editor, check if we have any changes to save
        if(sci->GetModify() && !WriteFileWithBackup(begin->second.GetFileName(), sci->GetText(), false)) {
            wxMessageBox(_("Failed to save file:\n") + begin->second.GetFileName(), _("CodeLite - Replace"),
                         wxICON_ERROR | wxOK);
            clDEBUG() << "Replace: Failed to write file" << begin->second.GetFileName();
            ok = false;
        }

        if(sci && ok) {
            // Keep the modified file name
            m_filesModified.Add(begin->second.GetFileName());
        }

        delete sci;
    }
    for(; begin != end; begin++) {
        if((m_sci->MarkerGet(begin->first) & 7 << 0x7) == 1 << 0x7) {
            m_sci->MarkerAdd(begin->first, ok ? 0x9 : 0x8);
        }
    }
}

wxStyledTextCtrl* ReplaceInFilesPanel::DoGetEditor(const wxString& fileName)
{
    // look for open editor first
    wxStyledTextCtrl* sci = clMainFrame::Get()->GetMainBook()->FindEditor(fileName);
    if(sci) {
        // FIXME: if editor is already modified, the found locations may not be accurate
        return sci;
    }

    // not open for editing, so make our own temp editor
    wxString content;
    if(!ReadFileWithConversion(fileName, content)) {
        wxMessageBox(_("Failed to open file:\n") + fileName, _("CodeLite - Replace"), wxICON_ERROR | wxOK);
        clDEBUG() << "Replace: Failed to read file" << fileName;
        return NULL;
    }

    sci = new wxStyledTextCtrl(this);
    sci->Hide();
    sci->SetText(content);
    return sci;
}

wxString ReplaceInFilesPanel::DoGetReplaceWith(const SearchResult& res) const
{
    const wxString& replaceWith = m_replaceWith->GetValue();
    if(!(res.GetFlags() & wxSD_REGULAREXPRESSION)) {
        // not a regular expression search, do nothing
        return replaceWith;
    }
    // replace regular expression backrefs
    wxString s;
    for(size_t i = 0; i < replaceWith.length(); ++i) {
        wxUniChar ch0 = replaceWith[i];
        wxUniChar ch1 = '\0';
        if(i + 1 < replaceWith.length()) {
            ch1 = replaceWith[i + 1];
        }
        if(ch0 == '\\' && isdigit(ch1)) {
            // regex backref (\0 ~ \9)
            int backref = wxAtoi(ch1);
            s << res.GetRegexCapture(backref);
            ++i;
        } else if(ch0 == '\\' && ch1 == '\\') {
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
    m_filesModified.clear();
    // FIX bug#2770561
    int lineNumber(0);
    clEditor* activeEditor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(activeEditor) {
        lineNumber = activeEditor->GetCurrentLine();
    }

    if(m_replaceWith->FindString(m_replaceWith->GetValue(), true) == wxNOT_FOUND) {
        m_replaceWith->Append(m_replaceWith->GetValue());
    }

    // Step 1: apply selected replacements

    wxStyledTextCtrl* sci = NULL; // file that is being altered by replacements

    wxString lastFile; // track offsets of pending substitutions caused by previous substitutions
    long lastLine = 0;
    long delta = 0, deltaInChars = 0;

    // remembers first entry in the file being updated
    MatchInfo_t::iterator firstInFile = m_matchInfo.begin();

    m_progress->SetRange(m_matchInfo.size());

    // Disable the 'buffer limit' feature during replace
    clMainFrame::Get()->GetMainBook()->SetUseBuffereLimit(false);
    MatchInfo_t::iterator i = firstInFile;
    for(; i != m_matchInfo.end(); ++i) {
        SearchResult& res = i->second;
        m_progress->SetValue(m_progress->GetValue() + 1);
        m_progress->Update();

        if(res.GetFileName() != lastFile) {
            // about to start a different file, save current results
            DoSaveResults(sci, firstInFile, i);
            firstInFile = i;
            lastFile = res.GetFileName();
            lastLine = 0;
            sci = NULL;
        }

        if(res.GetLineNumber() == lastLine) {
            // prior substitutions affected the location of this one
            res.SetColumn(res.GetColumn() + delta);
            res.SetColumnInChars(res.GetColumnInChars() + deltaInChars);
        } else {
            delta = deltaInChars = 0;
        }
        if((m_sci->MarkerGet(i->first) & 1 << 0x7) == 0)
            // not selected for application
            continue;

        wxString replaceText = DoGetReplaceWith(res);
        int replaceLenInChars = (int)replaceText.Len();
        int replaceLen = (int)::clUTF8Length(replaceText.ToStdWstring().c_str(), replaceLenInChars);

        // extract originally matched text for safety check later
        wxString text = res.GetPattern().Mid(res.GetColumnInChars() - deltaInChars, res.GetLenInChars());
        if(text == replaceText)
            continue; // no change needed

        // need an editor for this file (try only once per file though)
        if(!sci && lastLine == 0) {
            sci = DoGetEditor(res.GetFileName());
            lastLine = res.GetLineNumber();
        }
        if(!sci) {
            // couldn't open file
            m_sci->MarkerAdd(i->first, 0x8);
            continue;
        }

        long pos = sci->PositionFromLine(res.GetLineNumber() - 1);
        if(pos < 0) {
            // invalid line number
            m_sci->MarkerAdd(i->first, 0x8);
            continue;
        }
        pos += res.GetColumn();

        sci->SetSelection(pos, pos + res.GetLen());
        if(sci->GetSelectedText() != text) {
            // couldn't locate the original match (file may have been modified)
            m_sci->MarkerAdd(i->first, 0x8);
            continue;
        }
        sci->ReplaceSelection(replaceText);

        delta += replaceLen - res.GetLen();
        deltaInChars += replaceLenInChars - res.GetLenInChars();
        lastLine = res.GetLineNumber();

        res.SetPattern(m_sci->GetLine(i->first)); // includes prior updates to same line
        res.SetLen(replaceLen);
        res.SetLenInChars(replaceLenInChars);
    }
    m_progress->SetValue(0);
    DoSaveResults(sci, firstInFile, m_matchInfo.end());

    // Disable the 'buffer limit' feature during replace
    clMainFrame::Get()->GetMainBook()->SetUseBuffereLimit(true);

    // Step 2: Update the Replace pane

    std::set<wxString> updatedEditors;
    delta = 0;    // offset from old line number to new
    lastLine = 1; // points to the filename line
    lastFile.Clear();
    m_sci->MarkerDeleteAll(0x7);
    m_sci->SetReadOnly(false);

    std::vector<int> itemsToRemove;
    i = m_matchInfo.begin();
    for(; i != m_matchInfo.end(); i++) {
        int line = i->first + delta;
        if(i->second.GetFileName() != lastFile) {
            if(lastLine == line - 2) {
                // previous file's replacements are all done, so remove its filename line
                m_sci->SetCurrentPos(m_sci->PositionFromLine(lastLine));
                m_sci->LineDelete();
                delta--;
                line--;
            } else {
                lastLine = line - 1;
            }
            lastFile = i->second.GetFileName();
        }

        if(m_sci->MarkerGet(line) & 1 << 0x9) {
            clEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(lastFile);
            if(editor && editor->GetModify()) {
                updatedEditors.insert(lastFile);
            }
            // replacement done, remove from map
            m_sci->MarkerDelete(line, 0x9);
            m_sci->SetCurrentPos(m_sci->PositionFromLine(line));
            m_sci->LineDelete();
            itemsToRemove.push_back(i->first);
            delta--;
        } else if(line != i->first) {
            // need to adjust line number
            m_matchInfo[line] = i->second;
            itemsToRemove.push_back(i->first);
        }
    }

    // update the match info map
    for(std::vector<int>::size_type i = 0; i < itemsToRemove.size(); i++) {
        MatchInfo_t::iterator iter = m_matchInfo.find(itemsToRemove.at(i));
        if(iter != m_matchInfo.end()) {
            m_matchInfo.erase(iter);
        }
    }

    m_sci->SetReadOnly(true);
    m_sci->GotoLine(0);
    if(m_matchInfo.empty()) {
        Clear();
    }

    // Step 3: Notify user of changes to already opened files, ask to save
    std::vector<std::pair<wxFileName, bool>> filesToSave;
    for(std::set<wxString>::iterator i = updatedEditors.begin(); i != updatedEditors.end(); i++) {
        filesToSave.push_back(std::make_pair(wxFileName(*i), true));
    }
    if(!filesToSave.empty() && clMainFrame::Get()->GetMainBook()->UserSelectFiles(
                                   filesToSave, _("Save Modified Files"),
                                   _("Some files are modified.\nChoose the files you would like to save."), true)) {
        for(size_t i = 0; i < filesToSave.size(); i++) {
            if(filesToSave[i].second) {
                clEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(filesToSave[i].first.GetFullPath());
                if(editor) {
                    editor->SaveFile();
                }
            }
        }
    }

    // FIX bug#2770561
    if(activeEditor) {

        clMainFrame::Get()->GetMainBook()->SelectPage(activeEditor);

        // restore the line
        activeEditor->GotoLine(lineNumber);
    }

    if(!m_filesModified.IsEmpty()) {
        // Some files were modified directly on the file system, notify about it to the plugins
        clFileSystemEvent event(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES);
        event.SetStrings(m_filesModified);
        EventNotifier::Get()->AddPendingEvent(event);
        m_filesModified.clear();
    }
}

void ReplaceInFilesPanel::OnReplaceUI(wxUpdateUIEvent& e) { e.Enable((m_sci->GetLength() > 0) && !m_searchInProgress); }

void ReplaceInFilesPanel::OnReplaceWithComboUI(wxUpdateUIEvent& e)
{
    e.Enable((m_sci->GetLength() > 0) && !m_searchInProgress);
}

void ReplaceInFilesPanel::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
    int sel = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetSelection();
    if(clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) {
        return;
    }

    if(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {
        e.Enable(true);
        e.Check(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfReplace());

    } else {
        e.Enable(false);
        e.Check(false);
    }
}

void ReplaceInFilesPanel::OnMouseDClick(wxStyledTextEvent& e)
{
    int clickedLine = wxNOT_FOUND;
    m_styler->HitTest(m_sci, e, clickedLine);

    // Did we clicked on a togglable line?
    int toggleLine = m_styler->TestToggle(m_sci, e);
    if(toggleLine != wxNOT_FOUND) {
        m_sci->ToggleFold(toggleLine);

    } else {
        MatchInfo_t::const_iterator m = m_matchInfo.find(clickedLine);
        if(m != m_matchInfo.end()) {
            DoOpenSearchResult(m->second, NULL, m->first);
        }
    }
}

static void RenderCheckbox(wxWindow* win, wxDC& dc, const wxColour& bgColour, const wxColour& penColour,
                           const wxRect& _rect, bool checked)
{
    wxRect rect = _rect;
    dc.SetBrush(bgColour);
    dc.SetPen(bgColour);
    dc.DrawRectangle(rect);
    if(!checked) {
        return;
    }

    if(checked) {
        wxRect innerRect = rect;
        innerRect.Deflate(::clGetSize(4, win));
        dc.SetPen(wxPen(penColour, ::clGetSize(2, win)));

        wxPoint p1, p2, p3;
        p1.x = innerRect.GetTopLeft().x;
        p1.y = innerRect.GetTopLeft().y + (innerRect.GetHeight() / 2);

        p2.x = innerRect.GetBottomLeft().x + (innerRect.GetWidth() / 3);
        p2.y = innerRect.GetBottomLeft().y;

        p3 = innerRect.GetTopRight();
        dc.DrawLine(p1, p2);
        dc.DrawLine(p2, p3);
    }
}

void ReplaceInFilesPanel::SetStyles(wxStyledTextCtrl* sci)
{
    FindResultsTab::SetStyles(sci);

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if(!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("text");
    }

    // render two bitmaps: checked and unchecked
    int size = ::clGetSize(16, sci);
    const StyleProperty& styleProperty = lexer->GetProperty(0);

    wxColour bgColour = styleProperty.GetBgColour();
    wxColour fgColour = lexer->IsDark() ? *wxYELLOW : *wxBLACK;
    {
        wxMemoryDC memDC;
        m_bmpChecked = wxBitmap(size, size);
        memDC.SelectObject(m_bmpChecked);
        wxGCDC gcdc(memDC);
        wxRect rect(0, 0, size, size);
        RenderCheckbox(sci, gcdc, bgColour, fgColour, rect, true);
        memDC.SelectObject(wxNullBitmap); // Apply the changes
    }
#if wxCHECK_VERSION(3, 1, 0)
    sci->SetMarginWidth(3, clGetSize(1, sci)); // separator margin
    sci->SetMarginBackground(3, lexer->IsDark() ? bgColour.ChangeLightness(130) : bgColour.ChangeLightness(50));
#else
    sci->SetMarginWidth(3, 0); // separator margin
#endif
    sci->MarkerDefineBitmap(7, m_bmpChecked);
}
