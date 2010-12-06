///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "newclassbasedlg.h"

///////////////////////////////////////////////////////////////////////////

NewClassBaseDlg::NewClassBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Class Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textClassName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	m_textClassName->SetToolTip( _("The class name") );
	
	fgSizer1->Add( m_textClassName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, _("*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	m_staticText9->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	m_staticText9->SetForegroundColour( wxColour( 255, 0, 0 ) );
	
	fgSizer1->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextNamespace = new wxStaticText( this, wxID_ANY, _("Namespace:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextNamespace->Wrap( -1 );
	fgSizer1->Add( m_staticTextNamespace, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlNamespace = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	m_textCtrlNamespace->SetToolTip( _("Place this class inside a namespace") );
	
	fgSizer1->Add( m_textCtrlNamespace, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseNamespaces = new wxButton( this, wxID_ANY, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseNamespaces, 0, wxALL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Block Guard:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlBlockGuard = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	m_textCtrlBlockGuard->SetToolTip( _("Set block guard to prevent multiple file inclusion. If left empty, the class name is used") );
	
	fgSizer1->Add( m_textCtrlBlockGuard, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Generated Files Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGenFilePath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	m_textCtrlGenFilePath->SetToolTip( _("Generated files path (directory)") );
	
	fgSizer1->Add( m_textCtrlGenFilePath, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseFolder = new wxButton( this, wxID_ANY, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseFolder, 0, wxALL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Select Virtual Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlVD = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RICH2 );
	m_textCtrlVD->SetToolTip( _("Select the virtual folder to place the generated files inside CodeLite's project") );
	
	fgSizer1->Add( m_textCtrlVD, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonSelectVD = new wxButton( this, wxID_ANY, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonSelectVD, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, _("Generated File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer10->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline6 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer10->Add( m_staticline6, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer1->Add( bSizer10, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxInline = new wxCheckBox( this, wxID_ANY, _("Inline (generate declaration and implementation in the header file)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_checkBoxInline, 0, wxALL, 5 );
	
	m_checkBoxUseUnderscores = new wxCheckBox( this, wxID_ANY, _("Use underscores ('_') to separate between word captilizations "), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_checkBoxUseUnderscores, 0, wxALL, 5 );
	
	m_textCtrlFileName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	bSizer8->Add( m_textCtrlFileName, 1, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( bSizer8, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, _("Inherits:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	bSizer9->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer9->Add( m_staticline4, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer1->Add( bSizer9, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrl1 = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_VRULES );
	bSizer15->Add( m_listCtrl1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAddInheritance = new wxButton( this, ID_ADD_INHERITANCE, _("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_buttonAddInheritance, 0, wxALL, 5 );
	
	m_buttonDelInheritance = new wxButton( this, ID_DELETE_INHERITANCE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_buttonDelInheritance, 0, wxALL, 5 );
	
	bSizer15->Add( bSizer16, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer15, 1, wxEXPAND|wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxCopyable = new wxCheckBox( this, wxID_ANY, _("Declare this class as non-copyable class"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxCopyable, 0, wxALL, 5 );
	
	m_checkBoxImplVirtual = new wxCheckBox( this, wxID_ANY, _("Implement all virtual functions"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxImplVirtual, 0, wxALL, 5 );
	
	m_checkBoxSingleton = new wxCheckBox( this, wxID_ANY, _("This is a singleton class"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxSingleton, 0, wxALL, 5 );
	
	m_checkBoxImplPureVirtual = new wxCheckBox( this, wxID_ANY, _("Implement all pure virtual functions"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxImplPureVirtual, 0, wxALL, 5 );
	
	m_checkBoxVirtualDtor = new wxCheckBox( this, wxID_ANY, _("Virtual destructor"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxVirtualDtor, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer2, 0, wxEXPAND|wxALL, 5 );
	
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	buttonSizer->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_textClassName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewClassBaseDlg::OnTextEnter ), NULL, this );
	m_buttonBrowseNamespaces->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseNamespace ), NULL, this );
	m_buttonBrowseFolder->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseFolder ), NULL, this );
	m_buttonSelectVD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseVD ), NULL, this );
	m_checkBoxInline->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnCheckInline ), NULL, this );
	m_checkBoxUseUnderscores->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnUseUnderscores ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( NewClassBaseDlg::OnListItemActivated ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( NewClassBaseDlg::OnListItemDeSelected ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( NewClassBaseDlg::OnListItemSelected ), NULL, this );
	m_buttonAddInheritance->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnButtonAdd ), NULL, this );
	m_buttonDelInheritance->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnButtonDelete ), NULL, this );
	m_buttonDelInheritance->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( NewClassBaseDlg::OnButtonDeleteUI ), NULL, this );
	m_checkBoxImplVirtual->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnCheckImpleAllVirtualFunctions ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnButtonOK ), NULL, this );
}

NewClassBaseDlg::~NewClassBaseDlg()
{
	// Disconnect Events
	m_textClassName->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewClassBaseDlg::OnTextEnter ), NULL, this );
	m_buttonBrowseNamespaces->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseNamespace ), NULL, this );
	m_buttonBrowseFolder->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseFolder ), NULL, this );
	m_buttonSelectVD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseVD ), NULL, this );
	m_checkBoxInline->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnCheckInline ), NULL, this );
	m_checkBoxUseUnderscores->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnUseUnderscores ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( NewClassBaseDlg::OnListItemActivated ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( NewClassBaseDlg::OnListItemDeSelected ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( NewClassBaseDlg::OnListItemSelected ), NULL, this );
	m_buttonAddInheritance->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnButtonAdd ), NULL, this );
	m_buttonDelInheritance->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnButtonDelete ), NULL, this );
	m_buttonDelInheritance->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( NewClassBaseDlg::OnButtonDeleteUI ), NULL, this );
	m_checkBoxImplVirtual->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnCheckImpleAllVirtualFunctions ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnButtonOK ), NULL, this );
	
}
