///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mainPanel->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_bmp = new wxStaticBitmap( m_mainPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_bmp, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	m_mainPanel->SetSizer( bSizer3 );
	m_mainPanel->Layout();
	bSizer3->Fit( m_mainPanel );
	bSizer2->Add( m_mainPanel, 1, wxALL|wxEXPAND, 0 );
	
	bSizer1->Add( bSizer2, 0, wxEXPAND, 0 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND|wxALL|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textClassName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textClassName, 0, wxALL|wxEXPAND, 5 );
	
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizer1->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Generated Files Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGenFilePath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlGenFilePath, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseFolder = new wxButton( this, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseFolder, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Select Virtual Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlVD = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer1->Add( m_textCtrlVD, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonSelectVD = new wxButton( this, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonSelectVD, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_staticline6 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline6, 0, wxEXPAND | wxALL, 5 );
	
	m_checkBoxEnterFileName = new wxCheckBox( this, wxID_ANY, wxT("Manually enter file name (without the extension):"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_checkBoxEnterFileName, 0, wxALL, 5 );
	
	m_textCtrlFileName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_textCtrlFileName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Inherits:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer1->Add( m_staticText9, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrl1 = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_VRULES );
	bSizer15->Add( m_listCtrl1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAddInheritance = new wxButton( this, ID_ADD_INHERITANCE, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_buttonAddInheritance, 0, wxALL, 5 );
	
	m_buttonDelInheritance = new wxButton( this, ID_DELETE_INHERITANCE, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_buttonDelInheritance, 0, wxALL, 5 );
	
	bSizer15->Add( bSizer16, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer15, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxCopyable = new wxCheckBox( this, wxID_ANY, wxT("Declare this class as non-copyable class"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_checkBoxCopyable, 0, wxALL, 5 );
	
	m_checkBoxImplVirtual = new wxCheckBox( this, wxID_ANY, wxT("Implement all virtual functions"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_checkBoxImplVirtual, 0, wxALL, 5 );
	
	m_checkBox6 = new wxCheckBox( this, wxID_ANY, wxT("This is a singleton class"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_checkBox6, 0, wxALL, 5 );
	
	m_checkBoxImplPureVirtual = new wxCheckBox( this, wxID_ANY, wxT("Implement all pure virtual functions"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_checkBoxImplPureVirtual, 0, wxALL, 5 );
	
	m_checkBoxVirtualDtor = new wxCheckBox( this, wxID_ANY, wxT("Virtual destructor"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_checkBoxVirtualDtor, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	buttonSizer->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_textClassName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewClassBaseDlg::OnTextEnter ), NULL, this );
	m_buttonBrowseFolder->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseFolder ), NULL, this );
	m_buttonSelectVD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseVD ), NULL, this );
	m_checkBoxEnterFileName->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnCheckEnterFileNameManually ), NULL, this );
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
	m_buttonBrowseFolder->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseFolder ), NULL, this );
	m_buttonSelectVD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnBrowseVD ), NULL, this );
	m_checkBoxEnterFileName->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnCheckEnterFileNameManually ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( NewClassBaseDlg::OnListItemActivated ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( NewClassBaseDlg::OnListItemDeSelected ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( NewClassBaseDlg::OnListItemSelected ), NULL, this );
	m_buttonAddInheritance->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnButtonAdd ), NULL, this );
	m_buttonDelInheritance->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnButtonDelete ), NULL, this );
	m_buttonDelInheritance->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( NewClassBaseDlg::OnButtonDeleteUI ), NULL, this );
	m_checkBoxImplVirtual->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnCheckImpleAllVirtualFunctions ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewClassBaseDlg::OnButtonOK ), NULL, this );
}
