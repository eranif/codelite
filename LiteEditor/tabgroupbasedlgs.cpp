//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : tabgroupbasedlgs.cpp
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
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "tabgroupbasedlgs.h"

///////////////////////////////////////////////////////////////////////////

LoadTabGroupBaseDlg::LoadTabGroupBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainsizer;
	mainsizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Select a tab group, or browse for one"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer3->Add( m_staticText1, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_buttonBrowse = new wxButton( this, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonBrowse->SetToolTip( _("Click to search for other groups") );

	bSizer3->Add( m_buttonBrowse, 0, wxALL, 5 );

	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );

	m_listBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_listBox->SetToolTip( _("Double-click to choose one of these groups") );
	m_listBox->SetMinSize( wxSize( 450,100 ) );

	bSizer2->Add( m_listBox, 1, wxALL|wxEXPAND, 5 );

	m_replaceCheck = new wxCheckBox( this, wxID_ANY, _("Replace current tabs"), wxDefaultPosition, wxDefaultSize, 0 );
	m_replaceCheck->SetToolTip( _("By default, the tabs of this group will be added to the current tabs. Tick this to replace the current tabs instead.") );

	bSizer2->Add( m_replaceCheck, 0, wxALL, 5 );

	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer2->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );

	m_button6 = new wxButton( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button6->SetDefault();
	bSizer11->Add( m_button6, 0, wxALL, 5 );

	m_button5 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_button5, 0, wxALL, 5 );

	bSizer2->Add( bSizer11, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	mainsizer->Add( bSizer2, 1, wxEXPAND|wxALL, 5 );

	this->SetSizer( mainsizer );
	this->Layout();
	mainsizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_buttonBrowse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LoadTabGroupBaseDlg::OnBrowse ), NULL, this );
	m_listBox->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( LoadTabGroupBaseDlg::OnItemActivated ), NULL, this );
}

LoadTabGroupBaseDlg::~LoadTabGroupBaseDlg()
{
	// Disconnect Events
	m_buttonBrowse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LoadTabGroupBaseDlg::OnBrowse ), NULL, this );
	m_listBox->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( LoadTabGroupBaseDlg::OnItemActivated ), NULL, this );

}

SaveTabGroupBaseDlg::SaveTabGroupBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainsizer;
	mainsizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Select which tabs you want to be in the group"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	wxArrayString m_ListTabsChoices;
	m_ListTabs = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ListTabsChoices, 0 );
	m_ListTabs->SetMinSize( wxSize( 400,-1 ) );

	bSizer3->Add( m_ListTabs, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	m_buttonCheckAll = new wxButton( this, wxID_ANY, _("Check All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCheckAll->SetToolTip( _("Click to select all items") );

	bSizer13->Add( m_buttonCheckAll, 0, wxALL, 5 );

	m_buttonClearAll = new wxButton( this, wxID_ANY, _("Clear All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonClearAll->SetToolTip( _("Click to clear all items") );

	bSizer13->Add( m_buttonClearAll, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	bSizer3->Add( bSizer13, 0, wxALL, 5 );

	bSizer1->Add( bSizer3, 1, wxEXPAND|wxALL, 0 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Choose a name for the group:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer10->Add( m_staticText4, 0, wxALIGN_LEFT|wxALL, 5 );

	m_textName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_textName, 0, wxALIGN_CENTER|wxEXPAND|wxALL, 5 );

	bSizer1->Add( bSizer10, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 5 );

	m_staticline11 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline11, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );

	m_button6 = new wxButton( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button6->SetDefault();
	bSizer11->Add( m_button6, 0, wxALL, 5 );

	m_button5 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_button5, 0, wxALL, 5 );

	bSizer1->Add( bSizer11, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	mainsizer->Add( bSizer1, 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	this->SetSizer( mainsizer );
	this->Layout();
	mainsizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_buttonCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SaveTabGroupBaseDlg::OnCheckAll ), NULL, this );
	m_buttonCheckAll->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SaveTabGroupBaseDlg::OnCheckAllUpdateUI ), NULL, this );
	m_buttonClearAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SaveTabGroupBaseDlg::OnClearAll ), NULL, this );
	m_buttonClearAll->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SaveTabGroupBaseDlg::OnClearAllUpdateUI ), NULL, this );
}

SaveTabGroupBaseDlg::~SaveTabGroupBaseDlg()
{
	// Disconnect Events
	m_buttonCheckAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SaveTabGroupBaseDlg::OnCheckAll ), NULL, this );
	m_buttonCheckAll->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SaveTabGroupBaseDlg::OnCheckAllUpdateUI ), NULL, this );
	m_buttonClearAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SaveTabGroupBaseDlg::OnClearAll ), NULL, this );
	m_buttonClearAll->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SaveTabGroupBaseDlg::OnClearAllUpdateUI ), NULL, this );

}
