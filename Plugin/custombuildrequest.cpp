//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : custombuildrequest.cpp
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
#include "custombuildrequest.h"

#include "asyncprocess.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "cl_command_event.h"
#include "compiler.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "plugin.h"
#include "workspace.h"

#include <wx/app.h>
#include <wx/ffile.h>
#include <wx/log.h>

#ifdef __WXMSW__
#define ECHO_CMD wxT("@echo ")
#define ECHO_OFF wxT("@echo off")
#define SILENCE_OP wxT("@")
#define SCRIPT_EXT wxT(".bat")
#else
#define ECHO_CMD wxT("echo ")
#define ECHO_OFF wxT("")
#define SILENCE_OP wxT("")
#define SCRIPT_EXT wxT(".sh")
#endif

CustomBuildRequest::CustomBuildRequest(const QueueCommand& buildInfo, const wxString& fileName)
    : ShellCommand(buildInfo)
    , m_fileName(fileName)
{
}

CustomBuildRequest::~CustomBuildRequest() {}

void CustomBuildRequest::Process(IManager* manager)
{
    wxString cmd;
    wxString errMsg;
    wxStringMap_t om;

    clCxxWorkspace* w(manager->GetWorkspace());
    EnvironmentConfig* env(manager->GetEnv());

    ProjectPtr proj = w->FindProjectByName(m_info.GetProject(), errMsg);
    if(!proj) {
        AppendLine(_("Cant find project: ") + m_info.GetProject());
        return;
    }

    // Notify plugins that a compile process is going to start
    clBuildEvent event(wxEVT_BUILD_STARTING);
    event.SetProjectName(proj->GetName());
    event.SetConfigurationName(m_info.GetConfiguration());

    if(EventNotifier::Get()->ProcessEvent(event)) {
        // the build is being handled by some plugin, no need to build it
        // using the standard way
        return;
    }

    BuildConfigPtr bldConf = w->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
    if(!bldConf) {
        clLogMessage(wxString::Format(wxT("Failed to find build configuration for project '%s' and configuration '%s'"),
                                      m_info.GetProject().c_str(), m_info.GetConfiguration().c_str()));
        return;
    }
    SendStartMsg(bldConf->GetCompilerType());

    // try the special targets first:
    bool isClean(false);
    if(m_info.GetCustomBuildTarget() == wxT("Build")) {
        cmd = bldConf->GetCustomBuildCmd();

    } else if(m_info.GetCustomBuildTarget() == wxT("Clean")) {
        cmd = bldConf->GetCustomCleanCmd();
        isClean = true;
    } else if(m_info.GetCustomBuildTarget() == wxT("Rebuild")) {
        cmd = bldConf->GetCustomRebuildCmd();

    } else if(m_info.GetCustomBuildTarget() == wxT("Compile Single File")) {
        cmd = bldConf->GetSingleFileBuildCommand();

    } else if(m_info.GetCustomBuildTarget() == wxT("Preprocess File")) {
        cmd = bldConf->GetPreprocessFileCommand();
    }

    // if still no luck, try with the other custom targets
    if(cmd.IsEmpty()) {
        std::map<wxString, wxString> targets = bldConf->GetCustomTargets();
        std::map<wxString, wxString>::iterator iter = targets.find(m_info.GetCustomBuildTarget());
        if(iter != targets.end()) {
            cmd = iter->second;
        }
    }

    if(cmd.IsEmpty()) {
        // if we got an error string, use it
        if(errMsg.IsEmpty() == false) {
            AppendLine(errMsg);
        } else {
            AppendLine(_("Command line is empty. Build aborted."));
        }
        return;
    }

    // Working directory:
    // By default we use the project path
    //////////////////////////////////////////////////////

    DirSaver ds;

    // first set the path to the project working directory
    ::wxSetWorkingDirectory(proj->GetFileName().GetPath());

    // If a working directory was specified, use it instead
    wxString wd = bldConf->GetCustomBuildWorkingDir();
    wd.Trim().Trim(false);

    wxString filename = m_fileName;
    if(filename.IsEmpty() && manager->GetActiveEditor()) {
        filename = manager->GetActiveEditor()->GetFileName().GetFullPath();
    }

    if(wd.IsEmpty()) {
        // use the project path
        wd = proj->GetFileName().GetPath();

    } else {

        // expand macros from the working directory
        wd = ExpandAllVariables(wd, clCxxWorkspaceST::Get(), proj->GetName(), bldConf->GetName(), filename);
    }

    {
        // Ensure that the path to the working directory exist
        wxFileName fnwd(wd, "");
        if(!fnwd.DirExists()) {
            fnwd.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
    }

    ::wxSetWorkingDirectory(wd);

    // Print message to the build tab
    AppendLine(wxString::Format(_("MESSAGE: Entering directory `%s'\n"), wd.c_str()));

    // Command handling:
    //////////////////////////////////////////////////////

    // expand the variables of the command
    cmd = ExpandAllVariables(cmd, w, m_info.GetProject(), m_info.GetConfiguration(), filename);

    // in case our configuration includes post/pre build commands
    // we generate a makefile to include them as well and we update
    // the build command
    bool bCommandAltered = DoUpdateCommand(manager, cmd, proj, bldConf, isClean);

#ifdef __WXMSW__
    // Windows CD command requires the paths to be backslashe
    if(cmd.Find(wxT("cd ")) != wxNOT_FOUND)
        cmd.Replace(wxT("/"), wxT("\\"));
#endif

    // Wrap the build command in the shell, so it will be able
    // to perform 'chain' commands like
    // cd SOMEWHERE && make && ...

    size_t processFlags = IProcessCreateDefault;
    // Dont wrap the command if it was altered previously
    if(!bCommandAltered) {
        processFlags |= IProcessWrapInShell;
    }

    // print the build command
    AppendLine(cmd + wxT("\n"));
    wxString configName(m_info.GetConfiguration());

    // also, send another message to the main frame, indicating which project is being built
    // and what configuration
    wxString text;
    if(isClean) {
        text << CLEAN_PROJECT_PREFIX;
    } else {
        text << BUILD_PROJECT_PREFIX;
    }
    text << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]----------\n");

    AppendLine(text);

    // Avoid Unicode chars coming from the compiler by setting LC_ALL to "C"
    om["LC_ALL"] = "C";
    EnvSetter environment(env, &om, proj->GetName(), m_info.GetConfiguration());

    if(!StartProcess(cmd, processFlags)) {
        wxString message;
        message << _("Failed to start build process, command: ") << cmd << _(", process terminated with exit code: 0");
        AppendLine(message);
    }
}

