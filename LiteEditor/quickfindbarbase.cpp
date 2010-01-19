///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "quickfindbarbase.h"

///////////////////////////////////////////////////////////////////////////

QuickFindBarBase::QuickFindBarBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_closeButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	mainSizer->Add( m_closeButton, 0, 0, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticTextFind = new wxStaticText( this, wxID_ANY, _("Find:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFind->Wrap( -1 );
	fgSizer1->Add( m_staticTextFind, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_findWhat = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_RICH2 );
	m_findWhat->SetToolTip( _("Hit ENTER to search, or Shift + ENTER to search backward") );
	
	fgSizer1->Add( m_findWhat, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_buttonFindNext = new wxButton( this, wxID_ANY, _("Next"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_buttonFindNext->SetDefault(); 
	fgSizer1->Add( m_buttonFindNext, 0, wxEXPAND|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonFindPrevious = new wxButton( this, wxID_ANY, _("Previous"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	fgSizer1->Add( m_buttonFindPrevious, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_replaceStaticText = new wxStaticText( this, wxID_ANY, _("Replace with:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_replaceStaticText->Wrap( -1 );
	fgSizer1->Add( m_replaceStaticText, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_replaceWith = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_RICH2 );
	fgSizer1->Add( m_replaceWith, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_replaceButton = new wxButton( this, wxID_ANY, _("Replace"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	fgSizer1->Add( m_replaceButton, 0, wxEXPAND|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	mainSizer->Add( fgSizer1, 1, wxEXPAND, 0 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND|wxLEFT, 5 );
	
	optionsSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxCase = new wxCheckBox( this, wxID_ANY, _("Case"), wxDefaultPosition, wxDefaultSize, 0 );
	optionsSizer->Add( m_checkBoxCase, 0, wxRIGHT|wxLEFT, 5 );
	
	m_checkBoxWord = new wxCheckBox( this, wxID_ANY, _("Word"), wxDefaultPosition, wxDefaultSize, 0 );
	optionsSizer->Add( m_checkBoxWord, 0, wxRIGHT|wxLEFT, 5 );
	
	m_checkBoxRegex = new wxCheckBox( this, wxID_ANY, _("Regexp"), wxDefaultPosition, wxDefaultSize, 0 );
	optionsSizer->Add( m_checkBoxRegex, 0, wxRIGHT|wxLEFT, 5 );
	
	mainSizer->Add( optionsSizer, 0, wxALIGN_CENTER_HORIZONTAL, 1 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_closeButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnHide ), NULL, this );
	m_findWhat->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( QuickFindBarBase::OnKeyDown ), NULL, this );
	m_findWhat->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( QuickFindBarBase::OnText ), NULL, this );
	m_findWhat->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( QuickFindBarBase::OnEnter ), NULL, this );
	m_buttonFindNext->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnNext ), NULL, this );
	m_buttonFindNext->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QuickFindBarBase::OnUpdateUI ), NULL, this );
	m_buttonFindPrevious->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnPrev ), NULL, this );
	m_buttonFindPrevious->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QuickFindBarBase::OnUpdateUI ), NULL, this );
	m_replaceWith->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( QuickFindBarBase::OnKeyDown ), NULL, this );
	m_replaceWith->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( QuickFindBarBase::OnReplaceEnter ), NULL, this );
	m_replaceWith->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QuickFindBarBase::OnReplaceUI ), NULL, this );
	m_replaceButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnReplace ), NULL, this );
	m_replaceButton->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QuickFindBarBase::OnReplaceUI ), NULL, this );
	m_checkBoxCase->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnCheckBoxCase ), NULL, this );
	m_checkBoxWord->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnCheckBoxWord ), NULL, this );
	m_checkBoxRegex->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnCheckBoxRegex ), NULL, this );
}

QuickFindBarBase::~QuickFindBarBase()
{
	// Disconnect Events
	m_closeButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnHide ), NULL, this );
	m_findWhat->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( QuickFindBarBase::OnKeyDown ), NULL, this );
	m_findWhat->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( QuickFindBarBase::OnText ), NULL, this );
	m_findWhat->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( QuickFindBarBase::OnEnter ), NULL, this );
	m_buttonFindNext->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnNext ), NULL, this );
	m_buttonFindNext->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QuickFindBarBase::OnUpdateUI ), NULL, this );
	m_buttonFindPrevious->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnPrev ), NULL, this );
	m_buttonFindPrevious->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QuickFindBarBase::OnUpdateUI ), NULL, this );
	m_replaceWith->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( QuickFindBarBase::OnKeyDown ), NULL, this );
	m_replaceWith->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( QuickFindBarBase::OnReplaceEnter ), NULL, this );
	m_replaceWith->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QuickFindBarBase::OnReplaceUI ), NULL, this );
	m_replaceButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnReplace ), NULL, this );
	m_replaceButton->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QuickFindBarBase::OnReplaceUI ), NULL, this );
	m_checkBoxCase->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnCheckBoxCase ), NULL, this );
	m_checkBoxWord->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnCheckBoxWord ), NULL, this );
	m_checkBoxRegex->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnCheckBoxRegex ), NULL, this );
}
