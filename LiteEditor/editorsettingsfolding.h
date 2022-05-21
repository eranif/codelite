//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsfolding.h
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

#ifndef __EDITORSETTINGSFOLDING__
#define __EDITORSETTINGSFOLDING__

#include "OptionsConfigPage.hpp"

class EditorSettingsFolding : public OptionsConfigPage
{
public:
    EditorSettingsFolding(wxWindow* parent, OptionsConfigPtr options);
    virtual ~EditorSettingsFolding() {}
};

#endif // __EDITORSETTINGSFOLDING__
