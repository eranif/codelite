//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : editorsettingsdockingwidows.h
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

#ifndef __EDITORSETTINGSDOCKINGWIDOWS__
#define __EDITORSETTINGSDOCKINGWIDOWS__

#include "OptionsConfigPage.hpp"

class EditorSettingsDockingWindows : public OptionsConfigPage
{
public:
    EditorSettingsDockingWindows(wxWindow* parent, OptionsConfigPtr options);
    ~EditorSettingsDockingWindows() override = default;
};

#endif // __EDITORSETTINGSDOCKINGWIDOWS__
