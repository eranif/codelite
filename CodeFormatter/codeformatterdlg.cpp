//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : codeformatterdlg.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "codeformatter.h"
#include "windowattrmanager.h"
#include "codeformatterdlg.h"
#include "editor_config.h"
#include "lexer_configuration.h"

CodeFormatterDlg::CodeFormatterDlg( wxWindow* parent, CodeFormatter *cf, const FormatOptions& opts, const wxString &sampleCode )
	: CodeFormatterBaseDlg( parent )
	, m_cf(cf)
	, m_sampleCode(sampleCode)
{
	// center the dialog
	Centre();

	m_options = opts;
	m_buttonOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeFormatterDlg::OnOK), NULL, this);
	m_buttonHelp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeFormatterDlg::OnHelp), NULL, this);

	// Initialise dialog
	m_textCtrlPreview->SetText(m_sampleCode);

	GetSizer()->Fit(this);
	InitDialog();
	UpdatePreview();

	m_radioBoxPredefinedStyle->SetFocus();

	WindowAttrManager::Load(this, wxT("CodeFormatterDlgAttr"), m_cf->GetManager()->GetConfigTool());
}

void CodeFormatterDlg::UpdateCheckBox(wxCheckBox *obj, size_t flag)
{
	obj->SetValue(m_options.GetOptions() & flag ? true : false);
}

