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
#include "clStrings.h"
#include "clSystemSettings.h"
#include "cl_editor.h"
#include "ctags_manager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "findresultstab.h"
#include "frame.h"
#include "macros.h"
#include "plugin.h"

#include <map>
#include <wx/ffile.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/textbuf.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

struct FindUsageItemData : public wxTreeItemData {
    const LSP::Location* location = nullptr;
    FindUsageItemData(const LSP::Location* loc)
        : location(loc)
    {
    }
};

FindUsageTab::FindUsageTab(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new clThemedTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT | wxTR_ROW_LINES);
    m_ctrl->SetRendererType(eRendererType::RENDERER_DIRECT2D);
    m_ctrl->SetTreeStyle(wxTR_HIDE_ROOT | wxTR_ROW_LINES);
    m_ctrl->AddRoot("Hidden Root");

    m_ctrl->Bind(wxEVT_TREE_ITEM_ACTIVATED, &FindUsageTab::OnItemActivated, this);
    m_ctrl->Bind(wxEVT_TREE_ITEM_EXPANDING, &FindUsageTab::OnItemExpanding, this);
    m_ctrl->Bind(wxEVT_TREE_SEL_CHANGED, &FindUsageTab::OnItemActivated, this);

    // show context menu for the control
    m_ctrl->Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event) {
        wxMenu menu;
        menu.Append(wxID_CLEAR);
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& event) {
                wxUnusedVar(event);
                Clear();
            },
            wxID_CLEAR);
        m_ctrl->PopupMenu(&menu);
    });

    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Layout();
    UpdateStyle();

    // bind events
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_REFERENCES, &FindUsageTab::OnReferences, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_REFERENCES_INPROGRESS, &FindUsageTab::OnReferencesInProgress, this);
}

FindUsageTab::~FindUsageTab()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_REFERENCES, &FindUsageTab::OnReferences, this);

    EventNotifier::Get()->Unbind(wxEVT_LSP_REFERENCES_INPROGRESS, &FindUsageTab::OnReferencesInProgress, this);

    m_ctrl->Unbind(wxEVT_TREE_SEL_CHANGED, &FindUsageTab::OnItemActivated, this);
    m_ctrl->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &FindUsageTab::OnItemActivated, this);
    m_ctrl->Unbind(wxEVT_TREE_ITEM_EXPANDING, &FindUsageTab::OnItemExpanding, this);
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

void FindUsageTab::OnThemeChanged(clCommandEvent& e)
{
    e.Skip();
    UpdateStyle();
}

void FindUsageTab::UpdateStyle()
{
    // construct colours based on the current lexer

    auto lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    m_ctrl->SetDefaultFont(lexer->GetFontForStyle(0, this));

    clColours lexer_colours;
    lexer_colours.FromLexer(lexer);
    m_ctrl->SetColours(lexer_colours);

    m_headerColour = lexer->GetProperty(wxSTC_C_GLOBALCLASS).GetFgColour();

    wxColour match_colour_fg = lexer->GetProperty(wxSTC_C_WORD2).GetFgColour();
    auto& colours = m_ctrl->GetColours();
    colours.SetMatchedItemBgText(wxNullColour);
    colours.SetMatchedItemText(match_colour_fg);
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
    for(const LSP::Location* location : matches) {
        m_ctrl->AppendItem(child, "dummy_child", wxNOT_FOUND, wxNOT_FOUND, new FindUsageItemData(location));
    }
}

void FindUsageTab::OnReferences(LSPEvent& event)
{
    event.Skip();
    InitialiseView(event.GetLocations());

    // ensure that this view is shown
    clGetManager()->ShowOutputPane(SHOW_USAGE);
}

void FindUsageTab::OnReferencesInProgress(LSPEvent& event)
{
    event.Skip();
    Clear();
}

