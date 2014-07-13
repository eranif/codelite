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
#include <wx/menu.h>
#include "clSTCLineKeeper.h"

CodeFormatterDlg::CodeFormatterDlg( wxWindow* parent, IManager* mgr, CodeFormatter *cf, const FormatOptions& opts, const wxString &sampleCode )
    : CodeFormatterBaseDlg( parent )
    , m_cf(cf)
    , m_sampleCode(sampleCode)
    , m_isDirty(false)
    , m_mgr(mgr)
{
    m_pgMgr->GetGrid()->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);

    // center the dialog
    Centre();

    m_options = opts;
    m_textCtrlPreview->SetText(m_sampleCode);
    GetSizer()->Fit(this);
    InitDialog();
    UpdatePreview();
    ExpandCollapsUneededOptions();
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
    LexerConf::Ptr_t lexer = EditorConfigST::Get()->GetLexer("C++");
    if ( lexer ) {
        lexer->Apply( m_textCtrlPreview, true );
    }
    m_textCtrlPreview->SetViewWhiteSpace( wxSTC_WS_VISIBLEALWAYS );

    // Select the proper engine
    m_pgPropEngine->SetValueFromInt( (int) m_options.GetEngine() );

    //------------------------------------------------------------------
    // Clang options
    //------------------------------------------------------------------
    m_pgPropClangFormatExePath->SetValue( m_options.GetClangFormatExe() );
    if ( m_options.GetClangFormatOptions() & kClangFormatChromium ) {
        m_pgPropClangFormatStyle->SetValueFromInt( kClangFormatChromium, wxPG_FULL_VALUE );

    } else if ( m_options.GetClangFormatOptions() & kClangFormatMozilla ) {
        m_pgPropClangFormatStyle->SetValueFromInt( kClangFormatMozilla, wxPG_FULL_VALUE );

    } else if ( m_options.GetClangFormatOptions() & kClangFormatWebKit ) {
        m_pgPropClangFormatStyle->SetValueFromInt( kClangFormatWebKit, wxPG_FULL_VALUE );

    } else if ( m_options.GetClangFormatOptions() & kClangFormatGoogle ) {
        m_pgPropClangFormatStyle->SetValueFromInt( kClangFormatGoogle, wxPG_FULL_VALUE );

    } else if ( m_options.GetClangFormatOptions() & kClangFormatLLVM ) {
        m_pgPropClangFormatStyle->SetValueFromInt( kClangFormatLLVM, wxPG_FULL_VALUE );
    }
    m_pgPropClangFormattingOptions->SetValue( (int)m_options.GetClangFormatOptions() );
    m_pgPropClangBraceBreakStyle->SetValue( (int) m_options.GetClangBreakBeforeBrace() );
    m_pgPropColumnLimit->SetValue( (int)m_options.GetClangColumnLimit() );
}

void CodeFormatterDlg::OnOK(wxCommandEvent &e)
{
    OnApply( e );
    EndModal(wxID_OK);
}

#define ID_ASTYLE_HELP        1309
#define ID_CLANG_FORMAST_HELP 1310

void CodeFormatterDlg::OnHelp(wxCommandEvent &e)
{
    wxUnusedVar(e);
    static wxString astyleHelpUrl     (wxT("http://astyle.sourceforge.net/astyle.html"));
    static wxString clangFormatHelpUrl(wxT("http://clang.llvm.org/docs/ClangFormatStyleOptions.html"));

    wxMenu menu;
    menu.Append(ID_ASTYLE_HELP,         _("AStyle help page"));
    menu.Append(ID_CLANG_FORMAST_HELP,  _("clang-format help page"));

    wxRect size = m_buttonHelp->GetSize();
    wxPoint menuPos(0, size.GetHeight());

    int res = m_buttonHelp->GetPopupMenuSelectionFromUser( menu, menuPos );
    if ( res == ID_ASTYLE_HELP ) {
        ::wxLaunchDefaultBrowser(astyleHelpUrl);

    } else if ( res == ID_CLANG_FORMAST_HELP ) {
        ::wxLaunchDefaultBrowser(clangFormatHelpUrl);
    }
}

void CodeFormatterDlg::UpdatePreview()
{
    FormatterEngine engine = (FormatterEngine) m_pgPropEngine->GetValue().GetInteger();
    wxString output;
    if ( engine == kFormatEngineAStyle ) {
        m_cf->AstyleFormat(m_sampleCode, m_options.AstyleOptionsAsString(), output);

    } else {
        m_cf->ClangPreviewFormat(m_sampleCode, output, m_options);

    }
    m_textCtrlPreview->SetEditable(true);

    {
        clSTCLineKeeper lk( m_textCtrlPreview );
        m_textCtrlPreview->SetText(output);
    }
    m_textCtrlPreview->SetEditable(false);
}

CodeFormatterDlg::~CodeFormatterDlg()
{
    WindowAttrManager::Save(this, wxT("CodeFormatterDlgAttr"), m_cf->GetManager()->GetConfigTool());
}

void CodeFormatterDlg::OnAStylePropertyChanged(wxPropertyGridEvent& event)
{
    m_isDirty = true;

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

    // Save the selected engine
    m_options.SetEngine( (FormatterEngine) m_pgPropEngine->GetValue().GetInteger() );

    // Save clang options
    size_t clangOptions(0);
    clangOptions |= m_pgPropClangFormatStyle->GetValue().GetInteger();
    clangOptions |= m_pgPropClangFormattingOptions->GetValue().GetInteger();
    m_options.SetClangFormatOptions( clangOptions );
    m_options.SetClangBreakBeforeBrace( m_pgPropClangBraceBreakStyle->GetValue().GetInteger() );
    m_options.SetClangFormatExe( m_pgPropClangFormatExePath->GetValueAsString() );
    m_options.SetClangColumnLimit( m_pgPropColumnLimit->GetValue().GetInteger() );
    
    // Check the active engine
    ExpandCollapsUneededOptions();
    CallAfter( &CodeFormatterDlg::UpdatePreview );
}

void CodeFormatterDlg::OnApplyUI(wxUpdateUIEvent& event)
{
    event.Enable( m_isDirty );
}

void CodeFormatterDlg::OnCustomAstyleFlags(wxCommandEvent& event)
{
    event.Skip();
    m_isDirty = true;
}

void CodeFormatterDlg::OnApply(wxCommandEvent& event)
{
    m_isDirty = false;
    m_options.SetCustomFlags(m_textCtrlUserFlags->GetValue());
    m_mgr->GetConfigTool()->WriteObject(wxT("FormatterOptions"), &m_options);
    UpdatePreview();
}

void CodeFormatterDlg::ExpandCollapsUneededOptions()
{
    wxString engine = m_pgPropEngine->GetValueAsString();
    if ( engine == "AStyle" ) {
        m_pgMgr->Collapse( m_pgPropClangFormat );
        m_pgMgr->Expand( m_pgPropAstyleOptions );

    } else {
        m_pgMgr->Expand( m_pgPropClangFormat );
        m_pgMgr->Collapse( m_pgPropAstyleOptions );
    }
}
