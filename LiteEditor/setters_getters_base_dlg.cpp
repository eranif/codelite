//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : setters_getters_base_dlg.cpp
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
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "setters_getters_base_dlg.h"
#include "settersgetterstreectrl.h"

///////////////////////////////////////////////////////////////////////////

SettersGettersBaseDlg::SettersGettersBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Class Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_textClassName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer3->Add( m_textClassName, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Select members from the list below :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer3->Add( m_staticText3, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	wxArrayString m_checkListMembersChoices;
	m_checkListMembers = new SettersGettersTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_SINGLE|wxTR_HIDE_ROOT );
	m_checkListMembers->AddRoot(_("Root"), false);

	bSizer4->Add( m_checkListMembers, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_buttonCheckAll = new wxButton( this, wxID_ANY, _("Check &All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCheckAll->SetDefault();
	bSizer5->Add( m_buttonCheckAll, 0, wxALL, 5 );

	m_buttonUncheckAll = new wxButton( this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonUncheckAll, 0, wxALL, 5 );

	bSizer4->Add( bSizer5, 0, wxEXPAND, 5 );

	bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );

//	m_staticText31 = new wxStaticText( this, wxID_ANY, wxT("Preview:"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_staticText31->Wrap( -1 );
//	bSizer3->Add( m_staticText31, 0, wxALL, 5 );

//	m_textPreview = new LEditor(this);
//	m_textPreview->SetUseTabs( false );
//	m_textPreview->SetTabWidth( 4 );
//	m_textPreview->SetIndent( 4 );
//	m_textPreview->SetTabIndents( false );
//	m_textPreview->SetBackSpaceUnIndents( false );
//	m_textPreview->SetViewEOL( false );
//	m_textPreview->SetViewWhiteSpace( false );
//	m_textPreview->SetMarginWidth( 2, 0 );
//	m_textPreview->SetIndentationGuides( false );
//	m_textPreview->SetMarginWidth( 1, 0 );
//	m_textPreview->SetMarginWidth( 0, 0 );
//	m_textPreview->MarkerDefine( wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS );
//	m_textPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
//	m_textPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
//	m_textPreview->MarkerDefine( wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS );
//	m_textPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
//	m_textPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
//	m_textPreview->MarkerDefine( wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_EMPTY );
//	m_textPreview->MarkerDefine( wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUS );
//	m_textPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
//	m_textPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
//	m_textPreview->MarkerDefine( wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUS );
//	m_textPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
//	m_textPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
//	m_textPreview->MarkerDefine( wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_EMPTY );
//	m_textPreview->MarkerDefine( wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_EMPTY );
//	m_textPreview->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
//	m_textPreview->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
//	bSizer3->Add( m_textPreview, 1, wxEXPAND | wxALL, 5 );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Options:") ), wxHORIZONTAL );

	m_checkStartWithUppercase = new wxCheckBox( this, wxID_ANY, _("Function name starts with upper case letter"), wxDefaultPosition, wxDefaultSize, 0 );

	sbSizer1->Add( m_checkStartWithUppercase, 0, wxALL, 5 );

	m_checkFormat = new wxCheckBox( this, wxID_ANY, _("Format text after insertion"), wxDefaultPosition, wxDefaultSize, 0 );

	sbSizer1->Add( m_checkFormat, 0, wxALL, 5 );

	bSizer3->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );

	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOK = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonOK, 0, wxALL, 5 );
	m_buttonOK->SetDefault();

	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );

	bSizer1->Add( bSizer2, 0, wxALIGN_RIGHT, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	m_buttonCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettersGettersBaseDlg::OnCheckAll ), NULL, this );
	m_buttonUncheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettersGettersBaseDlg::OnUncheckAll ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettersGettersBaseDlg::OnButtonOk ), NULL, this );
}