void FindUsageTab::OnItemActivated(wxTreeEvent& event)
{
    event.Skip();
    CHECK_ITEM_RET(event.GetItem());

    FindUsageItemData* item_data = static_cast<FindUsageItemData*>(m_ctrl->GetItemData(event.GetItem()));
    if(!item_data) {
        // header entry
        DoExpandItem(event.GetItem());
        if(!m_ctrl->IsExpanded(event.GetItem())) {
            m_ctrl->Expand(event.GetItem());
        }
        return;
    }

    // Open the file
    wxFileName fn(item_data->location->GetPath());

    // prepare the "after-file-is-loaded" callback
    auto range = item_data->location->GetRange();
    auto callback = [=](IEditor* editor) {
        editor->GetCtrl()->ClearSelections();
        editor->SelectRange(range);
    };

    if(fn.FileExists()) {
        clGetManager()->OpenFileAndAsyncExecute(fn.GetFullPath(), std::move(callback));

    } else {
        // the file does not exist
        clCommandEvent open_file_event{ wxEVT_OPEN_FILE };
        open_file_event.SetFileName(item_data->location->GetPath());
        if(!EventNotifier::Get()->ProcessEvent(open_file_event)) {
            ::wxMessageBox(_("Failed to open file: ") + item_data->location->GetPath(), "CodeLite",
                           wxOK | wxOK_DEFAULT | wxICON_ERROR);
            event.Veto();
            return;
        }
        clGetManager()->OpenFileAndAsyncExecute(open_file_event.GetFileName(), std::move(callback));
    }
}

void FindUsageTab::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();

    wxBusyCursor bc;
    if(!DoExpandItem(event.GetItem())) {
        event.Veto();
    }
}

bool FindUsageTab::DoExpandItem(const wxTreeItemId& item)
{
    wxBusyCursor bc;
    CHECK_ITEM_RET_FALSE(item);

    wxTreeItemIdValue cookie;
    wxTreeItemId first_child = m_ctrl->GetFirstChild(item, cookie);
    if(!first_child) {
        return true;
    }

    // if the child text is not "dummy_child" -> return
    if(m_ctrl->GetItemText(first_child) != "dummy_child") {
        return true;
    }

    wxString content;
    wxString filepath = m_ctrl->GetItemText(item);
    if(!wxFileName::Exists(filepath)) {
        // not a local file, request for file download
        clCommandEvent event_download{ wxEVT_DOWNLOAD_FILE };
        event_download.SetFileName(filepath);
        clDEBUG() << "Sending event wxEVT_DOWNLOAD_FILE" << endl;
        if(!EventNotifier::Get()->ProcessEvent(event_download)) {
            ::wxMessageBox(_("Failed to download file: ") + filepath, "CodeLite", wxOK | wxOK_DEFAULT | wxICON_ERROR);
            return false;
        }
        filepath = event_download.GetFileName();
    }

    // load the file into wxTextBuffer
    wxTextFile text_buffer(filepath);
    if(!text_buffer.Open()) {
        ::wxMessageBox(_("Failed to open file: ") + filepath, "CodeLite", wxOK | wxOK_DEFAULT | wxICON_ERROR);
        return false;
    }

    // go over the entries and set the actual line
    wxTreeItemIdValue cookie2;
    wxTreeItemId child = m_ctrl->GetFirstChild(item, cookie2);
    while(child.IsOk()) {

        FindUsageItemData* item_data = static_cast<FindUsageItemData*>(m_ctrl->GetItemData(child));
        // incase this was a remote file, update the filepath
        const_cast<LSP::Location*>(item_data->location)->SetPath(filepath);

        size_t line_number = item_data->location->GetRange().GetStart().GetLine();
        if(line_number < text_buffer.GetLineCount()) {
            // update the text
            m_ctrl->SetItemText(child, text_buffer.GetLine(line_number));

            // update the highlight information
            const auto& start = item_data->location->GetRange().GetStart();
            const auto& end = item_data->location->GetRange().GetEnd();

            if(start.GetLine() == end.GetLine()) {
                m_ctrl->SetItemHighlightInfo(child, start.GetCharacter(), end.GetCharacter() - start.GetCharacter());
                m_ctrl->HighlightText(child, true);
            }
        }
        child = m_ctrl->GetNextChild(item, cookie2);
    }
    return true;
}
