//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : syntaxhighlightdlg.cpp
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

#include "syntaxhighlightdlg.h"

#include "ColoursAndFontsManager.h"
#include "NewThemeDialog.h"
#include "ThemeImporterCXX.hpp"
#include "ThemeImporterManager.hpp"
#include "clSystemSettings.h"
#include "clZipReader.h"
#include "clZipWriter.h"
#include "cl_config.h"
#include "cl_defs.h"
#include "context_manager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "frame.h"
#include "free_text_dialog.h"
#include "macros.h"
#include "manager.h"
#include "windowattrmanager.h"

#include <algorithm>
#include <wx/busyinfo.h>
#include <wx/choicdlg.h>
#include <wx/dir.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/richtooltip.h>
#include <wx/treebook.h>
#include <wx/utils.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

#define CXX_AND_JAVASCRIPT "c++"

bool SyntaxHighlightDlg::m_globalBgColourChangedTooltipShown = false;

const wxString sampleText = "class Demo {\n"
                            "private:\n"
                            "    std::string m_str;\n"
                            "    int m_integer;\n"
                            "    \n"
                            "public:\n"
                            "    /**\n"
                            "     * Creates a new demo.\n"
                            "     * @param o The object\n"
                            "     */\n"
                            "    CallMethod(const Demo& other) {\n"
                            "        std::string a_string = \"hello world\";\n"
                            "        m_str = other.m_str;\n"
                            "        m_integer = other.m_integer;\n"
                            "    }\n"
                            "}";

#define DARK_ICONS _("Dark Theme Icons Set")
#define LIGHT_ICONS _("Light Theme Icons Set")

SyntaxHighlightDlg::SyntaxHighlightDlg(wxWindow* parent)
    : SyntaxHighlightBaseDlg(parent)
    , m_isModified(false)
    , m_globalThemeChanged(false)
    , m_globalBgColourChanged(false)
{
    // Get list of available lexers
    wxString lexerName;
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor(true);
    wxArrayString lexers = ColoursAndFontsManager::Get().GetAllLexersNames();
    if(editor) {
        lexerName = editor->GetContext()->GetName().Lower();
    }

    for(size_t i = 0; i < lexers.GetCount(); ++i) {
        if(lexers.Item(i) == "c++") {
            m_listBox->Append(CXX_AND_JAVASCRIPT);
        } else {
            m_listBox->Append(lexers.Item(i));
        }
    }

    if(!m_listBox->IsEmpty()) {
        if(lexerName.IsEmpty()) {
            m_listBox->Select(0);
        } else {
            m_listBox->SetStringSelection(lexerName == "c++" ? CXX_AND_JAVASCRIPT : lexerName);
        }
        LoadLexer(""); // Load the default active theme
    }

    // Load the global colours
    m_choiceGlobalTheme->Append(ColoursAndFontsManager::Get().GetAvailableThemesForLexer("c++"));
    m_choiceGlobalTheme->SetStringSelection(ColoursAndFontsManager::Get().GetGlobalTheme());

    // Set the current editor font to the default one
    // but first check that we have entry in the configuration file
    // for this
    bool found_font = false;
    wxFont font;
    clConfig::Get().Read("GlobalThemeFont", [&found_font](const JSONItem& item) { found_font = true; });

    if(found_font) {
        wxMessageBox("Font found!");
        wxFont font = clConfig::Get().Read("GlobalThemeFont", wxNullFont);
        m_fontPickerGlobal->SetSelectedFont(font);
    }

    DoUpdatePreview();

    // incase we don't have initial font display, use the preview window's font
    if(!found_font) {
        wxFont initialFont = m_stcPreview->StyleGetFont(0);
        if(initialFont.IsOk()) {
            m_fontPickerGlobal->SetSelectedFont(initialFont);
        }
    }
    m_isModified = true;

    // for now, we only allow selection with fg colour
    m_checkBoxCustomSelectionFgColour->SetValue(true);
    m_checkBoxCustomSelectionFgColour->Enable(false);

    SetName("SyntaxHighlightDlg");
    WindowAttrManager::Load(this);
    auto images = clGetManager()->GetStdIcons();

    m_toolbar->AddTool(wxID_NEW, _("New.."), images->LoadBitmap("file_new", 24));
    m_toolbar->AddTool(wxID_SAVE, _("Export.."), images->LoadBitmap("file_save", 24));
    m_toolbar->AddTool(XRCID("export_all"), _("Export All"), images->LoadBitmap("file_save_all", 24));
    m_toolbar->AddTool(XRCID("import_zip"), _("Import from ZIP file"), images->LoadBitmap("file_open", 24));
    m_toolbar->AddTool(XRCID("revert_changes"), _("Restore"), images->LoadBitmap("file_reload", 24));
    m_toolbar->AddTool(XRCID("import_eclipse_theme"), _("Import"), images->LoadBitmap("file_open", 24));
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &SyntaxHighlightDlg::OnNewTheme, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &SyntaxHighlightDlg::OnExportSelective, this, wxID_SAVE);
    m_toolbar->Bind(wxEVT_TOOL, &SyntaxHighlightDlg::OnExportAll, this, XRCID("export_all"));
    m_toolbar->Bind(wxEVT_TOOL, &SyntaxHighlightDlg::OnImport, this, XRCID("import_zip"));
    m_toolbar->Bind(wxEVT_TOOL, &SyntaxHighlightDlg::OnRestoreDefaults, this, XRCID("revert_changes"));
    m_toolbar->Bind(wxEVT_TOOL, &SyntaxHighlightDlg::OnImportEclipseTheme, this, XRCID("import_eclipse_theme"));

    // Theme handling
    wxColour baseColour = clConfig::Get().Read("BaseColour", clSystemSettings::GetDefaultPanelColour());
    m_colourPickerBaseColour->SetColour(baseColour);
    m_useBaseColourInitial = clConfig::Get().Read("UseCustomBaseColour", false);
    m_useBaseColourEnding = m_useBaseColourInitial;
    m_cbUseCustomBaseColour->SetValue(m_useBaseColourInitial);
    if(m_cbUseCustomBaseColour) {
        m_initialTheme = DrawingUtils::IsDark(baseColour) ? kTHEME_DARK : kTHEME_LIGHT;
        m_endingTheme = m_initialTheme;
    }
    ::clSetTLWindowBestSizeAndPosition(this);
}

