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
#include <vector>
#include <wx/xrc/xmlres.h>
#include <wx/progdlg.h>

#include "globals.h"
#include "frame.h"
#include "cl_editor.h"
#include "manager.h"
#include "replaceinfilespanel.h"


BEGIN_EVENT_TABLE(ReplaceInFilesPanel, FindResultsTab)
	EVT_BUTTON(XRCID("unmark_all"),             ReplaceInFilesPanel::OnUnmarkAll)
	EVT_BUTTON(XRCID("mark_all"),               ReplaceInFilesPanel::OnMarkAll)
	EVT_BUTTON(XRCID("replace"),                ReplaceInFilesPanel::OnReplace)

	EVT_UPDATE_UI(XRCID("unmark_all"),          ReplaceInFilesPanel::OnUnmarkAllUI)
	EVT_UPDATE_UI(XRCID("mark_all"),            ReplaceInFilesPanel::OnMarkAllUI)
	EVT_UPDATE_UI(XRCID("replace"),             ReplaceInFilesPanel::OnReplaceUI)
	EVT_UPDATE_UI(XRCID("replace_with_combo"),  ReplaceInFilesPanel::OnReplaceWithComboUI)
	EVT_UPDATE_UI(XRCID("replace_with_label"),  ReplaceInFilesPanel::OnReplaceWithComboUI)
END_EVENT_TABLE()


