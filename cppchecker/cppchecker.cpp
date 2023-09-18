//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cppchecker.cpp
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

#include "cppchecker.h"

#include "IWorkspace.h"
#include "Notebook.h"
#include "Platform.hpp"
#include "StringUtils.h"
#include "build_settings_config.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clKeyboardManager.h"
#include "clWorkspaceManager.h"
#include "cl_process.h"
#include "codelite_events.h"
#include "cppchecksettingsdlg.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "processreaderthread.h"
#include "procutils.h"
#include "project.h"
#include "shell_command.h"
#include "workspace.h"

#include <wx/app.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/imaglist.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/sstream.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

static CppCheckPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new CppCheckPlugin(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor("Eran Ifrah & Jérémie (jfouche)");
    info.SetName("CppChecker");
    info.SetDescription(_("CppChecker integration for CodeLite IDE"));
    info.SetVersion("v2.0");
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CppCheckPlugin::CppCheckPlugin(IManager* manager)
    : IPlugin(manager)
{
    FileExtManager::Init();

    m_longName = _("CppCheck integration for CodeLite IDE");
    m_shortName = "CppCheck";

    // NB we can't load any project-specific settings here, as the workspace won't yet have loaded. We do it just before
    // they're used

    // Connect events
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &CppCheckPlugin::OnCppCheckReadData, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &CppCheckPlugin::OnCppCheckTerminated, this);
    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CppCheckPlugin::OnRun, this, XRCID("run_cppcheck"));
    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CppCheckPlugin::OnSettings, this, XRCID("cppcheck_settings_item"));

    EventNotifier::Get()->Bind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &CppCheckPlugin::OnIsBuildInProgress, this);
    EventNotifier::Get()->Bind(wxEVT_STOP_BUILD, &CppCheckPlugin::OnStopRun, this);

    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &CppCheckPlugin::OnWorkspaceClosed, this);
    clKeyboardManager::Get()->AddAccelerator(_("CppCheck"), { { "run_cppcheck", _("Run cppcheck...") } });
}

CppCheckPlugin::~CppCheckPlugin() {}

void CppCheckPlugin::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void CppCheckPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(XRCID("run_cppcheck"), _("Run cppcheck..."));
    menu->AppendSeparator();

    wxMenuItem* item =
        new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("CppCheck"), menu);
}

void CppCheckPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void CppCheckPlugin::UnPlug()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &CppCheckPlugin::OnCppCheckReadData, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &CppCheckPlugin::OnCppCheckTerminated, this);
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CppCheckPlugin::OnRun, this, XRCID("run_cppcheck"));
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CppCheckPlugin::OnSettings, this, XRCID("cppcheck_settings_item"));
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &CppCheckPlugin::OnWorkspaceClosed, this);
    // terminate the cppcheck daemon
    wxDELETE(m_cppcheckProcess);
    m_runStartedByUser = false;
}

void CppCheckPlugin::OnCppCheckTerminated(clProcessEvent& e)
{
    wxDELETE(m_cppcheckProcess);
    m_runStartedByUser = false;
    NotifyStopped();
}

void CppCheckPlugin::DoRun()
{
    wxString command = DoGetCommand();
    if(command.empty()) {
        return;
    }

    // notify about starting build process.
    // we pass the selected compiler in the event
    clBuildEvent eventStarted(wxEVT_BUILD_PROCESS_STARTED);
    eventStarted.SetToolchain(BuildSettingsConfigST::Get()->GetDefaultCompiler(wxEmptyString)->GetName());
    EventNotifier::Get()->AddPendingEvent(eventStarted);

    // Notify about build process started
    clBuildEvent eventStart(wxEVT_BUILD_STARTED);
    EventNotifier::Get()->AddPendingEvent(eventStart);

    AddOutputLine(command + "\n");

    size_t flags = IProcessCreateDefault | IProcessWrapInShell;
    m_cppcheckProcess = ::CreateAsyncProcess(this, command, flags);
    if(!m_cppcheckProcess) {
        wxMessageBox(_("Failed to launch cppcheck process.\nMake sure its installed and in your PATH"), _("Warning"),
                     wxOK | wxCENTER | wxICON_WARNING);
        return;
    }
    m_runStartedByUser = true;
}