void CodeFormatterDlg::InitDialog()
{
	UpdateCheckBox(m_checkBoxFormatBreakBlocks,AS_BREAK_BLOCKS);
	UpdateCheckBox(m_checkBoxFormatBreakBlocksAll,AS_BREAK_BLOCKS_ALL);
	UpdateCheckBox(m_checkBoxFormatBreakElseif,AS_BREAK_ELSEIF);
	UpdateCheckBox(m_checkBoxFormatFillEmptyLines,AS_FILL_EMPTY_LINES);
	UpdateCheckBox(m_checkBoxFormatOneLineKeepBlocks,AS_ONE_LINE_KEEP_BLOCKS);
	UpdateCheckBox(m_checkBoxFormatOneLineKeepStmnt,AS_ONE_LINE_KEEP_STATEMENT);
	UpdateCheckBox(m_checkBoxFormatPadOperators,AS_PAD_OPER);
	UpdateCheckBox(m_checkBoxFormatPadParenth,AS_PAD_PARENTHESIS);
	UpdateCheckBox(m_checkBoxFormatPadParentIn,AS_PAD_PARENTHESIS_IN);
	UpdateCheckBox(m_checkBoxFormatPadParentOut,AS_PAD_PARENTHESIS_OUT);
	UpdateCheckBox(m_checkBoxFormatUnPadParent,AS_UNPAD_PARENTHESIS);
	UpdateCheckBox(m_checkBoxIndentBrackets,AS_INDENT_BRACKETS);
	UpdateCheckBox(m_checkBoxIndentLabels,AS_INDENT_LABELS);
	UpdateCheckBox(m_checkBoxIndentMaxInst,AS_MAX_INSTATEMENT_INDENT);
	UpdateCheckBox(m_checkBoxIndentMinCond,AS_MIN_COND_INDENT);
	UpdateCheckBox(m_checkBoxIndentNamespaces,AS_INDENT_NAMESPACES);
	UpdateCheckBox(m_checkBoxIndentPreprocessors,AS_INDENT_PREPROCESSORS);
	UpdateCheckBox(m_checkBoxIndetBlocks,AS_INDENT_BLOCKS);
	UpdateCheckBox(m_checkBoxIndetCase,AS_INDENT_CASE);
	UpdateCheckBox(m_checkBoxIndetClass,AS_INDENT_CLASS);
	UpdateCheckBox(m_checkBoxIndetSwitch,AS_INDENT_SWITCHES);

	//update the two radio box controls
	int selection(3);	//AS_LINUX
	if (m_options.GetOptions() & AS_LINUX) {
		selection = 3;
	} else if (m_options.GetOptions() & AS_GNU) {
		selection =0;
	} else if (m_options.GetOptions() & AS_ANSI) {
		selection = 4;
	} else if (m_options.GetOptions() & AS_JAVA) {
		selection = 1;
	} else if (m_options.GetOptions() & AS_KR) {
		selection = 2;
	}

	m_radioBoxPredefinedStyle->SetSelection(selection);

	selection = 4; // None
	if (m_options.GetOptions() & AS_BRACKETS_BREAK_CLOSING) {
		selection = 0;
	} else if (m_options.GetOptions() & AS_BRACKETS_ATTACH) {
		selection = 1;
	} else if (m_options.GetOptions() & AS_BRACKETS_LINUX) {
		selection = 2;
	} else if (m_options.GetOptions() & AS_BRACKETS_BREAK) {
		selection = 3;
	}

	m_textCtrlPreview->SetLexer(wxSTC_LEX_CPP);

	LexerConfPtr cppLexer = EditorConfigST::Get()->GetLexer(wxT("C++"));
	wxFont font;
	if(cppLexer) {
		std::list<StyleProperty> styles = cppLexer->GetLexerProperties();
		std::list<StyleProperty>::iterator iter = styles.begin();
		for (; iter != styles.end(); iter++) {

			StyleProperty sp        = (*iter);
			int           size      = sp.GetFontSize();
			wxString      face      = sp.GetFaceName();
			bool          bold      = sp.IsBold();
			bool          italic    = sp.GetItalic();
			bool          underline = sp.GetUnderlined();
			int           alpha     = sp.GetAlpha();

			// handle special cases
			if ( sp.GetId() == FOLD_MARGIN_ATTR_ID ) {

				// fold margin foreground colour
				m_textCtrlPreview->SetFoldMarginColour(true, sp.GetBgColour());
				m_textCtrlPreview->SetFoldMarginHiColour(true, sp.GetFgColour());

			} else if ( sp.GetId() == SEL_TEXT_ATTR_ID ) {

				// selection colour
				if(wxColour(sp.GetBgColour()).IsOk()) {
					m_textCtrlPreview->SetSelBackground(true, sp.GetBgColour());
					m_textCtrlPreview->SetSelAlpha(alpha);
				}

			} else if ( sp.GetId() == CARET_ATTR_ID ) {

				// caret colour
				if(wxColour(sp.GetFgColour()).IsOk()) {
					m_textCtrlPreview->SetCaretForeground(sp.GetFgColour());
				}

			} else {
				int fontSize( size );

				wxFont font = wxFont(size, wxFONTFAMILY_TELETYPE, italic ? wxITALIC : wxNORMAL , bold ? wxBOLD : wxNORMAL, underline, face);
				if (sp.GetId() == 0) { //default
					m_textCtrlPreview->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
					m_textCtrlPreview->StyleSetSize(wxSTC_STYLE_DEFAULT, size);
					m_textCtrlPreview->StyleSetForeground(wxSTC_STYLE_DEFAULT, (*iter).GetFgColour());
					m_textCtrlPreview->StyleSetBackground(wxSTC_STYLE_DEFAULT, (*iter).GetBgColour());
					m_textCtrlPreview->StyleSetSize(wxSTC_STYLE_LINENUMBER, size);
					m_textCtrlPreview->SetFoldMarginColour(true, (*iter).GetBgColour());
					m_textCtrlPreview->SetFoldMarginHiColour(true, (*iter).GetBgColour());

					// test the background colour of the editor, if it is considered "dark"
					// set the indicator to be hollow rectanlgle
					StyleProperty sp = (*iter);
					if ( DrawingUtils::IsDark(sp.GetBgColour()) ) {
						m_textCtrlPreview->IndicatorSetStyle(1, wxSTC_INDIC_BOX);
						m_textCtrlPreview->IndicatorSetStyle(2, wxSTC_INDIC_BOX);
					}
				} else if(sp.GetId() == wxSTC_STYLE_CALLTIP) {
					// do nothing
				}

				m_textCtrlPreview->StyleSetFont(sp.GetId(), font);
				m_textCtrlPreview->StyleSetSize(sp.GetId(), fontSize);
				m_textCtrlPreview->StyleSetEOLFilled(sp.GetId(), iter->GetEolFilled());

				if(iter->GetId() == LINE_NUMBERS_ATTR_ID) {
					// Set the line number colours only if requested
					// otherwise, use default colours provided by scintilla
					if(sp.GetBgColour().IsEmpty() == false)
						m_textCtrlPreview->StyleSetBackground(sp.GetId(), sp.GetBgColour());

					if(sp.GetFgColour().IsEmpty() == false)
						m_textCtrlPreview->StyleSetForeground(sp.GetId(), sp.GetFgColour());
					else
						m_textCtrlPreview->StyleSetForeground(sp.GetId(), wxT("BLACK"));

				} else {
					m_textCtrlPreview->StyleSetForeground(sp.GetId(), sp.GetFgColour());
					m_textCtrlPreview->StyleSetBackground(sp.GetId(), sp.GetBgColour());
				}
			}
		}
		m_textCtrlPreview->SetKeyWords(0, cppLexer->GetKeyWords(0));
	}
	
	m_radioBoxBrackets->SetSelection(selection);
	m_textCtrlUserFlags->SetValue(m_options.GetCustomFlags());
	
	m_splitterSettingsPreview->Connect( wxEVT_IDLE, wxIdleEventHandler( CodeFormatterDlg::OnSplitterIdle), NULL, this );
}

