//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : WelcomePage.cpp
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

#include "WelcomePage.h"

#include "DataViewTypeHelper.h"
#include "UnicodeSymbols.hpp"
#include "bitmap_loader.h"
#include "clSystemSettings.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "fileextmanager.h"
#include "frame.h"
#include "imanager.h"
#include "macros.h"
#include "manager.h"

#include <algorithm>
#include <wx/arrstr.h>

wxDEFINE_EVENT(wxEVT_WELCOMEPAGE_CLOSE_BUTTON_CLICKED, clCommandEvent);

namespace
{

// The columns of the workspaces list
constexpr unsigned kColName = 0;
constexpr unsigned kColAccount = 1;
constexpr unsigned kColType = 2;
constexpr unsigned kColPath = 3;
const wxString kLocalHost = "localhost";

/// Symbol shown before the account name: a house for this machine, a network symbol for a remote one
wxString GetAccountSymbol(bool is_local) { return GetUnicodeSymbol(is_local ? IconType::kHouse : IconType::kNetwork); }

/// The account name. A workspace without an account is a workspace on this machine
wxString GetAccountName(const WelcomePageItemData& cd) { return cd.account.empty() ? kLocalHost : cd.account; }

/// The text of the cell of `cd` in column `column`
wxString GetCellText(const WelcomePageItemData& cd, unsigned column)
{
    switch (column) {
    case kColName:
        return cd.name;
    case kColAccount:
        return GetAccountSymbol(cd.account.empty()) + " " + GetAccountName(cd);
    case kColType:
        return cd.workspace_type;
    case kColPath:
        return cd.path;
    default:
        return wxEmptyString;
    }
}

/// The text to sort by. Same as the cell text, but without the account symbol
wxString GetSortKey(const WelcomePageItemData& cd, unsigned column)
{
    return column == kColAccount ? GetAccountName(cd) : GetCellText(cd, column);
}

} // namespace

