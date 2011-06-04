///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 29 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "codedesigneritembasedlg.h"

///////////////////////////////////////////////////////////////////////////

CodeDesignerItemBaseDlg::CodeDesignerItemBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("CodeDesigner project settings:") ), wxVERTICAL );
	
	wxFlexGridSizer* controlsSizer;
	controlsSizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	controlsSizer->SetFlexibleDirection( wxBOTH );
	controlsSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Package name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	controlsSizer->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textPackageName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_textPackageName->SetMinSize( wxSize( 300,-1 ) );
	
	controlsSizer->Add( m_textPackageName, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Diagram name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	controlsSizer->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textDiagramName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	controlsSizer->Add( m_textDiagramName, 0, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Language:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	controlsSizer->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceLanguageChoices[] = { _("C"), _("C++"), _("Python") };
	int m_choiceLanguageNChoices = sizeof( m_choiceLanguageChoices ) / sizeof( wxString );
	m_choiceLanguage = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceLanguageNChoices, m_choiceLanguageChoices, 0 );
	m_choiceLanguage->SetSelection( 0 );
	controlsSizer->Add( m_choiceLanguage, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	
	controlsSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxMain = new wxCheckBox( this, wxID_ANY, _("Application entry point"), wxDefaultPosition, wxDefaultSize, 0 );
	controlsSizer->Add( m_checkBoxMain, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	
	controlsSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkGenerate = new wxCheckBox( this, wxID_ANY, _("Generate source code"), wxDefaultPosition, wxDefaultSize, 0 );
	controlsSizer->Add( m_checkGenerate, 0, wxBOTTOM|wxRIGHT, 5 );
	
	sbSizer1->Add( controlsSizer, 1, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("CodeLite settings:") ), wxVERTICAL );
	
	wxFlexGridSizer* codeSizer;
	codeSizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	codeSizer->SetFlexibleDirection( wxBOTH );
	codeSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Project tree folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	codeSizer->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* treeSizer;
	treeSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_textTreeFolder = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 280,-1 ), 0 );
	m_textTreeFolder->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	m_textTreeFolder->SetMinSize( wxSize( 280,-1 ) );
	
	treeSizer->Add( m_textTreeFolder, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );
	
	m_buttonBrowse = new wxButton( this, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	treeSizer->Add( m_buttonBrowse, 0, wxALL, 5 );
	
	codeSizer->Add( treeSizer, 0, 0, 5 );
	
	sbSizer2->Add( codeSizer, 0, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer2, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( CodeDesignerItemBaseDlg::OnInit ) );
	m_checkBoxMain->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CodeDesignerItemBaseDlg::OnUpdateMain ), NULL, this );
	m_buttonBrowse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CodeDesignerItemBaseDlg::OnBrowse ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CodeDesignerItemBaseDlg::OnOk ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CodeDesignerItemBaseDlg::OnCancel ), NULL, this );
}

CodeDesignerItemBaseDlg::~CodeDesignerItemBaseDlg()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( CodeDesignerItemBaseDlg::OnInit ) );
	m_checkBoxMain->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CodeDesignerItemBaseDlg::OnUpdateMain ), NULL, this );
	m_buttonBrowse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CodeDesignerItemBaseDlg::OnBrowse ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CodeDesignerItemBaseDlg::OnOk ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CodeDesignerItemBaseDlg::OnCancel ), NULL, this );
	
}
