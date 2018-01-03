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

#include "debuggerpane.h"
#include "editor_config.h"
#include "fileutils.h"
#include "frame.h"
#include "globals.h"
#include "perspectivemanager.h"
#include <wx/aui/framemanager.h>
#include <wx/stdpaths.h>

wxString DEBUG_LAYOUT = wxT("debug.layout");
wxString NORMAL_LAYOUT = wxT("default.layout");

PerspectiveManager::PerspectiveManager()
    : m_active(NORMAL_LAYOUT)
    , m_aui(NULL)
{
    wxString active = EditorConfigST::Get()->GetString(wxT("ActivePerspective"));
    if(active.IsEmpty() == false) { m_active = active; }
    ClearIds();
}

PerspectiveManager::~PerspectiveManager() { DisconnectEvents(); }

void PerspectiveManager::DeleteAllPerspectives()
{
    wxArrayString files;
    wxDir::GetAllFiles(clStandardPaths::Get().GetUserDataDir() + wxT("/config/"), &files, wxT("*.layout"));

    wxLogNull noLog;
    for(size_t i = 0; i < files.GetCount(); i++) {
        clRemoveFile(files.Item(i));
    }
}

void PerspectiveManager::LoadPerspective(const wxString& name)
{
    wxString pname = name;
    if(pname.IsEmpty()) { pname = m_active; }

    wxString file = DoGetPathFromName(pname);
    wxString content;

    if(ReadFileWithConversion(file, content)) {
        clMainFrame::Get()->GetDockingManager().LoadPerspective(content);
        m_active = pname;
        EditorConfigST::Get()->SetString(wxT("ActivePerspective"), m_active);

        if(pname == DEBUG_LAYOUT) { DoEnsureDebuggerPanesAreVisible(); }

    } else {
        if(pname == DEBUG_LAYOUT) {
            DoEnsureDebuggerPanesAreVisible();

            SavePerspective(pname);
            m_active = pname;
            EditorConfigST::Get()->SetString(wxT("ActivePerspective"), m_active);

        } else if(pname == NORMAL_LAYOUT) {
            // Requested to load the Normal layout but we got no such layout
            // Make the current one the default layout
            SavePerspective(pname);
            m_active = pname;
            EditorConfigST::Get()->SetString(wxT("ActivePerspective"), m_active);
        }
    }
}

void PerspectiveManager::SavePerspective(const wxString& name, bool notify)
{
    wxString pname = name;
    if(pname.IsEmpty()) { pname = GetActive(); }

    WriteFileWithBackup(DoGetPathFromName(pname), clMainFrame::Get()->GetDockingManager().SavePerspective(), false);
    m_active = pname;

    EditorConfigST::Get()->SetString(wxT("ActivePerspective"), m_active);
    if(notify) {
        wxCommandEvent evt(wxEVT_REFRESH_PERSPECTIVE_MENU);
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evt);
    }
}

wxArrayString PerspectiveManager::GetAllPerspectives()
{
    wxArrayString files, perspectives;
    wxDir::GetAllFiles(clStandardPaths::Get().GetUserDataDir() + wxT("/config/"), &files, wxT("*.layout"));

    for(size_t i = 0; i < files.GetCount(); i++) {
        wxFileName fn(files.Item(i));
        wxString name = fn.GetFullName();
        perspectives.Add(name);
    }
    return perspectives;
}

void PerspectiveManager::ClearIds()
{
    m_menuIdToName.clear();
    m_nextId = FirstMenuId();
}

int PerspectiveManager::MenuIdFromName(const wxString& name)
{
    std::map<wxString, int>::iterator iter = m_menuIdToName.find(name);
    if(iter == m_menuIdToName.end()) {
        m_menuIdToName[name] = ++m_nextId;
        return m_menuIdToName[name];
    }
    return iter->second;
}

wxString PerspectiveManager::NameFromMenuId(int id)
{
    std::map<wxString, int>::iterator iter = m_menuIdToName.begin();
    for(; iter != m_menuIdToName.end(); iter++) {
        if(iter->second == id) { return iter->first; }
    }
    return wxT("");
}

void PerspectiveManager::LoadPerspectiveByMenuId(int id)
{
    wxString name = NameFromMenuId(id);
    if(name.IsEmpty()) return;

    LoadPerspective(name);
}

void PerspectiveManager::Delete(const wxString& name)
{
    wxLogNull noLog;
    wxString path = DoGetPathFromName(name);
    clRemoveFile(path);
}

void PerspectiveManager::Rename(const wxString& old, const wxString& new_name)
{
    wxString oldPath = DoGetPathFromName(old);
    wxString newPath = DoGetPathFromName(new_name);

    wxLogNull noLog;
    wxRename(oldPath, newPath);
}

wxString PerspectiveManager::DoGetPathFromName(const wxString& name)
{
    wxString file;
    wxString filename = name;

    filename.MakeLower();
    if(!filename.EndsWith(wxT(".layout"))) { filename << wxT(".layout"); }

    file << clStandardPaths::Get().GetUserDataDir() << wxT("/config/") << filename;
    return file;
}

void PerspectiveManager::SavePerspectiveIfNotExists(const wxString& name)
{
    wxString file = DoGetPathFromName(name);
    if(!wxFileName::FileExists(file)) { SavePerspective(name, false); }
}