void SyntaxHighlightDlg::DoUpdatePreview()
{
    // Populate the preview
    LexerConf::Ptr_t previewLexer =
        ColoursAndFontsManager::Get().GetLexer("c++", m_choiceGlobalTheme->GetStringSelection());

    int class_index = 1;
    int locals_index = 1;
    int methods_index = 1;
    int other_index = 1;

    if(previewLexer) {
        previewLexer->Apply(m_stcPreview, true);
        class_index = previewLexer->GetWordSetClassIndex();
        locals_index = previewLexer->GetWordSetLocalsIndex();
        methods_index = previewLexer->GetWordSetFunctionsIndex();
        other_index = previewLexer->GetWordSetOthersIndex();
    }

    m_stcPreview->SetKeyWords(class_index, "Demo std string");
    m_stcPreview->SetKeyWords(locals_index, "other");
    m_stcPreview->SetKeyWords(methods_index, "CallMethod");
    m_stcPreview->SetKeyWords(other_index, wxEmptyString);

    m_stcPreview->SetEditable(true);
    m_stcPreview->SetText(sampleText);
    m_stcPreview->HideSelection(true);
    m_stcPreview->SetEditable(false);

    ::clRecalculateSTCHScrollBar(m_stcPreview);
}

void SyntaxHighlightDlg::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SaveChanges();

    // and close the dialog
    EndModal(wxID_OK);
}

void SyntaxHighlightDlg::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);
}

void SyntaxHighlightDlg::OnButtonApply(wxCommandEvent& event)
{
    SaveChanges();
    clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();
    wxUnusedVar(event);
}

