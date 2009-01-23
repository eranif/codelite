///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "addoptioncheckdlgbase.h"

///////////////////////////////////////////////////////////////////////////

AddOptionCheckDialogBase::AddOptionCheckDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("You can enter command line options or check the options you need (select an option for help)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer24->Add( m_staticText1, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_checkListOptionsChoices;
	m_checkListOptions = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListOptionsChoices, wxLB_EXTENDED );
	bSizer26->Add( m_checkListOptions, 2, wxALL|wxEXPAND, 5 );
	
	m_textHelp = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	bSizer26->Add( m_textHelp, 1, wxALL|wxEXPAND, 5 );
	
	bSizer25->Add( bSizer26, 1, wxEXPAND, 5 );
	
	m_textOptions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer25->Add( m_textOptions, 2, wxALL|wxEXPAND, 5 );
	
	bSizer24->Add( bSizer25, 1, wxEXPAND, 5 );
	
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer24->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer4->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonCancel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer24->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer24 );
	this->Layout();
	
	// Connect Events
	m_checkListOptions->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionSelected ), NULL, this );
	m_checkListOptions->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionToggled ), NULL, this );
	m_textOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionsText ), NULL, this );
}

AddOptionCheckDialogBase::~AddOptionCheckDialogBase()
{
	// Disconnect Events
	m_checkListOptions->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionSelected ), NULL, this );
	m_checkListOptions->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionToggled ), NULL, this );
	m_textOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionsText ), NULL, this );
}
