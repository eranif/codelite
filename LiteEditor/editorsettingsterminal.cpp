//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : editorsettingsterminal.cpp
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

#include "editorsettingsterminal.h"
#include "editor_config.h"

EditorSettingsTerminal::EditorSettingsTerminal(wxWindow* parent)
    : EditorSettingsTerminalBase(parent)
    , TreeBookNode<EditorSettingsTerminal>()
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_textCtrlProgramConsoleCmd->SetValue(options->GetProgramConsoleCommand());
    m_checkBoxUseCodeLiteTerminal->SetValue(options->HasOption(OptionsConfig::Opt_Use_CodeLite_Terminal));
}

void EditorSettingsTerminal::Save(OptionsConfigPtr options)
{
    options->SetProgramConsoleCommand(m_textCtrlProgramConsoleCmd->GetValue());
    options->EnableOption(OptionsConfig::Opt_Use_CodeLite_Terminal, m_checkBoxUseCodeLiteTerminal->IsChecked());
}

void EditorSettingsTerminal::OnUseCodeLiteTerminalUI(wxUpdateUIEvent& event)
{
#if defined(__WXMSW__) || defined(__WXOSX__)
    event.Enable(false);
#else
    event.Enable(!m_checkBoxUseCodeLiteTerminal->IsChecked());
#endif
}

void EditorSettingsTerminal::OnCheckboxmswwrapdoublequotesUpdateUi(wxUpdateUIEvent& event)
{
#ifdef __WXMSW__
    event.Enable(true);
#else
    event.Enable(false);
#endif
}
