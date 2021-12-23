//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : compiler_action.cpp
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
#include "shell_command.h"

#include "asyncprocess.h"
#include "cl_command_event.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "globals.h"
#include "processreaderthread.h"
#include "workspace.h"
#include "wx/tokenzr.h"

#include <wx/xrc/xmlres.h>

wxDEFINE_EVENT(wxEVT_BUILD_PROCESS_ADDLINE, clBuildEvent);
wxDEFINE_EVENT(wxEVT_BUILD_PROCESS_STARTED, clBuildEvent);
wxDEFINE_EVENT(wxEVT_BUILD_PROCESS_ENDED, clBuildEvent);

ShellCommand::ShellCommand(const QueueCommand& buildInfo)
    : m_proc(NULL)
    , m_info(buildInfo)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ShellCommand::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ShellCommand::OnProcessTerminated, this);
}

void ShellCommand::AppendLine(const wxString& line)
{
    clBuildEvent add_line_event(wxEVT_BUILD_PROCESS_ADDLINE);
    add_line_event.SetString(line);
    EventNotifier::Get()->AddPendingEvent(add_line_event);
}

void ShellCommand::Stop()
{
    // kill the build process
    CleanUp();
}

void ShellCommand::SendStartMsg(const wxString& toolchain)
{
    // fire build-started event
    clBuildEvent start_event(wxEVT_BUILD_PROCESS_STARTED);
    start_event.SetCleanLog(m_info.GetCleanLog());
    start_event.SetProjectName(m_info.GetProject());
    start_event.SetConfigurationName(m_info.GetConfiguration());
    start_event.SetFlag(clBuildEvent::kCustomProject, m_info.GetKind() == QueueCommand::kCustomBuild);
    start_event.SetToolchain(toolchain);
    start_event.SetFlag(clBuildEvent::kClean, m_info.GetKind() == QueueCommand::kClean ||
                                                  (start_event.HasFlag(clBuildEvent::kCustomProject) &&
                                                   m_info.GetCustomBuildTarget() == wxT("clean")));
    EventNotifier::Get()->AddPendingEvent(start_event);
}

void ShellCommand::SendEndMsg()
{
    clBuildEvent event(wxEVT_BUILD_PROCESS_ENDED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void ShellCommand::DoPrintOutput(const wxString& out)
{
    // Write the buffer as-is to the build tab !!
    AppendLine(out);
}

void ShellCommand::CleanUp()
{
    wxDELETE(m_proc);
    SendEndMsg();
}

void ShellCommand::DoSetWorkingDirectory(ProjectPtr proj, bool isCustom, bool isFileOnly)
{
    // when using custom build, user can select different working directory
    if(proj) {
        if(isCustom) {
            // first set the path to the project working directory
            ::wxSetWorkingDirectory(proj->GetFileName().GetPath());

            BuildConfigPtr buildConf =
                clCxxWorkspaceST::Get()->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
            if(buildConf) {
                wxString wd = buildConf->GetCustomBuildWorkingDir();
                if(wd.IsEmpty()) {
                    // use the project path
                    wd = proj->GetFileName().GetPath();
                } else {
                    // expand macros from path
                    wd = ExpandAllVariables(wd, clCxxWorkspaceST::Get(), proj->GetName(), buildConf->GetName(),
                                            wxEmptyString);
                }
                ::wxSetWorkingDirectory(wd);
            }
        } else {
            if(m_info.GetProjectOnly() || isFileOnly) {
                // first set the path to the project working directory
                ::wxSetWorkingDirectory(proj->GetFileName().GetPath());
            }
        }
    }
}

void ShellCommand::OnProcessOutput(clProcessEvent& e) { DoPrintOutput(e.GetOutput()); }

void ShellCommand::OnProcessTerminated(clProcessEvent& e)
{
    wxUnusedVar(e);
    CleanUp();
}

bool ShellCommand::StartProcess(const wxString& cmd, size_t create_flags)
{
#ifndef __WXMSW__
    create_flags |= IProcessRawOutput;
#endif

    m_proc = ::CreateAsyncProcess(this, cmd, create_flags);
    if(!m_proc) {
        return false;
    }
    return true;
}
