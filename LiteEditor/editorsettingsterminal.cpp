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

#include "clConsoleBase.h"
#include "cl_config.h"
#include "editor_config.h"

#include <exelocator.h>

EditorSettingsTerminal::EditorSettingsTerminal(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    wxArrayString terminals = clConsoleBase::GetAvailaleTerminals();
#if defined(__WXGTK__)
    wxString where; // GetAvailableTerminals() doesn't, it gets a list of supported ones; so check for existence
    for(size_t t = terminals.GetCount(); t > 0; --t) {
        if(!ExeLocator::Locate(terminals.Item(t - 1), where)) {
            terminals.RemoveAt(t - 1);
        }
    }
#endif
    AddHeader(_("Terminals"));

    wxString selection = clConsoleBase::GetSelectedTerminalName();
    AddProperty(_("Choose your terminal"), terminals, selection, [&](const wxString& label, const wxAny& value) {
        wxString value_str;
        if(value.GetAs(&value_str)) {
            clConfig::Get().Write("Terminal", value_str);
        }
    });
}
