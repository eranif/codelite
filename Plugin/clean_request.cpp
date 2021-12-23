//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : clean_request.cpp
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
#include "clean_request.h"

#include "ICompilerLocator.h"
#include "asyncprocess.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "cl_command_event.h"
#include "compiler.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "plugin.h"
#include "workspace.h"
#include "wx/process.h"

#include <wx/app.h>
#include <wx/log.h>

CleanRequest::CleanRequest(const QueueCommand& info)
    : ShellCommand(info)
{
}

CleanRequest::~CleanRequest()
{
    // no need to delete the process, it will be deleted by the wx library
}

// do the actual cleanup
void CleanRequest::Process(IManager* manager)
{
    wxString cmd;
    wxString errMsg;
    wxStringMap_t om;

    BuildSettingsConfig* bsc(manager ? manager->GetBuildSettingsConfigManager() : BuildSettingsConfigST::Get());
    clCxxWorkspace* w(manager ? manager->GetWorkspace() : clCxxWorkspaceST::Get());

    ProjectPtr proj = w->FindProjectByName(m_info.GetProject(), errMsg);
    if(!proj) {
        AppendLine(_("Cant find project: ") + m_info.GetProject());
        return;
    }
    wxString pname(proj->GetName());
    BuildConfigPtr bldConf = w->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
    if(bldConf) {
        // BuilderPtr builder = bm->GetBuilder("Default");
        BuilderPtr builder = bldConf->GetBuilder();
        wxString args = bldConf->GetBuildSystemArguments();
        if(m_info.GetProjectOnly()) {
            cmd = builder->GetPOCleanCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
        } else {
            cmd = builder->GetCleanCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
        }

        if(cmd.IsEmpty()) {
            AppendLine(_("Sorry, there is no 'Clean' command available\n"));
            return;
        }

        wxString cmpType = bldConf->GetCompilerType();
        if(bldConf) {
            wxString cmpType = bldConf->GetCompilerType();
            CompilerPtr cmp = bsc->GetCompiler(cmpType);
            if(cmp) {
                // Add the 'bin' folder of the compiler to the PATH environment variable
                wxString scxx = cmp->GetTool("CXX");
                scxx.Trim().Trim(false);
                scxx.StartsWith("\"", &scxx);
                scxx.EndsWith("\"", &scxx);
                // Strip the double quotes
                wxFileName cxx(scxx);
                wxString pathvar;
                pathvar << cxx.GetPath() << clPATH_SEPARATOR;

                // If we have an additional path, add it as well
                if(!cmp->GetPathVariable().IsEmpty()) {
                    pathvar << cmp->GetPathVariable() << clPATH_SEPARATOR;
                }
                pathvar << "$PATH";
                om["PATH"] = pathvar;
            }
        }
    } else {
        AppendLine(_("Sorry, couldn't find the Build configuration\n"));
        return;
    }

    // Notify plugins that a compile process is going to start
    clBuildEvent event(wxEVT_BUILD_STARTING);
    event.SetProjectName(pname);
    event.SetConfigurationName(m_info.GetConfiguration());

    if(EventNotifier::Get()->ProcessEvent(event)) {
        // the build is being handled by some plugin, no need to build it
        // using the standard way
        return;
    }

    SendStartMsg(bldConf ? bldConf->GetCompilerType() : wxString());

    // Expand the variables of the command
    cmd = ExpandAllVariables(cmd, w, m_info.GetProject(), m_info.GetConfiguration(), wxEmptyString);
    DirSaver ds;
    DoSetWorkingDirectory(proj, false, false);

    if(m_info.GetProjectOnly()) {
        // need to change directory to project dir
        wxSetWorkingDirectory(proj->GetFileName().GetPath());
    }
    // print the build command
    AppendLine(cmd + wxT("\n"));

    // print the prefix message of the build start. This is important since the parser relies
    // on this message
    if(m_info.GetProjectOnly()) {
        wxString configName(m_info.GetConfiguration());

        // also, send another message to the main frame, indicating which project is being built
        // and what configuration
        wxString text;
        text << CLEAN_PROJECT_PREFIX << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]");
        text << wxT("----------\n");
        AppendLine(text);
    }

    // apply environment settings
    EnvSetter env(NULL, &om, proj->GetName(), m_info.GetConfiguration());
    if(!StartProcess(cmd, IProcessCreateDefault | IProcessWrapInShell)) {
        // remove environment settings applied
        wxString message;
        message << _("Failed to start clean process, command: ") << cmd << _(", process terminated with exit code: 0");
        AppendLine(message);
    }
}
