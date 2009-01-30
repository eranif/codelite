//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : svndlg.cpp
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
#include "svndlg.h"
#include "wx/tokenzr.h"
#include "windowattrmanager.h"
#include "svncommitmsgsmgr.h"
#include "imanager.h"

static void EscapeComment(wxString &comment)
{
	// first remove the comment section of the text
	wxStringTokenizer tok(comment, wxT("\n"), wxTOKEN_STRTOK);
	comment.Clear();
	while (tok.HasMoreTokens()) {
		wxString line = tok.GetNextToken();
		line = line.Trim().Trim(false);
		if (!line.StartsWith(wxT("#"))) {
			comment << line << wxT("\n");
		}
	}

	// SVN does not like any quotation marks in the comment -> escape them
	comment.Replace(wxT("\""), wxT("\\\""));
}

SvnDlg::SvnDlg( wxWindow* parent, const wxArrayString &files, IManager *manager )
		: SvnBaseDlg( parent )
		, m_files(files)
		, m_manager(manager)
{
	wxArrayString msgs;

	SvnCommitMsgsMgr::Instance()->GetAllMessages( msgs );
	m_comboBoxLastCommitMsgs->Append( msgs );

	if ( msgs.GetCount() > 0 ) {
		m_comboBoxLastCommitMsgs->SetSelection( msgs.GetCount()-1 );
	}
	m_textCtrl->SetFocus();

	// populate the check list
	for (size_t i=0; i<m_files.GetCount(); i++) {
		wxFileName fn(m_files.Item(i));
		m_checkList->Append(fn.GetFullName());
		m_checkList->Check(i);
	}
	WindowAttrManager::Load(this, wxT("SvnLog"), m_manager->GetConfigTool());
}

SvnDlg::~SvnDlg()
{
	WindowAttrManager::Save(this, wxT("SvnLog"), m_manager->GetConfigTool());
}

void SvnDlg::OnLastCommitMsgSelected(wxCommandEvent &e)
{
	m_textCtrl->AppendText( e.GetString() );
}

void SvnDlg::OnButtonOK(wxCommandEvent &e)
{
	wxString comment = m_textCtrl->GetValue();
	EscapeComment( comment );

	SvnCommitMsgsMgr::Instance()->AddMessage( comment );

	EndModal( wxID_OK );
}

wxString SvnDlg::GetLogMessage() const
{
	wxString comment( m_textCtrl->GetValue() );
	EscapeComment(comment);
	return comment;
}

wxArrayString SvnDlg::GetFiles() const
{
	wxArrayString files;
	for (unsigned int i = 0; i<m_checkList->GetCount(); i++) {
		if (m_checkList->IsChecked(i)) {
			if(i < m_files.GetCount()) {
				files.Add(m_files.Item(i));
			}
		}
	}
	return files;
}

void SvnDlg::SetLogMessage(const wxString& message)
{
	m_textCtrl->SetValue(message);
	m_textCtrl->SelectAll();
}

void SvnDlg::OnItemSelected(wxCommandEvent& e)
{
	wxUnusedVar(e);
	int where = m_checkList->GetSelection();
	if(where != wxNOT_FOUND){
		if(where >=0 && where < m_files.GetCount()){
			m_staticTextFileFullPath->SetLabel(m_files.Item(where));
		}
	}
}
