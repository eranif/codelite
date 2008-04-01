///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 20 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "newworkspacebasedlg.h"

///////////////////////////////////////////////////////////////////////////

NewWorkspaceBase::NewWorkspaceBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panelWorkspace = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_panelWorkspace, wxID_ANY, wxT("Workspace Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer4->Add( m_staticText1, 0, wxALL, 5 );
	
	m_textCtrlWorkspaceName = new wxTextCtrl( m_panelWorkspace, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_textCtrlWorkspaceName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( m_panelWorkspace, wxID_ANY, wxT("Workspace Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlWorkspacePath = new wxTextCtrl( m_panelWorkspace, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_textCtrlWorkspacePath, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonWorkspaceDirPicker = new wxButton( m_panelWorkspace, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonWorkspaceDirPicker, 0, wxALL, 5 );
	
	bSizer4->Add( bSizer3, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panelWorkspace, wxID_ANY, wxT("File Name:") ), wxVERTICAL );
	
	m_staticTextWorkspaceFileName = new wxStaticText( m_panelWorkspace, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextWorkspaceFileName->Wrap( -1 );
	sbSizer1->Add( m_staticTextWorkspaceFileName, 0, wxALL|wxEXPAND, 5 );
	
	bSizer4->Add( sbSizer1, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	bSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( m_panelWorkspace, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer2->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonCreateWorkspace = new wxButton( m_panelWorkspace, wxID_ANY, wxT("&Create"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonCreateWorkspace, 0, wxALL, 5 );
	
	m_buttonCancelWorkspace = new wxButton( m_panelWorkspace, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonCancelWorkspace, 0, wxALL, 5 );
	
	bSizer2->Add( bSizer5, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_panelWorkspace->SetSizer( bSizer2 );
	m_panelWorkspace->Layout();
	bSizer2->Fit( m_panelWorkspace );
	bSizer1->Add( m_panelWorkspace, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_textCtrlWorkspaceName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewWorkspaceBase::OnWorkspacePathUpdated ), NULL, this );
	m_textCtrlWorkspacePath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewWorkspaceBase::OnWorkspacePathUpdated ), NULL, this );
	m_buttonWorkspaceDirPicker->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewWorkspaceBase::OnWorkspaceDirPicker ), NULL, this );
	m_buttonCreateWorkspace->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewWorkspaceBase::OnButtonCreate ), NULL, this );
}

NewWorkspaceBase::~NewWorkspaceBase()
{
	// Disconnect Events
	m_textCtrlWorkspaceName->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewWorkspaceBase::OnWorkspacePathUpdated ), NULL, this );
	m_textCtrlWorkspacePath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewWorkspaceBase::OnWorkspacePathUpdated ), NULL, this );
	m_buttonWorkspaceDirPicker->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewWorkspaceBase::OnWorkspaceDirPicker ), NULL, this );
	m_buttonCreateWorkspace->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewWorkspaceBase::OnButtonCreate ), NULL, this );
}
