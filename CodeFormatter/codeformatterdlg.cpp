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
#include "ColoursAndFontsManager.h"
#include "clSTCLineKeeper.h"
#include "codeformatterdlg.h"
#include "editor_config.h"
#include "fileextmanager.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "windowattrmanager.h"
#include <wx/menu.h>

CodeFormatterDlg::CodeFormatterDlg(wxWindow* parent, IManager* mgr, CodeFormatter* cf, FormatOptions& options,
                                   const wxString& cppSampleCode, const wxString& phpSampleCode)
    : CodeFormatterBaseDlg(parent)
    , m_options(options)
    , m_cf(cf)
    , m_cppSampleCode(cppSampleCode)
    , m_phpSampleCode(phpSampleCode)
    , m_isDirty(false)
    , m_mgr(mgr)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrAstyle->GetGrid());
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrClang->GetGrid());
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrPhp->GetGrid());

    // center the dialog
    Centre();

    m_textCtrlPreview->SetText(m_cppSampleCode);
    m_textCtrlPreview_Clang->SetText(m_cppSampleCode);
    m_stcPhpPreview->SetText(m_phpSampleCode);
    m_textCtrlPreview_PhpCSFixer->SetText(m_phpSampleCode);
    m_textCtrlPreview_Phpcbf->SetText(m_phpSampleCode);

    GetSizer()->Fit(this);
    InitDialog();
    UpdatePreview();

    // Clear the modified status
    m_pgMgrPhp->GetGrid()->ClearModifiedStatus();
    m_pgMgrAstyle->GetGrid()->ClearModifiedStatus();
    m_pgMgrClang->GetGrid()->ClearModifiedStatus();

    // set the selection based on the editor
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor && FileExtManager::IsPHPFile(editor->GetFileName())) {
        m_notebook->SetSelection(2); // PHP page
    } else if(editor && FileExtManager::IsCxxFile(editor->GetFileName())) {
        m_notebook->SetSelection(1); // CXX page
    } else {
        m_notebook->SetSelection(0); // General
    }
	m_pgPropClangFormatExePath->SetAttribute("ShowFullPath", 1);
    SetName("CodeFormatterDlg");
    WindowAttrManager::Load(this);
}

