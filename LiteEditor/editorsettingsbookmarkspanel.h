//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsbookmarkspanel.h
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

#ifndef __EDITORSETTHINGSBOOKMARKSPANEL__
#define __EDITORSETTHINGSBOOKMARKSPANEL__

#include "OptionsConfigPage.hpp"

class EditorSettingsBookmarksPanel : public OptionsConfigPage
{
public:
    EditorSettingsBookmarksPanel(wxWindow* parent, OptionsConfigPtr options);
    virtual ~EditorSettingsBookmarksPanel() {}
};

#endif // __EDITORSETTHINGSBOOKMARKSPANEL__
