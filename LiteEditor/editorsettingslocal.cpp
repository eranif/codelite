//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : editorsettingslocal.cpp
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

#include "editorsettingslocal.h"

#include "globals.h"
#include "windowattrmanager.h"

#include <wx/fontmap.h>
#include <wx/stc/stc.h>

EditorSettingsLocal::EditorSettingsLocal(OptionsConfigPtr hrOptions, wxXmlNode* nde, enum prefsLevel level,
                                         wxWindow* parent, wxWindowID id, const wxString& title)
    : LocalEditorSettingsbase(parent, id, title)
    , higherOptions(hrOptions)
    , node(nde)
{
    wxString label = title;
    if(level == pLevel_workspace) {
        label = _("Workspace local editor preferences");
    } else if(level == pLevel_project) {
        label = _("Project local editor preferences");
    }
    SetTitle(label);

    localOptions = new LocalOptionsConfig;
    LocalOptionsConfig pOC(localOptions, node);

    DisplayHigherValues(higherOptions); // Sets the 'global'? defaults, and the enabling checkboxes to disabling
    DisplayLocalValues(
        localOptions); // Sets any previous 'local' prefs, and the corresponding enabling-checkboxes to enabling

    SetName("EditorSettingsLocal");
    WindowAttrManager::Load(this);
    GetSizer()->Fit(this);
}

EditorSettingsLocal::~EditorSettingsLocal() {}

void EditorSettingsLocal::DisplayHigherValues(const OptionsConfigPtr options)
{
    // There should be 'global' (or workspace if this will be a project setting) values for each setting
    // Insert them all, but leave the enabling checkboxes ticked, so the items will be disabled
    m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
    m_indentWidth->SetValue(options->GetIndentWidth());
    m_tabWidth->SetValue(options->GetTabWidth());
    m_displayLineNumbers->SetValue(options->GetDisplayLineNumbers());
    m_showIndentationGuideLines->SetValue(options->GetShowIndentationGuidelines());

    m_highlightCaretLine->SetValue(options->GetHighlightCaretLine());
    m_checkBoxTrimLine->SetValue(options->GetTrimLine());
    m_checkBoxAppendLF->SetValue(options->GetAppendLF());

    m_checkBoxTrackEditorChanges->SetValue(options->IsTrackChanges());
    m_checkBoxDisplayFoldMargin->SetValue(options->GetDisplayFoldMargin());
    m_displayBookmarkMargin->SetValue(options->GetDisplayBookmarkMargin());

    const wxString WhitespaceStyle[] = { wxTRANSLATE("Invisible"), wxTRANSLATE("Visible always"),
                                         wxTRANSLATE("Visible after indentation"), wxTRANSLATE("Indentation only") };
    wxString currentWhitespace;
    switch(options->GetShowWhitspaces()) {
    case wxSTC_WS_VISIBLEALWAYS:
        currentWhitespace = wxT("Visible always");
        break;
    case wxSTC_WS_VISIBLEAFTERINDENT:
        currentWhitespace = wxT("Visible after indentation");
        break;
    default:
        currentWhitespace = wxT("Invisible");
        break;
    }
    m_WSstringManager.AddStrings(sizeof(WhitespaceStyle) / sizeof(wxString), WhitespaceStyle, currentWhitespace,
                                 m_whitespaceStyle);

    const wxString EOLChoices[] = { wxTRANSLATE("Default"), wxT("Mac (CR)"), wxT("Windows (CRLF)"), wxT("Unix (LF)") };
    m_EOLstringManager.AddStrings(sizeof(EOLChoices) / sizeof(wxString), EOLChoices, options->GetEolMode(),
                                  m_choiceEOL);

    wxArrayString astrEncodings;
    wxFontEncoding fontEnc;
    int iCurrSelId = 0;
    size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
    for(size_t i = 0; i < iEncCnt; i++) {
        fontEnc = wxFontMapper::GetEncoding(i);
        if(wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
            continue;
        }
        astrEncodings.Add(wxFontMapper::GetEncodingName(fontEnc));
        if(fontEnc == options->GetFileFontEncoding()) {
            iCurrSelId = i;
        }
    }
    m_fileEncoding->Append(astrEncodings);
    m_fileEncoding->SetSelection(iCurrSelId);
}

