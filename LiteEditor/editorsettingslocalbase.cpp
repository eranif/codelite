///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingslocalbase.h"

///////////////////////////////////////////////////////////////////////////

LocalEditorSettingsbase::LocalEditorSettingsbase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText = new wxStaticText( this, wxID_ANY, wxT("Untick one or more checkboxes to set any local preferences, "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText->Wrap( -1 );
	mainSizer->Add( m_staticText, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Indentation") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 0, 2, 0, 20 );
	fgSizer31->AddGrowableCol( 1 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_indentsUsesTabsEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_indentsUsesTabsEnable->SetValue(true);
	
	m_indentsUsesTabsEnable->SetToolTip( wxT("Disply line numbers margin") );
	
	fgSizer31->Add( m_indentsUsesTabsEnable, 0, wxALL, 5 );
	
	m_indentsUsesTabs = new wxCheckBox( this, wxID_ANY, wxT("Use tabs in indentation"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_indentsUsesTabs->SetToolTip( wxT("If clear, only spaces will be used for indentation.\nIf set, a mixture of tabs and spaces will be used.") );
	
	fgSizer31->Add( m_indentsUsesTabs, 0, wxALL, 5 );
	
	m_tabWidthEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_tabWidthEnable->SetValue(true);
	
	m_tabWidthEnable->SetToolTip( wxT("Disply line numbers margin") );
	
	fgSizer31->Add( m_tabWidthEnable, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTexttabWidth = new wxStaticText( this, wxID_ANY, wxT("Columns per tab character in document:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTexttabWidth->Wrap( -1 );
	bSizer7->Add( m_staticTexttabWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	bSizer7->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_tabWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20, 4 );
	bSizer7->Add( m_tabWidth, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	fgSizer31->Add( bSizer7, 1, wxEXPAND, 5 );
	
	m_indentWidthEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_indentWidthEnable->SetValue(true);
	
	m_indentWidthEnable->SetToolTip( wxT("Disply line numbers margin") );
	
	fgSizer31->Add( m_indentWidthEnable, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextindentWidth = new wxStaticText( this, wxID_ANY, wxT("Columns per indentation level:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextindentWidth->Wrap( -1 );
	bSizer6->Add( m_staticTextindentWidth, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_indentWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20, 4 );
	bSizer6->Add( m_indentWidth, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	fgSizer31->Add( bSizer6, 1, wxEXPAND, 5 );
	
	sbSizer6->Add( fgSizer31, 1, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer6, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer11;
	sbSizer11 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Margins") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 0, 2, 0, 20 );
	fgSizer21->AddGrowableCol( 1 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_displayBookmarkMarginEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_displayBookmarkMarginEnable->SetValue(true);
	
	m_displayBookmarkMarginEnable->SetToolTip( wxT("Disply line numbers margin") );
	
	fgSizer21->Add( m_displayBookmarkMarginEnable, 0, wxALL, 5 );
	
	m_displayBookmarkMargin = new wxCheckBox( this, wxID_ANY, wxT("Display Selection / Bookmark margin"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_displayBookmarkMargin->SetToolTip( wxT("When enabled, mark matching brace with unique colour") );
	
	fgSizer21->Add( m_displayBookmarkMargin, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxDisplayFoldMarginEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_checkBoxDisplayFoldMarginEnable->SetValue(true);
	
	m_checkBoxDisplayFoldMarginEnable->SetToolTip( wxT("Disply line numbers margin") );
	
	fgSizer21->Add( m_checkBoxDisplayFoldMarginEnable, 0, wxALL, 5 );
	
	m_checkBoxDisplayFoldMargin = new wxCheckBox( this, wxID_ANY, wxT("Display Folding margin"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_checkBoxDisplayFoldMargin->SetToolTip( wxT("Display horizontal guides for matching braces \"{\"") );
	
	fgSizer21->Add( m_checkBoxDisplayFoldMargin, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxHideChangeMarkerMarginEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_checkBoxHideChangeMarkerMarginEnable->SetValue(true);
	
	m_checkBoxHideChangeMarkerMarginEnable->SetToolTip( wxT("Disply line numbers margin") );
	
	fgSizer21->Add( m_checkBoxHideChangeMarkerMarginEnable, 0, wxALL, 5 );
	
	m_checkBoxHideChangeMarkerMargin = new wxCheckBox( this, wxID_ANY, wxT("Hide change marker margin"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_checkBoxHideChangeMarkerMargin->SetToolTip( wxT("Display horizontal guides for matching braces \"{\"") );
	
	fgSizer21->Add( m_checkBoxHideChangeMarkerMargin, 0, wxALL, 5 );
	
	sbSizer11->Add( fgSizer21, 1, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer11, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Misc") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 20 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_displayLineNumbersEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_displayLineNumbersEnable->SetValue(true);
	
	m_displayLineNumbersEnable->SetToolTip( wxT("Disply line numbers margin") );
	
	fgSizer2->Add( m_displayLineNumbersEnable, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_displayLineNumbers = new wxCheckBox( this, wxID_ANY, wxT("Display line numbers"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_displayLineNumbers->Enable( false );
	m_displayLineNumbers->SetToolTip( wxT("Disply line numbers margin") );
	
	fgSizer2->Add( m_displayLineNumbers, 0, wxALL, 5 );
	
	m_showIndentationGuideLinesEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_showIndentationGuideLinesEnable->SetValue(true);
	
	m_showIndentationGuideLinesEnable->SetToolTip( wxT("Display horizontal guides for matching braces \"{\"") );
	
	fgSizer2->Add( m_showIndentationGuideLinesEnable, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_showIndentationGuideLines = new wxCheckBox( this, wxID_ANY, wxT("Show indentation guidelines"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_showIndentationGuideLines->SetToolTip( wxT("Display horizontal guides for matching braces \"{\"") );
	
	fgSizer2->Add( m_showIndentationGuideLines, 0, wxALL, 5 );
	
	m_highlightCaretLineEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_highlightCaretLineEnable->SetValue(true);
	
	m_highlightCaretLineEnable->SetToolTip( wxT("Set different background colour for the line containing the caret") );
	
	fgSizer2->Add( m_highlightCaretLineEnable, 0, wxALL, 5 );
	
	m_highlightCaretLine = new wxCheckBox( this, wxID_ANY, wxT("Highlight caret line"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_highlightCaretLine->Enable( false );
	m_highlightCaretLine->SetToolTip( wxT("Set different background colour for the line containing the caret") );
	
	fgSizer2->Add( m_highlightCaretLine, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_whitespaceStyleEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_whitespaceStyleEnable->SetValue(true);
	
	m_whitespaceStyleEnable->SetToolTip( wxT("Set different background colour for the line containing the caret") );
	
	fgSizer2->Add( m_whitespaceStyleEnable, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_staticTextwhitespaceStyle = new wxStaticText( this, wxID_ANY, wxT("Whitespace visibility:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextwhitespaceStyle->Wrap( -1 );
	gSizer2->Add( m_staticTextwhitespaceStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_whitespaceStyleChoices[] = { wxT("Invisible"), wxT("Visible always"), wxT("Visible after indentation"), wxT("Indentation only") };
	int m_whitespaceStyleNChoices = sizeof( m_whitespaceStyleChoices ) / sizeof( wxString );
	m_whitespaceStyle = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_whitespaceStyleNChoices, m_whitespaceStyleChoices, 0 );
	m_whitespaceStyle->SetSelection( 0 );
	gSizer2->Add( m_whitespaceStyle, 0, wxALL, 5 );
	
	fgSizer2->Add( gSizer2, 1, wxEXPAND, 5 );
	
	m_choiceEOLEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_choiceEOLEnable->SetValue(true);
	
	m_choiceEOLEnable->SetToolTip( wxT("Set different background colour for the line containing the caret") );
	
	fgSizer2->Add( m_choiceEOLEnable, 0, wxALL, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_EOLstatic = new wxStaticText( this, wxID_ANY, wxT("EOL Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_EOLstatic->Wrap( -1 );
	gSizer1->Add( m_EOLstatic, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceEOLChoices[] = { wxT("Default"), wxT("Mac (CR)"), wxT("Windows (CRLF)"), wxT("Unix (LF)") };
	int m_choiceEOLNChoices = sizeof( m_choiceEOLChoices ) / sizeof( wxString );
	m_choiceEOL = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceEOLNChoices, m_choiceEOLChoices, 0 );
	m_choiceEOL->SetSelection( 0 );
	m_choiceEOL->SetToolTip( wxT("Set the editor's EOL mode (End Of Line). When set to 'Default' CodeLite will set the EOL according to the hosting OS") );
	
	gSizer1->Add( m_choiceEOL, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer2->Add( gSizer1, 1, wxEXPAND, 5 );
	
	m_fileEncodingEnable = new wxCheckBox( this, wxID_ANY, wxT("Use global setting"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_fileEncodingEnable->SetValue(true);
	
	m_fileEncodingEnable->SetToolTip( wxT("Set different background colour for the line containing the caret") );
	
	fgSizer2->Add( m_fileEncodingEnable, 0, wxALL, 5 );
	
	wxGridSizer* gSizer21;
	gSizer21 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_staticTextfileEncoding = new wxStaticText( this, wxID_ANY, wxT("File font encoding"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextfileEncoding->Wrap( -1 );
	m_staticTextfileEncoding->Enable( false );
	
	gSizer21->Add( m_staticTextfileEncoding, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString m_fileEncodingChoices;
	m_fileEncoding = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_fileEncodingChoices, 0 );
	m_fileEncoding->SetSelection( 0 );
	gSizer21->Add( m_fileEncoding, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer2->Add( gSizer21, 1, wxEXPAND, 5 );
	
	sbSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button1 = new wxButton( this, wxID_OK, wxT("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->SetDefault(); 
	bSizer4->Add( m_button1, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button2, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_indentsUsesTabsEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::indentsUsesTabsUpdateUI ), NULL, this );
	m_tabWidthEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::tabWidthUpdateUI ), NULL, this );
	m_indentWidthEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::indentWidthUpdateUI ), NULL, this );
	m_displayBookmarkMarginEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::displayBookmarkMarginUpdateUI ), NULL, this );
	m_checkBoxDisplayFoldMarginEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::checkBoxDisplayFoldMarginUpdateUI ), NULL, this );
	m_checkBoxHideChangeMarkerMarginEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::checkBoxHideChangeMarkerMarginUpdateUI ), NULL, this );
	m_displayLineNumbersEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::displayLineNumbersUpdateUI ), NULL, this );
	m_showIndentationGuideLinesEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::showIndentationGuideLinesUpdateUI ), NULL, this );
	m_highlightCaretLineEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::highlightCaretLineUpdateUI ), NULL, this );
	m_whitespaceStyleEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::whitespaceStyleUpdateUI ), NULL, this );
	m_choiceEOLEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::choiceEOLUpdateUI ), NULL, this );
	m_fileEncodingEnable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::fileEncodingUpdateUI ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalEditorSettingsbase::OnOK ), NULL, this );
}

LocalEditorSettingsbase::~LocalEditorSettingsbase()
{
	// Disconnect Events
	m_indentsUsesTabsEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::indentsUsesTabsUpdateUI ), NULL, this );
	m_tabWidthEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::tabWidthUpdateUI ), NULL, this );
	m_indentWidthEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::indentWidthUpdateUI ), NULL, this );
	m_displayBookmarkMarginEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::displayBookmarkMarginUpdateUI ), NULL, this );
	m_checkBoxDisplayFoldMarginEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::checkBoxDisplayFoldMarginUpdateUI ), NULL, this );
	m_checkBoxHideChangeMarkerMarginEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::checkBoxHideChangeMarkerMarginUpdateUI ), NULL, this );
	m_displayLineNumbersEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::displayLineNumbersUpdateUI ), NULL, this );
	m_showIndentationGuideLinesEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::showIndentationGuideLinesUpdateUI ), NULL, this );
	m_highlightCaretLineEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::highlightCaretLineUpdateUI ), NULL, this );
	m_whitespaceStyleEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::whitespaceStyleUpdateUI ), NULL, this );
	m_choiceEOLEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::choiceEOLUpdateUI ), NULL, this );
	m_fileEncodingEnable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LocalEditorSettingsbase::fileEncodingUpdateUI ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalEditorSettingsbase::OnOK ), NULL, this );
}
