//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : continuousbuild.cpp
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

#include "build_settings_config.h"
#include "builder.h"
#include "buildmanager.h"
#include "cl_command_event.h"
#include "compile_request.h"
#include "continousbuildconf.h"
#include "continousbuildpane.h"
#include "continuousbuild.h"
#include "custombuildrequest.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "globals.h"
#include "processreaderthread.h"
#include "workspace.h"
#include <wx/app.h>
#include <wx/imaglist.h>
#include <wx/log.h>
#include <wx/xrc/xmlres.h>

static ContinuousBuild* thePlugin = NULL;
// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new ContinuousBuild(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("ContinuousBuild"));
    info.SetDescription(_("Continuous build plugin which compiles files on save and report errors"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

static const wxString CONT_BUILD = _("BuildQ");

ContinuousBuild::ContinuousBuild(IManager* manager)
    : IPlugin(manager)
    , m_buildInProgress(false)
{
    m_longName = _("Continuous build plugin which compiles files on save and report errors");
    m_shortName = wxT("ContinuousBuild");
    m_view = new ContinousBuildPane(m_mgr->GetOutputPaneNotebook(), m_mgr, this);

    // add our page to the output pane notebook
    m_mgr->GetOutputPaneNotebook()->AddPage(m_view, CONT_BUILD, false, m_mgr->GetStdIcons()->LoadBitmap("execute"));
    m_tabHelper.reset(new clTabTogglerHelper(CONT_BUILD, m_view, "", NULL));
    m_tabHelper->SetOutputTabBmp(m_mgr->GetStdIcons()->LoadBitmap("execute"));

    m_topWin = m_mgr->GetTheApp();
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(ContinuousBuild::OnFileSaved), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVE_BY_BUILD_START,
                                  wxCommandEventHandler(ContinuousBuild::OnIgnoreFileSaved), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVE_BY_BUILD_END,
                                  wxCommandEventHandler(ContinuousBuild::OnStopIgnoreFileSaved), NULL, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ContinuousBuild::OnBuildProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ContinuousBuild::OnBuildProcessEnded, this);
}

ContinuousBuild::~ContinuousBuild() {}

clToolBar* ContinuousBuild::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void ContinuousBuild::CreatePluginMenu(wxMenu* pluginsMenu) { wxUnusedVar(pluginsMenu); }

void ContinuousBuild::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void ContinuousBuild::UnPlug()
{
    m_tabHelper.reset(NULL);
    // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_view == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            break;
        }
    }
    m_view->Destroy();

    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(ContinuousBuild::OnFileSaved), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVE_BY_BUILD_START,
                                     wxCommandEventHandler(ContinuousBuild::OnIgnoreFileSaved), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVE_BY_BUILD_END,
                                     wxCommandEventHandler(ContinuousBuild::OnStopIgnoreFileSaved), NULL, this);
}

void ContinuousBuild::OnFileSaved(clCommandEvent& e)
{
    e.Skip();
    clDEBUG1() << "ContinuousBuild::OnFileSaved";
    // Dont build while the main build is in progress
    if(m_buildInProgress) {
        clDEBUG() << "Build already in progress, skipping";
        return;
    }

    ContinousBuildConf conf;
    m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);

    if(conf.GetEnabled()) {
        DoBuild(e.GetString());
    } else {
        clDEBUG1() << "ContinuousBuild is disabled";
    }
}

