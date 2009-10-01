//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : open_resouce_dlg.cpp
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
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "editor_config.h"

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "open_resouce_dlg.h"
#include "macros.h"
#include "manager.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(OpenResourceDlg, wxDialog)
	EVT_CHAR_HOOK(OpenResourceDlg::OnCharHook)
END_EVENT_TABLE()

OpenResourceDlg::OpenResourceDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style )
		: wxDialog( parent, id, title, pos, size, style )
{
	m_timer = new wxTimer(this);
	m_timer->Start(100);

	//load all files from the workspace
	ManagerST::Get()->GetWorkspaceFiles(m_files, true);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxVERTICAL );

	m_staticTitle = new wxStaticText( mainPanel, wxID_ANY, wxT("Find Resource (wildcards are allowed):"), wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( m_staticTitle, 0, wxALL, 5 );

	m_textResourceName = new wxTextCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	panelSizer->Add( m_textResourceName, 0, wxALL|wxEXPAND, 5 );

	wxStaticText *label = new wxStaticText( mainPanel, wxID_ANY, wxT("Matched resources:"), wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( label, 0, wxALL, 5 );

	m_listShortNames = new wxListBox( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	panelSizer->Add( m_listShortNames, 1, wxALL|wxEXPAND, 5 );

	m_fullText = new wxStaticText( mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( m_fullText, 0, wxALL, 5 );

	m_checkBoxPartialMatching = new wxCheckBox( mainPanel, wxID_ANY, wxT("Allow partial matching"), wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( m_checkBoxPartialMatching, 0, wxALL|wxEXPAND, 5 );

	long v(0);
	EditorConfigST::Get()->GetLongValue(wxT("OpenResourceAllowsPartialMatch"), v);
	m_checkBoxPartialMatching->SetValue(v == 1 ? true : false);

	mainPanel->SetSizer( panelSizer );
	mainPanel->Layout();
	panelSizer->Fit( mainPanel );
	mainSizer->Add( mainPanel, 1, wxEXPAND | wxALL, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );

	m_btnOk = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_btnOk, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_button2, 0, wxALL, 5 );

	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );

	this->SetSizer( mainSizer );
	this->Layout();
	ConnectEvents();
	Centre();
	m_textResourceName->SetFocus();
}

OpenResourceDlg::~OpenResourceDlg()
{
	m_timer->Stop();
	delete m_timer;
}

void OpenResourceDlg::ConnectEvents()
{
	ConnectButton(m_button2, OpenResourceDlg::OnButtonCancel);
	ConnectButton(m_btnOk, OpenResourceDlg::OnButtonOK);
	m_listShortNames->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(OpenResourceDlg::OnItemSelected), NULL, this);
	m_listShortNames->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(OpenResourceDlg::OnItemActivated), NULL, this);
	Connect(m_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(OpenResourceDlg::OnTimer), NULL, this);
}

void OpenResourceDlg::OnTimer(wxTimerEvent &event)
{
	wxUnusedVar(event);
	wxArrayString tmpArr;
	wxString curSel = m_textResourceName->GetValue();
	if (!curSel.Trim().Trim(false).IsEmpty()) {

		curSel = curSel.MakeLower().Trim().Trim(false);

		for (size_t i=0; i<m_files.size(); i++) {
			wxString fileName(m_files.at(i).GetFullName());
			fileName= fileName.MakeLower();

			//append wildcard at the end
			if (!curSel.EndsWith(wxT("*"))) {
				curSel << wxT("*");
			}

			// FR# [2008133]
			if (m_checkBoxPartialMatching->IsChecked() && !curSel.StartsWith(wxT("*"))) {
				curSel.Prepend(wxT("*"));
			}

			if (wxMatchWild(curSel, fileName)) {
				tmpArr.Add(m_files[i].GetFullPath());
			}
		}
	}

	// actValues represents the list of files as they appear in the ListBox
	// we need to loop over the list box, since the actual full file names
	// are stored as ClientObject
	wxArrayString actValues;
	for (unsigned int i=0; i<m_listShortNames->GetCount(); i++) {
		wxStringClientData *data = (wxStringClientData *)m_listShortNames->GetClientObject(i);
		actValues.Add(data->GetData());
	}
	// we need to sort the array
	actValues.Sort();

	// tmpArr contains the list of files (full paths) after filtering the typed pattern
	tmpArr.Sort();

	if (tmpArr == actValues)
		return;

	//change was done, update the file list
	Freeze();
	m_listShortNames->Clear();
	for (size_t i=0; i<tmpArr.GetCount(); i++) {
		wxFileName fn(tmpArr.Item(i));
		m_listShortNames->Append(fn.GetFullName(), new wxStringClientData(tmpArr.Item(i)));
	}
	Thaw();
	if (m_listShortNames->GetCount() > 0) {
		m_listShortNames->Select(0);
		// display the full name at the bottom static text control
		wxStringClientData *data = (wxStringClientData *)m_listShortNames->GetClientObject(0);
		m_fullText->SetLabel(data->GetData());
	}
}

