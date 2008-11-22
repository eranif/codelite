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
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "workspace.h"
#include "compiler_action.h"
#include "wx/tokenzr.h"

const wxEventType wxEVT_SHELL_COMMAND_ADDLINE = XRCID("wxEVT_SHELL_COMMAND_ADDLINE");
const wxEventType wxEVT_SHELL_COMMAND_STARTED = XRCID("wxEVT_SHELL_COMMAND_STARTED");
const wxEventType wxEVT_SHELL_COMMAND_PROCESS_ENDED = XRCID("wxEVT_SHELL_COMMAND_PROCESS_ENDED");
const wxEventType wxEVT_SHELL_COMMAND_STARTED_NOCLEAN = XRCID("wxEVT_SHELL_COMMAND_STARTED_NOCLEAN");

ShellCommand::ShellCommand(wxEvtHandler *owner, const QueueCommand &buildInfo)
		: m_proc(NULL)
		, m_owner(owner)
		, m_busy(false)
		, m_stop(false)
		, m_info(buildInfo)
{
	m_timer = new wxTimer(this);
}

void ShellCommand::AppendLine(const wxString &line)
{
	if ( !m_owner)
		return;

	wxCommandEvent event(wxEVT_SHELL_COMMAND_ADDLINE);
	event.SetString(line);
	m_owner->AddPendingEvent(event);

	m_lines.Add(line);
}

void ShellCommand::Stop()
{
	m_stop = true;
	//kill the build process
	if (m_proc) {
		m_proc->Terminate();
		CleanUp();
	}
}

void ShellCommand::SendStartMsg()
{
	if ( !m_owner)
		return;

    wxCommandEvent event(m_info.GetCleanLog() ? wxEVT_SHELL_COMMAND_STARTED
                                              : wxEVT_SHELL_COMMAND_STARTED_NOCLEAN);
    event.SetString(m_info.GetSynopsis());
    m_owner->AddPendingEvent(event);
}

void ShellCommand::SendEndMsg()
{
	if ( !m_owner)
		return;

	wxCommandEvent event(wxEVT_SHELL_COMMAND_PROCESS_ENDED);
    event.SetString(m_info.GetSynopsis());
	m_owner->AddPendingEvent(event);
}

void ShellCommand::OnTimer(wxTimerEvent &event)
{
	wxUnusedVar(event);
	if ( m_stop ) {
		m_proc->Terminate();
		return;
	}
	PrintOutput();
}

void ShellCommand::PrintOutput()
{
	wxString data, errors;
	m_proc->HasInput(data, errors);
	DoPrintOutput(data, errors);
}

void ShellCommand::OnProcessEnd(wxProcessEvent& event)
{
	wxUnusedVar(event);
	if ( !m_stop ) {
		//read all input before stopping the timer
		wxString err;
		wxString out;
		while (m_proc->HasInput(out, err)) {
			DoPrintOutput(out, err);
			out.Empty();
			err.Empty();
		}
		CleanUp();
	}

	EnvironmentConfig::Instance()->UnApplyEnv();
	//give back the focus to the editor
	event.Skip();
}

void ShellCommand::DoPrintOutput(const wxString &out, const wxString &err)
{
	//loop over the lines read from the compiler
	wxStringTokenizer tkz(out, wxT("\n"));
	while (tkz.HasMoreTokens()) {
		wxString line = tkz.NextToken();
		if ( line.Contains(wxT("Entering directory")) ||
		        line.Contains(wxT("Leaving directory"))  ||
		        line.Contains(wxT("type attributes are honored only at type definition")) ||
		        line.Contains(wxT("type attributes ignored after type is already defined")) ||
		        line.StartsWith(wxT("# "))
		   ) {
			//skip it
			continue;
		} else {
			//print it
			AppendLine(line + wxT("\n"));
		}
	}

	if (!err.IsEmpty()) {
		wxStringTokenizer tt(err, wxT("\n"));
		while (tt.HasMoreTokens()) {
			AppendLine(tt.NextToken() + wxT("\n"));
		}
	}
}

void ShellCommand::CleanUp()
{
	m_timer->Stop();
	m_busy = false;
	m_stop = false;
	SendEndMsg();
}

void ShellCommand::DoSetWorkingDirectory(ProjectPtr proj, bool isCustom, bool isFileOnly)
{
	//when using custom build, user can select different working directory
	if (proj) {
		if (isCustom) {
			//first set the path to the project working directory
			::wxSetWorkingDirectory(proj->GetFileName().GetPath());
			
			BuildConfigPtr buildConf = WorkspaceST::Get()->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
			if (buildConf) {
				wxString wd = buildConf->GetCustomBuildWorkingDir();
				if (wd.IsEmpty()) {
					// use the project path
					wd = proj->GetFileName().GetPath();
				} else {
					// expand macros from path
					wd = ExpandAllVariables(wd, WorkspaceST::Get(), proj->GetName(), buildConf->GetName(), wxEmptyString);
				}
				::wxSetWorkingDirectory(wd);
			}
		} else {
			if(m_info.GetProjectOnly() || isFileOnly) {
				//first set the path to the project working directory
				::wxSetWorkingDirectory(proj->GetFileName().GetPath());
			}
		}
	}
}
