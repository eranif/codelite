///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cc_boxbase.h"
#include <wx/xrc/xmlres.h>

///////////////////////////////////////////////////////////////////////////

CCBoxBase::CCBoxBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_listCtrl = new CCVirtualListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VIRTUAL );
	bSizer1->Add( m_listCtrl, 1, wxEXPAND|wxALL, 2 );

	wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(wxT("cc_private_members"));
	m_toolBar1 = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxSIMPLE_BORDER );
	m_toolBar1->AddTool( TOOL_SHOW_PRIVATE_MEMBERS, wxT("Show Protected / Private Items"), bmp, bmp, wxITEM_CHECK, wxT("Show Protected / Private Items"), wxEmptyString );
	m_toolBar1->Realize();

	bSizer1->Add( m_toolBar1, 0, wxEXPAND|wxRIGHT|wxLEFT, 2 );

	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CCBoxBase::OnItemActivated ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CCBoxBase::OnItemDeSelected ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CCBoxBase::OnItemSelected ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( CCBoxBase::OnKeyDown ), NULL, this );
	this->Connect( TOOL_SHOW_PRIVATE_MEMBERS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CCBoxBase::OnShowPublicItems ) );
}

CCBoxBase::~CCBoxBase()
{
	// Disconnect Events
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CCBoxBase::OnItemActivated ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CCBoxBase::OnItemDeSelected ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CCBoxBase::OnItemSelected ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( CCBoxBase::OnKeyDown ), NULL, this );
	this->Disconnect( TOOL_SHOW_PRIVATE_MEMBERS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CCBoxBase::OnShowPublicItems ) );

}
