//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : editorsettingscaret.cpp
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

#include "editorsettingscaret.h"
#include "editor_config.h"
#include "globals.h"

EditorSettingsCaret::EditorSettingsCaret(wxWindow* parent)
    : EditorSettingsCaretBase(parent)
    , TreeBookNode<EditorSettingsCaret>()
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_spinCtrlBlinkPeriod->SetValue(::wxIntToString(options->GetCaretBlinkPeriod()));
    m_spinCtrlCaretWidth->SetValue(::wxIntToString(options->GetCaretWidth()));
    m_checkBoxCaretUseCamelCase->SetValue(options->GetCaretUseCamelCase());
    m_checkBoxScrollBeyondLastLine->SetValue(options->GetScrollBeyondLastLine());
    m_checkBoxAdjustScrollbarSize->SetValue(options->GetAutoAdjustHScrollBarWidth());
    m_checkBoxCaretOnVirtualSpace->SetValue(options->GetOptions() & OptionsConfig::Opt_AllowCaretAfterEndOfLine);
    m_checkBoxBlockCaret->SetValue(options->GetOptions() & OptionsConfig::Opt_UseBlockCaret);
}

void EditorSettingsCaret::Save(OptionsConfigPtr options)
{
    options->SetCaretBlinkPeriod(::wxStringToInt(m_spinCtrlBlinkPeriod->GetValue(), 500, 0));
    options->SetCaretWidth(::wxStringToInt(m_spinCtrlCaretWidth->GetValue(), 1, 1, 10));
    options->SetCaretUseCamelCase(m_checkBoxCaretUseCamelCase->IsChecked());
    options->SetScrollBeyondLastLine(m_checkBoxScrollBeyondLastLine->IsChecked());
    options->SetAutoAdjustHScrollBarWidth(m_checkBoxAdjustScrollbarSize->IsChecked());

    size_t flags = options->GetOptions();
    flags &= ~OptionsConfig::Opt_AllowCaretAfterEndOfLine;

    if(m_checkBoxCaretOnVirtualSpace->IsChecked()) {
        flags |= OptionsConfig::Opt_AllowCaretAfterEndOfLine;
    }
    if(m_checkBoxBlockCaret->IsChecked()) {
        flags |= OptionsConfig::Opt_UseBlockCaret;
    } else {
        flags &= ~OptionsConfig::Opt_UseBlockCaret;
    }
    options->SetOptions(flags);
}

void EditorSettingsCaret::OnCaretWidthUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_checkBoxBlockCaret->IsChecked());
}
