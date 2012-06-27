///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  3 2011)
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
	m_ChecksPanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText5 = new wxStaticText( m_ChecksPanel, wxID_ANY, _("Run the following extra checks:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer3->Add( m_staticText5, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer511;
	bSizer511 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_checkListExtraWarningsChoices[] = { _("Coding style"), _("Performance"), _("Portability"), _("Unused functions"), _("Missing includes"), _("Output informative messages"), _("Posix standards"), _("C99 standards"), _("C++11 standards") };
	int m_checkListExtraWarningsNChoices = sizeof( m_checkListExtraWarningsChoices ) / sizeof( wxString );
	m_checkListExtraWarnings = new wxCheckListBox( m_ChecksPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListExtraWarningsNChoices, m_checkListExtraWarningsChoices, 0 );
	bSizer511->Add( m_checkListExtraWarnings, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer611;
	bSizer611 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAllChecks = new wxButton( m_ChecksPanel, wxID_ANY, _("Tick All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAllChecks->SetToolTip( _("Tick all the boxes") );
	
	bSizer611->Add( m_buttonAllChecks, 0, wxALL, 5 );
	
	m_buttonClearChecks = new wxButton( m_ChecksPanel, wxID_ANY, _("Untick All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonClearChecks->SetToolTip( _("Untick all the boxes") );
	
	bSizer611->Add( m_buttonClearChecks, 0, wxALL, 5 );
	
	bSizer511->Add( bSizer611, 0, wxEXPAND, 5 );
	
	bSizer3->Add( bSizer511, 1, wxEXPAND, 5 );
	
	m_cbOptionForce = new wxCheckBox( m_ChecksPanel, wxID_ANY, _("Force checking unlimited numbers of configurations"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbOptionForce->SetToolTip( _("By default the maximum number of configurations checked per file is 12. If that might not be enough, tick this box.") );
	
	bSizer3->Add( m_cbOptionForce, 0, wxALL|wxEXPAND, 10 );
	
	m_ChecksPanel->SetSizer( bSizer3 );
	m_ChecksPanel->Layout();
	bSizer3->Fit( m_ChecksPanel );
	m_notebook1->AddPage( m_ChecksPanel, _("Checks"), true );
	m_ExcludePanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_ExcludePanel, wxID_ANY, _("Files to exclude from CppCheck test:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer8->Add( m_staticText1, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBoxExcludelist = new wxListBox( m_ExcludePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_listBoxExcludelist, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAdd = new wxButton( m_ExcludePanel, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAdd->SetToolTip( _("Add file(s) to the excluded files list") );
	
	bSizer6->Add( m_buttonAdd, 0, wxALL, 5 );
	
	m_buttonRemove = new wxButton( m_ExcludePanel, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonRemove->SetToolTip( _("Remove the selected file from the excluded file list") );
	
	bSizer6->Add( m_buttonRemove, 0, wxALL, 5 );
	
	m_buttonClearList = new wxButton( m_ExcludePanel, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonClearList->SetToolTip( _("Clear the excluded files list") );
	
	bSizer6->Add( m_buttonClearList, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	bSizer8->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_ExcludePanel->SetSizer( bSizer8 );
	m_ExcludePanel->Layout();
	bSizer8->Fit( m_ExcludePanel );
	m_notebook1->AddPage( m_ExcludePanel, _("Exclude"), false );
	m_SupressPanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText11 = new wxStaticText( m_SupressPanel, wxID_ANY, _("Types of warnings NOT to display:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer81->Add( m_staticText11, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_checkListSuppressChoices;
	m_checkListSuppress = new wxCheckListBox( m_SupressPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListSuppressChoices, 0 );
	bSizer51->Add( m_checkListSuppress, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAddSuppression = new wxButton( m_SupressPanel, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAddSuppression->SetToolTip( _("Add an extra check to suppress. You'll need to know its id...") );
	
	bSizer61->Add( m_buttonAddSuppression, 0, wxALL, 5 );
	
	m_buttonRemoveSuppression = new wxButton( m_SupressPanel, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonRemoveSuppression->SetToolTip( _("Remove the selected suppression from the list") );
	
	bSizer61->Add( m_buttonRemoveSuppression, 0, wxALL, 5 );
	
	
	bSizer61->Add( 0, 0, 0, wxTOP, 15 );
	
	m_buttonSuppressAll = new wxButton( m_SupressPanel, wxID_ANY, _("Tick All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSuppressAll->SetToolTip( _("Tick all the boxes") );
	
	bSizer61->Add( m_buttonSuppressAll, 0, wxALL, 5 );
	
	m_buttonClearSuppressions = new wxButton( m_SupressPanel, wxID_ANY, _("Untick All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonClearSuppressions->SetToolTip( _("Untick all the boxes") );
	
	bSizer61->Add( m_buttonClearSuppressions, 0, wxALL, 5 );
	
	bSizer51->Add( bSizer61, 0, wxEXPAND, 5 );
	
	bSizer81->Add( bSizer51, 1, wxEXPAND, 5 );
	
	m_checkBoxSerialise = new wxCheckBox( m_SupressPanel, wxID_ANY, _("Remember these settings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxSerialise->SetToolTip( _("If ticked, these settings will be saved and be applied in the future. Otherwise the warnings will be back when you restart CodeLite, which may be what you should want.") );
	
	bSizer81->Add( m_checkBoxSerialise, 0, wxALL, 10 );
	
	m_SupressPanel->SetSizer( bSizer81 );
	m_SupressPanel->Layout();
	bSizer81->Fit( m_SupressPanel );
	m_notebook1->AddPage( m_SupressPanel, _("Suppress"), false );
	
	bSizer1->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_BtmOK = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_BtmOK->SetDefault(); 
	bSizer2->Add( m_BtmOK, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_BtnCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_BtnCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_buttonAllChecks->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnChecksTickAll ), NULL, this );
	m_buttonAllChecks->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnChecksTickAllUI ), NULL, this );
	m_buttonClearChecks->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnChecksUntickAll ), NULL, this );
	m_buttonClearChecks->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnChecksUntickAllUI ), NULL, this );
	m_buttonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnAddFile ), NULL, this );
	m_buttonRemove->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnRemoveFile ), NULL, this );
	m_buttonRemove->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnRemoveFileUI ), NULL, this );
	m_buttonClearList->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnClearList ), NULL, this );
	m_buttonClearList->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnClearListUI ), NULL, this );
	m_buttonAddSuppression->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnAddSuppression ), NULL, this );
	m_buttonRemoveSuppression->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnRemoveSuppression ), NULL, this );
	m_buttonRemoveSuppression->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnRemoveSuppressionUI ), NULL, this );
	m_buttonSuppressAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnSuppressTickAll ), NULL, this );
	m_buttonSuppressAll->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnSuppressTickAllUI ), NULL, this );
	m_buttonClearSuppressions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnSuppressUntickAll ), NULL, this );
	m_buttonClearSuppressions->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnSuppressUntickAllUI ), NULL, this );
	m_BtmOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnBtnOK ), NULL, this );
}

