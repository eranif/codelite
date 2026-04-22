//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : perspectivemanager.cpp
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

#include "perspectivemanager.h"

#include "SideBar.hpp"
#include "clStrings.h"
#include "debuggerpane.h"
#include "dockablepanemenumanager.h"
#include "editor_config.h"
#include "file_logger.h"
#include "fileutils.h"
#include "frame.h"
#include "globals.h"
#include "macros.h"

#include <wx/aui/framemanager.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>

wxString DEBUG_LAYOUT = wxT("debug.layout");
wxString NORMAL_LAYOUT = wxT("default.layout");

PerspectiveManager::PerspectiveManager()
    : m_active(NORMAL_LAYOUT)
    , m_aui(NULL)
{
    wxString active = EditorConfigST::Get()->GetString(wxT("ActivePerspective"));
    if (active.IsEmpty() == false) {
        m_active = active;
    }
}

PerspectiveManager::~PerspectiveManager()
{
    DisconnectEvents();
    FlushCacheToDisk();
}

void PerspectiveManager::DeleteAllPerspectives()
{
    wxArrayString files;
    wxDir::GetAllFiles(clStandardPaths::Get().GetUserDataDir() + wxT("/config/"), &files, wxT("*.layout"));

    {
        wxLogNull noLog;
        for (size_t i = 0; i < files.GetCount(); i++) {
            clRemoveFile(files.Item(i));
        }
    }
    m_perspectives.clear();
}

void PerspectiveManager::LoadPerspective(const wxString& name)
{
    m_active = name;
    wxString content;
    if (GetPerspective(name, &content)) {
        clMainFrame::Get()->GetDockingManager().LoadPerspective(content);
        m_active = name;
        EditorConfigST::Get()->SetString(wxT("ActivePerspective"), m_active);
    }

    // in case we are loading the debugger perspective, ensure that the debugger panes are visible
    if (name == DEBUG_LAYOUT) {
        DoEnsureDebuggerPanesAreVisible();
    }
    clGetManager()->GetDockingManager()->Update();
}

void PerspectiveManager::SavePerspective(const wxString& name, bool notify)
{
    wxString pname = name;
    if (pname.empty()) {
        pname = GetActive();
    }

    wxString currentLayout = clGetManager()->GetDockingManager()->SavePerspective();
    SetPerspectiveToCache(name, currentLayout);
    bool save_required = (m_active != pname);
    m_active = pname;

    if (save_required) {
        EditorConfigST::Get()->SetString(wxT("ActivePerspective"), m_active);
    }

    if (notify) {
        wxCommandEvent evt(wxEVT_REFRESH_PERSPECTIVE_MENU);
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evt);
    }
}

wxString PerspectiveManager::DoGetPathFromName(const wxString& name) const
{
    wxString file;
    wxString filename = name;

    filename.MakeLower();
    if (!filename.EndsWith(wxT(".layout"))) {
        filename << wxT(".layout");
    }

    file << clStandardPaths::Get().GetUserDataDir() << wxT("/config/") << filename;
    return file;
}

void PerspectiveManager::SavePerspectiveIfNotExists(const wxString& name)
{
    wxString file = DoGetPathFromName(name);
    if (!wxFileName::FileExists(file)) {
        SavePerspective(name, false);
    }
}

void PerspectiveManager::DoEnsureDebuggerPanesAreVisible() { ShowPane(PANE_DEBUGGER, true); }

void PerspectiveManager::OnPaneClosing(wxAuiManagerEvent& event)
{
    event.Skip();
    CHECK_COND_RET(m_aui);

    wxAuiPaneInfo& pane_info = m_aui->GetPane(PANE_OUTPUT);
    if (pane_info.IsOk() && pane_info.IsShown()) {
        // keep the last perspective where the output view
        // was visible
        m_buildPerspective = m_aui->SavePerspective();
    }
}

void PerspectiveManager::ConnectEvents(wxAuiManager* mgr)
{
    m_aui = mgr;
    CHECK_PTR_RET(mgr);
    CHECK_PTR_RET(mgr->GetFrame());

    mgr->GetFrame()->Bind(wxEVT_AUI_PANE_BUTTON, &PerspectiveManager::OnPaneClosing, this);
    mgr->GetFrame()->Bind(wxEVT_AUI_RENDER, &PerspectiveManager::OnAuiRender, this);
}

void PerspectiveManager::ShowOutputPane(const wxString& tab, bool show, bool take_focus)
{
    if (!ShowPane("Output View", show)) {
        return;
    }

    if (!show) {
        // Make sure we set a focus to something
        auto editor = clGetManager()->GetActiveEditor();
        CHECK_PTR_RET(editor);
        editor->GetCtrl()->CallAfter(&wxWindow::SetFocus);
        return;
    }

    // show == true
    if (tab.empty()) {
        return;
    }

    auto pane = clMainFrame::Get()->GetOutputPane();
    auto notebook = pane->GetNotebook();

    int index = wxNOT_FOUND;
    for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
        if (notebook->GetPageText(i) == tab) {
            index = i;
            break;
        }
    }

    if (index == wxNOT_FOUND) {
        return;
    }
    wxWindow* old_focus = wxWindow::FindFocus();
    notebook->ChangeSelection(index);
    // set the focus to the selected tab
    if (take_focus) {
        ::SetBestFocus(notebook->GetPage(index));
    } else {
        // On GTK port, wxNotbook::ChangeSelection steals the focus
        // whether we want it or not
        if (old_focus) {
            old_focus->CallAfter(&wxWindow::SetFocus);
        }
    }
}

