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

#include "ColoursAndFontsManager.h"
#include "EclipseCXXThemeImporter.h"
#include "EclipseThemeImporterManager.h"
#include "NewThemeDialog.h"
#include "clZipReader.h"
#include "clZipWriter.h"
#include "cl_config.h"
#include "context_manager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "frame.h"
#include "free_text_dialog.h"
#include "macros.h"
#include "manager.h"
#include "syntaxhighlightdlg.h"
#include "theme_handler.h"
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
                            "    Demo(const Demo &other) {\n"
                            "        m_str = other.m_str;\n"
                            "        m_integer = other.m_integer;\n"
                            "    }\n"
                            "}";

SyntaxHighlightDlg::SyntaxHighlightDlg(wxWindow* parent)
    : SyntaxHighlightBaseDlg(parent)
    , m_isModified(false)
    , m_globalThemeChanged(false)
    , m_globalBgColourChanged(false)
{
    // Get list of available lexers
    wxString lexerName;
    LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor(true);
    wxArrayString lexers = ColoursAndFontsManager::Get().GetAllLexersNames();
    if(editor) { lexerName = editor->GetContext()->GetName().Lower(); }

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
    m_colourPickerOutputPanesFgColour->SetColour(ColoursAndFontsManager::Get().GetGlobalFgColour());
    m_colourPickerOutputPanesBgColour->SetColour(ColoursAndFontsManager::Get().GetGlobalBgColour());
    m_choiceGlobalTheme->Append(ColoursAndFontsManager::Get().GetAvailableThemesForLexer("c++"));
    m_choiceGlobalTheme->SetStringSelection(ColoursAndFontsManager::Get().GetGlobalTheme());

    // Set the current editor font to the default one
    wxFont font = clConfig::Get().Read("GlobalThemeFont", wxNullFont);
    if(font.IsOk()) { m_fontPickerGlobal->SetSelectedFont(font); }

    DoUpdatePreview();

    m_isModified = false;
    SetName("SyntaxHighlightDlg");
    WindowAttrManager::Load(this);
    CentreOnParent();
}

void SyntaxHighlightDlg::DoUpdatePreview()
{
    // Populate the preview
    LexerConf::Ptr_t previewLexer =
        ColoursAndFontsManager::Get().GetLexer("c++", m_choiceGlobalTheme->GetStringSelection());
    if(previewLexer) { previewLexer->Apply(m_stcPreview, true); }
    m_stcPreview->SetKeyWords(1, "Demo std string");
    m_stcPreview->SetKeyWords(3, "other");
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
    m_globalFontPicker->SetSelectedFont(wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT));
    m_globalBgColourPicker->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    m_fileSpec->ChangeValue("");

    // Customize page
    m_properties->Clear();
    m_fontPicker->SetSelectedFont(wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT));
    m_colourPicker->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    m_bgColourPicker->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    m_eolFilled->SetValue(false);
    m_styleWithinPreProcessor->SetValue(false);

    // Text Selection page
    m_colourPickerSelTextBgColour->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    m_isModified = false;
}

void SyntaxHighlightDlg::LoadLexer(const wxString& themeName)
{
    wxWindowUpdateLocker locker(this);
    Clear();
    wxString lexer = m_listBox->GetStringSelection();
    if(lexer.IsEmpty()) return;

    if(lexer == CXX_AND_JAVASCRIPT) { lexer = "c++"; }

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

        m_lexer = ColoursAndFontsManager::Get().GetLexer(m_lexer->GetName());
        CallAfter(&SyntaxHighlightDlg::LoadLexer, m_lexer->GetThemeName());
    }

    // Now save the changes to the file system
    ColoursAndFontsManager::Get().Save();

    wxString oldFg = EditorConfigST::Get()->GetCurrentOutputviewFgColour();
    wxString oldBg = EditorConfigST::Get()->GetCurrentOutputviewBgColour();

    wxString newFg = m_colourPickerOutputPanesFgColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX);
    EditorConfigST::Get()->SetCurrentOutputviewFgColour(newFg);

    wxString newBg = m_colourPickerOutputPanesBgColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX);
    EditorConfigST::Get()->SetCurrentOutputviewBgColour(newBg);

    m_isModified = false;
}

