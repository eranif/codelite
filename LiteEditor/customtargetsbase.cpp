///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "customtargetsbase.h"

///////////////////////////////////////////////////////////////////////////

CustomTargetsBase::CustomTargetsBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrlTargets = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	m_listCtrlTargets->SetMinSize( wxSize( 500,200 ) );
	
	bSizer24->Add( m_listCtrlTargets, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewTarget = new wxButton( this, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer23->Add( m_buttonNewTarget, 0, wxALL, 5 );
	
	m_buttonEditTarget = new wxButton( this, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer23->Add( m_buttonEditTarget, 0, wxALL, 5 );
	
	m_buttonDeleteTarget = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer23->Add( m_buttonDeleteTarget, 0, wxALL, 5 );
	
	bSizer24->Add( bSizer23, 1, wxEXPAND, 5 );
	
	bSizer21->Add( bSizer24, 1, wxEXPAND, 5 );
	
	m_staticline11 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer21->Add( m_staticline11, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer22->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer21->Add( bSizer22, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer21 );
	this->Layout();
	bSizer21->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_listCtrlTargets->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CustomTargetsBase::OnItemActivated ), NULL, this );
	m_listCtrlTargets->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CustomTargetsBase::OnItemSelected ), NULL, this );
	m_buttonNewTarget->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomTargetsBase::OnNewTarget ), NULL, this );
	m_buttonEditTarget->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomTargetsBase::OnEditTarget ), NULL, this );
	m_buttonEditTarget->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CustomTargetsBase::OnEditUI ), NULL, this );
	m_buttonDeleteTarget->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomTargetsBase::OnDeleteTarget ), NULL, this );
	m_buttonDeleteTarget->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CustomTargetsBase::OnDeleteUI ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomTargetsBase::OnButtonOK ), NULL, this );
}

CustomTargetsBase::~CustomTargetsBase()
{
	// Disconnect Events
	m_listCtrlTargets->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CustomTargetsBase::OnItemActivated ), NULL, this );
	m_listCtrlTargets->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CustomTargetsBase::OnItemSelected ), NULL, this );
	m_buttonNewTarget->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomTargetsBase::OnNewTarget ), NULL, this );
	m_buttonEditTarget->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomTargetsBase::OnEditTarget ), NULL, this );
	m_buttonEditTarget->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CustomTargetsBase::OnEditUI ), NULL, this );
	m_buttonDeleteTarget->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomTargetsBase::OnDeleteTarget ), NULL, this );
	m_buttonDeleteTarget->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CustomTargetsBase::OnDeleteUI ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomTargetsBase::OnButtonOK ), NULL, this );
}
