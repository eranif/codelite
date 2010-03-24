//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findinfilesdlg.cpp
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
#include "search_thread.h"
#include <wx/tokenzr.h>
#include <wx/fontmap.h>
#include "dirpicker.h"
#include "manager.h"
#include "frame.h"
#include "macros.h"
#include "findinfilesdlg.h"
#include "findresultstab.h"
#include "replaceinfilespanel.h"

FindInFilesDialog::FindInFilesDialog(wxWindow* parent, wxWindowID id, const FindReplaceData& data)
		: FindInFilesDialogBase(parent, id)
		, m_data(data)
{
	// DirPicker values
	wxArrayString choices;
	choices.Add(SEARCH_IN_PROJECT);
	choices.Add(SEARCH_IN_WORKSPACE);
	choices.Add(SEARCH_IN_CURR_FILE_PROJECT);
	for (size_t i = 0; i < m_data.GetSearchPaths().GetCount(); ++i) {
		choices.Add(m_data.GetSearchPaths().Item(i));
	}
	m_dirPicker->SetValues(choices, 1);

	// Search for
	m_findString->Clear();
	m_findString->Append(m_data.GetFindStringArr());
	m_findString->SetValue(m_data.GetFindString());

	m_fileTypes->SetSelection(0);

	Connect(wxEVT_CHAR_HOOK, wxCharEventHandler(FindInFilesDialog::OnCharEvent));

	m_matchCase->SetValue(m_data.GetFlags() & wxFRD_MATCHCASE);
	m_matchWholeWord->SetValue(m_data.GetFlags() & wxFRD_MATCHWHOLEWORD);
	m_regualrExpression->SetValue(m_data.GetFlags() & wxFRD_REGULAREXPRESSION);
	m_printScope->SetValue(m_data.GetFlags() & wxFRD_DISPLAYSCOPE);
	m_checkBoxSaveFilesBeforeSearching->SetValue(m_data.GetFlags() & wxFRD_SAVE_BEFORE_SEARCH);

	// Set encoding
	wxArrayString  astrEncodings;
	wxFontEncoding fontEnc;
	int            selection(0);

	size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
	for (size_t i = 0; i < iEncCnt; i++) {
		fontEnc = wxFontMapper::GetEncoding(i);
		if (wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
			continue;
		}
		wxString encodingName = wxFontMapper::GetEncodingName(fontEnc);
		size_t pos = astrEncodings.Add(encodingName);

		if(data.GetEncoding() == encodingName)
			selection = static_cast<int>(pos);
	}

	m_choiceEncoding->Append(astrEncodings);
	if(m_choiceEncoding->IsEmpty() == false)
		m_choiceEncoding->SetSelection(selection);

	// Set the file mask
	wxString mask = m_data.GetFileMask();
	mask.Trim().Trim(false);
	if(mask.IsEmpty() == false) {
		m_fileTypes->Clear();
		wxArrayString fileTypes = wxStringTokenize(mask, wxT("\n"), wxTOKEN_STRTOK);
		for(size_t i=0; i<fileTypes.GetCount(); i++) {
			m_fileTypes->Append(fileTypes.Item(i));
		}
	}

	GetSizer()->Fit(this);
	Centre();
}

FindInFilesDialog::~FindInFilesDialog()
{
}

void FindInFilesDialog::SetSearchData(const SearchData &data)
{
	m_data.SetFindString(data.GetFindString());

	size_t flags = 0;
	flags |= data.IsMatchCase()         ? wxFRD_MATCHCASE         : 0;
	flags |= data.IsMatchWholeWord()    ? wxFRD_MATCHWHOLEWORD    : 0;
	flags |= data.IsRegularExpression() ? wxFRD_REGULAREXPRESSION : 0;
	flags |= data.GetDisplayScope()     ? wxFRD_DISPLAYSCOPE      : 0;
	m_data.SetFlags(flags);

	m_findString->SetValue(data.GetFindString());
	m_matchCase->SetValue(data.IsMatchCase());
	m_matchWholeWord->SetValue(data.IsMatchWholeWord());
	m_regualrExpression->SetValue(data.IsRegularExpression());

	// Set encoding
	int where = m_choiceEncoding->FindString(data.GetEncoding());

	if(where != wxNOT_FOUND) {
		m_choiceEncoding->SetSelection(where);

	} else {
		// try to locate the ISO-8859-1 encoding
		where = m_choiceEncoding->FindString(wxT("ISO-8859-1"));
		if(where != wxNOT_FOUND) {
			m_choiceEncoding->SetSelection(where);

		} else if(m_choiceEncoding->IsEmpty() == false) {
			m_choiceEncoding->SetSelection(0);
		}

	}

	m_printScope->SetValue(data.GetDisplayScope());
	m_fileTypes->SetValue(data.GetExtensions());

	m_listPaths->Clear();
	const wxArrayString& rootDirs = data.GetRootDirs();
	for (size_t i = 0; i < rootDirs.Count(); ++i) {
		m_listPaths->Append(rootDirs.Item(i));
	}
	if (rootDirs.IsEmpty() == false) {
		m_dirPicker->SetPath(rootDirs.Item(0));
	}
}

