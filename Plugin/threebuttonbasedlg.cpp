///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 20 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "threebuttonbasedlg.h"
#include "wx/xrc/xmlres.h"

///////////////////////////////////////////////////////////////////////////

ThreeButtonBaseDlg::ThreeButtonBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap = new wxStaticBitmap( m_mainPanel , wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("question_and_answer")));
	bSizer2->Add( m_bitmap, 0, wxALL, 5 );
	
	m_message = new wxStaticText( m_mainPanel, wxID_ANY, wxT("Message"), wxDefaultPosition, wxDefaultSize, 0 );
	m_message->Wrap( -1 );
	bSizer2->Add( m_message, 1, wxALL|wxEXPAND, 5 );
	
	m_mainPanel->SetSizer( bSizer2 );
	m_mainPanel->Layout();
	bSizer2->Fit( m_mainPanel );
	mainSizer->Add( m_mainPanel, 1, wxEXPAND | wxALL, 5 );
	
	m_checkBoxRememberMyAnwer = new wxCheckBox( this, wxID_ANY, wxT("Remember my answer and dont ask me again"), wxDefaultPosition, wxDefaultSize, 0 );
	
	mainSizer->Add( m_checkBoxRememberMyAnwer, 0, wxALL, 5 );
	
	m_staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonYes = new wxButton( this, wxID_OK, wxT("&Yes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonYes->SetDefault(); 
	buttonSizer->Add( m_buttonYes, 0, wxALL, 5 );
	
	m_buttonNo = new wxButton( this, wxID_NO, wxT("&No"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonNo, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
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
