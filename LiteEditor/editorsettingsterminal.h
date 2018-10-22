//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : editorsettingsterminal.h
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

#ifndef __editorsettingsterminal__
#define __editorsettingsterminal__

#include "editorsettings_terminal_base.h"
#include "treebooknodebase.h"

/** Implementing EditorSettingsTerminalBase */
class EditorSettingsTerminal : public EditorSettingsTerminalBase, public TreeBookNode<EditorSettingsTerminal>
{
public:
    /** Constructor */
    EditorSettingsTerminal(wxWindow* parent);
    void Save(OptionsConfigPtr options);
};

#endif // __editorsettingsterminal__
