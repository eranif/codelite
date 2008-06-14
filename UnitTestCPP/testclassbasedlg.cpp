///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "testclassbasedlg.h"

///////////////////////////////////////////////////////////////////////////

TestClassBaseDlg::TestClassBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Class name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlClassName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlClassName, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonRefresh = new wxButton( this, wxID_ANY, wxT("&Show Functions..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonRefresh, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, wxT("Generate the tests to the active editor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox1->SetValue(true);
	
	fgSizer2->Add( m_checkBox1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlFileName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrlFileName, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxUseFixture = new wxCheckBox( this, wxID_ANY, wxT("Use fixture test"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_checkBoxUseFixture, 0, wxALL, 5 );
	
	m_textCtrlFixtureName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlFixtureName->Enable( false );
	
	fgSizer2->Add( m_textCtrlFixtureName, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Functions to test:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer1->Add( m_staticText6, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_checkListMethodsChoices;
	m_checkListMethods = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListMethodsChoices, 0 );
	bSizer5->Add( m_checkListMethods, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonCheckAll = new wxButton( this, wxID_ANY, wxT("Check &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonCheckAll, 0, wxALL, 5 );
	
	m_buttonUnCheckAll = new wxButton( this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonUnCheckAll, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonCreate = new wxButton( this, wxID_OK, wxT("&Create"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCreate->SetDefault(); 
	bSizer4->Add( m_buttonCreate, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonRefresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnRefreshFunctions ), NULL, this );
	m_buttonRefresh->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TestClassBaseDlg::OnRefreshButtonUI ), NULL, this );
	m_checkBox1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnUseActiveEditor ), NULL, this );
	m_checkBoxUseFixture->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnUseFixture ), NULL, this );
	m_buttonCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnCheckAll ), NULL, this );
	m_buttonUnCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnUnCheckAll ), NULL, this );
}

TestClassBaseDlg::~TestClassBaseDlg()
{
	// Disconnect Events
	m_buttonRefresh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnRefreshFunctions ), NULL, this );
	m_buttonRefresh->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TestClassBaseDlg::OnRefreshButtonUI ), NULL, this );
	m_checkBox1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnUseActiveEditor ), NULL, this );
	m_checkBoxUseFixture->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnUseFixture ), NULL, this );
	m_buttonCheckAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnCheckAll ), NULL, this );
	m_buttonUnCheckAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnUnCheckAll ), NULL, this );
}
