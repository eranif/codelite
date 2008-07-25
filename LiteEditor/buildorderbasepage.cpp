///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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

#include "buildorderbasepage.h"

///////////////////////////////////////////////////////////////////////////

DependenciesPageBase::DependenciesPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Select build order for configuration:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	sbSizer1->Add( m_staticText3, 0, wxALL, 5 );
	
	wxArrayString m_choiceProjectConfigChoices;
	m_choiceProjectConfig = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceProjectConfigChoices, 0 );
	m_choiceProjectConfig->SetSelection( 0 );
	sbSizer1->Add( m_choiceProjectConfig, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Projects:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer31->Add( m_staticText1, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_checkListProjectListChoices;
	m_checkListProjectList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListProjectListChoices, 0 );
	bSizer31->Add( m_checkListProjectList, 1, wxEXPAND|wxALL, 5 );
	
	bSizer3->Add( bSizer31, 5, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Build Order:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_listBoxBuildOrder = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_listBoxBuildOrder, 1, wxEXPAND|wxALL, 5 );
	
	bSizer11->Add( bSizer5, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonDown = new wxButton( this, wxID_ANY, wxT("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonDown, 0, wxALL, 5 );
	
	m_buttonUp = new wxButton( this, wxID_ANY, wxT("Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonUp, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer8, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonApply = new wxButton( this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_buttonApply, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer7, 0, 0, 5 );
	
	bSizer11->Add( bSizer6, 0, wxEXPAND, 5 );
	
	bSizer4->Add( bSizer11, 1, wxEXPAND, 5 );
	
	bSizer3->Add( bSizer4, 6, wxEXPAND, 5 );
	
	sbSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	
	this->SetSizer( sbSizer1 );
	this->Layout();
	
	// Connect Events
	m_choiceProjectConfig->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DependenciesPageBase::OnConfigChanged ), NULL, this );
	m_checkListProjectList->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( DependenciesPageBase::OnCheckListItemToggled ), NULL, this );
	m_buttonDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DependenciesPageBase::OnMoveDown ), NULL, this );
	m_buttonUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DependenciesPageBase::OnMoveUp ), NULL, this );
	m_buttonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DependenciesPageBase::OnApplyButton ), NULL, this );
	m_buttonApply->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DependenciesPageBase::OnApplyButtonUI ), NULL, this );
}

DependenciesPageBase::~DependenciesPageBase()
{
	// Disconnect Events
	m_choiceProjectConfig->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DependenciesPageBase::OnConfigChanged ), NULL, this );
	m_checkListProjectList->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( DependenciesPageBase::OnCheckListItemToggled ), NULL, this );
	m_buttonDown->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DependenciesPageBase::OnMoveDown ), NULL, this );
	m_buttonUp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DependenciesPageBase::OnMoveUp ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DependenciesPageBase::OnApplyButton ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DependenciesPageBase::OnApplyButtonUI ), NULL, this );
}
