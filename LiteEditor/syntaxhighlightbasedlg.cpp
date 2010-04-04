///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "syntaxhighlightbasedlg.h"

///////////////////////////////////////////////////////////////////////////

SyntaxHighlightBaseDlg::SyntaxHighlightBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	m_buttonApply = new wxButton( this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonApply->SetToolTip( wxT("Apply the current changes to the editor without closing the dialog") );
	
	buttonSizer->Add( m_buttonApply, 0, wxALL, 5 );
	
	m_buttonDefaults = new wxButton( this, wxID_ANY, wxT("Defaults..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonDefaults->SetToolTip( wxT("Restore all lexers to default values") );
	
	buttonSizer->Add( m_buttonDefaults, 0, wxALL, 5 );
	
	bSizer1->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonOK ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonCancel ), NULL, this );
	m_buttonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonApply ), NULL, this );
	m_buttonDefaults->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnRestoreDefaults ), NULL, this );
}

SyntaxHighlightBaseDlg::~SyntaxHighlightBaseDlg()
{
	// Disconnect Events
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonOK ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonCancel ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonApply ), NULL, this );
	m_buttonDefaults->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnRestoreDefaults ), NULL, this );
}

LexerPageBase::LexerPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Style Settings:") ), wxHORIZONTAL );
	
	m_properties = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sbSizer2->Add( m_properties, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Edit Keywords:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button5 = new wxButton( this, wxID_ANY, wxT("Set &0"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer5->Add( m_button5, 0, wxALL, 5 );
	
	m_button6 = new wxButton( this, wxID_ANY, wxT("Set &1"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer5->Add( m_button6, 0, wxALL, 5 );
	
	m_button7 = new wxButton( this, wxID_ANY, wxT("Set &2"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer5->Add( m_button7, 0, wxALL, 5 );
	
	m_button8 = new wxButton( this, wxID_ANY, wxT("Set &3"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer5->Add( m_button8, 0, wxALL, 5 );
	
	m_button9 = new wxButton( this, wxID_ANY, wxT("Set &4"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer5->Add( m_button9, 0, wxALL, 5 );
	
	fgSizer1->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Style Font:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_fontPicker = new wxFontPickerCtrl( this, wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_USEFONT_FOR_LABEL );
	m_fontPicker->SetMaxPointSize( 100 ); 
	m_fontPicker->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_fontPicker->SetToolTip( wxT("Select a font to be used with the selected style") );
	
	fgSizer1->Add( m_fontPicker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Style Foregound Colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_colourPicker = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE|wxCLRP_SHOW_LABEL|wxCLRP_USE_TEXTCTRL );
	m_colourPicker->SetToolTip( wxT("Select the foreground colour for the selected style") );
	
	fgSizer1->Add( m_colourPicker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Style Background Colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_bgColourPicker = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE|wxCLRP_SHOW_LABEL|wxCLRP_USE_TEXTCTRL );
	m_bgColourPicker->SetToolTip( wxT("Select the background colour for the selected style") );
	
	fgSizer1->Add( m_bgColourPicker, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_eolFilled = new wxCheckBox( this, wxID_ANY, wxT("Style is EOL Filled"), wxDefaultPosition, wxDefaultSize, 0 );
	m_eolFilled->SetToolTip( wxT("If a line ends with a character/word which has this style, the remaining of the line will be coloured with this style background colour") );
	
	fgSizer1->Add( m_eolFilled, 0, wxALL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_styleWithinPreProcessor = new wxCheckBox( this, wxID_ANY, wxT("Styling Within Pre-processor Line"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_styleWithinPreProcessor, 0, wxALL, 5 );
	
	sbSizer2->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	bSizer3->Add( sbSizer2, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Lexer Global Settings:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Global Font:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer2->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_globalFontPicker = new wxFontPickerCtrl( this, wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_USEFONT_FOR_LABEL );
	m_globalFontPicker->SetMaxPointSize( 100 ); 
	m_globalFontPicker->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_globalFontPicker->SetToolTip( wxT("Use this to select a font to be used by *all* styles of this lexer") );
	
	fgSizer2->Add( m_globalFontPicker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Global background Colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer2->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_globalBgColourPicker = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE|wxCLRP_SHOW_LABEL|wxCLRP_USE_TEXTCTRL );
	m_globalBgColourPicker->SetToolTip( wxT("Use this to select a background colour to be used by *all* styles of this lexer") );
	
	fgSizer2->Add( m_globalBgColourPicker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("File Extensions:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer2->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_fileSpec = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_fileSpec->SetToolTip( wxT("Associate this lexer with files which have these extensions") );
	
	fgSizer2->Add( m_fileSpec, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer1->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	bSizer3->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Text Selection:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Selected Text Background Colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer3->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_colourPickerSelTextBgColoour = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 128, 128, 128 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE|wxCLRP_SHOW_LABEL|wxCLRP_USE_TEXTCTRL );
	fgSizer3->Add( m_colourPickerSelTextBgColoour, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Selected Text Alpha:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer3->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sliderSelTextAlpha = new wxSlider( this, wxID_ANY, 50, 0, 256, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizer3->Add( m_sliderSelTextAlpha, 0, wxEXPAND|wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	sbSizer3->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	bSizer3->Add( sbSizer3, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	bSizer3->Fit( this );
	
	// Connect Events
	m_properties->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( LexerPageBase::OnItemSelected ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton0 ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton1 ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton2 ), NULL, this );
	m_button8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton3 ), NULL, this );
	m_button9->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton4 ), NULL, this );
	m_fontPicker->Connect( wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEventHandler( LexerPageBase::OnFontChanged ), NULL, this );
	m_colourPicker->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( LexerPageBase::OnColourChanged ), NULL, this );
	m_bgColourPicker->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( LexerPageBase::OnColourChanged ), NULL, this );
	m_eolFilled->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LexerPageBase::OnEolFilled ), NULL, this );
	m_styleWithinPreProcessor->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LexerPageBase::OnStyleWithinPreprocessor ), NULL, this );
	m_styleWithinPreProcessor->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LexerPageBase::OnStyleWithingPreProcessorUI ), NULL, this );
	m_globalFontPicker->Connect( wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEventHandler( LexerPageBase::OnFontChanged ), NULL, this );
	m_globalBgColourPicker->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( LexerPageBase::OnColourChanged ), NULL, this );
	m_fileSpec->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( LexerPageBase::OnText ), NULL, this );
	m_colourPickerSelTextBgColoour->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( LexerPageBase::OnSelTextChanged ), NULL, this );
	m_sliderSelTextAlpha->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
}

LexerPageBase::~LexerPageBase()
{
	// Disconnect Events
	m_properties->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( LexerPageBase::OnItemSelected ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton0 ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton1 ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton2 ), NULL, this );
	m_button8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton3 ), NULL, this );
	m_button9->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LexerPageBase::OnEditKeyWordsButton4 ), NULL, this );
	m_fontPicker->Disconnect( wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEventHandler( LexerPageBase::OnFontChanged ), NULL, this );
	m_colourPicker->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( LexerPageBase::OnColourChanged ), NULL, this );
	m_bgColourPicker->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( LexerPageBase::OnColourChanged ), NULL, this );
	m_eolFilled->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LexerPageBase::OnEolFilled ), NULL, this );
	m_styleWithinPreProcessor->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LexerPageBase::OnStyleWithinPreprocessor ), NULL, this );
	m_styleWithinPreProcessor->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( LexerPageBase::OnStyleWithingPreProcessorUI ), NULL, this );
	m_globalFontPicker->Disconnect( wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEventHandler( LexerPageBase::OnFontChanged ), NULL, this );
	m_globalBgColourPicker->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( LexerPageBase::OnColourChanged ), NULL, this );
	m_fileSpec->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( LexerPageBase::OnText ), NULL, this );
	m_colourPickerSelTextBgColoour->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( LexerPageBase::OnSelTextChanged ), NULL, this );
	m_sliderSelTextAlpha->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
	m_sliderSelTextAlpha->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( LexerPageBase::OnAlphaChanged ), NULL, this );
}
