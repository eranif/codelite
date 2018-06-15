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

#include "plugin.h"
#include "zoomtext.h"
#include <wx/timer.h>
#include <set>
#include "cl_command_event.h"

extern const wxString ZOOM_PANE_TITLE;

class ZoomNavUpdateTimer;

class ZoomNavigator : public IPlugin
{
    IManager* mgr;
    wxPanel* zoompane;
    wxEvtHandler* m_topWindow;
    ZoomText* m_text;
    int m_markerFirstLine;
    int m_markerLastLine;
    bool m_enabled;
    clConfig* m_config;
    int m_lastLine;
    bool m_startupCompleted;
    wxString m_curfile;

protected:
    void DoInitialize();
    bool IsZoomPaneDetached();
    void PatchUpHighlights(const int first, const int last);
    void SetEditorText(IEditor* editor);
    void SetZoomTextScrollPosToMiddle(wxStyledTextCtrl* stc);
    void DoUpdate();
    void DoCleanup();

public:
    ZoomNavigator(IManager* manager);
    ~ZoomNavigator();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
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
    void OnToggleTab(clCommandEvent& event);
};

#endif // ZoomNavigator