WelcomePage::WelcomePage(wxWindow* parent)
    : WelcomePageBase(parent)
{
    ::AdjustDataViewAlternateColour(m_dvTreeCtrlWorkspaces);
    GetSizer()->Fit(this);
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, &WelcomePage::OnThemeChanged, this);

    m_closeButton->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("file_close", 24));

    // Add the columns only if the base class did not create them
    if (m_dvTreeCtrlWorkspaces->GetColumnCount() == 0) {
        m_dvTreeCtrlWorkspaces->AppendIconTextColumn(
            _("Name"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
        m_dvTreeCtrlWorkspaces->AppendTextColumn(
            _("Account"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
        m_dvTreeCtrlWorkspaces->AppendTextColumn(
            _("Type"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
        m_dvTreeCtrlWorkspaces->AppendTextColumn(
            _("Path"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    }

    // All the columns can be sorted by clicking their header. The sorting is done by us (see OnColumnHeaderClick), as
    // the default sorting of the control cannot compare the "Name" column (icon + text)
    for (unsigned i = 0; i < m_dvTreeCtrlWorkspaces->GetColumnCount(); ++i) {
        m_dvTreeCtrlWorkspaces->GetColumn(i)->SetSortable(true);
    }
    m_dvTreeCtrlWorkspaces->Bind(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, &WelcomePage::OnColumnHeaderClick, this);
    m_typeHelper = std::make_unique<DataViewTypeHelper>(m_dvTreeCtrlWorkspaces);

    // create the buttons
    m_buttonOpenWorkspace = new wxCommandLinkButton(m_buttonsPage,
                                                    wxID_ANY,
                                                    _("Open"),
                                                    _("Open an existing workspace"),
                                                    wxDefaultPosition,
                                                    wxDefaultSize,
                                                    wxBU_LEFT);
    m_buttonNewWorkspace = new wxCommandLinkButton(
        m_buttonsPage, wxID_ANY, _("New"), _("Create a new workspace"), wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
    m_buttonGithub = new wxCommandLinkButton(m_buttonsPage,
                                             wxID_ANY,
                                             _("GitHub"),
                                             _("Visit our GitHub project page"),
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxBU_LEFT);
    m_buttonGitter = new wxCommandLinkButton(
        m_buttonsPage, wxID_ANY, _("Gitter"), _("Join the chat!"), wxDefaultPosition, wxDefaultSize, wxBU_LEFT);

    // events
    m_buttonOpenWorkspace->Bind(wxEVT_BUTTON, &WelcomePage::OnOpenWorkspace, this);
    m_buttonNewWorkspace->Bind(wxEVT_BUTTON, &WelcomePage::OnNewWorkspace, this);
    m_buttonGithub->Bind(wxEVT_BUTTON, &WelcomePage::OnGitHub, this);
    m_buttonGitter->Bind(wxEVT_BUTTON, &WelcomePage::OnGitter, this);

    buttons_sizer->Add(m_buttonOpenWorkspace, wxSizerFlags(0).Expand().Border(wxALL, 5));
    buttons_sizer->Add(m_buttonNewWorkspace, wxSizerFlags(0).Expand().Border(wxALL, 5));
    buttons_sizer->Add(m_buttonGithub, wxSizerFlags(0).Expand().Border(wxALL, 5));
    buttons_sizer->Add(m_buttonGitter, wxSizerFlags(0).Expand().Border(wxALL, 5));

    UpdateRecentWorkspaces();
#if CL_USE_NATIVEBOOK
    Show();
#endif
    GetSizer()->Layout();
    // set the focus to the tree
    m_dvTreeCtrlWorkspaces->CallAfter(&wxWindow::SetFocus);
}

WelcomePage::~WelcomePage() { EventNotifier::Get()->Unbind(wxEVT_BITMAPS_UPDATED, &WelcomePage::OnThemeChanged, this); }

void WelcomePage::OnSize(wxSizeEvent& event) { event.Skip(); }

void WelcomePage::OnThemeChanged(clCommandEvent& e)
{
    e.Skip();

#ifndef __WXMAC__
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    GetMainPanel()->SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    // The icons are part of the rows, reload them
    UpdateRecentWorkspaces();
    Refresh();
#endif
}

void WelcomePage::OnNewWorkspace(wxCommandEvent& event)
{
    wxCommandEvent e(wxEVT_MENU, XRCID("new_workspace"));
    e.SetEventObject(clMainFrame::Get());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
}

void WelcomePage::OnOpenWorkspace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent e(wxEVT_MENU, XRCID("switch_to_workspace"));
    e.SetEventObject(clMainFrame::Get());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
}

// void WelcomePage::DoOpenFile(const wxString& filename) { clMainFrame::Get()->GetMainBook()->OpenFile(filename); }

void WelcomePage::UpdateRecentWorkspaces()
{
    m_items.clear();

    // get the recently opened workspaces (local)
    wxArrayString files;
    ManagerST::Get()->GetRecentlyOpenedWorkspaces(files);

    // request workspaces from the plugins
    clRecentWorkspaceEvent other_workspaces_event{wxEVT_RECENT_WORKSPACE};
    EventNotifier::Get()->ProcessEvent(other_workspaces_event);

    for (const wxString& filepath : files) {
        wxFileName fn{filepath};
        if (!fn.FileExists()) {
            // exclude non existing files
            continue;
        }

        wxString workspace_type;
        auto type = FileExtManager::GetType(fn.GetFullPath());
        int image_index = clGetManager()->GetStdIcons()->GetMimeImageId(type);
        switch (type) {
        case FileExtManager::TypeWorkspaceFileSystem:
            workspace_type = _("File System");
            break;
        case FileExtManager::TypeWorkspaceDocker:
            workspace_type = _("Docker");
            break;
        case FileExtManager::TypeWorkspacePHP:
            workspace_type = _("PHP");
            break;
        case FileExtManager::TypeWorkspace:
            workspace_type = _("C/C++");
            break;
        default:
            workspace_type = _("Other");
            image_index = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeWorkspaceFileSystem);
            break;
        }

        WelcomePageItemData cd;
        cd.type = WorkspaceSource::BUILTIN;
        cd.path = fn.GetFullPath();
        cd.name = fn.GetName();
        cd.workspace_type = workspace_type;
        cd.image_index = image_index;
        m_items.push_back(cd);
    }

    // Add the plugin base workspaces
    const int plugin_image_index =
        clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeWorkspaceFileSystem);
    for (const auto& w : other_workspaces_event.GetWorkspaces()) {
        WelcomePageItemData cd;
        cd.type = WorkspaceSource::PLUGIN;
        cd.path = w.path;
        cd.account = w.m_account;

        cd.name = cd.path;
        cd.name.Replace("\\", "/");
        cd.name = cd.name.AfterLast('/');
        cd.name = cd.name.BeforeLast('.');
        cd.workspace_type = w.m_category;
        cd.image_index = plugin_image_index;
        m_items.push_back(cd);
    }
    PopulateList();
}

void WelcomePage::PopulateList()
{
    // Sort the items. The rows keep the index of their data in `m_items`, so sort before adding them
    if (m_sortColumn != wxNOT_FOUND) {
        const unsigned column = static_cast<unsigned>(m_sortColumn);
        const bool ascending = m_sortAscending;
        std::stable_sort(m_items.begin(),
                         m_items.end(),
                         [column, ascending](const WelcomePageItemData& a, const WelcomePageItemData& b) {
                             const int res = GetSortKey(a, column).CmpNoCase(GetSortKey(b, column));
                             return ascending ? res < 0 : res > 0;
                         });
    }

    m_dvTreeCtrlWorkspaces->Freeze();
    m_dvTreeCtrlWorkspaces->DeleteAllItems();

    const auto* bitmaps = clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr();
    for (size_t i = 0; i < m_items.size(); ++i) {
        const WelcomePageItemData& cd = m_items[i];

        // Build the "Name" cell: icon + text
        wxIcon icon;
        if (bitmaps != nullptr && cd.image_index >= 0 && static_cast<size_t>(cd.image_index) < bitmaps->size()) {
            icon.CopyFromBitmap(bitmaps->at(cd.image_index));
        }
        wxVariant name_cell;
        name_cell << wxDataViewIconText(cd.name, icon);

        wxVector<wxVariant> cols;
        cols.resize(kColPath + 1);
        cols[kColName] = name_cell;
        cols[kColAccount] = wxVariant(GetCellText(cd, kColAccount));
        cols[kColType] = wxVariant(cd.workspace_type);
        cols[kColPath] = wxVariant(cd.path);
        m_dvTreeCtrlWorkspaces->AppendItem(cols, static_cast<wxUIntPtr>(i));
    }
    m_dvTreeCtrlWorkspaces->Thaw();
}

void WelcomePage::OnColumnHeaderClick(wxDataViewEvent& event)
{
    // Do not call event.Skip(): we handle the sorting
    const int column = event.GetColumn();
    if (column < 0 || static_cast<unsigned>(column) >= m_dvTreeCtrlWorkspaces->GetColumnCount()) {
        return;
    }

    // Clicking the current sort column toggles the order, any other column is sorted in ascending order
    m_sortAscending = (column == m_sortColumn) ? !m_sortAscending : true;
    m_sortColumn = column;

    // Show the sort indicator on the column
    for (unsigned i = 0; i < m_dvTreeCtrlWorkspaces->GetColumnCount(); ++i) {
        auto* col = m_dvTreeCtrlWorkspaces->GetColumn(i);
        if (static_cast<int>(i) == m_sortColumn) {
            col->SetSortOrder(m_sortAscending);
        } else {
            col->UnsetAsSortKey();
        }
    }
    PopulateList();
}

void WelcomePage::OnWorkspaceActivated(wxDataViewEvent& event)
{
    event.Skip();
    auto cd = GetWorkspaceItemData(event.GetItem());
    CHECK_PTR_RET(cd);

    switch (cd->type) {
    case WorkspaceSource::BUILTIN:
        OpenBuiltinWorkspace(cd);
        break;
    case WorkspaceSource::PLUGIN:
        OpenPluginWorkspace(cd);
        break;
    }
}

const WelcomePageItemData* WelcomePage::GetWorkspaceItemData(const wxDataViewItem& item) const
{
    CHECK_COND_RET_NULL(item.IsOk());

    // The row index in m_items is kept as the item data
    const size_t index = static_cast<size_t>(m_dvTreeCtrlWorkspaces->GetItemData(item));
    CHECK_COND_RET_NULL(index < m_items.size());
    return &m_items[index];
}

void WelcomePage::OpenBuiltinWorkspace(const WelcomePageItemData* cd)
{
    // post an event to the main frame requesting a workspace open
    wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("switch_to_workspace"));
    evtOpenworkspace.SetString(cd->path);
    evtOpenworkspace.SetEventObject(clMainFrame::Get());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
}

void WelcomePage::OpenPluginWorkspace(const WelcomePageItemData* cd)
{
    clWorkspaceEvent open_event{wxEVT_WORKSPACE_PLUGIN_OPEN};
    open_event.SetIsRemote(!cd->account.empty());
    open_event.SetFileName(cd->path);
    open_event.SetRemoteAccount(cd->account);
    EventNotifier::Get()->AddPendingEvent(open_event);
}

void WelcomePage::OnGitHub(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxLaunchDefaultBrowser("https://github.com/eranif/codelite");
}

void WelcomePage::OnGitter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxLaunchDefaultBrowser("https://gitter.im/eranif/codelite");
}

bool WelcomePage::Show(bool show)
{
    UpdateRecentWorkspaces();
    return wxControl::Show(show);
}

void WelcomePage::SelectSomething() { CallAfter(&WelcomePage::DoSomethingSomething); }

void WelcomePage::DoSomethingSomething()
{
    // for now, select the "Open workspace" button
    m_buttonOpenWorkspace->SetFocus();
}

void WelcomePage::GrabFocus() { CallAfter(&WelcomePage::SetFocus); }

void WelcomePage::OnCloseButtonClicked(wxCommandEvent& event)
{
    clCommandEvent event_close{wxEVT_WELCOMEPAGE_CLOSE_BUTTON_CLICKED};
    GetEventHandler()->AddPendingEvent(event_close);
}