CppCheckSettingsDialogBase::~CppCheckSettingsDialogBase()
{
	// Disconnect Events
	m_buttonAllChecks->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnChecksTickAll ), NULL, this );
	m_buttonAllChecks->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnChecksTickAllUI ), NULL, this );
	m_buttonClearChecks->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnChecksUntickAll ), NULL, this );
	m_buttonClearChecks->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnChecksUntickAllUI ), NULL, this );
	m_buttonAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnAddFile ), NULL, this );
	m_buttonRemove->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnRemoveFile ), NULL, this );
	m_buttonRemove->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnRemoveFileUI ), NULL, this );
	m_buttonClearList->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnClearList ), NULL, this );
	m_buttonClearList->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnClearListUI ), NULL, this );
	m_buttonAddSuppression->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnAddSuppression ), NULL, this );
	m_buttonRemoveSuppression->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnRemoveSuppression ), NULL, this );
	m_buttonRemoveSuppression->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnRemoveSuppressionUI ), NULL, this );
	m_buttonSuppressAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnSuppressTickAll ), NULL, this );
	m_buttonSuppressAll->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnSuppressTickAllUI ), NULL, this );
	m_buttonClearSuppressions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnSuppressUntickAll ), NULL, this );
	m_buttonClearSuppressions->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckSettingsDialogBase::OnSuppressUntickAllUI ), NULL, this );
	m_BtmOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckSettingsDialogBase::OnBtnOK ), NULL, this );
	
}

CppCheckAddSuppressionDialogBase::CppCheckAddSuppressionDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, _("Description to show in the dialog"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer1->Add( m_staticText12, 0, wxALL, 5 );
	
	m_txtDescription = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_txtDescription->SetToolTip( _("This is what you'll see in the settings dialog. Put whatever you like here; it's not used internally") );
	
	bSizer1->Add( m_txtDescription, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, _("The ID string that cppchecker will recognise"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer1->Add( m_staticText13, 0, wxALL, 5 );
	
	m_txtKey = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_txtKey->SetToolTip( _("Enter here the unique ID string that cppchecker can recognise. Examples are \"operatorEqVarError\" and \"uninitMemberVar\". You can find these by grepping the cppchecker source, or by running cppchecker on your app in a terminal and passing the additional parameter '--xml'.") );
	
	bSizer1->Add( m_txtKey, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer2->Add( m_buttonOK, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonOK->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckAddSuppressionDialogBase::OnOKButtonUpdateUI ), NULL, this );
}

CppCheckAddSuppressionDialogBase::~CppCheckAddSuppressionDialogBase()
{
	// Disconnect Events
	m_buttonOK->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckAddSuppressionDialogBase::OnOKButtonUpdateUI ), NULL, this );
	
}