void CodeFormatterDlg::OnRadioBoxPredefinedStyle( wxCommandEvent& event )
{
	int sel = event.GetSelection();

	//remove all predefined styles
	size_t options = m_options.GetOptions();
	options &= ~(AS_ANSI);//4
	options &= ~(AS_GNU); //0
	options &= ~(AS_LINUX);//3
	options &= ~(AS_JAVA);//1
	options &= ~(AS_KR);//2

	size_t flag = AS_LINUX;
	switch (sel) {
	case 0:
		flag = AS_GNU;
		break;
	case 1:
		flag = AS_JAVA;
		break;
	case 2:
		flag = AS_KR;
		break;
	case 3:
		flag = AS_LINUX;
		break;
	case 4:
		flag = AS_ANSI;
		break;
	default:
		flag = AS_LINUX;
		break;
	}
	m_options.SetOption(options | flag);
	UpdatePreview();
}

void CodeFormatterDlg::OnRadioBoxBrackets( wxCommandEvent& event )
{
	int sel = event.GetSelection();

	//remove all predefined styles
	size_t options = m_options.GetOptions();
	options &= ~(AS_BRACKETS_BREAK_CLOSING);//0
	options &= ~(AS_BRACKETS_ATTACH); //1
	options &= ~(AS_BRACKETS_LINUX);//2
	options &= ~(AS_BRACKETS_BREAK);//3

	size_t flag = AS_LINUX;
	switch (sel) {
	case 0:
		flag = AS_BRACKETS_BREAK_CLOSING;
		break;
	case 1:
		flag = AS_BRACKETS_ATTACH;
		break;
	case 2:
		flag = AS_BRACKETS_LINUX;
		break;
	case 3:
		flag = AS_BRACKETS_BREAK;
		break;
	}
	m_options.SetOption(options | flag);
	UpdatePreview();
}

void CodeFormatterDlg::OnCheckBox( wxCommandEvent& event )
{
	size_t flag(0);
	wxObject *obj = event.GetEventObject();
	if (obj == m_checkBoxFormatBreakBlocks) {
		flag = AS_BREAK_BLOCKS;
	} else if (obj == m_checkBoxFormatBreakBlocksAll) {
		flag = AS_BREAK_BLOCKS_ALL;
	} else if (obj == m_checkBoxFormatBreakElseif) {
		flag = AS_BREAK_ELSEIF;
	} else if (obj == m_checkBoxFormatFillEmptyLines) {
		flag = AS_FILL_EMPTY_LINES;
	} else if (obj == m_checkBoxFormatOneLineKeepBlocks) {
		flag = AS_ONE_LINE_KEEP_BLOCKS;
	} else if (obj == m_checkBoxFormatOneLineKeepStmnt) {
		flag = AS_ONE_LINE_KEEP_STATEMENT;
	} else if (obj == m_checkBoxFormatPadOperators) {
		flag = AS_PAD_OPER;
	} else if (obj == m_checkBoxFormatPadParenth) {
		flag = AS_PAD_PARENTHESIS;
	} else if (obj == m_checkBoxFormatPadParentIn) {
		flag = AS_PAD_PARENTHESIS_IN;
	} else if (obj == m_checkBoxFormatPadParentOut) {
		flag = AS_PAD_PARENTHESIS_OUT;
	} else if (obj == m_checkBoxFormatUnPadParent) {
		flag = AS_UNPAD_PARENTHESIS;
	} else if (obj == m_checkBoxIndentBrackets) {
		flag = AS_INDENT_BRACKETS;
	} else if (obj == m_checkBoxIndentLabels) {
		flag = AS_INDENT_LABELS;
	} else if (obj == m_checkBoxIndentMaxInst) {
		flag = AS_MAX_INSTATEMENT_INDENT;
	} else if (obj == m_checkBoxIndentMinCond) {
		flag = AS_MIN_COND_INDENT;
	} else if (obj == m_checkBoxIndentNamespaces) {
		flag = AS_INDENT_NAMESPACES;
	} else if (obj == m_checkBoxIndentPreprocessors) {
		flag = AS_INDENT_PREPROCESSORS;
	} else if (obj == m_checkBoxIndetBlocks) {
		flag = AS_INDENT_BLOCKS;
	} else if (obj == m_checkBoxIndetCase) {
		flag = AS_INDENT_CASE;
	} else if (obj == m_checkBoxIndetClass) {
		flag = AS_INDENT_CLASS;
	} else if (obj == m_checkBoxIndetSwitch) {
		flag = AS_INDENT_SWITCHES;
	}

	size_t options = m_options.GetOptions();
	EnableFlag(options, flag, event.IsChecked());
	m_options.SetOption(options);
	UpdatePreview();
}

