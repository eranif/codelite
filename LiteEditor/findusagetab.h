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
#include "clTerminalViewCtrl.hpp"
#include "clThemedTreeCtrl.h"
#include "clWorkspaceEvent.hpp"
#include "wxStringHash.h"

#include <wx/colour.h>
#include <wx/panel.h>

class FindUsageTab : public wxPanel
{
    std::vector<LSP::Location> m_locations;
    clThemedTreeCtrl* m_ctrl = nullptr;
    wxColour m_headerColour;

private:
    void DoAddFileEntries(const wxString& filename, const std::vector<const LSP::Location*>& matches);
    void InitialiseView(const std::vector<LSP::Location>& locations);
    void UpdateStyle();
    bool DoExpandItem(const wxTreeItemId& item);

public:
    FindUsageTab(wxWindow* parent);
    virtual ~FindUsageTab();

protected:
    void Clear();
    void OnClearAllUI(wxUpdateUIEvent& e);
    void OnClearAll(wxCommandEvent& e);
    void OnThemeChanged(clCommandEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnReferences(LSPEvent& event);
    void OnReferencesInProgress(LSPEvent& event);
    void OnItemActivated(wxTreeEvent& event);
    void OnItemExpanding(wxTreeEvent& event);
};

#endif // FINDUSAGETAB_H