void OpenResourceDlg::OnButtonCancel(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void OpenResourceDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (UpdateFileName()) {
		EndModal(wxID_OK);
	}
}

void OpenResourceDlg::OnItemSelected(wxCommandEvent &event)
{
	if (event.GetSelection() == wxNOT_FOUND) {
		// This would otherwise cause a wx assertion
		return;
	}

	wxStringClientData *data = (wxStringClientData *)m_listShortNames->GetClientObject(event.GetSelection());
	if(data) {
	m_fullText->SetLabel(data->GetData());
	UpdateFileName();
	}
}

void OpenResourceDlg::OnItemActivated(wxCommandEvent &event)
{
	wxUnusedVar(event);

	if (UpdateFileName()) {
		EndModal(wxID_OK);
	}
}

bool OpenResourceDlg::UpdateFileName()
{
	int idx = m_listShortNames->GetSelection();
	if (idx == wxNOT_FOUND) {
		return false;
	}

	m_fileName = m_fullText->GetLabelText();

	// save the checkbox value into the settings
	EditorConfigST::Get()->SaveLongValue(wxT("OpenResourceAllowsPartialMatch"), m_checkBoxPartialMatching->IsChecked() ? 1 : 0);

	return true;
}

void OpenResourceDlg::OnCharHook(wxKeyEvent &event)
{
	if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
		if (m_listShortNames->GetSelection() != wxNOT_FOUND) {
			if (UpdateFileName()) {
				EndModal(wxID_OK);
				return;
			}
		} else if (m_listShortNames->GetCount() == 1) {
			m_listShortNames->SetSelection(0);
			if (UpdateFileName()) {
				EndModal(wxID_OK);
				return;
			}
		}
	} else if (event.GetKeyCode() == WXK_DOWN && m_listShortNames->GetCount() > 0) {
		//up key
		int cursel = m_listShortNames->GetSelection();
		if (cursel != wxNOT_FOUND) {
			//there is a selection in the listbox
			cursel++;
			if (cursel >= (int)m_listShortNames->GetCount()) {
				//already at last item, cant scroll anymore
				return;
			}
			m_listShortNames->SetSelection(cursel);
			m_listShortNames->Select(cursel);
			m_listShortNames->SetFirstItem(cursel);

			// display the full name at the bottom static text control
			wxStringClientData *data = (wxStringClientData *)m_listShortNames->GetClientObject(cursel);
			m_fullText->SetLabel(data->GetData());

		} else {
			//no selection is made
			m_listShortNames->SetSelection(0);
			m_listShortNames->Select(0);

			// display the full name at the bottom static text control
			wxStringClientData *data = (wxStringClientData *)m_listShortNames->GetClientObject(0);
			m_fullText->SetLabel(data->GetData());

			m_listShortNames->SetFirstItem(0);
		}
		return;
	} else if (event.GetKeyCode() == WXK_UP && m_listShortNames->GetCount() > 0) {
		//up key
		int cursel = m_listShortNames->GetSelection();
		if (cursel != wxNOT_FOUND) {
			//there is a selection in the listbox
			cursel--;
			if (cursel < 0) {
				//already at first item, cant scroll anymore
				return;
			}
			m_listShortNames->SetSelection(cursel);
			m_listShortNames->SetFirstItem(cursel);

			// display the full name at the bottom static text control
			wxStringClientData *data = (wxStringClientData *)m_listShortNames->GetClientObject(cursel);
			m_fullText->SetLabel(data->GetData());
		} else {
			//no selection is made
			m_listShortNames->SetSelection(0);
			m_listShortNames->SetFirstItem(0);

			// display the full name at the bottom static text control
			wxStringClientData *data = (wxStringClientData *)m_listShortNames->GetClientObject(0);
			m_fullText->SetLabel(data->GetData());
		}
		return;
	}
	event.Skip();
}
