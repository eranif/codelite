//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : spellcheck.h
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

/////////////////////////////////////////////////////////////////////////////
// Name:        spellcheck.h
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: spellcheck.h 35 2014-02-22 18:18:49Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
#ifndef __SpellCheck__
#define __SpellCheck__
//------------------------------------------------------------
#include "plugin.h"
#include "spellcheckeroptions.h"
#include <wx/timer.h>
#include "cl_command_event.h"
//------------------------------------------------------------
class IHunSpell;
class SpellCheck : public IPlugin
{
public:
    wxString GetCurrentWspPath() const { return m_currentWspPath; }

    void SetCheckContinuous(bool value);
    bool GetCheckContinuous() const { return m_checkContinuous; }
    bool IsTag(const wxString& token);
    IEditor* GetEditor();
    wxMenu* CreateSubMenu();

    SpellCheck(IManager* manager);
    ~SpellCheck();

    // --------------------------------------------
    // Abstract methods
    // --------------------------------------------
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    void OnSettings(wxCommandEvent& e);
    void OnCheck(wxCommandEvent& e);
    void OnContinousCheck(wxCommandEvent& e);
    void OnContextMenu(wxCommandEvent& e);
    void OnTimer(wxTimerEvent& e);
    void OnWspLoaded(wxCommandEvent& e);
    void OnWspClosed(wxCommandEvent& e);
    void OnEditorContextMenuShowing(clContextMenuEvent& e);
    void OnEditorContextMenuDismissed(clContextMenuEvent& e);

    wxMenuItem* m_sepItem;
    wxEvtHandler* m_topWin;
    SpellCheckerOptions m_options;
    wxWindow* GetTopWnd() { return m_mgr->GetTheApp()->GetTopWindow(); }
    enum { IDM_BASE = 20500, IDM_SETTINGS };

protected:
    void Init();
    void LoadSettings();
    void SaveSettings();
    void ClearIndicatorsFromEditors();

protected:
    bool m_checkContinuous;
    IHunSpell* m_pEngine;
    wxTimer m_timer;
    wxString m_currentWspPath;
    wxAuiToolBar* m_pToolbar;
};
//------------------------------------------------------------
#endif // SpellCheck
