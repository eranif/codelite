///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "continousbuildbasepane.h"

///////////////////////////////////////////////////////////////////////////

ContinousBuildBasePane::ContinousBuildBasePane( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, _("Enable continous build"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_checkBox1, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Build concurrency:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer5->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceNumberOfJobsChoices;
	m_choiceNumberOfJobs = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceNumberOfJobsChoices, 0 );
	m_choiceNumberOfJobs->SetSelection( 0 );
	bSizer5->Add( m_choiceNumberOfJobs, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	mainSizer->Add( bSizer5, 0, wxEXPAND|wxTOP, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBoxQueue = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer2->Add( m_listBoxQueue, 1, wxEXPAND|wxTOP, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonCancel = new wxButton( this, wxID_ANY, _("&Stop All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCancel->SetDefault(); 
	bSizer3->Add( m_buttonCancel, 0, wxEXPAND|wxALL, 5 );
	
	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer2, 1, wxEXPAND, 5 );
	
	m_staticTextStatus = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextStatus->Wrap( -1 );
	mainSizer->Add( m_staticTextStatus, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_checkBox1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ContinousBuildBasePane::OnEnableCB ), NULL, this );
	m_choiceNumberOfJobs->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ContinousBuildBasePane::OnChoiceNumberOfJobs ), NULL, this );
	m_choiceNumberOfJobs->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnChoiceNumberOfJobsUI ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ContinousBuildBasePane::OnStopAll ), NULL, this );
	m_buttonCancel->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnStopUI ), NULL, this );
}

ContinousBuildBasePane::~ContinousBuildBasePane()
{
	// Disconnect Events
	m_checkBox1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ContinousBuildBasePane::OnEnableCB ), NULL, this );
	m_choiceNumberOfJobs->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ContinousBuildBasePane::OnChoiceNumberOfJobs ), NULL, this );
	m_choiceNumberOfJobs->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnChoiceNumberOfJobsUI ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ContinousBuildBasePane::OnStopAll ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnStopUI ), NULL, this );
}
