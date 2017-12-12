//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clDockingManager.h
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

#ifndef CLDOCKINGMANAGER_H
#define CLDOCKINGMANAGER_H

#include "codelite_exports.h"
#include <wx/aui/framemanager.h>
#include "Notebook.h"

class WXDLLIMPEXP_SDK clDockingManager : public wxAuiManager
{
protected:
    void OnRender(wxAuiManagerEvent& event);
    void OnButtonClicked(wxAuiManagerEvent& event);
    
    void ShowWorkspaceOpenTabMenu();
    void ShowOutputViewOpenTabMenu();
    
    /**
     * @brief show a popup menu and return the selected string
     * return an empty string if no selection was made
     */
    wxString ShowMenu(wxWindow* win, const wxArrayString& tabs, Notebook* book, bool& checked);

public:
    clDockingManager();
    virtual ~clDockingManager();
};

#endif // CLDOCKINGMANAGER_H