void SyntaxHighlightDlg::Clear()
{
    // Global Settings page
    m_choiceLexerThemes->Clear();
    m_globalFontPicker->SetSelectedFont(clSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT));
    m_globalBgColourPicker->SetColour(clSystemSettings::GetDefaultPanelColour());
    m_fileSpec->ChangeValue("");

    // Customize page
    m_properties->Clear();
    m_fontPicker->SetSelectedFont(clSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT));
    m_colourPicker->SetColour(clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    m_bgColourPicker->SetColour(clSystemSettings::GetDefaultPanelColour());
    m_eolFilled->SetValue(false);
    m_styleWithinPreProcessor->SetValue(false);

    // Text Selection page
    m_colourPickerSelTextBgColour->SetColour(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    m_isModified = false;
}

void SyntaxHighlightDlg::LoadLexer(const wxString& themeName)
{
    wxWindowUpdateLocker locker(this);
    Clear();
    wxString lexer = m_listBox->GetStringSelection();
    if(lexer.IsEmpty())
        return;

    if(lexer == CXX_AND_JAVASCRIPT) {
        lexer = "c++";
    }

    m_lexer = ColoursAndFontsManager::Get().GetLexer(lexer, themeName);
    CreateLexerPage();
    m_isModified = false;
}

void SyntaxHighlightDlg::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    wxString themeName = m_choiceLexerThemes->GetStringSelection();
    LoadLexer(themeName);
    m_isModified = true;
}

void SyntaxHighlightDlg::SaveChanges()
{
    // Save all lexers once
    // Update the lexer
    CHECK_PTR_RET(m_lexer);

    // Make sure we got a valid text selection
    // foreground colour
    StyleProperty& selProp = m_lexer->GetProperty(SEL_TEXT_ATTR_ID);
    if(!selProp.IsNull()) {
        selProp.SetFgColour(m_colourPickerSelTextFgColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
    }

    // Update the active theme for the lexer
    ColoursAndFontsManager::Get().SetActiveTheme(m_lexer->GetName(), m_choiceLexerThemes->GetStringSelection());

    if(m_globalThemeChanged) {
        wxBusyInfo bi(_("Applying changes..."));
        wxBusyCursor bc;
        // Set the theme, it will update the global theme as well
        ColoursAndFontsManager::Get().SetTheme(m_choiceGlobalTheme->GetStringSelection());
        m_globalThemeChanged = false;

        // Match the active tab colour with the selected theme
        LexerConf::Ptr_t cppLexer = ColoursAndFontsManager::Get().GetLexer("c++");
        if(cppLexer) {
            clConfig::Get().Write("ActiveTabMarkerColour", wxColour(cppLexer->GetProperty(wxSTC_C_WORD).GetFgColour()));
        }

        m_lexer = ColoursAndFontsManager::Get().GetLexer(m_lexer->GetName());
        CallAfter(&SyntaxHighlightDlg::LoadLexer, m_lexer->GetThemeName());
    }

    // Save the base colour changes
    clConfig::Get().Write("BaseColour", m_colourPickerBaseColour->GetColour());
    clConfig::Get().Write("UseCustomBaseColour", m_cbUseCustomBaseColour->IsChecked());

    // Update the text selection colours
    UpdateTextSelectionColours();

    // Now save the changes to the file system
    ColoursAndFontsManager::Get().Save();
    m_isModified = false;
}

SyntaxHighlightDlg::~SyntaxHighlightDlg()
{
    // Write the global font
    wxFont font = m_fontPickerGlobal->GetSelectedFont();
    if(font.IsOk()) {
        clConfig::Get().Write("GlobalThemeFont", font);
    }
}

void SyntaxHighlightDlg::OnColourChanged(wxColourPickerEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    m_isModified = true;
    // update colour
    wxObject* obj = event.GetEventObject();
    if(obj == m_colourPicker) {
        wxColour colour = event.GetColour();
        StyleProperty::Map_t::iterator iter = GetSelectedStyle();
        iter->second.SetFgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));

    } else if(obj == m_bgColourPicker) {

        wxColour colour = event.GetColour();
        StyleProperty::Map_t::iterator iter = GetSelectedStyle();

        iter->second.SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));

    } else if(obj == m_globalBgColourPicker) {
        wxColour colour = event.GetColour();
        DoSetGlobalBgColour(colour);
    }
}
void SyntaxHighlightDlg::EditKeyWords(int set)
{
    CHECK_PTR_RET(m_lexer);

    wxString keywords = m_lexer->GetKeyWords(set);
    FreeTextDialog dlg(this, keywords);
    if(dlg.ShowModal() == wxID_OK) {
        m_isModified = true;
        m_lexer->SetKeyWords(dlg.GetValue(), set);
    }
}

void SyntaxHighlightDlg::OnEditKeyWordsButton0(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(0);
}

void SyntaxHighlightDlg::OnEditKeyWordsButton1(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(1);
}
void SyntaxHighlightDlg::OnEditKeyWordsButton2(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(2);
}
void SyntaxHighlightDlg::OnEditKeyWordsButton3(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(3);
}
void SyntaxHighlightDlg::OnEditKeyWordsButton4(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(4);
}

