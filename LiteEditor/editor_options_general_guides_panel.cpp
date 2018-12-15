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

EditorOptionsGeneralGuidesPanel::EditorOptionsGeneralGuidesPanel(wxWindow* parent)
    : EditorOptionsGeneralGuidesPanelBase(parent)
    , TreeBookNode<EditorOptionsGeneralGuidesPanel>()
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrGeneral->GetGrid());
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_pgPropDisplayLineNumbers->SetValue(options->GetDisplayLineNumbers());
    m_pgPropRelativeLineNumbers->SetValue(options->GetRelativeLineNumbers());
    m_pgPropHighlightMatchedBrace->SetValue(options->GetHighlightMatchedBraces());
    m_pgPropShowIndentGuidelines->SetValue(options->GetShowIndentationGuidelines());
    m_pgPropEnableCaretLine->SetValue(options->GetHighlightCaretLine());
    wxVariant caretLineColour, debuggerLineColour;
    caretLineColour << options->GetCaretLineColour();
    m_pgPropCaretLineColour->SetValue(caretLineColour);
    m_pgPropDisableSemiColonShift->SetValue(options->GetDisableSemicolonShift());
    m_pgPropHideEditMargin->SetValue(options->GetHideChangeMarkerMargin());
    m_pgPropHighlightDebuggerMarker->SetValue(options->HasOption(OptionsConfig::Opt_Mark_Debugger_Line));
    debuggerLineColour << options->GetDebuggerMarkerLine();
    m_pgPropDebuggerLineColour->SetValue(debuggerLineColour);
    m_pgPropWhitespaceVisibility->SetChoiceSelection(options->GetShowWhitspaces());
    m_pgPropCaretLineAlpha->SetValue(options->GetCaretLineAlpha());
    
    // EOL
    // Default;Mac (CR);Windows (CRLF);Unix (LF)
    wxArrayString eolOptions;
    eolOptions.Add("Default");
    eolOptions.Add("Mac (CR)");
    eolOptions.Add("Windows (CRLF)");
    eolOptions.Add("Unix (LF)");
    int eolSel = eolOptions.Index(options->GetEolMode());
    if(eolSel != wxNOT_FOUND) {
        m_pgPropEOLMode->SetChoiceSelection(eolSel);
    }
}

void EditorOptionsGeneralGuidesPanel::Save(OptionsConfigPtr options)
{
    options->SetDisplayLineNumbers(m_pgPropDisplayLineNumbers->GetValue().GetBool());
    options->SetRelativeLineNumbers(m_pgPropRelativeLineNumbers->GetValue().GetBool());
    options->SetHighlightMatchedBraces(m_pgPropHighlightMatchedBrace->GetValue().GetBool());
    options->SetShowIndentationGuidelines(m_pgPropShowIndentGuidelines->GetValue().GetBool());
    options->SetHighlightCaretLine(m_pgPropEnableCaretLine->GetValue().GetBool());
    
    wxColourPropertyValue carteLineColour, debuggerLineColour;
    carteLineColour << m_pgPropCaretLineColour->GetValue();
    debuggerLineColour << m_pgPropDebuggerLineColour->GetValue();
    options->SetCaretLineColour(carteLineColour.m_colour);
    
    wxString eolMode = m_pgPropEOLMode->GetValueAsString();
    options->SetEolMode(eolMode);
    options->SetHideChangeMarkerMargin(m_pgPropHideEditMargin->GetValue().GetBool());
    options->SetDisableSemicolonShift(m_pgPropDisableSemiColonShift->GetValue().GetBool());
    options->SetDebuggerMarkerLine(debuggerLineColour.m_colour);
    options->EnableOption(OptionsConfig::Opt_Mark_Debugger_Line, m_pgPropHighlightDebuggerMarker->GetValue().GetBool());
    options->SetShowWhitspaces(m_pgPropWhitespaceVisibility->GetValue().GetInteger());
    options->SetCaretLineAlpha(m_pgPropCaretLineAlpha->GetValue().GetInteger());
}
