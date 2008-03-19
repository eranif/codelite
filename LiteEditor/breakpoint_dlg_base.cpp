///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 20 2008)
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

#include "breakpoint_dlg_base.h"

///////////////////////////////////////////////////////////////////////////

BreakpointTab::BreakpointTab( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrlBreakpoints = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer5->Add( m_listCtrlBreakpoints, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonDelete = new wxButton( this, wxID_ANY, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonDelete, 0, wxALL, 5 );
	
	m_buttonDeleteAll = new wxButton( this, wxID_ANY, wxT("Delete &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonDeleteAll, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	
	// Connect Events
	m_listCtrlBreakpoints->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( BreakpointTab::OnItemActivated ), NULL, this );
	m_listCtrlBreakpoints->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( BreakpointTab::OnItemDeselected ), NULL, this );
	m_listCtrlBreakpoints->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( BreakpointTab::OnItemSelected ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnDelete ), NULL, this );
	m_buttonDeleteAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnDeleteAll ), NULL, this );
}

BreakpointTab::~BreakpointTab()
{
	// Disconnect Events
	m_listCtrlBreakpoints->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( BreakpointTab::OnItemActivated ), NULL, this );
	m_listCtrlBreakpoints->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( BreakpointTab::OnItemDeselected ), NULL, this );
	m_listCtrlBreakpoints->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( BreakpointTab::OnItemSelected ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnDelete ), NULL, this );
	m_buttonDeleteAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnDeleteAll ), NULL, this );
}
