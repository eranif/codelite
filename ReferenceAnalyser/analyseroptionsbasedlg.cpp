///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "analyseroptionsbasedlg.h"

///////////////////////////////////////////////////////////////////////////

AnalyserOptionsBaseDlg::AnalyserOptionsBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxUpdateOnSave = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("Option foo"), wxDefaultPosition, wxDefaultSize, 0 );
	
	panelSizer->Add( m_checkBoxUpdateOnSave, 0, wxALL, 5 );
	
	m_checkBoxAutoAddNewFiles = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("Option bar"), wxDefaultPosition, wxDefaultSize, 0 );
	
	panelSizer->Add( m_checkBoxAutoAddNewFiles, 0, wxALL, 5 );
	
	m_mainPanel->SetSizer( panelSizer );
	m_mainPanel->Layout();
	panelSizer->Fit( m_mainPanel );
	bSizer1->Add( m_mainPanel, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( buttonSizer, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_checkBoxUpdateOnSave->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnalyserOptionsBaseDlg::OnUpdateOnSave ), NULL, this );
	m_checkBoxAutoAddNewFiles->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnalyserOptionsBaseDlg::OnAutoAddNewFiles ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyserOptionsBaseDlg::OnButtonOk ), NULL, this );
}

AnalyserOptionsBaseDlg::~AnalyserOptionsBaseDlg()
{
	// Disconnect Events
	m_checkBoxUpdateOnSave->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnalyserOptionsBaseDlg::OnUpdateOnSave ), NULL, this );
	m_checkBoxAutoAddNewFiles->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnalyserOptionsBaseDlg::OnAutoAddNewFiles ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyserOptionsBaseDlg::OnButtonOk ), NULL, this );
}
