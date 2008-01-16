///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "svnoptionsbasedlg.h"

///////////////////////////////////////////////////////////////////////////

SvnOptionsBaseDlg::SvnOptionsBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("SVN executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer5->Add( m_staticText1, 0, wxALL, 5 );
	
	m_filePicker = new FilePicker(m_mainPanel, wxID_ANY);
	bSizer5->Add( m_filePicker, 1, wxALL|wxEXPAND, 5 );
	
	panelSizer->Add( bSizer5, 0, wxEXPAND, 5 );
	
	m_staticline = new wxStaticLine( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	panelSizer->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_staticText3 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("Icons refresh interval (milliseconds):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	gSizer1->Add( m_staticText3, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_spinCtrl1 = new wxSpinCtrl( m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 500, 50000, 500 );
	gSizer1->Add( m_spinCtrl1, 0, wxALL|wxEXPAND, 5 );
	
	panelSizer->Add( gSizer1, 0, wxEXPAND, 5 );
	
	m_checkBoxUpdateOnSave = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("Update file explorer icons when saving a file"), wxDefaultPosition, wxDefaultSize, 0 );
	
	panelSizer->Add( m_checkBoxUpdateOnSave, 0, wxALL, 5 );
	
	m_checkBoxAutoAddNewFiles = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("When adding files to project, add them to SVN as well"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxAutoAddNewFiles->SetValue(true);
	
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
	m_checkBoxUpdateOnSave->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnUpdateOnSave ), NULL, this );
	m_checkBoxAutoAddNewFiles->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnAutoAddNewFiles ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnButtonOk ), NULL, this );
}
