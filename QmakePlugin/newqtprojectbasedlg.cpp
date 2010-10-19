///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "newqtprojectbasedlg.h"

///////////////////////////////////////////////////////////////////////////

NewQtProjBaseDlg::NewQtProjBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Project name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer2->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlProjName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrlProjName, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, _("Project path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer2->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrl, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonSelectProjectPath = new wxButton( this, wxID_ANY, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSelectProjectPath->SetToolTip( _("Click to select a directory") );
	
	fgSizer2->Add( m_buttonSelectProjectPath, 0, wxALL, 5 );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, _("Project kind:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer2->Add( m_staticText13, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceProjKindChoices[] = { _("Console"), _("GUI"), _("Static Library"), _("Dynamic Library") };
	int m_choiceProjKindNChoices = sizeof( m_choiceProjKindChoices ) / sizeof( wxString );
	m_choiceProjKind = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceProjKindNChoices, m_choiceProjKindChoices, 0 );
	m_choiceProjKind->SetSelection( 0 );
	fgSizer2->Add( m_choiceProjKind, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, _("QMake to use:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer2->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceQmakeChoices;
	m_choiceQmake = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceQmakeChoices, 0 );
	m_choiceQmake->SetSelection( 0 );
	fgSizer2->Add( m_choiceQmake, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonSelectConfig = new wxButton( this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSelectConfig->SetToolTip( _("Open the QMakeSettings configuration dialog") );
	
	fgSizer2->Add( m_buttonSelectConfig, 0, wxALL, 5 );
	
	bSizer5->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Other settings:") ), wxVERTICAL );
	
	m_checkBoxUseSepDirectory = new wxCheckBox( this, wxID_ANY, _("Create the project under a separate directory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUseSepDirectory->SetValue(true);
	
	sbSizer1->Add( m_checkBoxUseSepDirectory, 0, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonCancel, 0, wxALL, 5 );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	bSizer6->Add( m_buttonOk, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	bSizer5->Fit( this );
	
	// Connect Events
	m_buttonSelectProjectPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewQtProjBaseDlg::OnBrowseProjectPath ), NULL, this );
	m_buttonSelectConfig->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewQtProjBaseDlg::OnNewQmakeSettings ), NULL, this );
}

NewQtProjBaseDlg::~NewQtProjBaseDlg()
{
	// Disconnect Events
	m_buttonSelectProjectPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewQtProjBaseDlg::OnBrowseProjectPath ), NULL, this );
	m_buttonSelectConfig->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewQtProjBaseDlg::OnNewQmakeSettings ), NULL, this );
}
