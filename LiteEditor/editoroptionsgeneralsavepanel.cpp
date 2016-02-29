//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editoroptionsgeneralsavepanel.cpp
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

#include "editoroptionsgeneralsavepanel.h"

EditorOptionsGeneralSavePanel::EditorOptionsGeneralSavePanel(wxWindow* parent)
    : EditorOptionsGeneralSavePanelBase(parent)
    , TreeBookNode<EditorOptionsGeneralSavePanel>()
{
    long trim = EditorConfigST::Get()->GetInteger(wxT("EditorTrimEmptyLines"), 0);
    m_checkBoxTrimLine->SetValue(trim ? true : false);

    long appendLf = EditorConfigST::Get()->GetInteger(wxT("EditorAppendLf"), 0);
    m_checkBoxAppendLF->SetValue(appendLf ? true : false);

    long dontTrimCaretLine = EditorConfigST::Get()->GetInteger(wxT("DontTrimCaretLine"), 0);
    m_checkBoxDontTrimCurrentLine->SetValue((trim && dontTrimCaretLine) ? true : false);
    m_checkBoxTrimModifiedLines->SetValue(trim && EditorConfigST::Get()->GetOptions()->GetTrimOnlyModifiedLines());
}

void EditorOptionsGeneralSavePanel::Save(OptionsConfigPtr options)
{
    bool enableTrim = m_checkBoxTrimLine->IsChecked();
    EditorConfigST::Get()->SetInteger("EditorTrimEmptyLines", enableTrim ? 1 : 0);
    EditorConfigST::Get()->SetInteger("EditorAppendLf", m_checkBoxAppendLF->IsChecked() ? 1 : 0);
    EditorConfigST::Get()->SetInteger(
        "DontTrimCaretLine", (enableTrim && m_checkBoxDontTrimCurrentLine->IsChecked()) ? 1 : 0);
    options->SetTrimOnlyModifiedLines(enableTrim && m_checkBoxTrimModifiedLines->IsChecked());
}

void EditorOptionsGeneralSavePanel::OnTrimCaretLineUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxTrimLine->IsChecked());
    if(!m_checkBoxTrimLine->IsChecked()) {
        event.Check(false);
    }
}
