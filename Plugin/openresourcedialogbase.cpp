///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "openresourcedialogbase.h"

///////////////////////////////////////////////////////////////////////////

OpenResourceDialogBase::OpenResourceDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrlResourceName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_PROCESS_ENTER|wxTE_RICH2 );
	m_textCtrlResourceName->SetFont( wxFont( 12, 76, 90, 90, false, wxEmptyString ) );
	
	mainSizer->Add( m_textCtrlResourceName, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Matched resources:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	mainSizer->Add( m_staticText1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_listOptions = new wxListView( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	mainSizer->Add( m_listOptions, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_fullText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_fullText->Wrap( -1 );
	sbSizer1->Add( m_fullText, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
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
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_textCtrlResourceName->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( OpenResourceDialogBase::OnKeyDown ), NULL, this );
	m_textCtrlResourceName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( OpenResourceDialogBase::OnText ), NULL, this );
	m_textCtrlResourceName->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( OpenResourceDialogBase::OnEnter ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OpenResourceDialogBase::OnOK ), NULL, this );
	m_buttonOk->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( OpenResourceDialogBase::OnOKUI ), NULL, this );
}

OpenResourceDialogBase::~OpenResourceDialogBase()
{
	// Disconnect Events
	m_textCtrlResourceName->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( OpenResourceDialogBase::OnKeyDown ), NULL, this );
	m_textCtrlResourceName->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( OpenResourceDialogBase::OnText ), NULL, this );
	m_textCtrlResourceName->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( OpenResourceDialogBase::OnEnter ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OpenResourceDialogBase::OnOK ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( OpenResourceDialogBase::OnOKUI ), NULL, this );
	
}