void CodeFormatterDlg::InitDialog()
{
    long formatOptions = (m_options.GetOptions() & AS_ALL_FORMAT_OPTIONS);
    long indentOptions = (m_options.GetOptions() & AS_ALL_INDENT_OPTIONS);
    m_pgPropIndentation->SetValue(indentOptions);
    m_pgPropFormatting->SetValue(formatOptions);

    // Bracket options
    if(m_options.GetOptions() & AS_BRACKETS_BREAK_CLOSING)
        m_pgPropBrackets->SetValue("Break closing");
    else if(m_options.GetOptions() & AS_BRACKETS_BREAK)
        m_pgPropBrackets->SetValue("Break");
    else if(m_options.GetOptions() & AS_BRACKETS_ATTACH)
        m_pgPropBrackets->SetValue("Attach");
    else if(m_options.GetOptions() & AS_BRACKETS_LINUX)
        m_pgPropBrackets->SetValue("Linux");

    // Styles
    if(m_options.GetOptions() & AS_GNU)
        m_pgPropPreDefinedStyles->SetValue("GNU");
    else if(m_options.GetOptions() & AS_JAVA)
        m_pgPropPreDefinedStyles->SetValue("Java");
    else if(m_options.GetOptions() & AS_KR)
        m_pgPropPreDefinedStyles->SetValue("K&R");
    else if(m_options.GetOptions() & AS_LINUX)
        m_pgPropPreDefinedStyles->SetValue("Linux");
    else if(m_options.GetOptions() & AS_ANSI)
        m_pgPropPreDefinedStyles->SetValue("ANSI");

    m_textCtrlPreview->SetLexer(wxSTC_LEX_CPP);
    LexerConf::Ptr_t lexer = EditorConfigST::Get()->GetLexer("C++");
    if(lexer) {
        lexer->Apply(m_textCtrlPreview, true);
        lexer->Apply(m_textCtrlPreview_Clang, true);
    }

    LexerConf::Ptr_t phpLexer = EditorConfigST::Get()->GetLexer("php");
    if(phpLexer) {
        phpLexer->Apply(m_stcPhpPreview, true);
        phpLexer->Apply(m_textCtrlPreview_PhpCSFixer, true);
        phpLexer->Apply(m_textCtrlPreview_Phpcbf, true);
    }
    m_textCtrlPreview->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
    m_textCtrlPreview_Clang->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
    m_stcPhpPreview->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
    m_textCtrlPreview_PhpCSFixer->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
    m_textCtrlPreview_Phpcbf->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);

    // Select the proper engine
    m_choiceCxxEngine->SetSelection((int)m_options.GetEngine());
    m_choicePhpFormatter->SetSelection((int)m_options.GetPhpEngine());

    //------------------------------------------------------------------
    // Clang options
    //------------------------------------------------------------------
    m_pgPropClangFormatExePath->SetValue(m_options.GetClangFormatExe());
    if(m_options.GetClangFormatOptions() & kClangFormatChromium) {
        m_pgPropClangFormatStyle->SetValueFromInt(kClangFormatChromium, wxPG_FULL_VALUE);

    } else if(m_options.GetClangFormatOptions() & kClangFormatMozilla) {
        m_pgPropClangFormatStyle->SetValueFromInt(kClangFormatMozilla, wxPG_FULL_VALUE);

    } else if(m_options.GetClangFormatOptions() & kClangFormatWebKit) {
        m_pgPropClangFormatStyle->SetValueFromInt(kClangFormatWebKit, wxPG_FULL_VALUE);

    } else if(m_options.GetClangFormatOptions() & kClangFormatGoogle) {
        m_pgPropClangFormatStyle->SetValueFromInt(kClangFormatGoogle, wxPG_FULL_VALUE);

    } else if(m_options.GetClangFormatOptions() & kClangFormatLLVM) {
        m_pgPropClangFormatStyle->SetValueFromInt(kClangFormatLLVM, wxPG_FULL_VALUE);
    } else {
        // There should be at least one good formatting option, we choose WebKit for this purpose
        m_options.SetClangFormatOptions(m_options.GetClangFormatOptions() | kClangFormatWebKit);
        m_pgPropClangFormatStyle->SetValueFromInt(kClangFormatWebKit, wxPG_FULL_VALUE);
    }

    m_pgPropClangUseFile->SetValue(wxVariant((bool)(m_options.GetClangFormatOptions() & kClangFormatFile)));
    m_pgPropClangFormattingOptions->SetValue((int)m_options.GetClangFormatOptions());
    m_pgPropClangBraceBreakStyle->SetValue((int)m_options.GetClangBreakBeforeBrace());
    m_pgPropColumnLimit->SetValue((int)m_options.GetClangColumnLimit());

    // PHP flags
    m_pgPropPhpFormatterOptions->SetValue((int)m_options.GetPHPFormatterOptions());

    // PHP-CS-FIXER
    m_filePickerPHPCsFixerPhar->SetValue(m_options.GetPHPCSFixerPhar());
    wxString options = m_options.GetPHPCSFixerPharOptions();
    m_pgPropPHPCsFixerOptions->SetValue(options.Trim().Trim(false));

    m_pgPropPHPCsFixerUseFile->SetValue(
        wxVariant((bool)(m_options.GetPHPCSFixerPharSettings() & kPHPFixserFormatFile)));
    m_pgPropPHPCsFixerStandard->SetValue((int)m_options.GetPHPCSFixerPharRules() & (kPcfPSR1 | kPcfPSR2 | kPcfSymfony));
    m_pgPropPHPCsFixerMigration->SetValue((int)m_options.GetPHPCSFixerPharRules() &
                                          (kPcfPHP56Migration | kPcfPHP70Migration | kPcfPHP71Migration));
    m_pgPropPHPCsFixerDoubleArrows->SetValue((int)m_options.GetPHPCSFixerPharRules() &
                                             (kPcfAlignDoubleArrow | kPcfStripDoubleArrow | kPcfIgnoreDoubleArrow));
    m_pgPropPHPCsFixerEquals->SetValue((int)m_options.GetPHPCSFixerPharRules() &
                                       (kPcfAlignEquals | kPcfStripEquals | kPcfIgnoreEquals));
    m_pgPropPHPCsFixerArrays->SetValue((int)m_options.GetPHPCSFixerPharRules() & (kPcfShortArray | kPcfLongArray));
    m_pgPropPHPCsFixerConcatSpace->SetValue((int)m_options.GetPHPCSFixerPharRules() &
                                            (kPcfConcatSpaceNone | kPcfConcatSpaceOne));
    m_pgPropPHPCsFixerEmptyReturn->SetValue((int)m_options.GetPHPCSFixerPharRules() &
                                            (kPcfEmptyReturnStrip | kPcfEmptyReturnKeep));
    m_pgPropPHPCsFixerRules->SetValue((int)m_options.GetPHPCSFixerPharRules());

    // PHPCBF
    m_filePickerPhpcbfPhar->SetValue(m_options.GetPhpcbfPhar());
    m_pgPropPhpcbfSeverity->SetValue((int)m_options.GetPhpcbfSeverity());
    m_pgPropPhpcbfEncoding->SetValue(m_options.GetPhpcbfEncoding());
    m_pgPropPhpcbfUseFile->SetValue(wxVariant((bool)(m_options.GetPhpcbfOptions() & kPhpbcfFormatFile)));
    m_pgPropPhpcbfStandard->SetValue(m_options.GetPhpcbfStandard());
    m_pgPropPhpcbfOptions->SetValue((int)m_options.GetPhpcbfOptions());

    // General Options
    m_checkBoxFormatOnSave->SetValue(m_options.HasFlag(kCF_AutoFormatOnFileSave));

    // User custom flags
    m_textCtrlUserFlags->ChangeValue(m_options.GetCustomFlags());
}

