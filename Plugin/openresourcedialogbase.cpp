///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
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
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText2, 0, wxALIGN_BOTTOM|wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	m_staticText3->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText3, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textCtrlResourceName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_RICH2 );
	fgSizer1->Add( m_textCtrlResourceName, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_choiceResourceTypeChoices[] = { _("Workspace file"), _("Class, struct or union"), _("Function"), _("Typedef"), _("Macro"), _("Namespace") };
	int m_choiceResourceTypeNChoices = sizeof( m_choiceResourceTypeChoices ) / sizeof( wxString );
	m_choiceResourceType = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceResourceTypeNChoices, m_choiceResourceTypeChoices, 0 );
	m_choiceResourceType->SetSelection( 0 );
	fgSizer1->Add( m_choiceResourceType, 0, wxEXPAND|wxALL, 5 );
	
	mainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Matched resources:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	mainSizer->Add( m_staticText1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_listBoxOptions = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	mainSizer->Add( m_listBoxOptions, 1, wxALL|wxEXPAND, 5 );
	
	m_staticTextErrorMessage = new wxStaticText( this, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextErrorMessage->Wrap( -1 );
	m_staticTextErrorMessage->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	m_staticTextErrorMessage->SetForegroundColour( wxColour( 255, 0, 0 ) );
	m_staticTextErrorMessage->Hide();
	
	mainSizer->Add( m_staticTextErrorMessage, 0, wxALL|wxEXPAND, 5 );
	
	m_fullText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_fullText->Wrap( -1 );
	mainSizer->Add( m_fullText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_checkBoxUsePartialMatching = new wxCheckBox( this, wxID_ANY, _("Use partial matching"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_checkBoxUsePartialMatching, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_textCtrlResourceName->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( OpenResourceDialogBase::OnKeyDown ), NULL, this );
	m_textCtrlResourceName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( OpenResourceDialogBase::OnText ), NULL, this );
	m_textCtrlResourceName->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( OpenResourceDialogBase::OnEnter ), NULL, this );
	m_choiceResourceType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( OpenResourceDialogBase::OnType ), NULL, this );
	m_listBoxOptions->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( OpenResourceDialogBase::OnItemSelected ), NULL, this );
	m_listBoxOptions->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( OpenResourceDialogBase::OnItemActivated ), NULL, this );
	m_checkBoxUsePartialMatching->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( OpenResourceDialogBase::OnUsePartialMatching ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OpenResourceDialogBase::OnOK ), NULL, this );
	m_buttonOk->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( OpenResourceDialogBase::OnOKUI ), NULL, this );
}

OpenResourceDialogBase::~OpenResourceDialogBase()
{
	// Disconnect Events
	m_textCtrlResourceName->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( OpenResourceDialogBase::OnKeyDown ), NULL, this );
	m_textCtrlResourceName->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( OpenResourceDialogBase::OnText ), NULL, this );
	m_textCtrlResourceName->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( OpenResourceDialogBase::OnEnter ), NULL, this );
	m_choiceResourceType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( OpenResourceDialogBase::OnType ), NULL, this );
	m_listBoxOptions->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( OpenResourceDialogBase::OnItemSelected ), NULL, this );
	m_listBoxOptions->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( OpenResourceDialogBase::OnItemActivated ), NULL, this );
	m_checkBoxUsePartialMatching->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( OpenResourceDialogBase::OnUsePartialMatching ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OpenResourceDialogBase::OnOK ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( OpenResourceDialogBase::OnOKUI ), NULL, this );
}
