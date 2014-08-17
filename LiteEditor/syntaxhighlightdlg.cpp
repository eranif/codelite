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

#include <wx/xrc/xmlres.h>
#include "frame.h"
#include "windowattrmanager.h"
#include <wx/notebook.h>
#include <wx/treebook.h>
#include "macros.h"
#include "editor_config.h"
#include <wx/dir.h>
#include "syntaxhighlightdlg.h"
#include "manager.h"
#include "theme_handler.h"
#include "event_notifier.h"
#include <wx/wupdlock.h>
#include "context_manager.h"
#include "ColoursAndFontsManager.h"
#include <algorithm>
#include "free_text_dialog.h"
#include <wx/wupdlock.h>
#include "NewThemeDialog.h"
#include <wx/filedlg.h>
#include "clZipWriter.h"
#include "clZipReader.h"
#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include "EclipseCXXThemeImporter.h"
#include <wx/msgdlg.h>

SyntaxHighlightDlg::SyntaxHighlightDlg(wxWindow* parent)
    : SyntaxHighlightBaseDlg(parent)
    , m_isModified(false)
{
    // Get list of available lexers
    wxString lexerName;
    LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor(true);
    wxArrayString lexers = ColoursAndFontsManager::Get().GetAllLexersNames();
    if(editor) {
        lexerName = editor->GetContext()->GetName().Lower();
    }

    m_listBox->Append(lexers);
    if(!m_listBox->IsEmpty()) {
        if(lexerName.IsEmpty()) {
            m_listBox->Select(0);
        } else {
            m_listBox->SetStringSelection(lexerName);
        }
        LoadLexer(""); // Load the default active theme
    }

    // Load the global colours
    m_colourPickerOutputPanesFgColour->SetColour(ColoursAndFontsManager::Get().GetGlobalFgColour());
    m_colourPickerOutputPanesBgColour->SetColour(ColoursAndFontsManager::Get().GetGlobalBgColour());
    m_isModified = false;
    WindowAttrManager::Load(this, wxT("SyntaxHighlightDlgAttr"), NULL);
}

void SyntaxHighlightDlg::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SaveChanges();
    clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();
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
    ColoursAndFontsManager::Get().Save(m_lexer);
    // Update the active theme for the lexer
    ColoursAndFontsManager::Get().SetActiveTheme(m_lexer->GetName(), m_choiceLexerThemes->GetStringSelection());

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
    WindowAttrManager::Save(this, wxT("SyntaxHighlightDlgAttr"), NULL);
}

void SyntaxHighlightDlg::OnColourChanged(wxColourPickerEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    m_isModified = true;
    // update colour
    wxObject* obj = event.GetEventObject();
    if(obj == m_colourPicker) {
        wxColour colour = event.GetColour();
        std::list<StyleProperty>::iterator iter = GetSelectedStyle();
        iter->SetFgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));

    } else if(obj == m_bgColourPicker) {

        wxColour colour = event.GetColour();
        std::list<StyleProperty>::iterator iter = GetSelectedStyle();

        iter->SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));

    } else if(obj == m_globalBgColourPicker) {

        wxColour colour = event.GetColour();
        StyleProperty::List_t& properties = m_lexer->GetLexerProperties();
        StyleProperty::List_t::iterator iter = properties.begin();
        for(; iter != properties.end(); ++iter) {
            // Dont change the text selection using the global font picker
            if(iter->GetName() == wxT("Text Selection")) continue;
            iter->SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
        }

        // update the style background colour as well
        m_bgColourPicker->SetColour(colour);
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
    StyleProperty::List_t& properties = m_lexer->GetLexerProperties();
    StyleProperty::List_t::iterator iter = GetSelectedStyle();
    if(iter != properties.end()) {
        iter->SetEolFilled(event.IsChecked());
    }
}