void FindInFilesDialog::SetRootDir(const wxString &rootDir)
{
	m_dirPicker->SetPath(rootDir);
}

void FindInFilesDialog::DoSearchReplace()
{
	SearchData data = DoGetSearchData();
	data.SetOwner(Frame::Get()->GetOutputPane()->GetReplaceResultsTab());

	DoSaveOpenFiles();
	SearchThreadST::Get()->PerformSearch(data);

	DoSaveSearchPaths();
	Hide();
}

void FindInFilesDialog::DoSearch()
{
	SearchData data = DoGetSearchData();
	data.SetOwner(Frame::Get()->GetOutputPane()->GetFindResultsTab());

	// check to see if we require to save the files
	DoSaveOpenFiles();
	SearchThreadST::Get()->PerformSearch(data);

	DoSaveSearchPaths();
	Hide();
}

SearchData FindInFilesDialog::DoGetSearchData()
{
	SearchData data;
	wxString findStr(m_data.GetFindString());
	if (m_findString->GetValue().IsEmpty() == false) {
		findStr = m_findString->GetValue();
	}

	data.SetFindString(findStr);
	data.SetMatchCase( (m_data.GetFlags() & wxFRD_MATCHCASE) != 0);
	data.SetMatchWholeWord((m_data.GetFlags() & wxFRD_MATCHWHOLEWORD) != 0);
	data.SetRegularExpression((m_data.GetFlags() & wxFRD_REGULAREXPRESSION) != 0);
	data.SetDisplayScope((m_data.GetFlags() & wxFRD_DISPLAYSCOPE) != 0);
	data.SetEncoding(m_choiceEncoding->GetStringSelection());

	wxArrayString rootDirs;
	for (size_t i = 0; i < m_listPaths->GetCount(); ++i) {
		rootDirs.push_back(m_listPaths->GetString(i));
	}
	if (rootDirs.IsEmpty()) {
		wxString dir = m_dirPicker->GetPath();
		if (dir.IsEmpty() == false) {
			rootDirs.push_back(dir);
		}
	}
	data.SetRootDirs(rootDirs);

	wxArrayString files;
	for (size_t i = 0; i < rootDirs.GetCount(); ++i) {
		const wxString& rootDir = rootDirs.Item(i);
		if (rootDir == SEARCH_IN_WORKSPACE) {

			ManagerST::Get()->GetWorkspaceFiles(files);

		} else if (rootDir == SEARCH_IN_PROJECT) {

			ManagerST::Get()->GetProjectFiles(ManagerST::Get()->GetActiveProjectName(), files);

		} else if (rootDir == SEARCH_IN_CURR_FILE_PROJECT) {

			wxString project = ManagerST::Get()->GetActiveProjectName();

			if (Frame::Get()->GetMainBook()->GetActiveEditor()) {
				// use the active file's project
				wxFileName activeFile = Frame::Get()->GetMainBook()->GetActiveEditor()->GetFileName();
				project = ManagerST::Get()->GetProjectNameByFile(activeFile.GetFullPath());
			}
			ManagerST::Get()->GetProjectFiles(project, files);
		}
	}
	data.SetFiles(files);

	data.UseNewTab(m_resInNewTab->GetValue());
	data.SetExtensions(m_fileTypes->GetValue());
	return data;
}

