//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : findusagetab.cpp
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

#include "findusagetab.h"

#include "ColoursAndFontsManager.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "cl_editor.h"
#include "ctags_manager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "findresultstab.h"
#include "frame.h"
#include "plugin.h"

#include <map>
#include <wx/ffile.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

FindUsageTab::FindUsageTab(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new clThemedTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT);
    m_ctrl->AddRoot("Hidden Root");
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Layout();

    // bind events
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_REFERENCES, &FindUsageTab::OnReferences, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_REFERENCES_INPROGRESS, &FindUsageTab::OnReferencesInProgress, this);
}

FindUsageTab::~FindUsageTab()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_REFERENCES, &FindUsageTab::OnReferences, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_REFERENCES_INPROGRESS, &FindUsageTab::OnReferencesInProgress, this);
}

void FindUsageTab::Clear()
{
    m_locations.clear();
    m_ctrl->DeleteAllItems();
    m_ctrl->AddRoot("Hidden Root");
}

void FindUsageTab::OnClearAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    Clear();
}

void FindUsageTab::OnClearAllUI(wxUpdateUIEvent& e)
{
    wxTreeItemId root = m_ctrl->GetRootItem();
    e.Enable(root.IsOk() && m_ctrl->GetChildrenCount(root, false) != 0);
}

void FindUsageTab::InitialiseView(const std::vector<LSP::Location>& locations)
{
    Clear();
    UpdateStyle();

    m_locations = locations;
    // sort by file / location
    std::map<wxString, std::vector<const LSP::Location*>> sorted_entries;
    for(const LSP::Location& location : m_locations) {
        if(sorted_entries.count(location.GetPath()) == 0) {
            sorted_entries.insert({ location.GetPath(), {} });
        }
        std::vector<const LSP::Location*>& file_matches = sorted_entries[location.GetPath()];
        file_matches.push_back(&location);
    }

    // add the entries to the view
    for(const auto& vt : sorted_entries) {
        const wxString& filename = vt.first;
        const std::vector<const LSP::Location*>& locations_vec = vt.second;
        DoAddFileEntries(filename, locations_vec);
    }
    // expand the root, to show the file name
    m_ctrl->Expand(m_ctrl->GetRootItem());
}

void FindUsageTab::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    UpdateStyle();
}

void FindUsageTab::UpdateStyle()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    m_ctrl->SetDefaultFont(lexer->GetFontForSyle(0, this));

    m_headerColour = lexer->GetProperty(wxSTC_C_GLOBALCLASS).GetFgColour();
    m_ctrl->Refresh();
}

void FindUsageTab::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
}

void FindUsageTab::DoAddFileEntries(const wxString& filename, const std::vector<const LSP::Location*>& matches)
{
    wxTreeItemId child = m_ctrl->AppendItem(m_ctrl->GetRootItem(), filename);
    if(m_headerColour.IsOk()) {
        m_ctrl->SetItemTextColour(child, m_headerColour);
    }
    // append dummy child
    m_ctrl->AppendItem(child, "dummy_child");
}

void FindUsageTab::OnReferences(LSPEvent& event)
{
    event.Skip();
    InitialiseView(event.GetLocations());
}

void FindUsageTab::OnReferencesInProgress(LSPEvent& event)
{
    event.Skip();
    Clear();
}
