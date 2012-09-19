//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editor_options_general_guides_panel.cpp
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

#include "globals.h"
#include <wx/stc/stc.h>
#include "editor_options_general_guides_panel.h"

EditorOptionsGeneralGuidesPanel::EditorOptionsGeneralGuidesPanel( wxWindow* parent )
    : EditorOptionsGeneralGuidesPanelBase( parent )
    , TreeBookNode<EditorOptionsGeneralGuidesPanel>()
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();

    m_displayLineNumbers->SetValue(options->GetDisplayLineNumbers());
    m_checkBoxMatchBraces->SetValue(options->GetHighlightMatchedBraces());
    m_showIndentationGuideLines->SetValue(options->GetShowIndentationGuidelines());
    m_checkBoxAutoCompleteCurlyBraces->SetValue(options->GetAutoAddMatchedCurlyBraces());
    m_checkBoxAutoCompleteNormalBraces->SetValue(options->GetAutoAddMatchedNormalBraces());
    m_highlightCaretLine->SetValue(options->GetHighlightCaretLine());
    m_caretLineColourPicker->SetColour(options->GetCaretLineColour());
    const wxString EOLChoices[] = { wxTRANSLATE("Default"), wxT("Mac (CR)"), wxT("Windows (CRLF)"), wxT("Unix (LF)") };
    m_EOLstringManager.AddStrings(sizeof(EOLChoices)/sizeof(wxString), EOLChoices, options->GetEolMode(), m_choiceEOL);
    m_checkBoxHideChangeMarkerMargin->SetValue( options->GetHideChangeMarkerMargin() );
    m_checkBoxDisableSemicolonShift->SetValue( options->GetDisableSemicolonShift() );

    m_checkBoxDoubleQuotes->SetValue(options->GetAutoCompleteDoubleQuotes());

    const wxString WhitespaceStyle[] = { wxTRANSLATE("Invisible"), wxTRANSLATE("Visible always"), wxTRANSLATE("Visible after indentation") };
    wxString currentWhitespace;
    switch (options->GetShowWhitspaces()) {
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
    m_WSstringManager.AddStrings(sizeof(WhitespaceStyle)/sizeof(wxString), WhitespaceStyle, currentWhitespace, m_whitespaceStyle);

}


void EditorOptionsGeneralGuidesPanel::Save(OptionsConfigPtr options)
{
    options->SetDisplayLineNumbers( m_displayLineNumbers->IsChecked() );
    options->SetHighlightMatchedBraces(m_checkBoxMatchBraces->IsChecked());
    options->SetShowIndentationGuidelines( m_showIndentationGuideLines->IsChecked() );
    options->SetHighlightCaretLine( m_highlightCaretLine->IsChecked() );
    options->SetCaretLineColour(m_caretLineColourPicker->GetColour());
    options->SetAutoAddMatchedCurlyBraces(m_checkBoxAutoCompleteCurlyBraces->IsChecked());
    options->SetAutoAddMatchedNormalBraces(m_checkBoxAutoCompleteNormalBraces->IsChecked());
    options->SetEolMode(m_EOLstringManager.GetStringSelection());
    options->SetHideChangeMarkerMargin( m_checkBoxHideChangeMarkerMargin->IsChecked() );
    options->SetDisableSemicolonShift( m_checkBoxDisableSemicolonShift->IsChecked() );

    options->SetAutoCompleteDoubleQuotes(m_checkBoxDoubleQuotes->IsChecked());

    // save the whitespace visibility
    wxString Whitespace = m_WSstringManager.GetStringSelection();
    int style(wxSTC_WS_INVISIBLE); // invisible
    if (Whitespace == wxT("Visible always")) {
        style = wxSTC_WS_VISIBLEALWAYS;
    } else if (Whitespace == wxT("Visible after indentation")) {
        style = wxSTC_WS_VISIBLEAFTERINDENT;
    } else if (Whitespace == wxT("Indentation only")) {
        style = wxSTC_WS_VISIBLEAFTERINDENT;
    }
    options->SetShowWhitspaces(style);
}

void EditorOptionsGeneralGuidesPanel::OnhighlightCaretLineUI(wxUpdateUIEvent& event)
{
    event.Enable(m_highlightCaretLine->IsChecked());
}
