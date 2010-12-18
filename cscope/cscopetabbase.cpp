///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cscopetabbase.h"

///////////////////////////////////////////////////////////////////////////

CscopeTabBase::CscopeTabBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Search scope:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer31->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceSearchScopeChoices;
	m_choiceSearchScope = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceSearchScopeChoices, 0 );
	m_choiceSearchScope->SetSelection( 0 );
	bSizer31->Add( m_choiceSearchScope, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxUpdateDb = new wxCheckBox( this, wxID_ANY, _("Update Db"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUpdateDb->SetToolTip( _("Update the database before executing any CScope command") );
	
	bSizer31->Add( m_checkBoxUpdateDb, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxRevertedIndex = new wxCheckBox( this, wxID_ANY, _("Create reverted Index"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxRevertedIndex->SetToolTip( _("Create reverted Index database") );
	
	bSizer31->Add( m_checkBoxRevertedIndex, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonClear = new wxButton( this, wxID_ANY, _("&Clear Results"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_buttonClear, 0, wxRIGHT|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	mainSizer->Add( bSizer31, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	m_treeCtrlResults = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	mainSizer->Add( m_treeCtrlResults, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_statusMessage = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_statusMessage->Wrap( -1 );
	bSizer3->Add( m_statusMessage, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
	
	m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,15 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_gauge->SetValue( 0 ); 
	bSizer3->Add( m_gauge, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 0 );
	
	mainSizer->Add( bSizer3, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_choiceSearchScope->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CscopeTabBase::OnChangeSearchScope ), NULL, this );
	m_checkBoxUpdateDb->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CscopeTabBase::OnChangeSearchScope ), NULL, this );
	m_checkBoxRevertedIndex->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CscopeTabBase::OnChangeSearchScope ), NULL, this );
	m_buttonClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CscopeTabBase::OnClearResults ), NULL, this );
	m_buttonClear->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CscopeTabBase::OnClearResultsUI ), NULL, this );
	m_treeCtrlResults->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( CscopeTabBase::OnLeftDClick ), NULL, this );
	m_treeCtrlResults->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CscopeTabBase::OnItemActivated ), NULL, this );
}

CscopeTabBase::~CscopeTabBase()
{
	// Disconnect Events
	m_choiceSearchScope->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CscopeTabBase::OnChangeSearchScope ), NULL, this );
	m_checkBoxUpdateDb->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CscopeTabBase::OnChangeSearchScope ), NULL, this );
	m_checkBoxRevertedIndex->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CscopeTabBase::OnChangeSearchScope ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CscopeTabBase::OnClearResults ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CscopeTabBase::OnClearResultsUI ), NULL, this );
	m_treeCtrlResults->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( CscopeTabBase::OnLeftDClick ), NULL, this );
	m_treeCtrlResults->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CscopeTabBase::OnItemActivated ), NULL, this );
	
}