wxString CppCheckPlugin::DoGetCommand()
{
    // Linux / Mac way: spawn the process and execute the command
    wxString cppcheck;
    if(!ThePlatform->Which("cppcheck", &cppcheck)) {
        ::wxMessageBox(_("Could not locate \"cppcheck\". Please install it and try again"), "CodeLite",
                       wxICON_WARNING | wxOK | wxOK_DEFAULT | wxCENTRE);
        return wxEmptyString;
    }

    wxString command = clConfig::Get().Read("cppcheck/command", CPPCHECK_DEFAULT_COMMAND);

    wxString workspace_path;
    wxString current_file;
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if(workspace) {
        if(workspace->IsRemote()) {
            workspace_path = wxFileName(workspace->GetFileName()).GetPath(false, wxPATH_UNIX);
        } else {
            workspace_path = wxFileName(workspace->GetFileName()).GetPath();
        }
    }
    if(clGetManager()->GetActiveEditor()) {
        current_file = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
    }

    // replace the place holders
    command.Replace("${cppcheck}", StringUtils::WrapWithDoubleQuotes(cppcheck));
    command.Replace("${WorkspacePath}", StringUtils::WrapWithDoubleQuotes(workspace_path));
    command.Replace("${CurrentFileFullPath}", StringUtils::WrapWithDoubleQuotes(current_file));

    wxString cmd;
    auto lines = ::wxStringTokenize(command, "\n", wxTOKEN_STRTOK);
    for(auto& line : lines) {
        line.Trim().Trim(false);
        line = line.BeforeFirst('#');
        if(line.empty()) {
            continue;
        }

        // Create the cache dir if required
        wxString cache_dir;
        if(line.StartsWith("--cppcheck-build-dir=", &cache_dir)) {
            cache_dir.Trim().Trim(false);
            wxFileName::Mkdir(cache_dir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
        cmd << line << " ";
    }
    cmd.Trim();
    if(cmd.empty()) {
        ::wxMessageBox(_("Cannot run cppcheck. Empty command"), "CodeLite",
                       wxICON_WARNING | wxOK | wxOK_DEFAULT | wxCENTRE);
        return wxEmptyString;
    }
    return cmd;
}

void CppCheckPlugin::OnCppCheckReadData(clProcessEvent& e)
{
    e.Skip();
    AddOutputLine(e.GetOutputRaw());
}

void CppCheckPlugin::OnSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CppCheckSettingsDialog dlg{ EventNotifier::Get()->TopFrame() };
    dlg.ShowModal();
}

void CppCheckPlugin::OnRun(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoRun();
}

void CppCheckPlugin::OnWorkspaceClosed(clWorkspaceEvent& e) { e.Skip(); }

void CppCheckPlugin::AddOutputLine(const wxString& message)
{
    clBuildEvent eventAddLine(wxEVT_BUILD_PROCESS_ADDLINE);
    eventAddLine.SetString(message);
    EventNotifier::Get()->AddPendingEvent(eventAddLine);
}

void CppCheckPlugin::OnIsBuildInProgress(clBuildEvent& event)
{
    if(!m_runStartedByUser) {
        event.Skip();
        return;
    }

    event.SetIsRunning(m_cppcheckProcess != nullptr);
}

void CppCheckPlugin::OnStopRun(clBuildEvent& event)
{
    if(!m_runStartedByUser) {
        event.Skip();
        return;
    }

    m_runStartedByUser = false;
    if(m_cppcheckProcess) {
        wxDELETE(m_cppcheckProcess);
    }

    NotifyStopped();
}

void CppCheckPlugin::NotifyStopped()
{
    clBuildEvent eventStopped(wxEVT_BUILD_ENDED);
    EventNotifier::Get()->AddPendingEvent(eventStopped);

    clBuildEvent eventProcessStopped(wxEVT_BUILD_PROCESS_ENDED);
    EventNotifier::Get()->AddPendingEvent(eventProcessStopped);
}
