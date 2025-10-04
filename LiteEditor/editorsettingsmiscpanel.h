//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsmiscpanel.h
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

#ifndef __EDITORSETTINGSMISCPANEL__
#define __EDITORSETTINGSMISCPANEL__

#include "OptionsConfigPage.hpp"

#include <wx/arrstr.h>

class EditorSettingsMiscPanel : public OptionsConfigPage
{
private:
    int FindAvailableLocales(wxArrayString* locales);

public:
    EditorSettingsMiscPanel(wxWindow* parent, OptionsConfigPtr options);
    ~EditorSettingsMiscPanel() override = default;
};

#endif // __EDITORSETTINGSMISCPANEL__
