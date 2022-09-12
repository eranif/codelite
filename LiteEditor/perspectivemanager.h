//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : perspectivemanager.h
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

#ifndef PERSPECTIVEMANAGER_H
#define PERSPECTIVEMANAGER_H

#include "precompiled_header.h"
#include "wxStringHash.h"

#include <map>
#include <wx/arrstr.h>
#include <wx/aui/framemanager.h>
#include <wx/event.h>
#include <wx/string.h>

extern wxString DEBUG_LAYOUT;
extern wxString NORMAL_LAYOUT;

#define PERSPECTIVE_FIRST_MENU_ID 17000
#define PERSPECTIVE_LAST_MENU_ID 17020

class PerspectiveManager : public wxEvtHandler
{
protected:
    std::unordered_map<wxString, int> m_menuIdToName;
    std::unordered_map<wxString, wxString> m_perspectives;
    int m_nextId;
    wxString m_active;
    wxString m_buildPerspective;
    wxAuiManager* m_aui = nullptr;

protected:
    /**
     * @brief read the perspective from cache first, and then try to load it from the disk
     * if it does not exist
     */
    bool GetPerspective(const wxString& name, wxString* perspective) const;
    void SetPerspectiveToCache(const wxString& name, const wxString& content);

    wxString DoGetPathFromName(const wxString& name) const;
    void DoEnsureDebuggerPanesAreVisible();
    void DoShowPane(const wxString& panename, bool show, bool& needUpdate);
    // Event handlers
    void OnPaneClosing(wxAuiManagerEvent& event);
    void OnAuiRender(wxAuiManagerEvent& event);

public:
    PerspectiveManager();
    virtual ~PerspectiveManager();

    void FlushCacheToDisk();
    void ConnectEvents(wxAuiManager* mgr);
    void DisconnectEvents();

    void ToggleOutputPane(bool hide);
    void SetActive(const wxString& active) { this->m_active = active; }
    const wxString& GetActive() const { return m_active; }

    bool IsDefaultActive() const;
    void LoadPerspective(const wxString& name = wxT(""));
    void SavePerspective(const wxString& name = wxT(""), bool notify = true);
    void SavePerspectiveIfNotExists(const wxString& name);
    void DeleteAllPerspectives();
};

#endif // PERSPECTIVEMANAGER_H
