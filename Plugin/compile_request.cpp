//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : compile_request.cpp
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
#include "compile_request.h"

#include "ICompilerLocator.h"
#include "asyncprocess.h"
#include "build_config.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "cl_command_event.h"
#include "compiler.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "macromanager.h"
#include "macros.h"
#include "plugin.h"
#include "workspace.h"
#include "wx/process.h"

#include <wx/app.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/regex.h>

CompileRequest::CompileRequest(const QueueCommand& buildInfo, const wxString& fileName, bool runPremakeOnly,
                               bool preprocessOnly)
    : ShellCommand(buildInfo)
    , m_fileName(fileName)
    , m_premakeOnly(runPremakeOnly)
    , m_preprocessOnly(preprocessOnly)
{
}

CompileRequest::~CompileRequest()
{
    // no need to delete the process, it will be deleted by the wx library
}

// do the actual cleanup
void CompileRequest::Process(IManager* manager)
{
    wxString cmd;
    wxString errMsg;
    wxStringMap_t om;

    BuildSettingsConfig* bsc(manager ? manager->GetBuildSettingsConfigManager() : BuildSettingsConfigST::Get());
    clCxxWorkspace* w(manager ? manager->GetWorkspace() : clCxxWorkspaceST::Get());
    EnvironmentConfig* env(manager ? manager->GetEnv() : EnvironmentConfig::Instance());

    ProjectPtr proj = w->FindProjectByName(m_info.GetProject(), errMsg);
    if(!proj) {
        AppendLine(_("Cant find project: ") + m_info.GetProject());
        return;
    }

    wxString pname(proj->GetName());
    wxArrayString unresolvedVars;
    proj->GetUnresolvedMacros(m_info.GetConfiguration(), unresolvedVars);
    if(!unresolvedVars.IsEmpty()) {
        // We can't continue
        wxString msg;
        msg << _("The following environment variables are used in the project, but are not defined:\n");
        for(size_t i = 0; i < unresolvedVars.size(); ++i) {
            msg << unresolvedVars.Item(i) << "\n";
        }
        msg << _("Build anyway?");
        wxStandardID res = ::PromptForYesNoDialogWithCheckbox(msg, "UnresolvedMacros", _("Yes"), _("No"),
                                                              _("Remember my answer and don't ask me again"),
                                                              wxYES_NO | wxICON_WARNING | wxYES_DEFAULT);
        if(res != wxID_YES) {
            ::wxMessageBox(_("Build Cancelled!"), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
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

    // Send the EVENT_STARTED : even if this event is sent, next event will
    // be post, so no way to be sure the the build process has not started
    BuildConfigPtr bldConf = w->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
    SendStartMsg(bldConf ? bldConf->GetCompilerType() : wxString());

    // if we require to run the makefile generation command only, replace the 'cmd' with the
    // generation command line
    if(bldConf) {
        // BuilderPtr builder = bm->GetBuilder("Default");
        BuilderPtr builder = bldConf->GetBuilder();
        wxString args = bldConf->GetBuildSystemArguments();
        if(m_fileName.IsEmpty() == false) {
            // we got a complie request of a single file
            cmd = m_preprocessOnly
                      ? builder->GetPreprocessFileCmd(m_info.GetProject(), m_info.GetConfiguration(), args, m_fileName,
                                                      errMsg)
                      : builder->GetSingleFileCmd(m_info.GetProject(), m_info.GetConfiguration(), args, m_fileName);
        } else if(m_info.GetProjectOnly()) {

            switch(m_info.GetKind()) {
            case QueueCommand::kRebuild:
                cmd = builder->GetPORebuildCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
                break;
            default:
            case QueueCommand::kBuild:
                cmd = builder->GetPOBuildCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
                break;
            }

        } else {
            cmd = builder->GetBuildCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
        }

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

    if(cmd.IsEmpty()) {
        // if we got an error string, use it
        if(errMsg.IsEmpty() == false) {
            AppendLine(errMsg);
        } else {
            AppendLine(_("Command line is empty. Build aborted."));
        }
        return;
    }

    DirSaver ds;
    DoSetWorkingDirectory(proj, false, m_fileName.IsEmpty() == false);

    // expand the variables of the command
    cmd = ExpandAllVariables(cmd, w, m_info.GetProject(), m_info.GetConfiguration(), m_fileName);

    // print the build command
    AppendLine(cmd + wxT("\n"));
    if(m_info.GetProjectOnly() || m_fileName.IsEmpty() == false) {
        // set working directory
        DoSetWorkingDirectory(proj, false, m_fileName.IsEmpty() == false);
    }

    // print the prefix message of the build start. This is important since the parser relies
    // on this message
    if(m_info.GetProjectOnly() || m_fileName.IsEmpty() == false) {
        wxString configName(m_info.GetConfiguration());

        // also, send another message to the main frame, indicating which project is being built
        // and what configuration
        wxString text;
        text << BUILD_PROJECT_PREFIX << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]");
        if(m_fileName.IsEmpty()) {
            text << wxT("----------\n");
        } else if(m_preprocessOnly) {
            text << wxT(" (Preprocess Single File)----------\n");
        } else {
            text << wxT(" (Single File Build)----------\n");
        }
        AppendLine(text);
    }

    // Avoid Unicode chars coming from the compiler by setting LC_ALL to "C"
    om["LC_ALL"] = "C";

    EnvSetter envir(env, &om, proj->GetName(), m_info.GetConfiguration());
    if(!StartProcess(cmd, IProcessCreateDefault | IProcessWrapInShell)) {
        wxString message;
        message << _("Failed to start build process, command: ") << cmd << _(", process terminated with exit code: 0");
        AppendLine(message);
        return;
    }
}
