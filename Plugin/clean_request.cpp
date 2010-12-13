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
#include <wx/app.h>
#include <wx/log.h>
#include "build_settings_config.h"
#include "asyncprocess.h"
#include "imanager.h"
#include "macros.h"
#include "compiler.h"
 #include "clean_request.h"
#include "environmentconfig.h"
#include "globals.h"
#include "buildmanager.h"
#include "wx/process.h"
#include "dirsaver.h"
#include "workspace.h"
#include "plugin.h"

CleanRequest::CleanRequest(wxEvtHandler *owner, const QueueCommand &info)
		: ShellCommand(owner, info)
{
}

CleanRequest::~CleanRequest()
{
	//no need to delete the process, it will be deleted by the wx library
}


//do the actual cleanup
void CleanRequest::Process(IManager *manager)
{
	wxString cmd;
	wxString errMsg;
	StringMap om;

	BuildSettingsConfig *bsc(manager ? manager->GetBuildSettingsConfigManager() : BuildSettingsConfigST::Get());
	BuildManager *       bm(manager ? manager->GetBuildManager() : BuildManagerST::Get());
	Workspace *          w(manager ? manager->GetWorkspace() : WorkspaceST::Get());
	wxApp *              app = manager ? manager->GetTheApp() : wxTheApp;


	ProjectPtr proj = w->FindProjectByName(m_info.GetProject(), errMsg);
	if (!proj) {
		AppendLine(wxT("Cant find project: ") + m_info.GetProject());
		return;
	}
	wxString             pname (proj->GetName());

	//BuilderPtr builder = bm->GetBuilder(wxT("GNU makefile for g++/gcc"));
	BuilderPtr builder = bm->GetSelectedBuilder();
	if (m_info.GetProjectOnly()) {
		cmd = builder->GetPOCleanCommand(m_info.GetProject(), m_info.GetConfiguration());
	} else {
		cmd = builder->GetCleanCommand(m_info.GetProject(), m_info.GetConfiguration());
	}

	if ( cmd.IsEmpty() ) {
		AppendLine(wxT("Sorry, there is no 'Clean' command available\n"));
		return;
	}

	BuildConfigPtr bldConf = w->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
	if(bldConf) {
		wxString cmpType = bldConf->GetCompilerType();
		CompilerPtr cmp = bsc->GetCompiler(cmpType);
		if(cmp) {
			wxString value( cmp->GetPathVariable() );
			if(value.Trim().Trim(false).IsEmpty() == false) {
				wxLogMessage(wxString::Format(wxT("Setting PATH to '%s'"), value.c_str()));
				om[wxT("PATH")] = value.Trim().Trim(false);
			}
		}
	} else {
		AppendLine(wxT("Sorry, couldn't find the Build configuration\n"));
		return;
	}

	// Notify plugins that a compile process is going to start
	wxCommandEvent event(wxEVT_BUILD_STARTING);
	event.SetClientData((void*)&pname);
	event.SetString( m_info.GetConfiguration() );

	if (app->ProcessEvent(event)) {

		// the build is being handled by some plugin, no need to build it
		// using the standard way
		return;
	}
	SendStartMsg();

	//expand the variables of the command
	cmd = ExpandAllVariables(cmd, w, m_info.GetProject(), m_info.GetConfiguration(), wxEmptyString);
	WrapInShell(cmd);
	DirSaver ds;
	DoSetWorkingDirectory(proj, false, false);

	if (m_info.GetProjectOnly() ) {
		//need to change directory to project dir
		wxSetWorkingDirectory(proj->GetFileName().GetPath());
	}
	//print the build command
	AppendLine(cmd + wxT("\n"));

	// print the prefix message of the build start. This is important since the parser relies
	// on this message
	if(m_info.GetProjectOnly()){
		wxString configName(m_info.GetConfiguration());

		//also, send another message to the main frame, indicating which project is being built
		//and what configuration
		wxString text;
		text << wxGetTranslation(CLEAN_PROJECT_PREFIX) << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]");
		text << wxT("----------\n");
		AppendLine(text);
	}

	//apply environment settings
	EnvSetter env(&om);
	m_proc = CreateAsyncProcess(this, cmd);
	if ( !m_proc ) {

		//remove environment settings applied
		wxString message;
		message << wxT("Failed to start clean process, command: ") << cmd << wxT(", process terminated with exit code: 0");
		AppendLine(message);
		return;
	}
}
