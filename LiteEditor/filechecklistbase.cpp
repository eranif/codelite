///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "filechecklistbase.h"

///////////////////////////////////////////////////////////////////////////

FileCheckListBase::FileCheckListBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_caption = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_caption->Wrap( -1 );
	bSizer1->Add( m_caption, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_fileCheckListChoices;
	m_fileCheckList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), m_fileCheckListChoices, wxLB_NEEDED_SB|wxLB_SINGLE );
	bSizer3->Add( m_fileCheckList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_clearAll = new wxButton( this, wxID_ANY, wxT("C&lear All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_clearAll, 0, wxALL, 5 );
	
	m_checkAllButton = new wxButton( this, wxID_ANY, wxT("Check &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_checkAllButton, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_selectedFilePath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_selectedFilePath->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	bSizer1->Add( m_selectedFilePath, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_okButton = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_okButton->SetDefault(); 
	bSizer2->Add( m_okButton, 0, wxALL, 5 );
	
	m_cancelButton = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_cancelButton, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_fileCheckList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( FileCheckListBase::OnFileSelected ), NULL, this );
	m_fileCheckList->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( FileCheckListBase::OnFileCheckChanged ), NULL, this );
	m_clearAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCheckListBase::OnClearAll ), NULL, this );
	m_checkAllButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCheckListBase::OnCheckAll ), NULL, this );
}

FileCheckListBase::~FileCheckListBase()
{
	// Disconnect Events
	m_fileCheckList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( FileCheckListBase::OnFileSelected ), NULL, this );
	m_fileCheckList->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( FileCheckListBase::OnFileCheckChanged ), NULL, this );
	m_clearAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCheckListBase::OnClearAll ), NULL, this );
	m_checkAllButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCheckListBase::OnCheckAll ), NULL, this );
}
