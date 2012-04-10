///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "threebuttonbasedlg.h"

///////////////////////////////////////////////////////////////////////////

ThreeButtonBaseDlg::ThreeButtonBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap = new wxStaticBitmap( m_mainPanel, wxID_ANY, wxBitmap( wxT("question_and_answer"), wxBITMAP_TYPE_RESOURCE ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_bitmap, 0, wxALL, 5 );
	
	m_message = new wxStaticText( m_mainPanel, wxID_ANY, _("Message"), wxDefaultPosition, wxDefaultSize, 0 );
	m_message->Wrap( -1 );
	bSizer2->Add( m_message, 1, wxALL|wxEXPAND, 5 );
	
	m_mainPanel->SetSizer( bSizer2 );
	m_mainPanel->Layout();
	bSizer2->Fit( m_mainPanel );
	mainSizer->Add( m_mainPanel, 1, wxEXPAND | wxALL, 5 );
	
	m_checkBoxRememberMyAnwer = new wxCheckBox( this, wxID_ANY, _("Remember my answer and dont ask me again"), wxDefaultPosition, wxDefaultSize, 0 );
	
	mainSizer->Add( m_checkBoxRememberMyAnwer, 0, wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonYes = new wxButton( this, wxID_OK, _("&Yes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonYes->SetDefault(); 
	buttonSizer->Add( m_buttonYes, 0, wxALL, 5 );
	
	m_buttonNo = new wxButton( this, wxID_NO, _("&No"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonNo, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_buttonYes->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ThreeButtonBaseDlg::OnButtonYes ), NULL, this );
	m_buttonNo->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ThreeButtonBaseDlg::OnButtonNo ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ThreeButtonBaseDlg::OnButtonCancel ), NULL, this );
}

ThreeButtonBaseDlg::~ThreeButtonBaseDlg()
{
	// Disconnect Events
	m_buttonYes->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ThreeButtonBaseDlg::OnButtonYes ), NULL, this );
	m_buttonNo->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ThreeButtonBaseDlg::OnButtonNo ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ThreeButtonBaseDlg::OnButtonCancel ), NULL, this );
}
