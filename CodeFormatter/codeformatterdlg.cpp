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
#include "editor_config.h"
#include "lexer_configuration.h"

CodeFormatterDlg::CodeFormatterDlg( wxWindow* parent, CodeFormatter *cf, const FormatOptions& opts, const wxString &sampleCode )
    : CodeFormatterBaseDlg( parent )
    , m_cf(cf)
    , m_sampleCode(sampleCode)
{
    m_pgMgr->GetGrid()->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);
    
    // center the dialog
    Centre();

    m_options = opts;
    m_buttonOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeFormatterDlg::OnOK), NULL, this);
    m_buttonHelp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeFormatterDlg::OnHelp), NULL, this);

    m_textCtrlPreview->SetText(m_sampleCode);
    GetSizer()->Fit(this);
    InitDialog();
    UpdatePreview();

    WindowAttrManager::Load(this, wxT("CodeFormatterDlgAttr"), m_cf->GetManager()->GetConfigTool());
}

void CodeFormatterDlg::InitDialog()
{
    long formatOptions = m_options.GetOptions() & AS_ALL_FORMAT_OPTIONS;
    long indentOptions = m_options.GetOptions() & AS_ALL_INDENT_OPTIONS;
    m_pgPropIndentation->SetValue( indentOptions );
    m_pgPropFormatting->SetValue( formatOptions );
    
    // Bracket options
    if ( m_options.GetOptions() & AS_BRACKETS_BREAK_CLOSING )
        m_pgPropBrackets->SetValue("Break closing");
    else if ( m_options.GetOptions() & AS_BRACKETS_BREAK )
        m_pgPropBrackets->SetValue("Break");
    else if ( m_options.GetOptions() & AS_BRACKETS_ATTACH )
        m_pgPropBrackets->SetValue("Attach");
    else if ( m_options.GetOptions() & AS_BRACKETS_LINUX )
        m_pgPropBrackets->SetValue("Linux");
    
    // Styles
    if ( m_options.GetOptions() & AS_GNU )
        m_pgPropPreDefinedStyles->SetValue("GNU");
    else if ( m_options.GetOptions() & AS_JAVA )
        m_pgPropPreDefinedStyles->SetValue("Java");
    else if ( m_options.GetOptions() & AS_KR )
        m_pgPropPreDefinedStyles->SetValue("K&R");
    else if ( m_options.GetOptions() & AS_LINUX )
        m_pgPropPreDefinedStyles->SetValue("Linux");
    else if ( m_options.GetOptions() & AS_ANSI )
        m_pgPropPreDefinedStyles->SetValue("ANSI");
        
    m_textCtrlPreview->SetLexer(wxSTC_LEX_CPP);
    LexerConfPtr lexer = EditorConfigST::Get()->GetLexer("C++");
    if ( lexer ) {
        lexer->Apply( m_textCtrlPreview, true );
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
    m_textCtrlPreview->SetEditable(true);
    m_textCtrlPreview->SetText(output);
    m_textCtrlPreview->SetEditable(false);
}

CodeFormatterDlg::~CodeFormatterDlg()
{
    WindowAttrManager::Save(this, wxT("CodeFormatterDlgAttr"), m_cf->GetManager()->GetConfigTool());
}

void CodeFormatterDlg::OnAStylePropertyChanged(wxPropertyGridEvent& event)
{
    size_t options (0);
    // Build the options
    
    // Brakcets: Break closing;Attach;Linux;Break;None
    wxString brackets = m_pgPropBrackets->GetValueAsString();
    if ( brackets == "Break closing" ) {
        options |= AS_BRACKETS_BREAK_CLOSING;
    } else if ( brackets == "Attach" ) {
        options |= AS_BRACKETS_ATTACH;
    } else if ( brackets == "Linux" ) {
        options |= AS_BRACKETS_LINUX;
    } else if ( brackets == "Break" ) {
        options |= AS_BRACKETS_BREAK;
    }
    
    // PreDefined styles
    // GNU;Java;K&R;Linux;ANSI
    wxString predefStyle = m_pgPropPreDefinedStyles->GetValueAsString();
    if ( predefStyle == "GNU" ) {
        options |= AS_GNU;
    } else if ( predefStyle == "Java" ) {
        options |= AS_JAVA;
    } else if ( predefStyle == "K&R" ) {
        options |= AS_KR;
    } else if ( predefStyle == "Linux" ) {
        options |= AS_LINUX;
    } else if ( predefStyle == "ANSI" ) {
        options |= AS_ANSI;
    }
    
    options |= m_pgPropFormatting->GetValue().GetInteger();
    options |= m_pgPropIndentation->GetValue().GetInteger();
    m_options.SetOption( options );
    
    CallAfter( &CodeFormatterDlg::UpdatePreview );
    
}
