///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "manageperspectivesbasedlg.h"

///////////////////////////////////////////////////////////////////////////

ManagePerspectivesBaseDlg::ManagePerspectivesBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer3->Add( m_listBox, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_button3 = new wxButton( this, wxID_DELETE, _("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button3, 0, wxALL|wxEXPAND, 5 );
	
	m_button4 = new wxButton( this, wxID_RENAME, _("&Rename"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button4, 0, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ManagePerspectivesBaseDlg::OnDeletePerspective ), NULL, this );
	m_button3->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ManagePerspectivesBaseDlg::OnDeletePerspectiveUI ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ManagePerspectivesBaseDlg::OnRenamePerspective ), NULL, this );
	m_button4->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ManagePerspectivesBaseDlg::OnRenamePerspectiveUI ), NULL, this );
}

ManagePerspectivesBaseDlg::~ManagePerspectivesBaseDlg()
{
	// Disconnect Events
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ManagePerspectivesBaseDlg::OnDeletePerspective ), NULL, this );
	m_button3->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ManagePerspectivesBaseDlg::OnDeletePerspectiveUI ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ManagePerspectivesBaseDlg::OnRenamePerspective ), NULL, this );
	m_button4->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ManagePerspectivesBaseDlg::OnRenamePerspectiveUI ), NULL, this );
	
}
