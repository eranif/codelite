///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cc_boxbase.h"

///////////////////////////////////////////////////////////////////////////

CCBoxBase::CCBoxBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_listCtrl = new CCVirtualListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxBORDER_SIMPLE|wxLC_VIRTUAL );
	bSizer1->Add( m_listCtrl, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CCBoxBase::OnItemActivated ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CCBoxBase::OnItemDeSelected ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CCBoxBase::OnItemSelected ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( CCBoxBase::OnKeyDown ), NULL, this );
}

CCBoxBase::~CCBoxBase()
{
	// Disconnect Events
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CCBoxBase::OnItemActivated ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CCBoxBase::OnItemDeSelected ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CCBoxBase::OnItemSelected ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( CCBoxBase::OnKeyDown ), NULL, this );
}