void PerspectiveManager::ShowSideBar(bool left_sidebar, const wxString& tab, bool show, bool take_focus)
{
    const wxString pane_name = left_sidebar ? PANE_LEFT_SIDEBAR : PANE_RIGHT_SIDEBAR;
    if (!ShowPane(pane_name, show)) {
        return;
    }

    if (!show) {
        // Make sure we set a focus to something
        auto editor = clGetManager()->GetActiveEditor();
        CHECK_PTR_RET(editor);
        editor->GetCtrl()->CallAfter(&wxWindow::SetFocus);
        return;
    }

    // show == true
    if (tab.empty()) {
        return;
    }

    clSideBarCtrl* book{nullptr};
    if (left_sidebar) {
        book = clMainFrame::Get()->GetWorkspacePane()->GetNotebook();
    } else {
        book = clMainFrame::Get()->GetSecondarySideBar()->GetNotebook();
    }

    int index = wxNOT_FOUND;
    for (size_t i = 0; i < book->GetPageCount(); ++i) {
        if (book->GetPageText(i) == tab) {
            index = i;
            break;
        }
    }

    if (index == wxNOT_FOUND) {
        return;
    }

    wxWindow* old_focus = wxWindow::FindFocus();
    book->ChangeSelection(index);
    // set the focus to the selected tab
    if (take_focus) {
        ::SetBestFocus(book->GetPage(index));
    } else {
        // On GTK port, wxNotbook::ChangeSelection steals the focus
        // whether we want it or not
        if (old_focus) {
            old_focus->CallAfter(&wxWindow::SetFocus);
        }
    }
}

void PerspectiveManager::DisconnectEvents()
{
    if (m_aui && m_aui->GetFrame()) {
        m_aui->GetFrame()->Unbind(wxEVT_AUI_PANE_BUTTON, &PerspectiveManager::OnPaneClosing, this);
        m_aui->GetFrame()->Unbind(wxEVT_AUI_RENDER, &PerspectiveManager::OnAuiRender, this);
    }
    m_aui = NULL;
}

void PerspectiveManager::OnAuiRender(wxAuiManagerEvent& event)
{
    event.Skip();
    //    CHECK_PTR_RET(m_aui);
    //    wxString name = GetActive().empty() ? NORMAL_LAYOUT : GetActive();
    //    SavePerspective(GetActive(), false);
}

bool PerspectiveManager::GetPerspective(const wxString& name, wxString* perspective) const
{
    if (m_perspectives.count(name)) {
        *perspective = m_perspectives.find(name)->second;
        return true;
    }

    wxString path = DoGetPathFromName(name);
    if (!wxFileName::FileExists(path)) {
        return false;
    }

    wxString content;
    if (!FileUtils::ReadFileContent(path, content)) {
        clWARNING() << "Failed to read perspective file:" << path << endl;
        return false;
    }
    *perspective = content;
    return true;
}

void PerspectiveManager::SetPerspectiveToCache(const wxString& name, const wxString& content)
{
    m_perspectives.insert_or_assign(name, content);
}

void PerspectiveManager::FlushCacheToDisk()
{
    for (const auto& [name, value] : m_perspectives) {
        wxString path = DoGetPathFromName(name);
        FileUtils::WriteFileContent(path, value);
        clDEBUG() << "Saving perspective:" << name << "to file:" << path << endl;
    }
}

void PerspectiveManager::DoShowPane(wxAuiPaneInfo& pane_info)
{
    if (pane_info.IsOk() && !pane_info.IsShown() && m_aui && pane_info.best_size != wxDefaultSize) {
        pane_info.MinSize(pane_info.best_size); // saved while hiding
        pane_info.Show();
        m_aui->Update();
        pane_info.MinSize(10, 5); // so it can't disappear if undocked
        m_aui->Update();
    }
}

void PerspectiveManager::DoHidePane(wxAuiPaneInfo& pane_info)
{
    if (pane_info.IsOk() && pane_info.IsShown() && m_aui) {
        int width = 0;
        int height = 0;
        pane_info.window->GetSize(&width, &height);
        pane_info.BestSize(width, height); // save for later subsequent show
        pane_info.Hide();
        m_aui->Update();
    }
}

bool PerspectiveManager::ShowPane(wxAuiPaneInfo& pane, bool show)
{
    if (!show) {
        DoHidePane(pane);
    } else {
        DoShowPane(pane);
    }
    return true;
}

bool PerspectiveManager::ShowPane(const wxString& pane, bool show)
{
    CHECK_PTR_RET_FALSE(m_aui);

    wxAuiPaneInfo& aui_pane_info = m_aui->GetPane(pane);
    CHECK_COND_RET_FALSE(aui_pane_info.IsOk());
    return ShowPane(aui_pane_info, show);
}

bool PerspectiveManager::IsPaneVisible(const wxString& name) const
{
    CHECK_PTR_RET_FALSE(m_aui);
    wxAuiPaneInfo& aui_pane_info = m_aui->GetPane(name);
    return aui_pane_info.IsOk() && aui_pane_info.IsShown();
}