void SyntaxHighlightDlg::OnEolFilled(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    m_isModified = true;
    StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
    StyleProperty::Map_t::iterator iter = GetSelectedStyle();
    if(iter != properties.end()) {
        iter->second.SetEolFilled(event.IsChecked());
    }
}

void SyntaxHighlightDlg::OnFontChanged(wxFontPickerEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    m_isModified = true;

    StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
    // update f
    wxObject* obj = event.GetEventObject();
    if(obj == m_fontPicker) {
        wxFont f = event.GetFont();
        StyleProperty::Map_t::iterator iter = GetSelectedStyle();
        DoFontChanged(iter->second, f);

    } else if(obj == m_globalFontPicker) {
        wxFont f = event.GetFont();
        StyleProperty::Map_t::iterator iter = properties.begin();
        for(; iter != properties.end(); ++iter) {
            DoFontChanged(iter->second, f);
        }
        // update the style f picker as well
        m_fontPicker->SetSelectedFont(f);
        ColoursAndFontsManager::Get().SetGlobalFont(f);
    }
}

void SyntaxHighlightDlg::OnItemSelected(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_lexer);

    // update colour picker & font pickers
    wxString selectionString = event.GetString();
    StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
    StyleProperty::Map_t::iterator iter = properties.begin();
    for(; iter != properties.end(); ++iter) {
        if(iter->second.GetName() == selectionString) {
            // update font & color
            StyleProperty p = iter->second;
            wxString colour = p.GetFgColour();
            wxString bgColour = p.GetBgColour();

            int size = p.GetFontSize();
            wxString face = p.GetFaceName();
            bool bold = p.IsBold();

            wxFont font = wxFont(size, wxFONTFAMILY_TELETYPE, p.GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
                                 bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, p.GetUnderlined(), face);
            m_fontPicker->SetSelectedFont(font);
            m_bgColourPicker->SetColour(bgColour);
            m_colourPicker->SetColour(colour);
            m_eolFilled->SetValue(p.GetEolFilled());
        }
    }
}

void SyntaxHighlightDlg::OnOutputViewColourChanged(wxColourPickerEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    event.Skip();
    m_isModified = true;
    m_globalBgColourChanged = true;
}

void SyntaxHighlightDlg::OnSelTextChanged(wxColourPickerEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    event.Skip();
    m_isModified = true;
    ColoursAndFontsManager::Get().SetThemeTextSelectionColours(m_lexer->GetThemeName(),
                                                               m_colourPickerSelTextBgColour->GetColour(),
                                                               m_colourPickerSelTextFgColour->GetColour(), true);
}

void SyntaxHighlightDlg::OnStyleWithinPreprocessor(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    m_isModified = true;
    m_lexer->SetStyleWithinPreProcessor(event.IsChecked());
}

void SyntaxHighlightDlg::OnStyleWithingPreProcessorUI(wxUpdateUIEvent& event)
{
    CHECK_PTR_RET(m_lexer);

    StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
    StyleProperty::Map_t::iterator iter = GetSelectedStyle();
    if(iter == properties.end())
        event.Enable(false);
    else if(iter->second.GetName() == "Preprocessor")
        event.Enable(true);
    else
        event.Enable(false);
}

void SyntaxHighlightDlg::OnText(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    event.Skip();
    m_isModified = true;
    m_lexer->SetFileSpec(m_fileSpec->GetValue());
}