SyntaxHighlightDlg::~SyntaxHighlightDlg()
{
    // Write the global font
    wxFont font = m_fontPickerGlobal->GetSelectedFont();
    if(font.IsOk()) { clConfig::Get().Write("GlobalThemeFont", font); }
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
    if(iter != properties.end()) { iter->second.SetEolFilled(event.IsChecked()); }
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

        iter->second.SetBold(f.GetWeight() == wxFONTWEIGHT_BOLD);
        iter->second.SetFaceName(f.GetFaceName());
        iter->second.SetFontSize(f.GetPointSize());
        iter->second.SetUnderlined(f.GetUnderlined());
        iter->second.SetItalic(f.GetStyle() == wxFONTSTYLE_ITALIC);

    } else if(obj == m_globalFontPicker) {
        wxFont f = event.GetFont();
        StyleProperty::Map_t::iterator iter = properties.begin();
        for(; iter != properties.end(); ++iter) {
            iter->second.SetBold(f.GetWeight() == wxFONTWEIGHT_BOLD);
            iter->second.SetFaceName(f.GetFaceName());
            iter->second.SetFontSize(f.GetPointSize());
            iter->second.SetUnderlined(f.GetUnderlined());
            iter->second.SetItalic(f.GetStyle() == wxFONTSTYLE_ITALIC);
        }
        // update the style f picker as well
        m_fontPicker->SetSelectedFont(f);
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
    StyleProperty& selProp = m_lexer->GetProperty(SEL_TEXT_ATTR_ID);
    if(!selProp.IsNull()) {
        m_isModified = true;
        selProp.SetBgColour(event.GetColour().GetAsString(wxC2S_HTML_SYNTAX));
    }
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

    if(m_properties->GetCount()) { m_properties->SetSelection(0); }

    wxString initialColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT).GetAsString(wxC2S_HTML_SYNTAX);
    wxString bgInitialColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX);
    wxFont initialFont = wxNullFont;
    // bool     initialEolFilled (false);
    bool initialStyleWithinPreProcessor(true);

    if(m_propertyList.empty() == false) {
        StyleProperty p;
        p = m_propertyList.begin()->second;
        initialColor = p.GetFgColour();
        bgInitialColor = p.GetBgColour();

        int size = p.GetFontSize();
        wxString face = p.GetFaceName();
        bool bold = p.IsBold();
        initialFont = wxFont(size, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                             bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, false, face);
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
    m_colourPickerSelTextBgColour->SetColour(selTextProperties.GetBgColour());
    m_colourPickerSelTextFgColour->SetColour(selTextProperties.GetFgColour());
    m_checkBoxCustomSelectionFgColour->SetValue(m_lexer->IsUseCustomTextSelectionFgColour());

    if(m_propertyList.empty()) {
        m_fontPicker->Enable(false);
        m_colourPicker->Enable(false);
    }

    // Fill the themes for this lexer
    m_choiceLexerThemes->Clear();
    wxArrayString themes = ColoursAndFontsManager::Get().GetAvailableThemesForLexer(m_lexer->GetName());
    int sel = themes.Index(m_lexer->GetThemeName());
    if(sel == -1) { sel = 0; }
    m_choiceLexerThemes->Append(themes);
    if(!m_choiceLexerThemes->IsEmpty()) { m_choiceLexerThemes->SetSelection(sel); }
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
    if(lexerName == CXX_AND_JAVASCRIPT) { lexerName = "c++"; }

    if(m_isModified) { SaveChanges(); }
    m_isModified = false;
    m_lexer = ColoursAndFontsManager::Get().GetLexer(lexerName);
    LoadLexer("");
}

void SyntaxHighlightDlg::OnButtonApplyUI(wxUpdateUIEvent& event) { event.Enable(m_isModified); }

void SyntaxHighlightDlg::OnTextSelFgUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxCustomSelectionFgColour->IsChecked());
}

void SyntaxHighlightDlg::OnSelTextFgChanged(wxColourPickerEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    event.Skip();
    StyleProperty& selProp = m_lexer->GetProperty(SEL_TEXT_ATTR_ID);
    if(!selProp.IsNull()) {
        m_isModified = true;
        selProp.SetFgColour(event.GetColour().GetAsString(wxC2S_HTML_SYNTAX));
    }
}

