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
    m_checkBoxDontTrimCurrentLine->SetValue(dontTrimCaretLine ? true : false);
    m_checkBoxTrimModifiedLines->SetValue(EditorConfigST::Get()->GetOptions()->GetTrimOnlyModifiedLines());
}

void EditorOptionsGeneralSavePanel::Save(OptionsConfigPtr options)
{
    EditorConfigST::Get()->SetInteger(wxT("EditorTrimEmptyLines"), m_checkBoxTrimLine->IsChecked() ? 1 : 0);
    EditorConfigST::Get()->SetInteger(wxT("EditorAppendLf"), m_checkBoxAppendLF->IsChecked() ? 1 : 0);
    EditorConfigST::Get()->SetInteger(wxT("DontTrimCaretLine"), m_checkBoxDontTrimCurrentLine->IsChecked() ? 1 : 0);
    options->SetTrimOnlyModifiedLines(m_checkBoxTrimModifiedLines->IsChecked());
}

void EditorOptionsGeneralSavePanel::OnTrimCaretLineUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxTrimLine->IsChecked());
}
