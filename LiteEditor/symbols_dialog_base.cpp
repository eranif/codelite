///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
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
BEGIN_EVENT_TABLE( SymbolsDialogBase, wxDialog )
	EVT_LIST_ITEM_ACTIVATED( wxID_ANY, SymbolsDialogBase::_wxFB_OnItemActivated )
	EVT_LIST_ITEM_SELECTED( wxID_ANY, SymbolsDialogBase::_wxFB_OnItemSelected )
	EVT_BUTTON( wxID_OK, SymbolsDialogBase::_wxFB_OnButtonOK )
	EVT_BUTTON( wxID_CANCEL, SymbolsDialogBase::_wxFB_OnButtonCancel )
END_EVENT_TABLE()

SymbolsDialogBase::SymbolsDialogBase( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_results = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	mainSizer->Add( m_results, 1, wxALL|wxEXPAND, 5 );
	
	statLine = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( statLine, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_okButton = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_okButton, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_cancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_cancel, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}