void SyntaxHighlightDlg::OnFontChanged(wxFontPickerEvent& event)
{
    CHECK_PTR_RET(m_lexer);
    m_isModified = true;

    StyleProperty::List_t& properties = m_lexer->GetLexerProperties();
    // update f
    wxObject* obj = event.GetEventObject();
    if(obj == m_fontPicker) {
        wxFont f = event.GetFont();
        std::list<StyleProperty>::iterator iter = GetSelectedStyle();

        iter->SetBold(f.GetWeight() == wxFONTWEIGHT_BOLD);
        iter->SetFaceName(f.GetFaceName());
        iter->SetFontSize(f.GetPointSize());
        iter->SetUnderlined(f.GetUnderlined());
        iter->SetItalic(f.GetStyle() == wxFONTSTYLE_ITALIC);

    } else if(obj == m_globalFontPicker) {
        wxFont f = event.GetFont();
        StyleProperty::List_t::iterator iter = properties.begin();
        for(; iter != properties.end(); iter++) {
            iter->SetBold(f.GetWeight() == wxFONTWEIGHT_BOLD);
            iter->SetFaceName(f.GetFaceName());
            iter->SetFontSize(f.GetPointSize());
            iter->SetUnderlined(f.GetUnderlined());
            iter->SetItalic(f.GetStyle() == wxFONTSTYLE_ITALIC);
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
    StyleProperty::List_t& properties = m_lexer->GetLexerProperties();
    StyleProperty::List_t::iterator iter = properties.begin();
    for(; iter != properties.end(); iter++) {
        if(iter->GetName() == selectionString) {
            // update font & color
            StyleProperty p = (*iter);
            wxString colour = p.GetFgColour();
            wxString bgColour = p.GetBgColour();
            wxFont font = wxNullFont;

            int size = p.GetFontSize();
            wxString face = p.GetFaceName();
            bool bold = p.IsBold();

            font = wxFont(size,
                          wxFONTFAMILY_TELETYPE,
                          p.GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
                          bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
                          p.GetUnderlined(),
                          face);
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

    StyleProperty::List_t& properties = m_lexer->GetLexerProperties();
    StyleProperty::List_t::iterator iter = GetSelectedStyle();
    if(iter == properties.end())
        event.Enable(false);
    else if(iter->GetName() == "Preprocessor")
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

    const StyleProperty::List_t& m_propertyList = m_lexer->GetLexerProperties();
    std::list<StyleProperty>::const_iterator it = m_propertyList.begin();
    StyleProperty selTextProperties;

    for(; it != m_propertyList.end(); it++) {
        if(it->GetId() != SEL_TEXT_ATTR_ID) {
            m_properties->Append((*it).GetName());
        } else {
            selTextProperties = *it;
        }
    }

    if(m_properties->GetCount()) m_properties->SetSelection(0);

    wxString initialColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT).GetAsString();
    wxString bgInitialColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString();
    wxFont initialFont = wxNullFont;
    // bool     initialEolFilled (false);
    bool initialStyleWithinPreProcessor(true);

    if(m_propertyList.empty() == false) {
        StyleProperty p;
        p = (*m_propertyList.begin());
        initialColor = p.GetFgColour();
        bgInitialColor = p.GetBgColour();

        int size = p.GetFontSize();
        wxString face = p.GetFaceName();
        bool bold = p.IsBold();
        initialFont = wxFont(size,
                             wxFONTFAMILY_TELETYPE,
                             wxFONTSTYLE_NORMAL,
                             bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
                             false,
                             face);
    }
    initialStyleWithinPreProcessor = m_lexer->GetStyleWithinPreProcessor();

    m_fontPicker->SetSelectedFont(initialFont);
    m_colourPicker->SetColour(wxColour(initialColor));
    m_bgColourPicker->SetColour(wxColour(bgInitialColor));
    m_globalFontPicker->SetSelectedFont(initialFont);
    m_globalBgColourPicker->SetColour(wxColour(bgInitialColor));
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
    if(sel == -1) {
        sel = 0;
    }
    m_choiceLexerThemes->Append(themes);
    if(!m_choiceLexerThemes->IsEmpty()) {
        m_choiceLexerThemes->SetSelection(sel);
    }
}

StyleProperty::List_t::iterator SyntaxHighlightDlg::GetSelectedStyle()
{
    wxString selectedProperty = m_properties->GetStringSelection();
    StyleProperty::List_t& lexerProperties = m_lexer->GetLexerProperties();
    return std::find_if(lexerProperties.begin(), lexerProperties.end(), StyleProperty::FindByName(selectedProperty));
}

void SyntaxHighlightDlg::OnLexerSelected(wxCommandEvent& event)
{
    wxString lexerName = m_listBox->GetStringSelection();
    if(m_isModified) {
        SaveChanges();
    }
    m_isModified = false;
    m_lexer = ColoursAndFontsManager::Get().GetLexer(lexerName);
    LoadLexer("");
}

void SyntaxHighlightDlg::OnButtonApplyUI(wxUpdateUIEvent& event)
{
    event.Enable(m_isModified);
}

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
        if(newLexer) {
            LoadLexer(newLexer->GetName());
        }
    }
}

void SyntaxHighlightDlg::OnExport(wxCommandEvent& event)
{
    // Get list of choices
    wxArrayString lexers = ColoursAndFontsManager::Get().GetAllLexersNames();
    wxArrayInt choices;
    if(::wxGetSelectedChoices(choices, _("Select which lexers you wish to export"), _("Export Lexers"), lexers, this) ==
       wxNOT_FOUND) {
        return;
    }

    // Select the 'save' path
    wxString path = ::wxFileSelector(
        _("Save as"), "", "MySettings.zip", "", wxFileSelectorDefaultWildcardStr, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(path.IsEmpty()) return;

    clZipWriter zw(path);
    for(size_t i = 0; i < choices.GetCount(); ++i) {
        wxString file;
        file << "lexer_" << lexers.Item(choices.Item(i)).Lower() << "_*.xml";
        zw.AddDirectory(clStandardPaths::Get().GetUserLexersDir(), file);
    }
    zw.Close();

    ::wxMessageBox(_("Settings have been saved into:\n") + zw.GetFilename().GetFullPath());
}

void SyntaxHighlightDlg::OnImport(wxCommandEvent& event)
{
    wxString path = ::wxFileSelector(_("Select file"), "", "", "", "Zip Files (*.zip)|*.zip", wxFD_OPEN);
    if(path.IsEmpty()) return;

    wxFileName fn(path);
    clZipReader zr(fn);
    zr.Extract("*", clStandardPaths::Get().GetUserLexersDir());

    // reload the settings
    ColoursAndFontsManager::Get().Reload();
    EndModal(wxID_OK);

    // relaunch the dialog
    wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("syntax_highlight"));
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(openEvent);
}

void SyntaxHighlightDlg::OnExportSelective(wxCommandEvent& event)
{
    OnExport(event);
}

void SyntaxHighlightDlg::OnExportAll(wxCommandEvent& event)
{
    // Get list of choices
    wxArrayString lexers = ColoursAndFontsManager::Get().GetAllLexersNames();
    // Select the 'save' path
    wxString path = ::wxFileSelector(
        _("Save as"), "", "MySettings.zip", "", wxFileSelectorDefaultWildcardStr, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(path.IsEmpty()) return;

    clZipWriter zw(path);
    zw.AddDirectory(clStandardPaths::Get().GetUserLexersDir(), "lexer_*.xml");
    zw.Close();

    ::wxMessageBox(_("Settings have been saved into:\n") + zw.GetFilename().GetFullPath());
}

void SyntaxHighlightDlg::OnToolExportAll(wxAuiToolBarEvent& event)
{
    OnExportAll(event);
}
void SyntaxHighlightDlg::OnRestoreDefaults(wxCommandEvent& event)
{
    // Ask for confirmation
    if(::wxMessageBox(_("Are you sure you want to restore colours to factory defaults?\nBy choosing 'Yes', you will "
                        "lose your modifications"),
                      _("Confirm"),
                      wxICON_WARNING | wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxCENTER,
                      this) == wxYES) {
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
    wxString eclipseThemeXml =
        ::wxFileSelector(_("Select eclipse XML theme file"), "", "", "", "Eclipse Theme Files (*.xml)|*.xml");
    wxString outputFile;
    if(ColoursAndFontsManager::Get().ImportEclipseTheme(eclipseThemeXml, outputFile)) {
        ::wxMessageBox(_("File imported successfully!\n") + outputFile);
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