void SyntaxHighlightDlg::CreateLexerPage()
{
    CHECK_PTR_RET(m_lexer);

    const StyleProperty::Map_t& m_propertyList = m_lexer->GetLexerProperties();
    StyleProperty::Map_t::const_iterator it = m_propertyList.begin();
    StyleProperty selTextProperties;

    for(; it != m_propertyList.end(); it++) {
        if(it->second.GetId() != SEL_TEXT_ATTR_ID) {
            m_properties->Append(it->second.GetName());
        } else {
            selTextProperties = it->second;
        }
    }

    if(m_properties->GetCount()) {
        m_properties->SetSelection(0);
    }

    wxString initialColor = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT).GetAsString(wxC2S_HTML_SYNTAX);
    wxString bgInitialColor = clSystemSettings::GetDefaultPanelColour().GetAsString(wxC2S_HTML_SYNTAX);
    // bool     initialEolFilled (false);
    bool initialStyleWithinPreProcessor(true);

    wxFont initialFont = ColoursAndFontsManager::Get().GetGlobalFont();
    if(!initialFont.IsOk()) {
        initialFont = DrawingUtils::GetDefaultFixedFont();
    }

    initialStyleWithinPreProcessor = m_lexer->GetStyleWithinPreProcessor();
    const StyleProperty& defaultStyle = m_lexer->GetProperty(0);
    if(!defaultStyle.IsNull()) {
        m_colourPicker->SetColour(defaultStyle.GetFgColour());
        m_bgColourPicker->SetColour(defaultStyle.GetBgColour());
        m_globalBgColourPicker->SetColour(defaultStyle.GetBgColour());
    }

    m_fontPicker->SetSelectedFont(initialFont);
    m_globalFontPicker->SetSelectedFont(initialFont);

    m_fileSpec->ChangeValue(m_lexer->GetFileSpec());
    m_styleWithinPreProcessor->SetValue(initialStyleWithinPreProcessor);

    // Update selected text properties
    clDEBUG() << "Setting theme:" << m_lexer->GetName() << "selection colour: bg:" << selTextProperties.GetBgColour()
              << ", fg:" << selTextProperties.GetFgColour() << endl;
    m_colourPickerSelTextBgColour->SetColour(selTextProperties.GetBgColour());
    m_colourPickerSelTextFgColour->SetColour(selTextProperties.GetFgColour());
    m_checkBoxCustomSelectionFgColour->SetValue(true);

    if(m_propertyList.empty()) {
        m_fontPicker->Enable(false);
        m_colourPicker->Enable(false);
    }

    // Fill the themes for this lexer
    m_choiceLexerThemes->Clear();
    wxArrayString themes = ColoursAndFontsManager::Get().GetAvailableThemesForLexer(m_lexer->GetName());
    int sel = themes.Index(m_lexer->GetThemeName());
    if(sel == -1) {
        sel = 0;
    }
    m_choiceLexerThemes->Append(themes);
    if(!m_choiceLexerThemes->IsEmpty()) {
        m_choiceLexerThemes->SetSelection(sel);
    }
}

StyleProperty::Map_t::iterator SyntaxHighlightDlg::GetSelectedStyle()
{
    wxString selectedProperty = m_properties->GetStringSelection();
    StyleProperty::Map_t& lexerProperties = m_lexer->GetLexerProperties();
    return std::find_if(lexerProperties.begin(), lexerProperties.end(), StyleProperty::FindByName(selectedProperty));
}

void SyntaxHighlightDlg::OnLexerSelected(wxCommandEvent& event)
{
    wxString lexerName = m_listBox->GetStringSelection();
    if(lexerName == CXX_AND_JAVASCRIPT) {
        lexerName = "c++";
    }

    if(m_isModified) {
        SaveChanges();
    }
    m_isModified = false;
    m_lexer = ColoursAndFontsManager::Get().GetLexer(lexerName);
    LoadLexer("");
}

void SyntaxHighlightDlg::OnButtonApplyUI(wxUpdateUIEvent& event) { event.Enable(m_isModified); }

void SyntaxHighlightDlg::OnTextSelFgUI(wxUpdateUIEvent& event) { event.Enable(true); }

void SyntaxHighlightDlg::OnSelTextFgChanged(wxColourPickerEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    event.Skip();
    m_isModified = true;
    UpdateTextSelectionColours();
}

void SyntaxHighlightDlg::OnUseCustomFgTextColour(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    event.Skip();
    m_isModified = true;
    UpdateTextSelectionColours();
}

void SyntaxHighlightDlg::UpdateTextSelectionColours()
{
    ColoursAndFontsManager::Get().SetThemeTextSelectionColours(m_lexer->GetThemeName(),
                                                               m_colourPickerSelTextBgColour->GetColour(),
                                                               m_colourPickerSelTextFgColour->GetColour(), true);
}

void SyntaxHighlightDlg::OnNewTheme(wxCommandEvent& event)
{
    // Create new theme
    CHECK_PTR_RET(m_lexer);
    NewThemeDialog dlg(this, m_lexer);
    if(dlg.ShowModal() == wxID_OK) {
        // Create new XML and load it
        LexerConf::Ptr_t newLexer =
            ColoursAndFontsManager::Get().CopyTheme(dlg.GetLexerName(), dlg.GetThemeName(), dlg.GetBaseTheme());
        if(newLexer) {
            LoadLexer(newLexer->GetName());
        }
    }
}

