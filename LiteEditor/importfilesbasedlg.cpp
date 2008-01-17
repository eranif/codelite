///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "importfilesbasedlg.h"

///////////////////////////////////////////////////////////////////////////

ImportFilesBaseDlg::ImportFilesBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("Select Directory to import:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	panelSizer->Add( m_staticText1, 0, wxALL, 5 );
	
	m_dirpicker = new DirPicker(m_mainPanel);
	panelSizer->Add( m_dirpicker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("Files Mask:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	panelSizer->Add( m_staticText2, 0, wxALL, 5 );
	
	m_textCtrlFileMask = new wxTextCtrl( m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( m_textCtrlFileMask, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxNoExtFiles = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("Import files without extension"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxNoExtFiles->SetValue(true);
	
	panelSizer->Add( m_checkBoxNoExtFiles, 0, wxALL, 5 );
	
	m_mainPanel->SetSizer( panelSizer );
	m_mainPanel->Layout();
	panelSizer->Fit( m_mainPanel );
	bSizer1->Add( m_mainPanel, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	btnSizer->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( btnSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportFilesBaseDlg::OnButtonOK ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportFilesBaseDlg::OnButtonCancel ), NULL, this );
}
