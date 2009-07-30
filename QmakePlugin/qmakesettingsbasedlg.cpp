///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "qmakesettingsbasedlg.h"

///////////////////////////////////////////////////////////////////////////

QMakeSettingsBaseDlg::QMakeSettingsBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Add / modfiy qmake configurations:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	mainSizer->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer3->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );
	
	mainSizer->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonNew = new wxButton( this, wxID_ANY, _("&New..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonNew->SetToolTip( _("Create new qmake settings") );
	
	bSizer2->Add( m_buttonNew, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	bSizer2->Add( m_buttonOk, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_notebook->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( QMakeSettingsBaseDlg::OnRightDown ), NULL, this );
	m_buttonNew->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QMakeSettingsBaseDlg::OnNewQmakeSettings ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QMakeSettingsBaseDlg::OnOK ), NULL, this );
}

QMakeSettingsBaseDlg::~QMakeSettingsBaseDlg()
{
	// Disconnect Events
	m_notebook->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( QMakeSettingsBaseDlg::OnRightDown ), NULL, this );
	m_buttonNew->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QMakeSettingsBaseDlg::OnNewQmakeSettings ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QMakeSettingsBaseDlg::OnOK ), NULL, this );
}

QmakeSettingsTabBase::QmakeSettingsTabBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("qmake executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer2->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_filePickerQmakeExec = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	fgSizer2->Add( m_filePickerQmakeExec, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("QMAKESPEC:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer2->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_comboBoxQmakespec = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer2->Add( m_comboBoxQmakespec, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("QTDIR:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlQtdir = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlQtdir->SetToolTip( _("Set QTDIR to the directory where you've installed Qt") );
	
	fgSizer2->Add( m_textCtrlQtdir, 0, wxALL|wxEXPAND, 5 );
	
	bSizer4->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer4 );
	this->Layout();
	
	// Connect Events
	m_filePickerQmakeExec->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( QmakeSettingsTabBase::OnFileSelected ), NULL, this );
}

QmakeSettingsTabBase::~QmakeSettingsTabBase()
{
	// Disconnect Events
	m_filePickerQmakeExec->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( QmakeSettingsTabBase::OnFileSelected ), NULL, this );
}
