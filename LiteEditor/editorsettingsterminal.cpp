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

#include "clConsoleBase.h"
#include "cl_config.h"
#include "editor_config.h"
#include "editorsettingsterminal.h"

EditorSettingsTerminal::EditorSettingsTerminal(wxWindow* parent)
    : EditorSettingsTerminalBase(parent)
    , TreeBookNode<EditorSettingsTerminal>()
{
    wxArrayString terminals = clConsoleBase::GetAvailaleTerminals();
    m_choiceTerminals->Append(terminals);
    wxString selection = clConsoleBase::GetSelectedTerminalName();
    if(!selection.IsEmpty()) { m_choiceTerminals->SetStringSelection(selection); }
}

void EditorSettingsTerminal::Save(OptionsConfigPtr options)
{
    wxUnusedVar(options);
    clConfig::Get().Write("Terminal", m_choiceTerminals->GetStringSelection());
}