bool CustomBuildRequest::DoUpdateCommand(IManager* manager, wxString& cmd, ProjectPtr proj, BuildConfigPtr bldConf,
                                         bool isClean)
{
    BuildCommandList preBuildCmds, postBuildCmds;
    wxArrayString pre, post;
    bldConf->GetPreBuildCommands(preBuildCmds);
    bldConf->GetPostBuildCommands(postBuildCmds);

    // collect all enabled commands
    BuildCommandList::iterator iter = preBuildCmds.begin();
    for(; iter != preBuildCmds.end(); iter++) {
        BuildCommand command = *iter;
        if(command.GetEnabled()) {
            pre.Add(command.GetCommand());
        }
    }

    iter = postBuildCmds.begin();
    for(; iter != postBuildCmds.end(); iter++) {
        BuildCommand command = *iter;
        if(command.GetEnabled()) {
            post.Add(command.GetCommand());
        }
    }

    if(pre.empty() && post.empty()) {
        return false;
    }

    // we need to create a makefile which includes all the pre-build, the actual build command and the post-build
    // commands
    // (in this exact order).

    wxString script;
    script << ECHO_OFF << wxT("\n");

    if(pre.IsEmpty() == false && !isClean) {
        script << ECHO_CMD << wxT("Executing Pre Build commands ...\n");
        for(size_t i = 0; i < pre.GetCount(); i++) {
            script << SILENCE_OP << pre.Item(i) << wxT("\n");
        }
        script << ECHO_CMD << wxT("Done\n");
    }

    // add the command
    script << cmd << wxT("\n");

    if(post.IsEmpty() == false && !isClean) {
        script << ECHO_CMD << wxT("Executing Post Build commands ...\n");
        for(size_t i = 0; i < post.GetCount(); i++) {
            script << SILENCE_OP << post.Item(i) << wxT("\n");
        }
        script << ECHO_CMD << wxT("Done\n");
    }

    // write the makefile
    wxFFile output;
    wxString fn;
    fn << proj->GetName() << SCRIPT_EXT;

    output.Open(fn, wxT("w+"));
    if(output.IsOpened()) {
        output.Write(script);
        output.Close();
    }

    cmd.Clear();
#ifdef __WXMSW__
    cmd << wxT("\"") << fn << wxT("\"");
#else
    // *nix
    cmd << wxT("/bin/bash './") << fn << wxT("'");
#endif
    return true;
}
