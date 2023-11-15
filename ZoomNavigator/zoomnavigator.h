//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : zoomnavigator.h
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

//	Copyright: 2013 Brandon Captain
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.

#ifndef __ZoomNavigator__
#define __ZoomNavigator__

#include "cl_command_event.h"
#include "plugin.h"
#include "zoomtext.h"

#include <set>
#include <wx/timer.h>

extern const wxString ZOOM_PANE_TITLE;

class ZoomNavUpdateTimer;

class ZoomNavigator : public IPlugin
{
    IManager* mgr;
    wxPanel* m_zoompane;
    wxEvtHandler* m_topWindow;
    ZoomText* m_text;
    int m_markerFirstLine = wxNOT_FOUND;
    int m_markerLastLine = wxNOT_FOUND;
    bool m_enabled;
    clConfig* m_config;
    int m_lastLine;
    bool m_startupCompleted;
    wxString m_curfile;
    wxTimer* m_timer = nullptr;

protected:
    void DoInitialize();
    void PatchUpHighlights(const int first, const int last);
    void SetEditorText(IEditor* editor);
    void SetZoomTextScrollPosToMiddle(wxStyledTextCtrl* stc);
    void DoUpdate();
    void DoCleanup();
    void OnTimer(wxTimerEvent& event);

public:
    ZoomNavigator(IManager* manager);
    ~ZoomNavigator();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    void OnIdle(wxIdleEvent& e);

    void OnShowHideClick(wxCommandEvent& e);
    void OnPreviewClicked(wxMouseEvent& e);
    void OnSettings(wxCommandEvent& e);
    void OnSettingsChanged(wxCommandEvent& e);
    void OnFileSaved(clCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnEnablePlugin(wxCommandEvent& e);
    void OnInitDone(wxCommandEvent& e);
};

#endif // ZoomNavigator
