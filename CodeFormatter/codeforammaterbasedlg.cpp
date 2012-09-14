///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "codeforammaterbasedlg.h"

///////////////////////////////////////////////////////////////////////////

CodeFormatterBaseDlg::CodeFormatterBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	m_splitterSettingsPreview = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE|wxSP_NO_XP_THEME );
	m_splitterSettingsPreview->SetSashGravity( 0.5 );
	m_splitterSettingsPreview->Connect( wxEVT_IDLE, wxIdleEventHandler( CodeFormatterBaseDlg::m_splitterSettingsPreviewOnIdle ), NULL, this );
	
	m_panelSettings = new wxPanel( m_splitterSettingsPreview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerSettings;
	bSizerSettings = new wxBoxSizer( wxVERTICAL );
	
	m_notebookSettings = new wxNotebook( m_panelSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panelPredefinedStyles = new wxPanel( m_notebookSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerPredefinedStyles;
	bSizerPredefinedStyles = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* bSizerPredefinedStylesText;
	bSizerPredefinedStylesText = new wxStaticBoxSizer( new wxStaticBox( m_panelPredefinedStyles, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText2 = new wxStaticText( m_panelPredefinedStyles, wxID_ANY, _("Choose one of the following predefined styles.\nYou may optionally override one or more of its settings from the next pages"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizerPredefinedStylesText->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );
	
	bSizerPredefinedStyles->Add( bSizerPredefinedStylesText, 0, wxEXPAND|wxALL, 5 );
	
	wxString m_radioBoxPredefinedStyleChoices[] = { _("GNU"), _("Java"), _("K&&R"), _("Linux"), _("ANSI") };
	int m_radioBoxPredefinedStyleNChoices = sizeof( m_radioBoxPredefinedStyleChoices ) / sizeof( wxString );
	m_radioBoxPredefinedStyle = new wxRadioBox( m_panelPredefinedStyles, wxID_ANY, _("Predefined Style:"), wxDefaultPosition, wxDefaultSize, m_radioBoxPredefinedStyleNChoices, m_radioBoxPredefinedStyleChoices, 1, wxRA_SPECIFY_COLS );
	m_radioBoxPredefinedStyle->SetSelection( 0 );
	bSizerPredefinedStyles->Add( m_radioBoxPredefinedStyle, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbPredefineHelpSizer;
	sbPredefineHelpSizer = new wxStaticBoxSizer( new wxStaticBox( m_panelPredefinedStyles, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticTextPredefineHelp = new wxStaticText( m_panelPredefinedStyles, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPredefineHelp->Wrap( -1 );
	sbPredefineHelpSizer->Add( m_staticTextPredefineHelp, 1, wxALL|wxEXPAND, 5 );
	
	bSizerPredefinedStyles->Add( sbPredefineHelpSizer, 1, wxEXPAND|wxALL, 5 );
	
	m_panelPredefinedStyles->SetSizer( bSizerPredefinedStyles );
	m_panelPredefinedStyles->Layout();
	bSizerPredefinedStyles->Fit( m_panelPredefinedStyles );
	m_notebookSettings->AddPage( m_panelPredefinedStyles, _("Predefined Styles"), true );
	m_panelBrackets = new wxPanel( m_notebookSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bBracketsSizer;
	bBracketsSizer = new wxBoxSizer( wxVERTICAL );
	
	wxString m_radioBoxBracketsChoices[] = { _("Break Closing"), _("Attach"), _("Linux"), _("Break"), _("None") };
	int m_radioBoxBracketsNChoices = sizeof( m_radioBoxBracketsChoices ) / sizeof( wxString );
	m_radioBoxBrackets = new wxRadioBox( m_panelBrackets, wxID_ANY, _("Brackets:"), wxDefaultPosition, wxDefaultSize, m_radioBoxBracketsNChoices, m_radioBoxBracketsChoices, 1, wxRA_SPECIFY_COLS );
	m_radioBoxBrackets->SetSelection( 3 );
	bBracketsSizer->Add( m_radioBoxBrackets, 0, wxALL|wxEXPAND, 5 );
	
	m_panelBrackets->SetSizer( bBracketsSizer );
	m_panelBrackets->Layout();
	bBracketsSizer->Fit( m_panelBrackets );
	m_notebookSettings->AddPage( m_panelBrackets, _("Brackets"), false );
	m_panelIndentation = new wxPanel( m_notebookSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbIndentationSizer;
	sbIndentationSizer = new wxStaticBoxSizer( new wxStaticBox( m_panelIndentation, wxID_ANY, _("Indentation:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgIdentationSizer;
	fgIdentationSizer = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgIdentationSizer->AddGrowableCol( 0 );
	fgIdentationSizer->AddGrowableCol( 1 );
	fgIdentationSizer->AddGrowableCol( 2 );
	fgIdentationSizer->SetFlexibleDirection( wxBOTH );
	fgIdentationSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_checkBoxIndetClass = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Class"), wxDefaultPosition, wxDefaultSize, 0 );
	fgIdentationSizer->Add( m_checkBoxIndetClass, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxIndentBrackets = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Brackets"), wxDefaultPosition, wxDefaultSize, 0 );
	fgIdentationSizer->Add( m_checkBoxIndentBrackets, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxIndetSwitch = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Switches"), wxDefaultPosition, wxDefaultSize, 0 );
	fgIdentationSizer->Add( m_checkBoxIndetSwitch, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxIndentNamespaces = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Namespaces"), wxDefaultPosition, wxDefaultSize, 0 );
	fgIdentationSizer->Add( m_checkBoxIndentNamespaces, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxIndetCase = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Case"), wxDefaultPosition, wxDefaultSize, 0 );
	fgIdentationSizer->Add( m_checkBoxIndetCase, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxIndentLabels = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Labels"), wxDefaultPosition, wxDefaultSize, 0 );
	fgIdentationSizer->Add( m_checkBoxIndentLabels, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxIndetBlocks = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	fgIdentationSizer->Add( m_checkBoxIndetBlocks, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxIndentPreprocessors = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Preprocessors"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxIndentPreprocessors->SetValue(true); 
	fgIdentationSizer->Add( m_checkBoxIndentPreprocessors, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxIndentMaxInst = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Max Instatement Indent"), wxDefaultPosition, wxDefaultSize, 0 );
	fgIdentationSizer->Add( m_checkBoxIndentMaxInst, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxIndentMinCond = new wxCheckBox( m_panelIndentation, wxID_ANY, _("Min Conditional Indent"), wxDefaultPosition, wxDefaultSize, 0 );
	fgIdentationSizer->Add( m_checkBoxIndentMinCond, 0, wxALL|wxEXPAND, 5 );
	
	sbIndentationSizer->Add( fgIdentationSizer, 1, wxEXPAND, 5 );
	
	m_panelIndentation->SetSizer( sbIndentationSizer );
	m_panelIndentation->Layout();
	sbIndentationSizer->Fit( m_panelIndentation );
	m_notebookSettings->AddPage( m_panelIndentation, _("Indentation"), false );
	m_panelFormatting = new wxPanel( m_notebookSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbFormattingSizer;
	sbFormattingSizer = new wxStaticBoxSizer( new wxStaticBox( m_panelFormatting, wxID_ANY, _("Formatting Options:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgFormattingSizer;
	fgFormattingSizer = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgFormattingSizer->AddGrowableCol( 0 );
	fgFormattingSizer->AddGrowableCol( 1 );
	fgFormattingSizer->AddGrowableCol( 2 );
	fgFormattingSizer->SetFlexibleDirection( wxBOTH );
	fgFormattingSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxFormatBreakBlocks = new wxCheckBox( m_panelFormatting, wxID_ANY, _("Break Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatBreakBlocks, 0, wxALL, 5 );
	
	m_checkBoxFormatPadParenth = new wxCheckBox( m_panelFormatting, wxID_ANY, _("Pad Parenthesis"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatPadParenth, 0, wxALL, 5 );
	
	m_checkBoxFormatBreakBlocksAll = new wxCheckBox( m_panelFormatting, wxID_ANY, _("Break Blocks All"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatBreakBlocksAll, 0, wxALL, 5 );
	
	m_checkBoxFormatPadParentOut = new wxCheckBox( m_panelFormatting, wxID_ANY, _("Pad Parenthesis Outside"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatPadParentOut, 0, wxALL, 5 );
	
	m_checkBoxFormatBreakElseif = new wxCheckBox( m_panelFormatting, wxID_ANY, _("Break else-if"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatBreakElseif, 0, wxALL, 5 );
	
	m_checkBoxFormatPadParentIn = new wxCheckBox( m_panelFormatting, wxID_ANY, _("Pad Parenthesis  Inside"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatPadParentIn, 0, wxALL, 5 );
	
	m_checkBoxFormatPadOperators = new wxCheckBox( m_panelFormatting, wxID_ANY, _("Pad Operators"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatPadOperators, 0, wxALL, 5 );
	
	m_checkBoxFormatUnPadParent = new wxCheckBox( m_panelFormatting, wxID_ANY, _("UnPad Parenthesis"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatUnPadParent, 0, wxALL, 5 );
	
	m_checkBoxFormatOneLineKeepStmnt = new wxCheckBox( m_panelFormatting, wxID_ANY, _("One Line Keep Statement"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatOneLineKeepStmnt, 0, wxALL, 5 );
	
	m_checkBoxFormatFillEmptyLines = new wxCheckBox( m_panelFormatting, wxID_ANY, _("Fill Empty Lines"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatFillEmptyLines, 0, wxALL, 5 );
	
	m_checkBoxFormatOneLineKeepBlocks = new wxCheckBox( m_panelFormatting, wxID_ANY, _("One Line Keep Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	fgFormattingSizer->Add( m_checkBoxFormatOneLineKeepBlocks, 0, wxALL, 5 );
	
	sbFormattingSizer->Add( fgFormattingSizer, 1, wxEXPAND, 5 );
	
	m_panelFormatting->SetSizer( sbFormattingSizer );
	m_panelFormatting->Layout();
	sbFormattingSizer->Fit( m_panelFormatting );
	m_notebookSettings->AddPage( m_panelFormatting, _("Formatting"), false );
	m_panelCustomSettings = new wxPanel( m_notebookSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bCustomSettingsSizer;
	bCustomSettingsSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText3 = new wxStaticText( m_panelCustomSettings, wxID_ANY, _("Custom user settings:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bCustomSettingsSizer->Add( m_staticText3, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrlUserFlags = new wxTextCtrl( m_panelCustomSettings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlUserFlags->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bCustomSettingsSizer->Add( m_textCtrlUserFlags, 1, wxALL|wxEXPAND, 5 );
	
	m_panelCustomSettings->SetSizer( bCustomSettingsSizer );
	m_panelCustomSettings->Layout();
	bCustomSettingsSizer->Fit( m_panelCustomSettings );
	m_notebookSettings->AddPage( m_panelCustomSettings, _("Custom Settings"), false );
	
	bSizerSettings->Add( m_notebookSettings, 3, wxEXPAND | wxALL, 5 );
	
	m_panelSettings->SetSizer( bSizerSettings );
	m_panelSettings->Layout();
	bSizerSettings->Fit( m_panelSettings );
	m_panelPreview = new wxPanel( m_splitterSettingsPreview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bPreviewSizer;
	bPreviewSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlPreview = new wxScintilla( m_panelPreview, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxNO_BORDER, wxEmptyString );
	m_textCtrlPreview->SetUseTabs( true );
	m_textCtrlPreview->SetTabWidth( 4 );
	m_textCtrlPreview->SetIndent( 4 );
	m_textCtrlPreview->SetTabIndents( true );
	m_textCtrlPreview->SetBackSpaceUnIndents( true );
	m_textCtrlPreview->SetViewEOL( false );
	m_textCtrlPreview->SetViewWhiteSpace( false );
	m_textCtrlPreview->SetMarginWidth( 2, 0 );
	m_textCtrlPreview->SetIndentationGuides( true );
	m_textCtrlPreview->SetMarginWidth( 1, 0 );
	m_textCtrlPreview->SetMarginWidth( 0, 0 );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS );
	m_textCtrlPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	m_textCtrlPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS );
	m_textCtrlPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	m_textCtrlPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_EMPTY );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUS );
	m_textCtrlPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	m_textCtrlPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUS );
	m_textCtrlPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	m_textCtrlPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_EMPTY );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_EMPTY );
	m_textCtrlPreview->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	m_textCtrlPreview->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	m_textCtrlPreview->SetMinSize( wxSize( 300,-1 ) );
	
	bPreviewSizer->Add( m_textCtrlPreview, 1, wxEXPAND | wxALL, 5 );
	
	m_panelPreview->SetSizer( bPreviewSizer );
	m_panelPreview->Layout();
	bPreviewSizer->Fit( m_panelPreview );
	m_splitterSettingsPreview->SplitVertically( m_panelSettings, m_panelPreview, 450 );
	bSizerMain->Add( m_splitterSettingsPreview, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizerButtons->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonClose = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( m_buttonClose, 0, wxALL, 5 );
	
	m_buttonHelp = new wxButton( this, wxID_ANY, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( m_buttonHelp, 0, wxALL, 5 );
	
	bSizerMain->Add( bSizerButtons, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizerMain );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_radioBoxPredefinedStyle->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( CodeFormatterBaseDlg::OnRadioBoxPredefinedStyle ), NULL, this );
	m_radioBoxBrackets->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( CodeFormatterBaseDlg::OnRadioBoxBrackets ), NULL, this );
	m_checkBoxIndetClass->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentBrackets->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndetSwitch->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentNamespaces->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndetCase->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentLabels->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndetBlocks->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentPreprocessors->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentMaxInst->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentMinCond->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatBreakBlocks->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatPadParenth->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatBreakBlocksAll->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatPadParentOut->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatBreakElseif->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatPadParentIn->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatPadOperators->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatUnPadParent->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatOneLineKeepStmnt->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatFillEmptyLines->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatOneLineKeepBlocks->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
}

CodeFormatterBaseDlg::~CodeFormatterBaseDlg()
{
	// Disconnect Events
	m_radioBoxPredefinedStyle->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( CodeFormatterBaseDlg::OnRadioBoxPredefinedStyle ), NULL, this );
	m_radioBoxBrackets->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( CodeFormatterBaseDlg::OnRadioBoxBrackets ), NULL, this );
	m_checkBoxIndetClass->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentBrackets->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndetSwitch->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentNamespaces->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndetCase->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentLabels->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndetBlocks->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentPreprocessors->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentMaxInst->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxIndentMinCond->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatBreakBlocks->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatPadParenth->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatBreakBlocksAll->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatPadParentOut->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatBreakElseif->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatPadParentIn->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatPadOperators->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatUnPadParent->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatOneLineKeepStmnt->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatFillEmptyLines->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	m_checkBoxFormatOneLineKeepBlocks->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CodeFormatterBaseDlg::OnCheckBox ), NULL, this );
	
}
