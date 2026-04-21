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

#include <unordered_set>
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
public:
    PerspectiveManager();
    virtual ~PerspectiveManager();

    void FlushCacheToDisk();
    void ConnectEvents(wxAuiManager* mgr);
    void DisconnectEvents();

    /**
     * @brief Shows or hides the Output View pane and optionally selects a tab within it.
     *
     * If the pane is being shown, this method searches the Output View notebook for the
     * tab named by "tab" and selects it when found. When hiding the pane, it restores
     * focus to the active editor if possible. The method may also move keyboard focus
     * to the selected notebook page depending on "take_focus".
     *
     * @param tab wxString The label of the Output View tab to select when showing the pane.
     * @param show bool True to show the Output View pane, false to hide it.
     * @param take_focus bool True to give focus to the selected tab after switching; false to preserve the previous
     * focus when possible.
     *
     * @return void
     */
    void ShowOutputPane(const wxString& tab, bool show, bool take_focus);
    /**
     * Show or hide a sidebar pane and optionally select a tab within it.
     *
     * If the pane is shown and a tab name is provided, the matching notebook page
     * is selected in either the left or right sidebar. When hiding a sidebar, the
     * function ensures focus is moved back to the active editor so keyboard focus
     * is not left on a hidden control.
     *
     * @param left_sidebar bool True to operate on the left sidebar pane; false for the right sidebar pane.
     * @param tab const wxString& The tab title to select when showing the sidebar. Ignored if empty.
     * @param show bool True to show the sidebar pane; false to hide it.
     * @param take_focus bool True to move focus to the selected sidebar page after changing tabs.
     *
     * @return void This function does not return a value.
     */
    void ShowSideBar(bool left_sidebar, const wxString& tab, bool show, bool take_focus);
    bool ShowPane(const wxString& pane, bool show);
    void SetActive(const wxString& active) { this->m_active = active; }
    const wxString& GetActive() const { return m_active; }

    void LoadPerspective(const wxString& name = wxT(""));
    void SavePerspective(const wxString& name = wxT(""), bool notify = true);
    void SavePerspectiveIfNotExists(const wxString& name);
    void DeleteAllPerspectives();
    bool IsPaneVisible(const wxString& name) const;

protected:
    /**
     * @brief read the perspective from cache first, and then try to load it from the disk
     * if it does not exist
     */
    bool GetPerspective(const wxString& name, wxString* perspective) const;
    void SetPerspectiveToCache(const wxString& name, const wxString& content);
    bool ShowPane(wxAuiPaneInfo& pane, bool show);

    wxString DoGetPathFromName(const wxString& name) const;
    void DoEnsureDebuggerPanesAreVisible();

    // Event handlers
    void OnPaneClosing(wxAuiManagerEvent& event);
    void OnAuiRender(wxAuiManagerEvent& event);

    void DoShowPane(wxAuiPaneInfo& pane_info);
    void DoHidePane(wxAuiPaneInfo& pane_info);

private:
    struct SavedPaneInfo {
        wxSize min_size;
        wxString perspective;
    };
    std::unordered_map<wxString, int> m_menuIdToName;
    std::unordered_map<wxString, wxString> m_perspectives;
    std::unordered_map<wxString, SavedPaneInfo> m_hiddenPanes;
    bool m_showPaneInProgress = false;
    int m_nextId;
    wxString m_active;
    wxString m_buildPerspective;
    wxAuiManager* m_aui = nullptr;
};

#endif // PERSPECTIVEMANAGER_H
