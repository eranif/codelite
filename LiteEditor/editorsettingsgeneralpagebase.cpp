///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingsgeneralpagebase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsGeneralPageBase::EditorSettingsGeneralPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Guides") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_displayLineNumbers = new wxCheckBox( this, wxID_ANY, wxT("Display line numbers"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_displayLineNumbers, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxMatchBraces = new wxCheckBox( this, wxID_ANY, wxT("Highlight matched braces"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_checkBoxMatchBraces, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_showIndentationGuideLines = new wxCheckBox( this, wxID_ANY, wxT("Show indentation guidelines"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_showIndentationGuideLines, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_highlighyCaretLine = new wxCheckBox( this, wxID_ANY, wxT("Highlight caret line"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add( m_highlighyCaretLine, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Caret line background colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer2->Add( m_staticText9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_caretLineColourPicker = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE|wxCLRP_SHOW_LABEL );
	fgSizer2->Add( m_caretLineColourPicker, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Whitespace visibility:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_whitespaceStyleChoices[] = { wxT("Invisible"), wxT("Visible always"), wxT("Visible after indentation"), wxT("Indentation only") };
	int m_whitespaceStyleNChoices = sizeof( m_whitespaceStyleChoices ) / sizeof( wxString );
	m_whitespaceStyle = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_whitespaceStyleNChoices, m_whitespaceStyleChoices, 0 );
	m_whitespaceStyle->SetSelection( 0 );
	fgSizer2->Add( m_whitespaceStyle, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	sbSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Indentation") ), wxVERTICAL );
	
	m_indentsUsesTabs = new wxCheckBox( this, wxID_ANY, wxT("Use tabs in indentation"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_indentsUsesTabs->SetToolTip( wxT("If clear, only spaces will be used for indentation.\nIf set, a mixture of tabs and spaces will be used.") );
	
	sbSizer6->Add( m_indentsUsesTabs, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer31->AddGrowableCol( 1 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Columns per indentation level:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer31->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_indentWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20, 4 );
	fgSizer31->Add( m_indentWidth, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText31 = new wxStaticText( this, wxID_ANY, wxT("Columns per tab character in document:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizer31->Add( m_staticText31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_tabWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20, 4 );
	fgSizer31->Add( m_tabWidth, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	sbSizer6->Add( fgSizer31, 1, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer6, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Right Margin Indicator") ), wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtnRMDisabled = new wxRadioButton( this, wxID_ANY, wxT("Disabled"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_radioBtnRMDisabled->SetValue( true ); 
	bSizer3->Add( m_radioBtnRMDisabled, 0, wxALL, 5 );
	
	m_radioBtnRMLine = new wxRadioButton( this, wxID_ANY, wxT("Line"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_radioBtnRMLine, 0, wxALL, 5 );
	
	m_radioBtnRMBackground = new wxRadioButton( this, wxID_ANY, wxT("Background"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_radioBtnRMBackground, 0, wxALL, 5 );
	
	sbSizer4->Add( bSizer3, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText41 = new wxStaticText( this, wxID_ANY, wxT("Indicator Column:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText41->Wrap( -1 );
	fgSizer3->Add( m_staticText41, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_rightMarginColumn = new wxSpinCtrl( this, wxID_ANY, wxT("80"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 256, 0 );
	fgSizer3->Add( m_rightMarginColumn, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Indicator Colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer3->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_rightMarginColour = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 192, 192, 192 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE|wxCLRP_SHOW_LABEL );
	fgSizer3->Add( m_rightMarginColour, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	sbSizer4->Add( fgSizer3, 1, wxEXPAND|wxALL, 5 );
	
	mainSizer->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Save Options") ), wxVERTICAL );
	
	m_checkBoxTrimLine = new wxCheckBox( this, wxID_ANY, wxT("When saving file, trim empty lines"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer3->Add( m_checkBoxTrimLine, 0, wxALL, 5 );
	
	m_checkBoxAppendLF = new wxCheckBox( this, wxID_ANY, wxT("If missing, append EOL at end of file"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer3->Add( m_checkBoxAppendLF, 0, wxALL, 5 );
	
	mainSizer->Add( sbSizer3, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_radioBoxNavigationMethodChoices[] = { wxT("Ctrl+Mouse left button jumps to declaration / Ctrl+Alt+Mouse left button jumps to implementation"), wxT("Mouse middle button jumps to declaration / Ctrl+Mouse middle button jumps to implementation") };
	int m_radioBoxNavigationMethodNChoices = sizeof( m_radioBoxNavigationMethodChoices ) / sizeof( wxString );
	m_radioBoxNavigationMethod = new wxRadioBox( this, wxID_ANY, wxT("Quick Code Navigation "), wxDefaultPosition, wxDefaultSize, m_radioBoxNavigationMethodNChoices, m_radioBoxNavigationMethodChoices, 1, wxRA_SPECIFY_COLS );
	m_radioBoxNavigationMethod->SetSelection( 0 );
	mainSizer->Add( m_radioBoxNavigationMethod, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_highlighyCaretLine->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorSettingsGeneralPageBase::OnHighlightCaretLine ), NULL, this );
	m_radioBtnRMDisabled->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorSettingsGeneralPageBase::OnRightMarginIndicator ), NULL, this );
	m_radioBtnRMLine->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorSettingsGeneralPageBase::OnRightMarginIndicator ), NULL, this );
	m_radioBtnRMBackground->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorSettingsGeneralPageBase::OnRightMarginIndicator ), NULL, this );
}

EditorSettingsGeneralPageBase::~EditorSettingsGeneralPageBase()
{
	// Disconnect Events
	m_highlighyCaretLine->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorSettingsGeneralPageBase::OnHighlightCaretLine ), NULL, this );
	m_radioBtnRMDisabled->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorSettingsGeneralPageBase::OnRightMarginIndicator ), NULL, this );
	m_radioBtnRMLine->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorSettingsGeneralPageBase::OnRightMarginIndicator ), NULL, this );
	m_radioBtnRMBackground->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorSettingsGeneralPageBase::OnRightMarginIndicator ), NULL, this );
}
