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

#include "debuggerpane.h"
#include "editor_config.h"
#include "file_logger.h"
#include "fileutils.h"
#include "frame.h"
#include "globals.h"
#include "macros.h"

#include <wx/aui/framemanager.h>
#include <wx/stdpaths.h>

wxString DEBUG_LAYOUT = wxT("debug.layout");
wxString NORMAL_LAYOUT = wxT("default.layout");

PerspectiveManager::PerspectiveManager()
    : m_active(NORMAL_LAYOUT)
    , m_aui(NULL)
{
    wxString active = EditorConfigST::Get()->GetString(wxT("ActivePerspective"));
    if(active.IsEmpty() == false) {
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
        for(size_t i = 0; i < files.GetCount(); i++) {
            clRemoveFile(files.Item(i));
        }
    }
    m_perspectives.clear();
}

void PerspectiveManager::LoadPerspective(const wxString& name)
{
    m_active = name;
    wxString content;
    if(GetPerspective(name, &content)) {
        clMainFrame::Get()->GetDockingManager().LoadPerspective(content);
        m_active = name;
        EditorConfigST::Get()->SetString(wxT("ActivePerspective"), m_active);
    }

    // in case we are loading the debugger perspective, ensure that the debugger panes are visible
    if(name == DEBUG_LAYOUT) {
        DoEnsureDebuggerPanesAreVisible();
    }
    clGetManager()->GetDockingManager()->Update();
}

void PerspectiveManager::SavePerspective(const wxString& name, bool notify)
{
    wxString pname = name;
    if(pname.empty()) {
        pname = GetActive();
    }

    wxString currentLayout = clGetManager()->GetDockingManager()->SavePerspective();
    SetPerspectiveToCache(name, currentLayout);
    bool save_required = (m_active != pname);
    m_active = pname;

    if(save_required) {
        EditorConfigST::Get()->SetString(wxT("ActivePerspective"), m_active);
    }

    if(notify) {
        wxCommandEvent evt(wxEVT_REFRESH_PERSPECTIVE_MENU);
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evt);
    }
}

wxString PerspectiveManager::DoGetPathFromName(const wxString& name) const
{
    wxString file;
    wxString filename = name;

    filename.MakeLower();
    if(!filename.EndsWith(wxT(".layout"))) {
        filename << wxT(".layout");
    }

    file << clStandardPaths::Get().GetUserDataDir() << wxT("/config/") << filename;
    return file;
}

void PerspectiveManager::SavePerspectiveIfNotExists(const wxString& name)
{
    wxString file = DoGetPathFromName(name);
    if(!wxFileName::FileExists(file)) {
        SavePerspective(name, false);
    }
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

    if(needUpdate) {
        clMainFrame::Get()->GetDockingManager().Update();
    }
}

void PerspectiveManager::OnPaneClosing(wxAuiManagerEvent& event)
{
    event.Skip();
    if(!m_aui)
        return;

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
        mgr->Bind(wxEVT_AUI_PANE_BUTTON, &PerspectiveManager::OnPaneClosing, this);
        mgr->Bind(wxEVT_AUI_RENDER, &PerspectiveManager::OnAuiRender, this);
    }
}

void PerspectiveManager::ToggleOutputPane(bool hide)
{
    if(!m_aui)
        return;

    wxAuiPaneInfo& pane_info = m_aui->GetPane(wxT("Output View"));
    if(!pane_info.IsOk())
        return;

    pane_info.Show(!hide);
    m_aui->Update();
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

void PerspectiveManager::OnAuiRender(wxAuiManagerEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_aui);
    CHECK_COND_RET(!GetActive().IsEmpty());
    SavePerspective(GetActive(), false);
}

bool PerspectiveManager::GetPerspective(const wxString& name, wxString* perspective) const
{
    if(m_perspectives.count(name)) {
        *perspective = m_perspectives.find(name)->second;
        return true;
    }

    wxString path = DoGetPathFromName(name);
    if(!wxFileName::FileExists(path)) {
        return false;
    }

    wxString content;
    if(!FileUtils::ReadFileContent(path, content)) {
        clWARNING() << "Failed to read perspective file:" << path << endl;
        return false;
    }
    *perspective = content;
    return true;
}

void PerspectiveManager::SetPerspectiveToCache(const wxString& name, const wxString& content)
{
    if(m_perspectives.count(name)) {
        m_perspectives.erase(name);
    }
    m_perspectives.insert({ name, content });
}

void PerspectiveManager::FlushCacheToDisk()
{
    for(const auto& vt : m_perspectives) {
        const wxString& name = vt.first;
        const wxString& value = vt.second;

        wxString path = DoGetPathFromName(name);
        FileUtils::WriteFileContent(path, value);
        clDEBUG() << "Saving perspective:" << name << "to file:" << path << endl;
    }
}
