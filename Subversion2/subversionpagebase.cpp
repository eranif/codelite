///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "subversionpagebase.h"

///////////////////////////////////////////////////////////////////////////

SubversionPageBase::SubversionPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlRootDir = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer2->Add( m_textCtrlRootDir, 1, wxALL, 5 );
	
	m_buttonChangeRootDir = new wxButton( this, wxID_ANY, _("&Select"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonChangeRootDir->SetToolTip( _("Select a different root directory") );
	
	bSizer2->Add( m_buttonChangeRootDir, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( bSizer2, 0, wxEXPAND, 5 );
	
	m_treeCtrl = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_MULTIPLE );
	mainSizer->Add( m_treeCtrl, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_buttonChangeRootDir->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SubversionPageBase::OnChangeRootDir ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( SubversionPageBase::OnTreeMenu ), NULL, this );
}

SubversionPageBase::~SubversionPageBase()
{
	// Disconnect Events
	m_buttonChangeRootDir->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SubversionPageBase::OnChangeRootDir ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( SubversionPageBase::OnTreeMenu ), NULL, this );
}
