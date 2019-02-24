//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : theme_handler.h
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

#ifndef THEMEHANDLER_H
#define THEMEHANDLER_H

#include <wx/event.h> // Base class: wxEvtHandler
#include <wx/stc/stc.h>
#include "plugin.h"
#include "theme_handler_helper.h"
#include "cl_command_event.h"

class ThemeHandler : public wxEvtHandler
{
    ThemeHandlerHelper::Ptr m_helper;

protected:
    void UpdateColours();

public:
    ThemeHandler();
    virtual ~ThemeHandler();
    
    void UpdateNotebookColours(wxWindow* parent);
    void OnEditorThemeChanged(wxCommandEvent& e);
    void OnColoursChanged(clCommandEvent& e);
    void OnInitDone(wxCommandEvent& e);
};

#endif // THEMEHANDLER_H
