//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : envvar_table.cpp
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

#include "precompiled_header.h"
#include "envvar_table.h"
#include "evnvarlist.h"
#include "environmentconfig.h"
#include "windowattrmanager.h"
#include "window_locker.h"
#include <wx/regex.h>

EnvVarsTableDlg::EnvVarsTableDlg( wxWindow* parent )
	: EnvVarsTableDlgBase( parent )
{
	EvnVarList vars;
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
	std::map<wxString, wxString> envSets = vars.GetEnvVarSets();
	wxString activePage = vars.GetActiveSet();

	wxStyledTextCtrl *sci = m_textCtrlDefault;
	sci->StyleClearAll();
	sci->SetLexer(wxSTC_LEX_NULL);

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	for (int i=0; i<=wxSTC_STYLE_DEFAULT; i++) {
		sci->StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		sci->StyleSetForeground(i, *wxBLACK);
		sci->StyleSetFont(i, font);
		sci->SetWrapMode(wxSTC_WRAP_WORD);
	}

	WindowAttrManager::Load(this, wxT("EnvVarsTableDlg"), NULL);
	std::map<wxString, wxString>::iterator iter = envSets.begin();
	for (; iter != envSets.end(); iter++) {
		wxString name    = iter->first;
		wxString content = iter->second;

		if (name == wxT("Default")) {
			m_textCtrlDefault->SetText(content);
		} else {
			DoAddPage(name, content, false);
		}
	}

	m_notebook1->SetSelection(0);
	for (size_t i=0; i<m_notebook1->GetPageCount(); i++) {
		if (m_notebook1->GetPageText(i) == activePage) {
			m_notebook1->GetPage(i)->SetFocus();
			m_notebook1->SetSelection(i);
			break;
		}
	}
}

EnvVarsTableDlg::~EnvVarsTableDlg()
{
	WindowAttrManager::Save(this, wxT("EnvVarsTableDlg"), NULL);
}

void EnvVarsTableDlg::OnLeftUp( wxMouseEvent& event )
{
}

void EnvVarsTableDlg::OnButtonOk( wxCommandEvent& event )
{
	EvnVarList vars;

	std::map<wxString, wxString> envSets;

	wxString content = m_textCtrlDefault->GetText();
	wxString name    = wxT("Default");
	content.Trim().Trim(false);
	envSets[name] = content;

	for (size_t i=1; i<m_notebook1->GetPageCount(); i++) {
		if (i == (size_t)m_notebook1->GetSelection()) {
			vars.SetActiveSet(m_notebook1->GetPageText(i));
		}

		EnvVarSetPage *page = (EnvVarSetPage*) m_notebook1->GetPage(i);
		wxString content = page->m_textCtrl->GetText();
		wxString name    = m_notebook1->GetPageText(i);
		content.Trim().Trim(false);
		envSets[name] = content;
	}
	vars.SetEnvVarSets(envSets);
	EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
	event.Skip();
}

void EnvVarsTableDlg::OnNewSet( wxCommandEvent& event )
{
	wxString name = wxGetTextFromUser(wxT("Enter Name:"));
	if(name.IsEmpty())
		return;

	DoAddPage(name, wxT(""), false);
}

void EnvVarsTableDlg::OnDeleteSet(wxCommandEvent& event)
{
	wxUnusedVar(event);

	int selection = m_notebook1->GetSelection();
	if(selection == wxNOT_FOUND)
		return;

	wxString name = m_notebook1->GetPageText((size_t)selection);
	if(wxMessageBox(wxString::Format(wxT("Are you sure you want to delete '%s' environment variables set?"), name.c_str()), wxT("Confirm"), wxYES_NO|wxICON_QUESTION) != wxYES)
		return;
	m_notebook1->DeletePage((size_t)selection);
}

void EnvVarsTableDlg::OnDeleteSetUI(wxUpdateUIEvent& event)
{
	int i = m_notebook1->GetSelection();
	event.Enable(i != wxNOT_FOUND && m_notebook1->GetPageText(i) != wxT("Default"));
}

void EnvVarsTableDlg::DoAddPage(const wxString &name, const wxString &content, bool select)
{
	clWindowUpdateLocker locker(this);
	EnvVarSetPage *page = new EnvVarSetPage(m_notebook1, wxID_ANY, wxDefaultPosition, wxSize(0,0));
	wxStyledTextCtrl *sci = page->m_textCtrl;
	sci->StyleClearAll();
	sci->SetLexer(wxSTC_LEX_NULL);

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	for (int i=0; i<=wxSTC_STYLE_DEFAULT; i++) {
		sci->StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		sci->StyleSetForeground(i, *wxBLACK);
		sci->StyleSetFont(i, font);
	}
	sci->SetText(content);
	m_notebook1->AddPage(page, name, select);
}

void EnvVarsTableDlg::OnExport(wxCommandEvent& event)
{
	int selection = m_notebook1->GetSelection();
	if(selection == wxNOT_FOUND)
		return;

#ifdef __WXMSW__
	bool isWindows = true;
#else
	bool isWindows = false;
#endif

	wxString text;
	if(selection == 0) {
		text = m_textCtrlDefault->GetText();
	} else {
		EnvVarSetPage *page = dynamic_cast<EnvVarSetPage*>(m_notebook1->GetPage((size_t)selection));
		if(page) {
			text = page->m_textCtrl->GetText();
		}
	}
	
	if(text.IsEmpty())
		return;
	
	wxArrayString lines = wxStringTokenize(text, wxT("\r\n"), wxTOKEN_STRTOK);
	wxString envfile;
	if(isWindows) {
		envfile << wxT("environment.bat");
	} else {
		envfile << wxT("environment");
	}

	wxFileName fn(wxGetCwd(), envfile);
	wxFFile fp(fn.GetFullPath(), wxT("w+b"));
	if(fp.IsOpened() == false) {
		wxMessageBox(wxString::Format(_("Failed to open file: '%s' for write"), fn.GetFullPath().c_str()), wxT("CodeLite"), wxOK|wxCENTER|wxICON_WARNING);
		return;
	}

	for(size_t i=0; i<lines.GetCount(); i++) {

		wxString sLine = lines.Item(i).Trim().Trim(false);
		if(sLine.IsEmpty())
			continue;

		static wxRegEx reVarPattern(wxT("\\$\\(( *)([a-zA-Z0-9_]+)( *)\\)"));
		if(isWindows) {
			while(reVarPattern.Matches(sLine)) {
				wxString varName = reVarPattern.GetMatch(sLine, 2);
				wxString text    = reVarPattern.GetMatch(sLine);
				sLine.Replace(text, wxString::Format(wxT("%%%s%%"), varName.c_str()));
			}
			sLine.Prepend(wxT("set "));
			sLine.Append(wxT("\r\n"));
			
		} else {
			while(reVarPattern.Matches(sLine)) {
				wxString varName = reVarPattern.GetMatch(sLine, 2);
				wxString text    = reVarPattern.GetMatch(sLine);
				sLine.Replace(text, wxString::Format(wxT("$%s"), varName.c_str()));
			}
			sLine.Prepend(wxT("export "));
			sLine.Append(wxT("\n"));
		}
		fp.Write(sLine);
	}

	wxMessageBox(wxString::Format(_("Environment exported to: '%s' successfully"), fn.GetFullPath().c_str()), wxT("CodeLite"));
}