bool PerspectiveManager::IsDefaultActive() const { return GetActive().CmpNoCase(NORMAL_LAYOUT) == 0; }

void PerspectiveManager::DoEnsureDebuggerPanesAreVisible()
{
    bool needUpdate = false;
    // First time, make sure that the debugger pane is visible
    wxAuiPaneInfo& info = clMainFrame::Get()->GetDockingManager().GetPane(wxT("Debugger"));
    if(info.IsOk() && !info.IsShown()) {
        info.Show();
        needUpdate = true;
    }

    // read the debugger pane configuration and make sure that all the windows
    // are visible according to the configuration
    clConfig conf("debugger-view.conf");
    DebuggerPaneConfig item;
    conf.ReadItem(&item);

    DoShowPane(item.WindowName(DebuggerPaneConfig::AsciiViewer), (item.GetWindows() & DebuggerPaneConfig::AsciiViewer),
               needUpdate);
    DoShowPane(item.WindowName(DebuggerPaneConfig::Threads), (item.GetWindows() & DebuggerPaneConfig::Threads),
               needUpdate);
    DoShowPane(item.WindowName(DebuggerPaneConfig::Callstack), (item.GetWindows() & DebuggerPaneConfig::Callstack),
               needUpdate);
    DoShowPane(item.WindowName(DebuggerPaneConfig::Breakpoints), (item.GetWindows() & DebuggerPaneConfig::Breakpoints),
               needUpdate);
    DoShowPane(item.WindowName(DebuggerPaneConfig::Watches), (item.GetWindows() & DebuggerPaneConfig::Watches),
               needUpdate);
    DoShowPane(item.WindowName(DebuggerPaneConfig::Locals), (item.GetWindows() & DebuggerPaneConfig::Locals),
               needUpdate);
    DoShowPane(item.WindowName(DebuggerPaneConfig::Output), (item.GetWindows() & DebuggerPaneConfig::Output),
               needUpdate);
    DoShowPane(item.WindowName(DebuggerPaneConfig::Memory), (item.GetWindows() & DebuggerPaneConfig::Memory),
               needUpdate);
    DoShowPane(item.WindowName(DebuggerPaneConfig::Disassemble), (item.GetWindows() & DebuggerPaneConfig::Disassemble),
               needUpdate);

    if(needUpdate) { clMainFrame::Get()->GetDockingManager().Update(); }
}

void PerspectiveManager::OnPaneClosing(wxAuiManagerEvent& event)
{
    event.Skip();
    if(!m_aui) return;

    wxAuiPaneInfo& pane_info = m_aui->GetPane(wxT("Output View"));
    if(pane_info.IsOk() && pane_info.IsShown()) {
        // keep the last perspective where the output view
        // was visible
        m_buildPerspective = m_aui->SavePerspective();
    }
}

void PerspectiveManager::ConnectEvents(wxAuiManager* mgr)
{
    m_aui = mgr;
    if(m_aui) {
        mgr->Connect(wxEVT_AUI_PANE_BUTTON, wxAuiManagerEventHandler(PerspectiveManager::OnPaneClosing), NULL, this);
    }
}

void PerspectiveManager::ToggleOutputPane(bool hide)
{
    if(!m_aui) return;

    wxAuiPaneInfo& pane_info = m_aui->GetPane(wxT("Output View"));
    if(!pane_info.IsOk()) return;

    pane_info.Show(!hide);
    m_aui->Update();
    //    if(hide && pane_info.IsShown()) {
    //        pane_info.Hide();
    //    } else if(!hide && !pane_info.IsShown()) {
    //
    //    }
    //    m_aui->Update();
    //    if(hide && pane_info.IsShown()) {
    //    } else {
    //
    //    }
    //    if(pane_info.IsOk() && !pane_info.IsShown()) {
    //        // keep the last perspective where the output view
    //        // was visible
    //        m_buildPerspective = m_aui->SavePerspective();
    //    }
    //
    //    if(pane_info.IsOk() && hide && pane_info.IsShown()) {
    //        pane_info.Hide();
    //        m_aui->Update();
    //
    //    } else if(pane_info.IsOk() && !hide && !pane_info.IsShown()) {
    //        if(!m_buildPerspective.IsEmpty()) {
    //            m_aui->LoadPerspective(m_buildPerspective, true);
    //
    //        } else {
    //            // Else just show it
    //            pane_info.Show();
    //            m_aui->Update();
    //        }
    //    }
}

void PerspectiveManager::DisconnectEvents()
{
    if(m_aui) {
        m_aui->Disconnect(wxEVT_AUI_PANE_BUTTON, wxAuiManagerEventHandler(PerspectiveManager::OnPaneClosing), NULL,
                          this);
    }
    m_aui = NULL;
}

void PerspectiveManager::DoShowPane(const wxString& panename, bool show, bool& needUpdate)
{
    wxAuiManager* aui = &clMainFrame::Get()->GetDockingManager();
    wxAuiPaneInfo& pane_info = aui->GetPane(panename);
    if(pane_info.IsOk()) {
        if(show && !pane_info.IsShown()) {
            pane_info.Show();
            needUpdate = true;

        } else if(!show && pane_info.IsShown()) {
            pane_info.Show(false);
            needUpdate = true;
        }
    }
}
