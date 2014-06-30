//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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

#include "continuousbuild.h"
#include "event_notifier.h"
#include "buildmanager.h"
#include "globals.h"
#include "builder.h"
#include "file_logger.h"
#include "processreaderthread.h"
#include "fileextmanager.h"
#include "build_settings_config.h"
#include "workspace.h"
#include "custombuildrequest.h"
#include "compile_request.h"
#include <wx/app.h>
#include "environmentconfig.h"
#include "continousbuildpane.h"
#include <wx/xrc/xmlres.h>
#include "continousbuildconf.h"
#include <wx/log.h>
#include <wx/imaglist.h>
#include "cl_command_event.h"

static ContinuousBuild* thePlugin = NULL;
//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == 0) {
        thePlugin = new ContinuousBuild(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("ContinuousBuild"));
    info.SetDescription(_("Continuous build plugin which compiles files on save and report errors"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

BEGIN_EVENT_TABLE(ContinuousBuild, IPlugin)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  ContinuousBuild::OnBuildProcessOutput)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, ContinuousBuild::OnBuildProcessEnded)
END_EVENT_TABLE()

static const wxString CONT_BUILD = wxT("BuildQ");

ContinuousBuild::ContinuousBuild(IManager *manager)
    : IPlugin(manager)
    , m_buildInProgress(false)
{
    m_longName = _("Continuous build plugin which compiles files on save and report errors");
    m_shortName = wxT("ContinuousBuild");
    m_view = new ContinousBuildPane(m_mgr->GetOutputPaneNotebook(), m_mgr, this);

    // add our page to the output pane notebook
    m_mgr->GetOutputPaneNotebook()->AddPage(m_view, CONT_BUILD, false, LoadBitmapFile(wxT("compfile.png")));

    m_topWin = m_mgr->GetTheApp();
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED,               clCommandEventHandler(ContinuousBuild::OnFileSaved),           NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVE_BY_BUILD_START, wxCommandEventHandler(ContinuousBuild::OnIgnoreFileSaved),     NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVE_BY_BUILD_END,   wxCommandEventHandler(ContinuousBuild::OnStopIgnoreFileSaved), NULL, this);
}

ContinuousBuild::~ContinuousBuild()
{
}

clToolBar *ContinuousBuild::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
    return tb;
}

void ContinuousBuild::CreatePluginMenu(wxMenu *pluginsMenu)
{
    wxUnusedVar(pluginsMenu);
}

void ContinuousBuild::HookPopupMenu(wxMenu *menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void ContinuousBuild::UnPlug()
{
    // before this plugin is un-plugged we must remove the tab we added
    for (size_t i=0; i<m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if (m_view == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            m_view->Destroy();
            break;
        }
    }
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED,               clCommandEventHandler(ContinuousBuild::OnFileSaved),           NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVE_BY_BUILD_START, wxCommandEventHandler(ContinuousBuild::OnIgnoreFileSaved),     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVE_BY_BUILD_END,   wxCommandEventHandler(ContinuousBuild::OnStopIgnoreFileSaved), NULL, this);
}

void ContinuousBuild::OnFileSaved(clCommandEvent& e)
{
    e.Skip();
    CL_DEBUG(wxT("ContinuousBuild::OnFileSaved\n"));
    // Dont build while the main build is in progress
    if (m_buildInProgress) {
        CL_DEBUG(wxT("Build already in progress, skipping\n"));
        return;
    }

    ContinousBuildConf conf;
    m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);

    if (conf.GetEnabled()) {
        DoBuild( e.GetString() );
    } else {
        CL_DEBUG(wxT("ContinuousBuild is disabled\n"));
    }
}

void ContinuousBuild::DoBuild(const wxString& fileName)
{
    CL_DEBUG(wxT("DoBuild\n"));
    // Make sure a workspace is opened
    if (!m_mgr->IsWorkspaceOpen()) {
        CL_DEBUG(wxT("No workspace opened!\n"));
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
        CL_DEBUG(wxT("Non source file\n"));
        return;
    }
    }

    wxString projectName = m_mgr->GetProjectNameByFile(fileName);
    if(projectName.IsEmpty()) {
        CL_DEBUG(wxT("Project name is empty\n"));
        return;
    }

    wxString errMsg;
    ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectName, errMsg);
    if(!project) {
        CL_DEBUG(wxT("Could not find project for file\n"));
        return;
    }

    // get the selected configuration to be build
    BuildConfigPtr bldConf = m_mgr->GetWorkspace()->GetProjBuildConf( project->GetName(), wxEmptyString );
    if ( !bldConf ) {
        CL_DEBUG(wxT("Failed to locate build configuration\n"));
        return;
    }

    BuilderPtr builder = m_mgr->GetBuildManager()->GetBuilder( wxT( "GNU makefile for g++/gcc" ) );
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
    wxString cmd      = builder->GetSingleFileCmd(projectName, bldConf->GetName(), fileName);
    WrapInShell(cmd);

    if( m_buildProcess.IsBusy() ) {
        // add the build to the queue
        if (m_files.Index(fileName) == wxNOT_FOUND) {
            m_files.Add(fileName);

            // update the UI
            m_view->AddFile(fileName);
        }
        return;
    }

    clCommandEvent event(wxEVT_SHELL_COMMAND_STARTED);

    // Associate the build event details
    BuildEventDetails *eventData = new BuildEventDetails();
    eventData->SetProjectName(projectName);
    eventData->SetConfiguration(bldConf->GetName());
    eventData->SetIsCustomProject(bldConf->IsCustomBuild());
    eventData->SetIsClean(false);

    event.SetClientObject(eventData);
    // Fire it up
    EventNotifier::Get()->AddPendingEvent(event);

    EnvSetter env(NULL, NULL, projectName);
    CL_DEBUG(wxString::Format(wxT("cmd:%s\n"), cmd.c_str()));
    if(!m_buildProcess.Execute(cmd, fileName, project->GetFileName().GetPath(), this))
        return;

    // Set some messages
    m_mgr->SetStatusMessage(wxString::Format(wxT("%s %s..."), _("Compiling"), wxFileName(fileName).GetFullName().c_str()), 0);

    // Add this file to the UI queue
    m_view->AddFile(fileName);
}

void ContinuousBuild::OnBuildProcessEnded(wxCommandEvent& e)
{
    // remove the file from the UI
    ProcessEventData *ped = (ProcessEventData*)e.GetClientData();
    delete ped;

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
    if (m_files.IsEmpty() == false) {

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

void ContinuousBuild::OnBuildProcessOutput(wxCommandEvent& e)
{
    ProcessEventData *ped = (ProcessEventData*)e.GetClientData();

    clCommandEvent event(wxEVT_SHELL_COMMAND_ADDLINE);
    event.SetString(ped->GetData());
    EventNotifier::Get()->AddPendingEvent(event);

    //m_mgr->AddBuildOuptut(ped->GetData(), false);
    delete ped;
}