void SyntaxHighlightDlg::OnImport(wxCommandEvent& event)
{
    wxString path = ::wxFileSelector(_("Select file"), "", "", "", "Zip Files (*.zip)|*.zip", wxFD_OPEN);
    if(path.IsEmpty())
        return;

    wxFileName fn(path);
    clZipReader zr(fn);
    zr.Extract("lexers.json", clStandardPaths::Get().GetTempDir());

    wxFileName fileToImport(clStandardPaths::Get().GetTempDir(), "lexers.json");
    if(ColoursAndFontsManager::Get().ImportLexersFile(fileToImport, true)) {
        EndModal(wxID_OK);
        // relaunch the dialog
        wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("syntax_highlight"));
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(openEvent);
    }
}

void SyntaxHighlightDlg::OnExportSelective(wxCommandEvent& event)
{
    // Get list of choices
    wxArrayString lexers = ColoursAndFontsManager::Get().GetAllThemes();
    wxArrayInt choices;
    if(::wxGetSelectedChoices(choices, _("Select which themes to export:"), _("Export Themes"), lexers, this) ==
       wxNOT_FOUND) {
        return;
    }
    wxArrayString lexersToExport;
    for(size_t i = 0; i < choices.GetCount(); ++i) {
        wxString lexerName = lexers.Item(choices.Item(i)).Lower();
        lexersToExport.push_back(lexerName);
    }
    DoExport(lexersToExport);
}

void SyntaxHighlightDlg::OnExportAll(wxCommandEvent& event) { DoExport(); }

void SyntaxHighlightDlg::OnToolExportAll(wxCommandEvent& event) { DoExport(); }

void SyntaxHighlightDlg::OnRestoreDefaults(wxCommandEvent& event)
{
    // Ask for confirmation
    if(::wxMessageBox(_("Are you sure you want to restore colours to factory defaults?\nBy choosing 'Yes', you will "
                        "lose all your local modifications"),
                      _("Confirm"), wxICON_WARNING | wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxCENTER, this) != wxYES) {
        return;
    }

    // Restore defaults
    ColoursAndFontsManager::Get().RestoreDefaults();

    // Dismiss the dialog
    EndModal(wxID_OK);
    // and reload it
    wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("syntax_highlight"));
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(openEvent);
}

void SyntaxHighlightDlg::OnImportEclipseTheme(wxCommandEvent& event)
{
    wxFileDialog selector(this, _("Select theme to import"), "", "",
                          "VSCode JSON Theme (*.json)|*.json|Eclipse Theme Files (*.xml)|*.xml",
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(selector.ShowModal() == wxID_OK) {

        wxString path = selector.GetPath();
        if(path.empty())
            return;

        wxBusyCursor bc;
        wxString theme_name = ColoursAndFontsManager::Get().ImportEclipseTheme(path);

        wxString message;
        message << _("Theme : '") << theme_name << _("' imported successfully!");
        ::wxMessageBox(message);

        int pos = m_choiceGlobalTheme->Append(theme_name);
        if(pos != wxNOT_FOUND) {
            m_choiceGlobalTheme->SetSelection(pos);
        }

        // Make this lexer the active one
        LoadLexer(theme_name);
        DoUpdatePreview();

        // Mark the dialg is modified and force a save
        m_isModified = true;
        SaveChanges();

        // Dismiss the dialog
        EndModal(wxID_OK);
        // and reload it
        wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("syntax_highlight"));
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(openEvent);
    }
}

void SyntaxHighlightDlg::OnLoadEclipseThemeWebsite(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ::wxLaunchDefaultBrowser("http://eclipsecolorthemes.org/");
}