void CodeFormatterDlg::EnableFlag(size_t &options, size_t flag, bool enable)
{
	if (enable) {
		options |= flag;
	} else {
		options &= ~(flag);
	}
}

void CodeFormatterDlg::OnOK(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//Save the options
	m_options.SetCustomFlags(m_textCtrlUserFlags->GetValue());
	EditorConfigST::Get()->SaveLongValue(wxT("CodeFormatterDlgSashPos"), m_splitterSettingsPreview->GetSashPosition());
	EndModal(wxID_OK);
}

void CodeFormatterDlg::OnHelp(wxCommandEvent &e)
{
	wxUnusedVar(e);
	static wxString helpUrl(wxT("http://astyle.sourceforge.net/astyle.html"));
	wxLaunchDefaultBrowser(helpUrl);
}

void CodeFormatterDlg::UpdatePreview()
{
	wxString output;
	m_cf->AstyleFormat(m_sampleCode, m_options.ToString(), output);
	m_textCtrlPreview->SetText(output);

	UpdatePredefinedHelpText();
}

void CodeFormatterDlg::UpdatePredefinedHelpText()
{
	int sel = m_radioBoxPredefinedStyle->GetSelection();
	switch ( sel ) {
	case 0: // AS_GNU
		m_staticTextPredefineHelp->SetLabel(
		    wxString(_("GNU style formatting/indenting.  Brackets are broken,\n")) +
		    wxString(_("blocks are indented, and indentation is 2 spaces. \n")) +
		    wxString(_("Namespaces, classes, and switches are NOT indented.")));
		break;
	case 1: // AS_JAVA
		m_staticTextPredefineHelp->SetLabel(
		    wxString(_("Java style formatting/indenting. Brackets are attached,\n")) +
		    wxString(_("indentation is 4 spaces. Switches are NOT indented."))
		);
		break;
	case 2: // AS_KR
		m_staticTextPredefineHelp->SetLabel(
		    _("Kernighan & Ritchie style formatting/indenting.\nBrackets are attached, indentation is 4 spaces.\nNamespaces, classes, and switches are NOT indented.")
		);
		break;
	case 3: // AS_KR
		m_staticTextPredefineHelp->SetLabel(
		    _("Linux style formatting/indenting.\nAll brackets are linux style, indentation is 8 spaces.\nNamespaces, classes, and switches are NOT indented.")
		);
		break;
	case 4: // AS_ANSI
		m_staticTextPredefineHelp->SetLabel(_("ANSI style formatting/indenting.\nBrackets are broken, indentation is 4 spaces.\nNamespaces, classes, and switches are NOT indented."));
		break;
	}
}

CodeFormatterDlg::~CodeFormatterDlg()
{
	WindowAttrManager::Save(this, wxT("CodeFormatterDlgAttr"), m_cf->GetManager()->GetConfigTool());
}

void CodeFormatterDlg::OnSplitterIdle(wxIdleEvent& e)
{
	long sashPosition(wxNOT_FOUND);
	if(EditorConfigST::Get()->GetLongValue(wxT("CodeFormatterDlgSashPos"), sashPosition)) {
		m_splitterSettingsPreview->SetSashPosition(sashPosition);
	}
	m_splitterSettingsPreview->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CodeFormatterDlg::OnSplitterIdle), NULL, this );
}
