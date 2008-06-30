///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "symbols_dialog_base.h"

///////////////////////////////////////////////////////////////////////////

SymbolsDialogBase::SymbolsDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_results = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	mainSizer->Add( m_results, 1, wxALL|wxEXPAND, 5 );
	
	statLine = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( statLine, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_okButton = new wxButton( this, wxID_ANY, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_okButton, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_cancel = new wxButton( this, wxID_ANY, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_cancel, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_results->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( SymbolsDialogBase::OnItemActivated ), NULL, this );
	m_results->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SymbolsDialogBase::OnItemSelected ), NULL, this );
	m_okButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SymbolsDialogBase::OnButtonOK ), NULL, this );
	m_cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SymbolsDialogBase::OnButtonCancel ), NULL, this );
}

SymbolsDialogBase::~SymbolsDialogBase()
{
	// Disconnect Events
	m_results->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( SymbolsDialogBase::OnItemActivated ), NULL, this );
	m_results->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SymbolsDialogBase::OnItemSelected ), NULL, this );
	m_okButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SymbolsDialogBase::OnButtonOK ), NULL, this );
	m_cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SymbolsDialogBase::OnButtonCancel ), NULL, this );
}
