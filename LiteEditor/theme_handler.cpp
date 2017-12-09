//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : theme_handler.cpp
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

#include "cl_aui_tb_are.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "frame.h"
#include "globals.h"
#include "theme_handler.h"
#include "workspace_pane.h"
#include "wxcl_log_text_ctrl.h"
#include <algorithm>
#include <wx/aui/framemanager.h>
#include <wx/image.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>

#ifdef __WXMAC__
#include <wx/srchctrl.h>
#endif

#define CHECK_POINTER(p) \
    if(!p) return;

ThemeHandler::ThemeHandler()
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ThemeHandler::OnEditorThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &ThemeHandler::OnInitDone, this);
}

ThemeHandler::~ThemeHandler()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &ThemeHandler::OnEditorThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &ThemeHandler::OnInitDone, this);
}

void ThemeHandler::OnEditorThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    wxclTextCtrl* log = dynamic_cast<wxclTextCtrl*>(wxLog::GetActiveTarget());
    if(log) { log->Reset(); }
}

void ThemeHandler::OnInitDone(wxCommandEvent& e)
{
    e.Skip();
    m_helper.reset(new ThemeHandlerHelper(clMainFrame::Get()));
    m_helper->UpdateColours(clMainFrame::Get());
    // Fire "wxEVT_EDITOR_SETTINGS_CHANGED" to ensure that the notebook appearance is in sync with the settings
    PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
}