void EditorSettingsLocal::DisplayLocalValues(const LocalOptionsConfigPtr options)
{
    // There will be 'global' values already present for each setting
    // Override this with any valid previously-make local pref, then untick the corresponding enabling checkbox, so that
    // item will be enabled

    if(options->IndentUsesTabsIsValid()) {
        m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
        m_indentsUsesTabsEnable->SetValue(false);
    }
    if(options->IndentWidthIsValid()) {
        m_indentWidth->SetValue(options->GetIndentWidth());
        m_indentWidthEnable->SetValue(false);
    }

    if(options->TabWidthIsValid()) {
        m_tabWidth->SetValue(options->GetTabWidth());
        m_tabWidthEnable->SetValue(false);
    }
    if(options->DisplayLineNumbersIsValid()) {
        m_displayLineNumbers->SetValue(options->GetDisplayLineNumbers());
        m_displayLineNumbersEnable->SetValue(false);
    }
    if(options->ShowIndentationGuidelinesIsValid()) {
        m_showIndentationGuideLines->SetValue(options->GetShowIndentationGuidelines());
        m_showIndentationGuideLinesEnable->SetValue(false);
    }
    if(options->HighlightCaretLineIsValid()) {
        m_highlightCaretLine->SetValue(options->GetHighlightCaretLine());
        m_highlightCaretLineEnable->SetValue(false);
    }
    if(options->TrimLineIsValid()) {
        m_checkBoxTrimLine->SetValue(options->GetHighlightCaretLine());
        m_checkBoxTrimLineEnable->SetValue(false);
    }
    if(options->AppendLFIsValid()) {
        m_checkBoxAppendLF->SetValue(options->GetHighlightCaretLine());
        m_checkBoxAppendLFEnable->SetValue(false);
    }
    if(options->IsTrackChangesIsValid()) {
        m_checkBoxTrackEditorChanges->SetValue(options->IsTrackChanges());
        m_checkBoxHideChangeMarkerMarginEnable->SetValue(false);
    }
    if(options->DisplayFoldMarginIsValid()) {
        m_checkBoxDisplayFoldMargin->SetValue(options->GetDisplayFoldMargin());
        m_checkBoxDisplayFoldMarginEnable->SetValue(false);
    }
    if(options->DisplayBookmarkMarginIsValid()) {
        m_displayBookmarkMargin->SetValue(options->GetDisplayBookmarkMargin());
        m_displayBookmarkMarginEnable->SetValue(false);
    }

    if(options->ShowWhitespacesIsValid()) {
        switch(options->GetShowWhitespaces()) {
        case wxSTC_WS_VISIBLEALWAYS:
            m_WSstringManager.SetStringSelection(wxT("Visible always"));
            break;
        case wxSTC_WS_VISIBLEAFTERINDENT:
            m_WSstringManager.SetStringSelection(wxT("Visible after indentation"));
            break;
        default:
            m_WSstringManager.SetStringSelection(wxT("Invisible"));
            break;
        }
        m_whitespaceStyleEnable->SetValue(false);
    }

    if(options->EolModeIsValid()) {
        m_EOLstringManager.SetStringSelection(options->GetEolMode());
        m_choiceEOLEnable->SetValue(false);
    }

    if(options->FileFontEncodingIsValid()) {
        wxArrayString entries = m_fileEncoding->GetStrings();
        for(size_t n = 0; n < entries.GetCount(); ++n) {
            if(entries.Item(n) == wxFontMapper::GetEncodingName(options->GetFileFontEncoding())) {
                m_fileEncoding->SetSelection(n);
                break;
            }
        }
        m_fileEncodingEnable->SetValue(false);
    }
}

