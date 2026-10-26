//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : findusagetab.h
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

#ifndef FINDUSAGETAB_H
#define FINDUSAGETAB_H

#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "clWorkspaceEvent.hpp"

#include <vector>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/panel.h>

class FindUsageRenderer;
class FindUsageStore;

class FindUsageTab : public wxPanel
{
    std::vector<LSP::Location> m_locations;
    wxAuiToolBar* m_toolbar = nullptr;
    wxDataViewCtrl* m_ctrl = nullptr;
    FindUsageStore* m_store = nullptr;       // the model of m_ctrl, owned by it
    FindUsageRenderer* m_renderer = nullptr; // owned by the column of m_ctrl

private:
    void DoAddFileEntries(const wxString& filename, const std::vector<const LSP::Location*>& matches);
    void InitialiseView(const std::vector<LSP::Location>& locations);
    void UpdateStyle();
    /// (Re)create the buttons. Called again when the bitmaps change, to load the icons of the new theme
    void BuildToolbar();
    bool DoExpandItem(const wxDataViewItem& item);

public:
    FindUsageTab(wxWindow* parent);
    virtual ~FindUsageTab();

protected:
    void Clear();
    void OnClearAllUI(wxUpdateUIEvent& e);
    void OnClearAll(wxCommandEvent& e);
    void OnThemeChanged(clCommandEvent& e);
    void OnBitmapsUpdated(clCommandEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnReferences(LSPEvent& event);
    void OnReferencesInProgress(LSPEvent& event);
    void OnItemActivated(wxDataViewEvent& event);
    void OnItemExpanding(wxDataViewEvent& event);
};

#endif // FINDUSAGETAB_H