void CodeFormatterDlg::OnOK(wxCommandEvent& e)
{
    OnApply(e);
    EndModal(wxID_OK);
}

#define ID_ASTYLE_HELP 1309
#define ID_CLANG_FORMAST_HELP 1310
#define ID_PHP_FORMAST_HELP 1311

void CodeFormatterDlg::OnHelp(wxCommandEvent& e)
{
    wxUnusedVar(e);
    static wxString astyleHelpUrl(wxT("http://astyle.sourceforge.net/astyle.html"));
    static wxString clangFormatHelpUrl(wxT("http://clang.llvm.org/docs/ClangFormatStyleOptions.html"));
    static wxString phpFormatHelpUrl(wxT("https://github.com/FriendsOfPHP/PHP-CS-Fixer"));

    wxMenu menu;
    menu.Append(ID_ASTYLE_HELP, _("AStyle help page"));
    menu.Append(ID_CLANG_FORMAST_HELP, _("clang-format help page"));
    menu.Append(ID_PHP_FORMAST_HELP, _("PHP-CS-Fixer help page"));

    wxRect size = m_buttonHelp->GetSize();
    wxPoint menuPos(0, size.GetHeight());

    int res = m_buttonHelp->GetPopupMenuSelectionFromUser(menu, menuPos);
    if(res == ID_ASTYLE_HELP) {
        ::wxLaunchDefaultBrowser(astyleHelpUrl);

    } else if(res == ID_CLANG_FORMAST_HELP) {
        ::wxLaunchDefaultBrowser(clangFormatHelpUrl);

    } else if(res == ID_PHP_FORMAST_HELP) {
        ::wxLaunchDefaultBrowser(phpFormatHelpUrl);
    }
}

