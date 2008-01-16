///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "listctrlpanelbase.h"

///////////////////////////////////////////////////////////////////////////

ListCtrlPanelBase::ListCtrlPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_listCtrl = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	mainSizer->Add( m_listCtrl, 1, wxALL|wxEXPAND, 1 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ListCtrlPanelBase::OnItemActivated ), NULL, this );
}