void SyntaxHighlightDlg::OnGlobalThemeSelected(wxCommandEvent& event)
{
    DoUpdatePreview();
    LexerConf::Ptr_t previewLexer =
        ColoursAndFontsManager::Get().GetLexer("text", m_choiceGlobalTheme->GetStringSelection());

    LoadLexer(m_choiceGlobalTheme->GetStringSelection());
    if(previewLexer && previewLexer->IsDark() && m_cbUseCustomBaseColour->IsChecked()) {
        wxColour bgColour = ColoursAndFontsManager::Get().GetBackgroundColourFromLexer(previewLexer);
        m_colourPickerBaseColour->SetColour(bgColour);
        m_endingTheme = kTHEME_DARK;
    } else if(previewLexer && m_cbUseCustomBaseColour->IsChecked()) {
        // Light colour
        wxColour bgColour = ColoursAndFontsManager::Get().GetBackgroundColourFromLexer(previewLexer);
        m_colourPickerBaseColour->SetColour(bgColour);
        m_endingTheme = kTHEME_LIGHT;
    }
    m_globalThemeChanged = true;
    m_isModified = true;
}

void SyntaxHighlightDlg::OnGlobalFontSelected(wxFontPickerEvent& event)
{
    m_isModified = true;
    ColoursAndFontsManager::Get().SetGlobalFont(event.GetFont());
    DoUpdatePreview();
}

void SyntaxHighlightDlg::DoSetGlobalBgColour(const wxColour& colour)
{
    StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
    StyleProperty::Map_t::iterator iter = properties.begin();
    for(; iter != properties.end(); ++iter) {
        // Dont change the text selection using the global font picker
        if(iter->second.GetName() == wxT("Text Selection"))
            continue;
        iter->second.SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
    }

    // update the style background colour as well
    m_bgColourPicker->SetColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
}

void SyntaxHighlightDlg::DoShowTooltipForGlobalBgColourChanged() {}

void SyntaxHighlightDlg::DoExport(const wxArrayString& lexers)
{
    // Select the 'save' path
    wxString path = ::wxFileSelector(_("Save as"), "", "MySettings.zip", "", wxFileSelectorDefaultWildcardStr,
                                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(path.IsEmpty()) {
        return;
    }

    wxFileName jsonFile(path);
    jsonFile.SetFullName("lexers.json");
    // Delete the file when done
    FileUtils::Deleter deleter(jsonFile);
    if(!ColoursAndFontsManager::Get().ExportThemesToFile(jsonFile, lexers)) {
        return;
    }

    // Add the file to the zip
    clZipWriter zw(path);
    zw.Add(jsonFile);
    zw.Close();

    ::wxMessageBox(_("Settings have been saved into:\n") + zw.GetFilename().GetFullPath());
}

void SyntaxHighlightDlg::OnUseCustomColourUI(wxUpdateUIEvent& event)
{
    event.Enable(m_cbUseCustomBaseColour->IsChecked());
}
void SyntaxHighlightDlg::OnCustomBaseColourPIcked(wxColourPickerEvent& event)
{
    m_isModified = true;
    m_endingTheme = DrawingUtils::IsDark(event.GetColour()) ? kTHEME_DARK : kTHEME_LIGHT;
    event.Skip();
}

void SyntaxHighlightDlg::OnUseCustomBaseColour(wxCommandEvent& event)
{
    m_isModified = true;
    m_useBaseColourEnding = event.IsChecked();
    if(event.IsChecked()) {
        // Adjust the colour to the selected theme
        LexerConf::Ptr_t lexer =
            ColoursAndFontsManager::Get().GetLexer("text", m_choiceGlobalTheme->GetStringSelection());
        wxColour bgColour = ColoursAndFontsManager::Get().GetBackgroundColourFromLexer(lexer);
        if(bgColour.IsOk()) {
            m_colourPickerBaseColour->SetColour(bgColour);
        }
    }
    m_endingTheme = DrawingUtils::IsDark(m_colourPickerBaseColour->GetColour()) ? kTHEME_DARK : kTHEME_LIGHT;
    event.Skip();
}

bool SyntaxHighlightDlg::IsRestartRequired() const { return false; }
void SyntaxHighlightDlg::OnUseCustomBaseColourUI(wxUpdateUIEvent& event)
{
#if CL_USE_NATIVEBOOK
    event.Enable(false);
    event.Check(false);
#else
    event.Enable(true);
#endif
}

void SyntaxHighlightDlg::DoFontChanged(StyleProperty& sp, const wxFont& font)
{
    sp.SetBold(font.GetWeight() == wxFONTWEIGHT_BOLD);
    sp.SetFaceName(font.GetFaceName());
    sp.SetFontSize(font.GetPointSize());
    sp.SetUnderlined(font.GetUnderlined());
    sp.SetItalic(font.GetStyle() == wxFONTSTYLE_ITALIC);
}