void EditorSettingsLocal::OnOK(wxCommandEvent& event)
{
    // Kill the old LocalOptionsConfigPtr, which now holds stale data
    localOptions.Reset(new LocalOptionsConfig);

    // Assume that, for any items still disabled, the user wants to use the global setting
    // That's true even if he decided to change an item, then disabled it again
    // or if there *was* a local pref, which is now to be deleted
    if(m_indentsUsesTabs->IsEnabled()) {
        GetLocalOpts()->SetIndentUsesTabs(m_indentsUsesTabs->GetValue());
    }
    if(m_indentWidth->IsEnabled()) {
        GetLocalOpts()->SetIndentWidth(m_indentWidth->GetValue());
    }
    if(m_tabWidth->IsEnabled()) {
        GetLocalOpts()->SetTabWidth(m_tabWidth->GetValue());
    }
    if(m_displayLineNumbers->IsEnabled()) {
        GetLocalOpts()->SetDisplayLineNumbers(m_displayLineNumbers->GetValue());
    }
    if(m_showIndentationGuideLines->IsEnabled()) {
        GetLocalOpts()->SetShowIndentationGuidelines(m_showIndentationGuideLines->GetValue());
    }
    if(m_highlightCaretLine->IsEnabled()) {
        GetLocalOpts()->SetHighlightCaretLine(m_highlightCaretLine->GetValue());
    }
    if(m_checkBoxTrimLine->IsEnabled()) {
        GetLocalOpts()->SetTrimLine(m_checkBoxTrimLine->GetValue());
    }
    if(m_checkBoxAppendLF->IsEnabled()) {
        GetLocalOpts()->SetAppendLF(m_checkBoxAppendLF->GetValue());
    }
    if(m_checkBoxTrackEditorChanges->IsEnabled()) {
        GetLocalOpts()->SetTrackChanges(m_checkBoxTrackEditorChanges->GetValue());
    }
    if(m_checkBoxDisplayFoldMargin->IsEnabled()) {
        GetLocalOpts()->SetDisplayFoldMargin(m_checkBoxDisplayFoldMargin->GetValue());
    }
    if(m_displayBookmarkMargin->IsEnabled()) {
        GetLocalOpts()->SetDisplayBookmarkMargin(m_displayBookmarkMargin->GetValue());
    }
    if(m_whitespaceStyle->IsEnabled()) {
        wxString Whitespace = m_WSstringManager.GetStringSelection();
        int style(wxSTC_WS_INVISIBLE); // invisible
        if(Whitespace == wxT("Visible always")) {
            style = wxSTC_WS_VISIBLEALWAYS;
        } else if(Whitespace == wxT("Visible after indentation")) {
            style = wxSTC_WS_VISIBLEAFTERINDENT;
        } else if(Whitespace == wxT("Indentation only")) {
            style = wxSTC_WS_VISIBLEAFTERINDENT;
        }
        GetLocalOpts()->SetShowWhitespaces(style);
    }
    if(m_choiceEOL->IsEnabled()) {
        GetLocalOpts()->SetEolMode(m_EOLstringManager.GetStringSelection());
    }
    if(m_fileEncoding->IsEnabled()) {
        GetLocalOpts()->SetFileFontEncoding(m_fileEncoding->GetStringSelection());
    }

    EndModal(wxID_OK);
}

void EditorSettingsLocal::indentsUsesTabsUpdateUI(wxUpdateUIEvent& event)
{
    m_indentsUsesTabs->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::indentWidthUpdateUI(wxUpdateUIEvent& event)
{
    m_indentWidth->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    m_staticTextindentWidth->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::tabWidthUpdateUI(wxUpdateUIEvent& event)
{
    m_tabWidth->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    m_staticTexttabWidth->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::displayBookmarkMarginUpdateUI(wxUpdateUIEvent& event)
{
    m_displayBookmarkMargin->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::checkBoxDisplayFoldMarginUpdateUI(wxUpdateUIEvent& event)
{
    m_checkBoxDisplayFoldMargin->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::checkBoxHideChangeMarkerMarginUpdateUI(wxUpdateUIEvent& event)
{
    m_checkBoxTrackEditorChanges->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::displayLineNumbersUpdateUI(wxUpdateUIEvent& event)
{
    m_displayLineNumbers->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::showIndentationGuideLinesUpdateUI(wxUpdateUIEvent& event)
{
    m_showIndentationGuideLines->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::highlightCaretLineUpdateUI(wxUpdateUIEvent& event)
{
    m_highlightCaretLine->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::checkBoxTrimLineUpdateUI(wxUpdateUIEvent& event)
{
    m_checkBoxTrimLine->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::checkBoxAppendLFUpdateUI(wxUpdateUIEvent& event)
{
    m_checkBoxAppendLF->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::whitespaceStyleUpdateUI(wxUpdateUIEvent& event)
{
    m_whitespaceStyle->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    m_staticTextwhitespaceStyle->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::choiceEOLUpdateUI(wxUpdateUIEvent& event)
{
    m_choiceEOL->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    m_EOLstatic->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}

void EditorSettingsLocal::fileEncodingUpdateUI(wxUpdateUIEvent& event)
{
    m_fileEncoding->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    m_staticTextfileEncoding->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
}