void CodeFormatterDlg::UpdatePreview()
{
    wxString output, command;

    if(m_notebook->GetSelection() == 1) { // CXX page
        output = m_cppSampleCode;

        if(m_notebookCxx->GetSelection() == 0) { // Clang
            m_cf->DoFormatPreview(output, "cpp", kFormatEngineClangFormat);
            UpdatePreviewText(m_textCtrlPreview_Clang, output);
        } else if(m_notebookCxx->GetSelection() == 1) { // Astyle
            m_cf->DoFormatPreview(output, "cpp", kFormatEngineAStyle);
            UpdatePreviewText(m_textCtrlPreview, output);
        }
    } else if(m_notebook->GetSelection() == 2) { // PHP page
        output = m_phpSampleCode;

        if(m_notebookPhp->GetSelection() == 0) { // Build In PHP
            m_cf->DoFormatPreview(output, "php", kFormatEngineBuildInPhp);
            UpdatePreviewText(m_stcPhpPreview, output);
        } else if(m_notebookPhp->GetSelection() == 1) { // PhpCsFixer
            m_cf->DoFormatPreview(output, "php", kFormatEnginePhpCsFixer);
            UpdatePreviewText(m_textCtrlPreview_PhpCSFixer, output);
        } else if(m_notebookPhp->GetSelection() == 2) { // Phpcbf
            m_cf->DoFormatPreview(output, "php", kFormatEnginePhpcbf);
            UpdatePreviewText(m_textCtrlPreview_Phpcbf, output);
        }
    }
}

void CodeFormatterDlg::UpdatePreviewText(wxStyledTextCtrl*& textCtrl, const wxString& text)
{
    textCtrl->SetEditable(true);
    clSTCLineKeeper lk(textCtrl);
    textCtrl->SetText(text);
    textCtrl->SetEditable(false);
}

CodeFormatterDlg::~CodeFormatterDlg() {}

void CodeFormatterDlg::OnApplyUI(wxUpdateUIEvent& event) { event.Enable(m_isDirty); }

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

void CodeFormatterDlg::OnPgmgrastylePgChanged(wxPropertyGridEvent& event)
{
    m_isDirty = true;
    size_t options(0);
    // Build the options

    // Brakcets: Break closing;Attach;Linux;Break;None
    wxString brackets = m_pgPropBrackets->GetValueAsString();
    if(brackets == "Break closing") {
        options |= AS_BRACKETS_BREAK_CLOSING;
    } else if(brackets == "Attach") {
        options |= AS_BRACKETS_ATTACH;
    } else if(brackets == "Linux") {
        options |= AS_BRACKETS_LINUX;
    } else if(brackets == "Break") {
        options |= AS_BRACKETS_BREAK;
    }

    // PreDefined styles
    // GNU;Java;K&R;Linux;ANSI
    wxString predefStyle = m_pgPropPreDefinedStyles->GetValueAsString();
    if(predefStyle == "GNU") {
        options |= AS_GNU;
    } else if(predefStyle == "Java") {
        options |= AS_JAVA;
    } else if(predefStyle == "K&R") {
        options |= AS_KR;
    } else if(predefStyle == "Linux") {
        options |= AS_LINUX;
    } else if(predefStyle == "ANSI") {
        options |= AS_ANSI;
    }

    options |= m_pgPropFormatting->GetValue().GetInteger();
    options |= m_pgPropIndentation->GetValue().GetInteger();
    m_options.SetOption(options);

    CallAfter(&CodeFormatterDlg::UpdatePreview);
}

void CodeFormatterDlg::OnPgmgrclangPgChanged(wxPropertyGridEvent& event)
{
    m_isDirty = true;
    // Save clang options
    size_t clangOptions(0);
    clangOptions |= m_pgPropClangFormatStyle->GetValue().GetInteger();
    clangOptions |= m_pgPropClangFormattingOptions->GetValue().GetInteger();
    if(m_pgPropClangUseFile->GetValue().GetBool()) {
        clangOptions |= kClangFormatFile;
    }

    m_options.SetClangFormatOptions(clangOptions);
    m_options.SetClangBreakBeforeBrace(m_pgPropClangBraceBreakStyle->GetValue().GetInteger());
    m_options.SetClangFormatExe(m_pgPropClangFormatExePath->GetValueAsString());
    m_options.SetClangColumnLimit(m_pgPropColumnLimit->GetValue().GetInteger());

    CallAfter(&CodeFormatterDlg::UpdatePreview);
}