void SyntaxHighlightDlg::OnUseCustomFgTextColour(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    event.Skip();
    m_isModified = true;
    m_lexer->SetUseCustomTextSelectionFgColour(event.IsChecked());
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
        if(newLexer) { LoadLexer(newLexer->GetName()); }
    }
}

void SyntaxHighlightDlg::OnImport(wxCommandEvent& event)
{
    wxString path = ::wxFileSelector(_("Select file"), "", "", "", "Zip Files (*.zip)|*.zip", wxFD_OPEN);
    if(path.IsEmpty()) return;

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

void SyntaxHighlightDlg::OnToolExportAll(wxAuiToolBarEvent& event) { DoExport(); }

void SyntaxHighlightDlg::OnRestoreDefaults(wxCommandEvent& event)
{
    // Ask for confirmation
    if(::wxMessageBox(_("Are you sure you want to restore colours to factory defaults?\nBy choosing 'Yes', you will "
                        "lose all your local modifications"),
                      _("Confirm"), wxICON_WARNING | wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxCENTER, this) == wxYES) {
        // Restore defaults
        ColoursAndFontsManager::Get().RestoreDefaults();
        // Dismiss the dialog
        EndModal(wxID_OK);
        // and reload it
        wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("syntax_highlight"));
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(openEvent);
    }
}

void SyntaxHighlightDlg::OnImportEclipseTheme(wxAuiToolBarEvent& event)
{
    wxFileDialog selector(this, _("Select eclipse XML theme file"), "", "", "Eclipse Theme Files (*.xml)|*.xml",
                          wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);
    if(selector.ShowModal() == wxID_OK) {
        wxArrayString files;
        selector.GetPaths(files);
        if(files.IsEmpty()) return;
        for(size_t i = 0; i < files.size(); ++i) {
            ColoursAndFontsManager::Get().ImportEclipseTheme(files.Item(i));
        }
        // Mark the dialg is modified and force a save
        wxBusyCursor bc;
        m_isModified = true;
        SaveChanges();

        ::wxMessageBox(_("File imported successfully!"));
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
    m_globalThemeChanged = true;
    m_isModified = true;
    if(!m_globalBgColourChanged) {
        // If the user did not alter the global bg colour, select a proper bg colour
        // for him/her
        LexerConf::Ptr_t lexerText =
            ColoursAndFontsManager::Get().GetLexer("text", m_choiceGlobalTheme->GetStringSelection());
        CHECK_PTR_RET(lexerText);
        wxColour bgColour =
            lexerText->IsDark() ? DrawingUtils::GetPanelBgColour() : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        m_colourPickerOutputPanesBgColour->SetColour(bgColour);
        // CallAfter(&SyntaxHighlightDlg::DoShowTooltipForGlobalBgColourChanged);
    }
    DoUpdatePreview();
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
        if(iter->second.GetName() == wxT("Text Selection")) continue;
        iter->second.SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
    }

    // update the style background colour as well
    m_bgColourPicker->SetColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
}

void SyntaxHighlightDlg::DoShowTooltipForGlobalBgColourChanged()
{
    if(!m_globalBgColourChangedTooltipShown) {
        wxRichToolTip tip(_("Global Background Colour Adjusted"),
                          _("Notice that CodeLite altered the global output panes\n"
                            "background colour to match the theme background colour"));
        tip.SetIcon(wxICON_INFORMATION);
        tip.ShowFor(m_colourPickerOutputPanesBgColour);
        m_globalBgColourChangedTooltipShown = true;
    }
}

void SyntaxHighlightDlg::DoExport(const wxArrayString& lexers)
{
    // Select the 'save' path
    wxString path = ::wxFileSelector(_("Save as"), "", "MySettings.zip", "", wxFileSelectorDefaultWildcardStr,
                                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(path.IsEmpty()) { return; }

    wxFileName jsonFile(path);
    jsonFile.SetFullName("lexers.json");
    // Delete the file when done
    FileUtils::Deleter deleter(jsonFile);
    if(!ColoursAndFontsManager::Get().ExportThemesToFile(jsonFile, lexers)) { return; }

    // Add the file to the zip
    clZipWriter zw(path);
    zw.Add(jsonFile);
    zw.Close();

    ::wxMessageBox(_("Settings have been saved into:\n") + zw.GetFilename().GetFullPath());
}
