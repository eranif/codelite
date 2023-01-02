//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
#include "cl_command_event.h"
#include "plugin.h"
#include "spellcheckeroptions.h"
#include <wx/timer.h>
//------------------------------------------------------------
class IHunSpell;
class SpellCheck : public IPlugin
{
public:
    wxString GetCurrentWspPath() const { return m_currentWspPath; }

    void SetCheckContinuous(bool value);
    bool GetCheckContinuous() const { return m_options.GetCheckContinuous(); }
    IEditor* GetEditor();

    SpellCheck(IManager* manager);
    ~SpellCheck();

    // --------------------------------------------
    // Abstract methods
    // --------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar) override;
    virtual void CreatePluginMenu(wxMenu* pluginsMenu) override;
    virtual void UnPlug() override;

    void OnSettings(wxCommandEvent& e);
    void OnCheck(wxCommandEvent& e);
    void OnContinousCheck(wxCommandEvent& e);
    void OnTimer(wxTimerEvent& e);
    void OnWspLoaded(clWorkspaceEvent& e);
    void OnWspClosed(clWorkspaceEvent& e);
    void OnSuggestion(wxCommandEvent& e);
    void OnIgnoreWord(wxCommandEvent& e);
    void OnAddWord(wxCommandEvent& e);

    wxMenuItem* m_sepItem;
    wxEvtHandler* m_topWin;
    SpellCheckerOptions m_options;
    wxWindow* GetTopWnd() { return m_mgr->GetTheApp()->GetTopWindow(); }

protected:
    void Init();
    void LoadSettings();
    void SaveSettings();
    void ClearIndicatorsFromEditors();
    void OnContextMenu(clContextMenuEvent& e);
    void AppendSubMenuItems(wxMenu& subMenu);

protected:
    IHunSpell* m_pEngine;
    wxTimer m_timer;
    wxString m_currentWspPath;

    IEditor* m_pLastEditor;           // The editor checked last time the spell check ran.
    wxUint64 m_lastModificationCount; // Modification count of the editor last time the spell check ran.
};
//------------------------------------------------------------
#endif // SpellCheck