void FindInFilesDialog::OnClick(wxCommandEvent &event)
{
	wxObject *btnClicked = event.GetEventObject();
	size_t flags = m_data.GetFlags();

	wxString findWhat = m_findString->GetValue();
	findWhat = findWhat.Trim().Trim(false);

	m_data.SetFindString( m_findString->GetValue() );
	m_data.SetEncoding  ( m_choiceEncoding->GetStringSelection() );

	wxString fileMask;
	for(unsigned int i=0; i<m_fileTypes->GetCount(); i++) {
		fileMask << m_fileTypes->GetString(i) << wxT("\n");
	}
	if(fileMask.IsEmpty() == false)
		fileMask.RemoveLast();
	m_data.SetFileMask( fileMask );

	if (btnClicked == m_stop) {
		SearchThreadST::Get()->StopSearch();

	} else if (btnClicked == m_find) {
		if ( findWhat.IsEmpty() ) {
			return;
		}
		DoSearch();

	} else if (btnClicked == m_replaceAll) {
		if ( findWhat.IsEmpty() ) {
			return;
		}
		DoSearchReplace();

	} else if (btnClicked == m_cancel) {
		// Hide the dialog
		DoSaveSearchPaths();
		Hide();

	} else if (btnClicked == m_matchCase) {
		if (m_matchCase->IsChecked()) {
			flags |= wxFRD_MATCHCASE;
		} else {
			flags &= ~(wxFRD_MATCHCASE);
		}
	} else if (btnClicked == m_matchWholeWord) {
		if (m_matchWholeWord->IsChecked()) {
			flags |= wxFRD_MATCHWHOLEWORD;
		} else {
			flags &= ~(wxFRD_MATCHWHOLEWORD);
		}
	} else if (btnClicked == m_regualrExpression) {
		if (m_regualrExpression->IsChecked()) {
			flags |= wxFRD_REGULAREXPRESSION;
		} else {
			flags &= ~(wxFRD_REGULAREXPRESSION);
		}
	} else if (btnClicked == m_printScope) {
		if (m_printScope->IsChecked()) {
			flags |= wxFRD_DISPLAYSCOPE;
		} else {
			flags &= ~(wxFRD_DISPLAYSCOPE);
		}
	} else if (btnClicked == m_checkBoxSaveFilesBeforeSearching) {
		if (m_checkBoxSaveFilesBeforeSearching->IsChecked()) {
			flags |= wxFRD_SAVE_BEFORE_SEARCH;
		} else {
			flags &= ~(wxFRD_SAVE_BEFORE_SEARCH);
		}
	}

	// Set the updated flags
	m_data.SetFlags(flags);
}

void FindInFilesDialog::OnClose(wxCloseEvent &e)
{
	wxUnusedVar(e);
	DoSaveSearchPaths();
	Hide();
}

void FindInFilesDialog::OnCharEvent(wxKeyEvent &event)
{
	if (event.GetKeyCode() == WXK_ESCAPE) {
		Hide();
		return;
	} else if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
		m_data.SetFindString( m_findString->GetValue() );
		DoSearch();
		return;
	}
	event.Skip();
}

void FindInFilesDialog::OnAddPath( wxCommandEvent& event )
{
	wxString path = m_dirPicker->GetPath();
	if (m_listPaths->FindString(path) == wxNOT_FOUND) {
		m_listPaths->Append(path);
	}
}

void FindInFilesDialog::OnRemovePath( wxCommandEvent& event )
{
	int sel = m_listPaths->GetSelection();
	if (sel != wxNOT_FOUND) {
		m_listPaths->Delete(sel);
	}
}

void FindInFilesDialog::OnClearPaths( wxCommandEvent& event )
{
	m_listPaths->Clear();
}

bool FindInFilesDialog::Show()
{
	bool res = IsShown() || wxDialog::Show();
	if (res) {

		// update the combobox
		m_findString->Clear();
		m_findString->Append(m_data.GetFindStringArr());
		m_findString->SetValue(m_data.GetFindString());

		LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
		if (editor) {
			//if we have an open editor, and a selected text, make this text the search string
			wxString selText = editor->GetSelectedText();
			if (!selText.IsEmpty()) {
				m_findString->SetValue(selText);
			}
		}

		m_findString->SetSelection(-1, -1); // select all
		m_findString->SetFocus();
	}
	return res;
}

void FindInFilesDialog::DoSaveSearchPaths()
{
	wxArrayString paths = m_dirPicker->GetValues();
	int where = paths.Index(SEARCH_IN_PROJECT);
	if (where != wxNOT_FOUND) {
		paths.RemoveAt(where);
	}
	where = paths.Index(SEARCH_IN_WORKSPACE);
	if (where != wxNOT_FOUND) {
		paths.RemoveAt(where);
	}
	where = paths.Index(SEARCH_IN_CURR_FILE_PROJECT);
	if (where != wxNOT_FOUND) {
		paths.RemoveAt(where);
	}

	m_data.SetSearchPaths(paths);
}

void FindInFilesDialog::DoSaveOpenFiles()
{
	if (m_checkBoxSaveFilesBeforeSearching->IsChecked()) {
		Frame::Get()->GetMainBook()->SaveAll(false, false);
	}
}

void FindInFilesDialog::OnClearPathsUI(wxUpdateUIEvent& event)
{
	event.Enable(m_listPaths->IsEmpty() == false);
}

void FindInFilesDialog::OnRemovePathUI(wxUpdateUIEvent& event)
{
	event.Enable(m_listPaths->GetSelection() != wxNOT_FOUND);
}

void FindInFilesDialog::OnFindWhatUI(wxUpdateUIEvent& event)
{
	event.Enable(m_findString->GetValue().IsEmpty() == false);
}
