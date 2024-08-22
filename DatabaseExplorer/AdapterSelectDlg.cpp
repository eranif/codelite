//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : AdapterSelectDlg.cpp
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

#include "AdapterSelectDlg.h"
#include "ErdPanel.h"

#include <wx/wxxmlserializer/XmlSerializer.h>


AdapterSelectDlg::AdapterSelectDlg(wxWindow* parent, wxWindow* pParentBook, IManager* pManager , xsSerializable* pConnections) : _AdapterSelectDlg(parent) {
	m_pParentBook = pParentBook;
	m_pManager = pManager;
	m_pConnectons = pConnections;
}

AdapterSelectDlg::~AdapterSelectDlg() {
}

void AdapterSelectDlg::OnMysqlClick(wxCommandEvent& event) {
//#ifdef DBL_USE_MYSQL
	m_pManager->AddEditorPage(new ErdPanel(m_pParentBook, new MySqlDbAdapter(), m_pConnectons),_("MySQL ERD"));
	Close();
//#else
//	wxMessageBox( _("MySQL ERD is not supported."), _("DB Error"), wxOK | wxICON_WARNING );
//#endif
}
void AdapterSelectDlg::OnSqliteClick(wxCommandEvent& event) {
//#ifdef DBL_USE_SQLITE
	m_pManager->AddEditorPage(new ErdPanel(m_pParentBook, new SQLiteDbAdapter(), m_pConnectons),_("SQLite ERD"));
	Close();
//#else
//	wxMessageBox( _("SQLite ERD is not supported."), _("DB Error"), wxOK | wxICON_WARNING );
//#endif
}
void AdapterSelectDlg::OnPostgresClick(wxCommandEvent& event) {
//#ifdef DBL_USE_POSTGRES
	m_pManager->AddEditorPage(new ErdPanel(m_pParentBook, new PostgreSqlDbAdapter(), m_pConnectons),_("PostgreSQL ERD"));
	Close();
//#else
//	wxMessageBox( _("PostgreSQL ERD is not supported."), _("DB Error"), wxOK | wxICON_WARNING );
//#endif
}
