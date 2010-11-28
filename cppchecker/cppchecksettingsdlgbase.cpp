///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cppchecksettingsdlgbase.h"

///////////////////////////////////////////////////////////////////////////

CppCheckSettingsDialogBase::CppCheckSettingsDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_cbOptionAll = new wxCheckBox( m_panel1, wxID_ANY, _("Make the checking more sensitive."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_cbOptionAll, 0, wxALL|wxEXPAND, 5 );
	
	m_cbOptionForce = new wxCheckBox( m_panel1, wxID_ANY, _("Force checking on files that have 'too many' configurations"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_cbOptionForce, 0, wxALL|wxEXPAND, 5 );
	
	m_cbOptionStyle = new wxCheckBox( m_panel1, wxID_ANY, _("Check coding style"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_cbOptionStyle, 0, wxALL|wxEXPAND, 5 );
	
	m_cbOptionUnusedFunctions = new wxCheckBox( m_panel1, wxID_ANY, _("Check if there are unused functions"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_cbOptionUnusedFunctions, 0, wxALL|wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_notebook1->AddPage( m_panel1, _("General"), true );
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_panel2, wxID_ANY, _("Files to exclude from CppCheck test:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer8->Add( m_staticText1, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBoxExcludelist = new wxListBox( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_listBoxExcludelist, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAdd = new wxButton( m_panel2, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAdd->SetToolTip( _("Add file(s) to the excluded files list") );
	
	bSizer6->Add( m_buttonAdd, 0, wxALL, 5 );
	
	m_buttonRemove = new wxButton( m_panel2, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonRemove->SetToolTip( _("Remove the selected file from the excluded file list") );
	
	bSizer6->Add( m_buttonRemove, 0, wxALL, 5 );
	
	m_buttonClearList = new wxButton( m_panel2, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonClearList->SetToolTip( _("Clear the excluded files list") );
	
	bSizer6->Add( m_buttonClearList, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	bSizer8->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer8 );
	m_panel2->Layout();
	bSizer8->Fit( m_panel2 );
	m_notebook1->AddPage( m_panel2, _("Exclude list"), false );
	
	bSizer1->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button1 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->SetDefault(); 
	bSizer2->Add( m_button1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_button2 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button2, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_listBoxExcludelist->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnFileSelected ), NULL, this );
	m_listBoxExcludelist->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnFileSelected ), NULL, this );
	m_buttonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnAddFile ), NULL, this );
	m_buttonRemove->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnRemoveFile ), NULL, this );
	m_buttonRemove->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnRemoveFileUI ), NULL, this );
	m_buttonClearList->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnClearList ), NULL, this );
	m_buttonClearList->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnClearListUI ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnBtnOK ), NULL, this );
}

CppCheckSettingsDialogBase::~CppCheckSettingsDialogBase()
{
	// Disconnect Events
	m_listBoxExcludelist->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnFileSelected ), NULL, this );
	m_listBoxExcludelist->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnFileSelected ), NULL, this );
	m_buttonAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnAddFile ), NULL, this );
	m_buttonRemove->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnRemoveFile ), NULL, this );
	m_buttonRemove->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnRemoveFileUI ), NULL, this );
	m_buttonClearList->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnClearList ), NULL, this );
	m_buttonClearList->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnClearListUI ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnBtnOK ), NULL, this );
	
}