void CodeFormatterDlg::OnPgmgrphpPgChanged(wxPropertyGridEvent& event)
{
    m_isDirty = true;
    // save php options
    m_options.SetPHPFormatterOptions(m_pgPropPhpFormatterOptions->GetValue().GetInteger());
    CallAfter(&CodeFormatterDlg::UpdatePreview);
}

void CodeFormatterDlg::OnChoicecxxengineChoiceSelected(wxCommandEvent& event)
{
    m_isDirty = true;
    m_options.SetEngine((CXXFormatterEngine)event.GetSelection());
}

void CodeFormatterDlg::OnFormatOnSave(wxCommandEvent& event)
{
    m_isDirty = true;
    m_options.SetFlag(kCF_AutoFormatOnFileSave, event.IsChecked());
}

void CodeFormatterDlg::OnPgmgrPHPCsFixerPgChanged(wxPropertyGridEvent& event)
{
    m_isDirty = true;
    m_options.SetPHPCSFixerPhar(m_filePickerPHPCsFixerPhar->GetValueAsString());
    m_options.SetPHPCSFixerPharOptions(m_pgPropPHPCsFixerOptions->GetValueAsString().Trim().Trim(false));
    size_t phpcsfixerSettings(0);
    if(m_pgPropPHPCsFixerUseFile->GetValue().GetBool()) {
        phpcsfixerSettings |= kPHPFixserFormatFile;
    }
    m_options.SetPHPCSFixerPharSettings(phpcsfixerSettings);
    size_t phpcsfixerOptions(0);
    phpcsfixerOptions |= m_pgPropPHPCsFixerStandard->GetValue().GetInteger();
    phpcsfixerOptions |= m_pgPropPHPCsFixerMigration->GetValue().GetInteger();
    phpcsfixerOptions |= m_pgPropPHPCsFixerDoubleArrows->GetValue().GetInteger();
    phpcsfixerOptions |= m_pgPropPHPCsFixerEquals->GetValue().GetInteger();
    phpcsfixerOptions |= m_pgPropPHPCsFixerArrays->GetValue().GetInteger();
    phpcsfixerOptions |= m_pgPropPHPCsFixerConcatSpace->GetValue().GetInteger();
    phpcsfixerOptions |= m_pgPropPHPCsFixerEmptyReturn->GetValue().GetInteger();
    phpcsfixerOptions |= m_pgPropPHPCsFixerRules->GetValue().GetInteger();
    m_options.SetPHPCSFixerPharRules(phpcsfixerOptions);

    CallAfter(&CodeFormatterDlg::UpdatePreview);
}
void CodeFormatterDlg::OnChoicephpformatterChoiceSelected(wxCommandEvent& event)
{
    m_isDirty = true;
    m_options.SetPhpEngine((PHPFormatterEngine)event.GetSelection());
}

void CodeFormatterDlg::OnPgmgrPhpcbfPgChanged(wxPropertyGridEvent& event)
{
    m_isDirty = true;
    m_options.SetPhpcbfPhar(m_filePickerPhpcbfPhar->GetValueAsString());
    m_options.SetPhpcbfSeverity(m_pgPropPhpcbfSeverity->GetValue().GetInteger());
    m_options.SetPhpcbfEncoding(m_pgPropPhpcbfEncoding->GetValueAsString());
    m_options.SetPhpcbfStandard(m_pgPropPhpcbfStandard->GetValueAsString());
    size_t phpcbfOptions(0);
    phpcbfOptions |= m_pgPropPhpcbfOptions->GetValue().GetInteger();
    if(m_pgPropPhpcbfUseFile->GetValue().GetBool()) {
        phpcbfOptions |= kPhpbcfFormatFile;
    }
    m_options.SetPhpcbfOptions(phpcbfOptions);

    CallAfter(&CodeFormatterDlg::UpdatePreview);
}

void CodeFormatterDlg::UpdatePreviewUI(wxNotebookEvent& event)
{
    UpdatePreview();
    event.Skip();
}
