 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "threadlistbasepanel.h"

///////////////////////////////////////////////////////////////////////////

ThreadListBasePanel::ThreadListBasePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_VRULES );
	mainSizer->Add( m_list, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_list->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ThreadListBasePanel::OnItemActivated ), NULL, this );
}

ThreadListBasePanel::~ThreadListBasePanel()
{
	// Disconnect Events
	m_list->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ThreadListBasePanel::OnItemActivated ), NULL, this );
	
}