ReplaceInFilesPanel::ReplaceInFilesPanel(wxWindow* parent, int id, const wxString &name)
    : FindResultsTab(parent, id, name)
{

	wxBoxSizer *horzSizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton *unmark = new wxButton(this, XRCID("unmark_all"), _("&Unmark All"));
	horzSizer->Add(unmark, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	wxButton *mark = new wxButton(this, XRCID("mark_all"), _("Mark &All"));
	horzSizer->Add(mark, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	m_replaceWithText = new wxStaticText( this, XRCID("replace_with_label"), _("Replace With:"));
	horzSizer->Add(m_replaceWithText, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5);

	m_replaceWith = new wxComboBox(this, XRCID("replace_with_combo"));
	horzSizer->Add(m_replaceWith, 2, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5);

	wxButton *repl = new wxButton(this, XRCID("replace"), _("&Replace Marked"));
	horzSizer->Add(repl, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	m_progress = new wxGauge(this, wxID_ANY, 1, wxDefaultPosition, wxSize(-1, 15), wxGA_HORIZONTAL);
	horzSizer->Add(m_progress, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxGA_SMOOTH, 5);

	wxBoxSizer *vertSizer = new wxBoxSizer(wxVERTICAL);
	vertSizer->Add(horzSizer, 0, wxEXPAND|wxTOP|wxBOTTOM);

	// grab the base class scintilla and put our sizer in its place
	wxSizer *mainSizer = m_hSizer;
	mainSizer->Detach(m_sci);
	vertSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);

	m_tb->RemoveTool ( XRCID ( "repeat_output" ) );
	m_tb->Realize();

#ifdef __WXMAC__
	mainSizer->Insert(0, vertSizer, 1, wxEXPAND);
#else
	mainSizer->Add(vertSizer, 1, wxEXPAND);
#endif
	mainSizer->Layout();

	m_sci->SetMarginMask(4, 7<<0x7 | wxSCI_MASK_FOLDERS);
    DefineMarker(m_sci, 0x7, wxSCI_MARK_SMALLRECT, wxColor(0x00, 0x80, 0x00), wxColor(0x00, 0xc0, 0x00)); // user selection
    DefineMarker(m_sci, 0x8, wxSCI_MARK_CIRCLE,    wxColor(0x80, 0x00, 0x00), wxColor(0xff, 0x00, 0x00)); // error occurred
    DefineMarker(m_sci, 0x9, wxSCI_MARK_EMPTY,     wxColor(0x00, 0x00, 0x00), wxColor(0x00, 0x00, 0x00)); // replacement successful
}

void ReplaceInFilesPanel::OnSearchStart(wxCommandEvent &e)
{
	SearchData *data = (SearchData*) e.GetClientData();
	wxString label = data ? data->GetFindString() : wxT("");

	FindResultsTab::OnSearchStart(e);

	// set the search string to be the 'replace with' string as well
	if(label.IsEmpty() == false) {
		m_replaceWith->SetValue(label);
		m_replaceWith->SetSelection(-1, -1);
		m_replaceWith->SetFocus();
	}
}

void ReplaceInFilesPanel::OnSearchMatch(wxCommandEvent &e)
{
	FindResultsTab::OnSearchMatch(e);
	const MatchInfo& matchInfo = GetMatchInfo();
	if (matchInfo.size() != 1 || !m_replaceWith->GetValue().IsEmpty())
		return;
	m_replaceWith->SetValue(matchInfo.begin()->second.GetFindWhat());
	m_replaceWith->SetSelection(-1, -1);
	m_replaceWith->SetFocus();
}

void ReplaceInFilesPanel::OnSearchEnded(wxCommandEvent &e)
{
	FindResultsTab::OnSearchEnded(e);
	OnMarkAll(e);
}

void ReplaceInFilesPanel::OnMarginClick(wxScintillaEvent& e)
{
	int line = m_sci->LineFromPosition(e.GetPosition());
	const MatchInfo& matchInfo = GetMatchInfo();
	if (matchInfo.find(line) == matchInfo.end()) {
		FindResultsTab::OnMarginClick(e);
	} else if (m_sci->MarkerGet(line) & 7<<0x7) {
		m_sci->MarkerDelete(line, 0x7);
	} else {
		m_sci->MarkerAdd(line, 0x7);
	}
}

void ReplaceInFilesPanel::OnMarkAll(wxCommandEvent& e)
{
	const MatchInfo& matchInfo = GetMatchInfo();
	for (MatchInfo::const_iterator i = matchInfo.begin(); i != matchInfo.end(); ++i) {
		if (m_sci->MarkerGet(i->first) & 7<<0x7)
			continue;
		m_sci->MarkerAdd(i->first, 0x7);
	}
}

void ReplaceInFilesPanel::OnMarkAllUI(wxUpdateUIEvent& e)
{
    e.Enable(m_sci->GetLength() > 0);
}

void ReplaceInFilesPanel::OnUnmarkAll(wxCommandEvent& e)
{
	m_sci->MarkerDeleteAll(0x7);
}

void ReplaceInFilesPanel::OnUnmarkAllUI(wxUpdateUIEvent& e)
{
    e.Enable(m_sci->GetLength() > 0);
}

void ReplaceInFilesPanel::DoSaveResults(wxScintilla *sci,
                                        std::map<int,SearchResult>::iterator begin,
                                        std::map<int,SearchResult>::iterator end)
{
	if (!sci || begin == end)
		return;
	bool ok = true;
	if (dynamic_cast<LEditor*>(sci) == NULL) {
		// it's a temp editor, check if we have any changes to save
		if (sci->GetModify() && !WriteFileWithBackup(begin->second.GetFileName(), sci->GetText(), false)) {
			wxMessageBox(_("Failed to save file:\n") + begin->second.GetFileName(), _("CodeLite - Replace"),
			             wxICON_ERROR|wxOK);
			wxLogMessage(wxT("Replace: Failed to write file ") + begin->second.GetFileName());
			ok = false;
		}
		delete sci;
	}
	for (; begin != end; begin++) {
		if ((m_sci->MarkerGet(begin->first) & 7<<0x7) == 1<<0x7) {
			m_sci->MarkerAdd(begin->first, ok ? 0x9 : 0x8);
		}
	}
}

wxScintilla *ReplaceInFilesPanel::DoGetEditor(const wxString &fileName)
{
	// look for open editor first
	wxScintilla *sci = clMainFrame::Get()->GetMainBook()->FindEditor(fileName);
	if (sci) {
		// FIXME: if editor is already modified, the found locations may not be accurate
		return sci;
	}

	// not open for editing, so make our own temp editor
	wxString content;
	if (!ReadFileWithConversion(fileName, content)) {
		wxMessageBox(_("Failed to open file:\n") + fileName, _("CodeLite - Replace"), wxICON_ERROR|wxOK);
		wxLogMessage(wxT("Replace: Failed to read file ") + fileName);
		return NULL;
	}

	sci = new wxScintilla(this);
	sci->Hide();
	sci->SetText(content);
	return sci;
}

void ReplaceInFilesPanel::OnReplace(wxCommandEvent& e)
{
	// FIX bug#2770561
	int lineNumber(0);
	LEditor *activeEditor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
	if( activeEditor ) {
		lineNumber = activeEditor->GetCurrentLine();
	}

	if (m_replaceWith->FindString(m_replaceWith->GetValue(), true) == wxNOT_FOUND) {
		m_replaceWith->Append(m_replaceWith->GetValue());
	}

	// Step 1: apply selected replacements

	wxScintilla *sci = NULL; // file that is being altered by replacements

	wxString lastFile; // track offsets of pending substitutions caused by previous substitutions
	long lastLine = 0;
	long delta = 0;

	// remembers first entry in the file being updated
	MatchInfo& matchInfo = GetMatchInfo();
	MatchInfo::iterator firstInFile = matchInfo.begin();

	m_progress->SetRange(matchInfo.size());

	// Disable the 'buffer limit' feature during replace
	clMainFrame::Get()->GetMainBook()->SetUseBuffereLimit(false);
	for (MatchInfo::iterator i = firstInFile; i != matchInfo.end(); i++) {
		m_progress->SetValue(m_progress->GetValue()+1);
		m_progress->Update();

		if (i->second.GetFileName() != lastFile) {
			// about to start a different file, save current results
			DoSaveResults(sci, firstInFile, i);
			firstInFile = i;
			lastFile = i->second.GetFileName();
			lastLine = 0;
			sci = NULL;
		}

		if (i->second.GetLineNumber() == lastLine) {
			// prior substitutions affected the location of this one
			i->second.SetColumn(i->second.GetColumn()+delta);
		} else {
			delta = 0;
		}
		if ((m_sci->MarkerGet(i->first) & 1<<0x7) == 0)
			// not selected for application
			continue;

		// extract originally matched text for safety check later
		wxString text = i->second.GetPattern().Mid(i->second.GetColumn()-delta, i->second.GetLen());
		if (text == m_replaceWith->GetValue())
			continue; // no change needed

		// need an editor for this file (try only once per file though)
		if (!sci && lastLine == 0) {
			sci = DoGetEditor(i->second.GetFileName());
			lastLine = i->second.GetLineNumber();
		}
		if (!sci) {
			// couldn't open file
			m_sci->MarkerAdd(i->first, 0x8);
			continue;
		}

		long pos = sci->PositionFromLine(i->second.GetLineNumber()-1);
		if (pos < 0) {
			// invalid line number
			m_sci->MarkerAdd(i->first, 0x8);
			continue;
		}
		pos += i->second.GetColumn();

		sci->SetSelection(pos, pos + i->second.GetLen());
		if (sci->GetSelectedText() != text) {
			// couldn't locate the original match (file may have been modified)
			m_sci->MarkerAdd(i->first, 0x8);
			continue;
		}
		sci->ReplaceSelection(m_replaceWith->GetValue());

		delta += m_replaceWith->GetValue().Length() - i->second.GetLen();
		lastLine = i->second.GetLineNumber();

		i->second.SetPattern(m_sci->GetLine(i->first)); // includes prior updates to same line
		i->second.SetLen(m_replaceWith->GetValue().Length());
	}
	m_progress->SetValue(0);
	DoSaveResults(sci, firstInFile, matchInfo.end());

	// Disable the 'buffer limit' feature during replace
	clMainFrame::Get()->GetMainBook()->SetUseBuffereLimit(true);

	// Step 2: Update the Replace pane

	std::set<wxString> updatedEditors;
	delta = 0; // offset from old line number to new
	lastLine = 1; // points to the filename line
	lastFile.Clear();
	m_sci->MarkerDeleteAll(0x7);
	m_sci->SetReadOnly(false);

	std::vector<int> itemsToRemove;
	for (MatchInfo::iterator i = matchInfo.begin();i != matchInfo.end(); i++) {
		int line = i->first + delta;
		if (i->second.GetFileName() != lastFile) {
			if (lastLine == line-2) {
				// previous file's replacements are all done, so remove its filename line
				m_sci->SetCurrentPos(m_sci->PositionFromLine(lastLine));
				m_sci->LineDelete();
				delta--;
				line--;
			} else {
				lastLine = line-1;
			}
			lastFile = i->second.GetFileName();
		}

		if (m_sci->MarkerGet(line) & 1<<0x9) {
			LEditor *editor = clMainFrame::Get()->GetMainBook()->FindEditor(lastFile);
			if (editor && editor->GetModify()) {
				updatedEditors.insert(lastFile);
			}
			// replacement done, remove from map
			m_sci->MarkerDelete(line, 0x9);
			m_sci->SetCurrentPos(m_sci->PositionFromLine(line));
			m_sci->LineDelete();
			itemsToRemove.push_back(i->first);
			delta--;
		} else if (line != i->first) {
			// need to adjust line number
			matchInfo[line] = i->second;
			itemsToRemove.push_back(i->first);
		}
	}

	// update the match info map
	for(std::vector<int>::size_type i=0; i<itemsToRemove.size(); i++){
		MatchInfo::iterator iter = matchInfo.find(itemsToRemove.at(i));
		if (iter != matchInfo.end()){
			matchInfo.erase(iter);
		}
	}

	m_sci->SetReadOnly(true);
	m_sci->GotoLine(0);
	if (matchInfo.empty()) {
		Clear();
	}

	// Step 3: Notify user of changes to already opened files, ask to save

	std::vector<std::pair<wxFileName, bool> > filesToSave;
	for (std::set<wxString>::iterator i = updatedEditors.begin(); i != updatedEditors.end(); i++) {
		filesToSave.push_back(std::make_pair(wxFileName(*i), true));
	}
	if (!filesToSave.empty() &&
	        clMainFrame::Get()->GetMainBook()->UserSelectFiles(filesToSave, _("Save Modified Files"),
	                _("Some files are modified.\nChoose the files you would like to save."), true)) {
		for (size_t i = 0; i < filesToSave.size(); i++) {
			if (filesToSave[i].second) {
				LEditor *editor = clMainFrame::Get()->GetMainBook()->FindEditor(filesToSave[i].first.GetFullPath());
				if (editor) {
					editor->SaveFile();
				}
			}
		}
	}

	// FIX bug#2770561
	if(activeEditor) {

		clMainFrame::Get()->GetMainBook()->SelectPage(activeEditor);

		// restore the line
		activeEditor->GotoLine( lineNumber );

	}
}

void ReplaceInFilesPanel::OnReplaceUI(wxUpdateUIEvent& e)
{
    e.Enable(m_sci->GetLength() > 0);
}

void ReplaceInFilesPanel::OnReplaceWithComboUI(wxUpdateUIEvent& e)
{
	e.Enable(m_sci->GetLength() > 0);
}