void ContinuousBuild::DoBuild(const wxString& fileName)
{
    clDEBUG() << "ContinuousBuild::DoBuild is called";
    // Make sure a workspace is opened
    if(!m_mgr->IsWorkspaceOpen()) {
        clDEBUG() << "ContinuousBuild::DoBuild: No workspace opened!";
        return;
    }

    // Filter non source files
    FileExtManager::FileType type = FileExtManager::GetType(fileName);
    switch(type) {
    case FileExtManager::TypeSourceC:
    case FileExtManager::TypeSourceCpp:
    case FileExtManager::TypeResource:
        break;

    default: {
        clDEBUG1() << "ContinuousBuild::DoBuild: Non source file";
        return;
    }
    }

    wxString projectName = m_mgr->GetProjectNameByFile(fileName);
    if(projectName.IsEmpty()) {
        clDEBUG() << "ContinuousBuild::DoBuild: project name is empty";
        return;
    }

    wxString errMsg;
    ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectName, errMsg);
    if(!project) {
        clDEBUG() << "Could not find project for file";
        return;
    }

    // get the selected configuration to be build
    BuildConfigPtr bldConf = m_mgr->GetWorkspace()->GetProjBuildConf(project->GetName(), wxEmptyString);
    if(!bldConf) {
        CL_DEBUG(wxT("Failed to locate build configuration\n"));
        return;
    }

    BuilderPtr builder = bldConf->GetBuilder();
    if(!builder) {
        CL_DEBUG(wxT("Failed to located builder\n"));
        return;
    }

    // Only normal file builds are supported
    if(bldConf->IsCustomBuild()) {
        CL_DEBUG(wxT("Build is custom. Skipping\n"));
        return;
    }

    // get the single file command to use
    wxString cmd =
        builder->GetSingleFileCmd(projectName, bldConf->GetName(), bldConf->GetBuildSystemArguments(), fileName);
    WrapInShell(cmd);

    if(m_buildProcess.IsBusy()) {
        // add the build to the queue
        if(m_files.Index(fileName) == wxNOT_FOUND) {
            m_files.Add(fileName);

            // update the UI
            m_view->AddFile(fileName);
        }
        return;
    }

    clCommandEvent event(wxEVT_SHELL_COMMAND_STARTED);

    // Associate the build event details
    BuildEventDetails* eventData = new BuildEventDetails();
    eventData->SetProjectName(projectName);
    eventData->SetConfiguration(bldConf->GetName());
    eventData->SetIsCustomProject(bldConf->IsCustomBuild());
    eventData->SetIsClean(false);

    event.SetClientObject(eventData);
    // Fire it up
    EventNotifier::Get()->AddPendingEvent(event);

    EnvSetter env(NULL, NULL, projectName, bldConf->GetName());
    CL_DEBUG(wxString::Format(wxT("cmd:%s\n"), cmd.c_str()));
    if(!m_buildProcess.Execute(cmd, fileName, project->GetFileName().GetPath(), this)) return;

    // Set some messages
    m_mgr->SetStatusMessage(
        wxString::Format(wxT("%s %s..."), _("Compiling"), wxFileName(fileName).GetFullName().c_str()), 0);

    // Add this file to the UI queue
    m_view->AddFile(fileName);
}

void ContinuousBuild::OnBuildProcessEnded(clProcessEvent& e)
{
    // remove the file from the UI
    int pid = m_buildProcess.GetPid();
    m_view->RemoveFile(m_buildProcess.GetFileName());

    clCommandEvent event(wxEVT_SHELL_COMMAND_PROCESS_ENDED);
    EventNotifier::Get()->AddPendingEvent(event);

    int exitCode(-1);
    if(IProcess::GetProcessExitCode(pid, exitCode) && exitCode != 0) {
        m_view->AddFailedFile(m_buildProcess.GetFileName());
    }

    // Release the resources allocted for this build
    m_buildProcess.Stop();

    // if the queue is not empty, start another build
    if(m_files.IsEmpty() == false) {

        wxString fileName = m_files.Item(0);
        m_files.RemoveAt(0);

        DoBuild(fileName);
    }
}

void ContinuousBuild::StopAll()
{
    // empty the queue
    m_files.Clear();
    m_buildProcess.Stop();
}

void ContinuousBuild::OnIgnoreFileSaved(wxCommandEvent& e)
{
    e.Skip();

    m_buildInProgress = true;

    // Clear the queue
    m_files.Clear();

    // Clear the view
    m_view->ClearAll();
}

void ContinuousBuild::OnStopIgnoreFileSaved(wxCommandEvent& e)
{
    e.Skip();
    m_buildInProgress = false;
}

void ContinuousBuild::OnBuildProcessOutput(clProcessEvent& e)
{
    clCommandEvent event(wxEVT_SHELL_COMMAND_ADDLINE);
    event.SetString(e.GetOutput());
    EventNotifier::Get()->AddPendingEvent(event);
}
