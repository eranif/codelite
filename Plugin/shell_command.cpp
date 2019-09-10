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
#include "environmentconfig.h"
#include "processreaderthread.h"
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "workspace.h"
#include "shell_command.h"
#include "event_notifier.h"
#include "wx/tokenzr.h"
#include "asyncprocess.h"
#include "cl_command_event.h"

const wxEventType wxEVT_SHELL_COMMAND_ADDLINE = XRCID("wxEVT_SHELL_COMMAND_ADDLINE");
const wxEventType wxEVT_SHELL_COMMAND_STARTED = XRCID("wxEVT_SHELL_COMMAND_STARTED");
const wxEventType wxEVT_SHELL_COMMAND_PROCESS_ENDED = XRCID("wxEVT_SHELL_COMMAND_PROCESS_ENDED");
const wxEventType wxEVT_SHELL_COMMAND_STARTED_NOCLEAN = XRCID("wxEVT_SHELL_COMMAND_STARTED_NOCLEAN");

ShellCommand::ShellCommand(const QueueCommand& buildInfo)
    : m_proc(NULL)
    , m_info(buildInfo)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ShellCommand::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ShellCommand::OnProcessTerminated, this);
}

void ShellCommand::AppendLine(const wxString& line)
{
    wxCommandEvent event(wxEVT_SHELL_COMMAND_ADDLINE);
    event.SetString(line);
    event.SetInt(m_info.GetKind());
    EventNotifier::Get()->AddPendingEvent(event);
}

void ShellCommand::Stop()
{
    // kill the build process
    CleanUp();
}

void ShellCommand::SendStartMsg()
{
    clCommandEvent event(m_info.GetCleanLog() ? wxEVT_SHELL_COMMAND_STARTED : wxEVT_SHELL_COMMAND_STARTED_NOCLEAN);
    event.SetString(m_info.GetSynopsis());

    BuildEventDetails* eventData = new BuildEventDetails();
    eventData->SetProjectName(m_info.GetProject());
    eventData->SetConfiguration(m_info.GetConfiguration());
    eventData->SetIsCustomProject(m_info.GetKind() == QueueCommand::kCustomBuild);
    eventData->SetIsClean(m_info.GetKind() == QueueCommand::kClean || (m_info.GetKind() == QueueCommand::kCustomBuild &&
                                                                       m_info.GetCustomBuildTarget() == wxT("clean")));

    event.SetClientObject(eventData);
    EventNotifier::Get()->AddPendingEvent(event);
}

void ShellCommand::SendEndMsg()
{
    wxCommandEvent event(wxEVT_SHELL_COMMAND_PROCESS_ENDED);
    event.SetString(m_info.GetSynopsis());
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
