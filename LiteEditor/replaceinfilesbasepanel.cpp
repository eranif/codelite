///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 20 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "replaceinfilesbasepanel.h"

///////////////////////////////////////////////////////////////////////////

ReplaceInFilesBasePanel::ReplaceInFilesBasePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("Matched results (double click on an entry to replace single selection):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer1->Add( m_staticText8, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	bSizer4->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( bSizer4, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Replace With:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer15->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlReplaceWith = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_textCtrlReplaceWith, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonReplaceAll = new wxButton( this, wxID_ANY, wxT("Replace &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_buttonReplaceAll, 0, wxALL, 5 );
	
	m_buttonClear = new wxButton( this, wxID_ANY, wxT("&Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_buttonClear, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer15, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ReplaceInFilesBasePanel::OnItemSelected ), NULL, this );
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ReplaceInFilesBasePanel::OnItemDClicked ), NULL, this );
	m_buttonReplaceAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplaceInFilesBasePanel::OnReplaceAll ), NULL, this );
	m_buttonReplaceAll->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ReplaceInFilesBasePanel::OnReplaceAllUI ), NULL, this );
	m_buttonClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplaceInFilesBasePanel::OnClearResults ), NULL, this );
	m_buttonClear->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ReplaceInFilesBasePanel::OnClearResultsUI ), NULL, this );
}

ReplaceInFilesBasePanel::~ReplaceInFilesBasePanel()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ReplaceInFilesBasePanel::OnItemSelected ), NULL, this );
	m_listBox1->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ReplaceInFilesBasePanel::OnItemDClicked ), NULL, this );
	m_buttonReplaceAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplaceInFilesBasePanel::OnReplaceAll ), NULL, this );
	m_buttonReplaceAll->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ReplaceInFilesBasePanel::OnReplaceAllUI ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ReplaceInFilesBasePanel::OnClearResults ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ReplaceInFilesBasePanel::OnClearResultsUI ), NULL, this );
}
