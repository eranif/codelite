//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : windowattrmanager.h
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

#ifndef __windowattrmanager__
#define __windowattrmanager__

#include "codelite_exports.h"
#include "iconfigtool.h"

#include <wx/toplevel.h>
#include <wx/window.h>

/**
 * @class WindowAttrManager
 * @author Eran
 * @date 09/30/08
 * @file windowattrmanager.h
 * @brief a helper class that helps restoring wxWindow size and position
 */
class WXDLLIMPEXP_SDK WindowAttrManager
{
protected:
    static void DoLoad(wxWindow* win, const wxString& parentName, int depth);

public:
    /**
     * @brief load windows' size and position from the disk
     * @param win window to work on
     * @param name the windows' name
     * @param conf conf configuration tool to use. If set to NULL the default EditorConfigST will be used
     */
    static void Load(wxTopLevelWindow* win);
};
#endif // __windowattrmanager__
