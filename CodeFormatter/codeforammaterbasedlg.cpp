///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "codeforammaterbasedlg.h"

///////////////////////////////////////////////////////////////////////////

CodeFormatterBaseDlg::CodeFormatterBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 589,618 ), wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxString m_radioBoxPredefinedStyleChoices[] = { wxT("GNU"), wxT("Java"), wxT("K&&R"), wxT("Linux"), wxT("ANSI") };
	int m_radioBoxPredefinedStyleNChoices = sizeof( m_radioBoxPredefinedStyleChoices ) / sizeof( wxString );
	m_radioBoxPredefinedStyle = new wxRadioBox( m_panel1, wxID_ANY, wxT("Predefined Style:"), wxDefaultPosition, wxDefaultSize, m_radioBoxPredefinedStyleNChoices, m_radioBoxPredefinedStyleChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxPredefinedStyle->SetSelection( 3 );
	bSizer3->Add( m_radioBoxPredefinedStyle, 0, wxALL|wxEXPAND, 0 );
	
	wxString m_radioBoxBracketsChoices[] = { wxT("Break Closing"), wxT("Attach"), wxT("Linux"), wxT("Break") };
	int m_radioBoxBracketsNChoices = sizeof( m_radioBoxBracketsChoices ) / sizeof( wxString );
	m_radioBoxBrackets = new wxRadioBox( m_panel1, wxID_ANY, wxT("Brackets:"), wxDefaultPosition, wxDefaultSize, m_radioBoxBracketsNChoices, m_radioBoxBracketsChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxBrackets->SetSelection( 2 );
	bSizer3->Add( m_radioBoxBrackets, 0, wxALL|wxEXPAND, 0 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, -1, wxT("Indentation:") ), wxVERTICAL );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 5, 2, 0, 0 );
	
	m_checkBoxIndetClass = new wxCheckBox( m_panel1, wxID_ANY, wxT("Class"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndetClass, 0, wxALL, 5 );
	
	m_checkBoxIndentBrackets = new wxCheckBox( m_panel1, wxID_ANY, wxT("Brackets"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndentBrackets, 0, wxALL, 5 );
	
	m_checkBoxIndetSwitch = new wxCheckBox( m_panel1, wxID_ANY, wxT("Switches"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndetSwitch, 0, wxALL, 5 );
	
	m_checkBoxIndentNamespaces = new wxCheckBox( m_panel1, wxID_ANY, wxT("Namespaces"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndentNamespaces, 0, wxALL, 5 );
	
	m_checkBoxIndetCase = new wxCheckBox( m_panel1, wxID_ANY, wxT("Case"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndetCase, 0, wxALL, 5 );
	
	m_checkBoxIndentLabels = new wxCheckBox( m_panel1, wxID_ANY, wxT("Labels"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndentLabels, 0, wxALL, 5 );
	
	m_checkBoxIndetBlocks = new wxCheckBox( m_panel1, wxID_ANY, wxT("Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndetBlocks, 0, wxALL, 5 );
	
	m_checkBoxIndentPreprocessors = new wxCheckBox( m_panel1, wxID_ANY, wxT("Preprocessors"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxIndentPreprocessors->SetValue(true);
	
	gSizer1->Add( m_checkBoxIndentPreprocessors, 0, wxALL, 5 );
	
	m_checkBoxIndentMaxInst = new wxCheckBox( m_panel1, wxID_ANY, wxT("Max Instatement Indent"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndentMaxInst, 0, wxALL, 5 );
	
	m_checkBoxIndentMinCond = new wxCheckBox( m_panel1, wxID_ANY, wxT("Min Conditional Indent"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndentMinCond, 0, wxALL, 5 );
	
	sbSizer1->Add( gSizer1, 0, wxEXPAND, 5 );
	
	bSizer3->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, -1, wxT("Formatting Options:") ), wxVERTICAL );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_checkBoxFormatBreakBlocks = new wxCheckBox( m_panel1, wxID_ANY, wxT("Break Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatBreakBlocks, 0, wxALL, 5 );
	
	m_checkBoxFormatPadParenth = new wxCheckBox( m_panel1, wxID_ANY, wxT("Pad Parenthesis"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatPadParenth, 0, wxALL, 5 );
	
	m_checkBoxFormatBreakBlocksAll = new wxCheckBox( m_panel1, wxID_ANY, wxT("Break Blocks All"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatBreakBlocksAll, 0, wxALL, 5 );
	
	m_checkBoxFormatPadParentOut = new wxCheckBox( m_panel1, wxID_ANY, wxT("Pad Parenthesis Outside"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatPadParentOut, 0, wxALL, 5 );
	
	m_checkBoxFormatBreakElseif = new wxCheckBox( m_panel1, wxID_ANY, wxT("Break else-if"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatBreakElseif, 0, wxALL, 5 );
	
	m_checkBoxFormatPadParentIn = new wxCheckBox( m_panel1, wxID_ANY, wxT("Pad Parenthesis  Inside"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatPadParentIn, 0, wxALL, 5 );
	
	m_checkBoxFormatPadOperators = new wxCheckBox( m_panel1, wxID_ANY, wxT("Pad Operators"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatPadOperators, 0, wxALL, 5 );
	
	m_checkBoxFormatUnPadParent = new wxCheckBox( m_panel1, wxID_ANY, wxT("UnPad Parenthesis"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatUnPadParent, 0, wxALL, 5 );
	
	m_checkBoxFormatOneLineKeepStmnt = new wxCheckBox( m_panel1, wxID_ANY, wxT("One Line Keep Statement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatOneLineKeepStmnt, 0, wxALL, 5 );
	
	m_checkBoxFormatFillEmptyLines = new wxCheckBox( m_panel1, wxID_ANY, wxT("Fill Empty Lines"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatFillEmptyLines, 0, wxALL, 5 );
	
	m_checkBoxFormatOneLineKeepBlocks = new wxCheckBox( m_panel1, wxID_ANY, wxT("One Line Keep Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer2->Add( m_checkBoxFormatOneLineKeepBlocks, 0, wxALL, 5 );
	
	sbSizer2->Add( gSizer2, 0, wxEXPAND, 5 );
	
	bSizer3->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	m_textCtrlPreview = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	m_textCtrlPreview->SetFont( wxFont( 8, 76, 90, 90, false, wxEmptyString ) );
	m_textCtrlPreview->SetMinSize( wxSize( -1,200 ) );
	
	bSizer3->Add( m_textCtrlPreview, 1, wxALL|wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonClose = new wxButton( this, wxID_CANCEL, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonClose, 0, wxALL, 5 );
	
	m_buttonHelp = new wxButton( this, wxID_ANY, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonHelp, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
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
