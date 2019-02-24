//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : theme_handler_helper.h
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

#ifndef THEMEHANDLERHELPER_H
#define THEMEHANDLERHELPER_H

#include <wx/event.h>
#include <wx/window.h>
#include "codelite_exports.h"
#include <wx/sharedptr.h>
#include "cl_command_event.h"

class WXDLLIMPEXP_SDK ThemeHandlerHelper : public wxEvtHandler
{
    wxWindow* m_window;

public:
    typedef wxSharedPtr<ThemeHandlerHelper> Ptr;

private:
    void DoUpdateNotebookStyle(wxWindow* win);

public:
    void UpdateColours(wxWindow* topWindow);
    void UpdateNotebookColours(wxWindow* topWindow);

public:
    ThemeHandlerHelper(wxWindow* win);
    virtual ~ThemeHandlerHelper();
    void OnThemeChanged(wxCommandEvent& e);
    void OnPreferencesUpdated(wxCommandEvent& e);
    void OnColoursUpdated(clCommandEvent& e);
};

#endif // THEMEHANDLERHELPER_H
