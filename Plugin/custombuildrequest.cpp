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
#include <wx/app.h>
#include <wx/log.h>
#include "buildmanager.h"
#include "asyncprocess.h"
#include "imanager.h"
#include <wx/ffile.h>
#include "environmentconfig.h"
#include "globals.h"
#include "dirsaver.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "custombuildrequest.h"
#include "workspace.h"
#include "plugin.h"
#include "macros.h"

CustomBuildRequest::CustomBuildRequest(wxEvtHandler *owner, const QueueCommand &buildInfo, const wxString &fileName)
		: ShellCommand(owner, buildInfo)
		, m_fileName(fileName)
{
}

CustomBuildRequest::~CustomBuildRequest()
{
}

void CustomBuildRequest::Process(IManager *manager)
{
	wxString  cmd;
	wxString  errMsg;
	StringMap om;

	BuildSettingsConfig *bsc ( manager->GetBuildSettingsConfigManager() );
	Workspace *          w   ( manager->GetWorkspace()                  );
	EnvironmentConfig *  env ( manager->GetEnv()                        );

	ProjectPtr proj = w->FindProjectByName(m_info.GetProject(), errMsg);
	if (!proj) {
		AppendLine(_("Cant find project: ") + m_info.GetProject());
		return;
	}

	// Notify plugins that a compile process is going to start
	wxCommandEvent event(wxEVT_BUILD_STARTING);

	wxString pname (proj->GetName());
	event.SetClientData((void*)&pname);
	event.SetString( m_info.GetConfiguration() );

	// since this code can be called from inside the application OR
	// from inside a DLL, we use the application pointer from the manager
	// when available, otherwise, events will not be processed inside
	// plugins
	wxApp *app = manager ? manager->GetTheApp() : wxTheApp;
	app->ProcessEvent(event);

	if (app->ProcessEvent(event)) {

		// the build is being handled by some plugin, no need to build it
		// using the standard way
		return;
	}

	SendStartMsg();

	//TODO:: make the builder name configurable
	BuildConfigPtr bldConf = w->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
	if ( !bldConf ) {
		wxLogMessage(wxString::Format(wxT("Failed to find build configuration for project '%s' and configuration '%s'"), m_info.GetProject().c_str(), m_info.GetConfiguration().c_str()));
		return;
	}

	wxString cmpType = bldConf->GetCompilerType();
	CompilerPtr cmp = bsc->GetCompiler(cmpType);
	if (cmp) {
		wxString value( cmp->GetPathVariable() );
		if (value.Trim().Trim(false).IsEmpty() == false) {
			wxLogMessage(wxString::Format(wxT("Setting PATH to '%s'"), value.c_str()));
			om[wxT("PATH")] = value.Trim().Trim(false);
		}
	}

	// try the special targets first:
	bool isClean(false);
	if (m_info.GetCustomBuildTarget() == wxT("Build")) {
		cmd = bldConf->GetCustomBuildCmd();

	} else if (m_info.GetCustomBuildTarget() == wxT("Clean")) {
		cmd = bldConf->GetCustomCleanCmd();
		isClean = true;
	} else if ( m_info.GetCustomBuildTarget() == wxT("Rebuild")) {
		cmd = bldConf->GetCustomRebuildCmd();

	} else if (m_info.GetCustomBuildTarget() == wxT("Compile Single File")) {
		cmd = bldConf->GetSingleFileBuildCommand();

	} else if (m_info.GetCustomBuildTarget() == wxT("Preprocess File")) {
		cmd = bldConf->GetPreprocessFileCommand();
	}

	// if still no luck, try with the other custom targets
	if (cmd.IsEmpty()) {
		std::map<wxString, wxString> targets = bldConf->GetCustomTargets();
		std::map<wxString, wxString>::iterator iter = targets.find(m_info.GetCustomBuildTarget());
		if (iter != targets.end()) {
			cmd = iter->second;
		}
	}

	if (cmd.IsEmpty()) {
		//if we got an error string, use it
		if (errMsg.IsEmpty() == false) {
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

	//first set the path to the project working directory
	::wxSetWorkingDirectory(proj->GetFileName().GetPath());

	// If a working directory was specified, use it instead
	wxString wd = bldConf->GetCustomBuildWorkingDir();
	wd.Trim().Trim(false);

	wxString filename;
	if(manager->GetActiveEditor()) {
		filename = manager->GetActiveEditor()->GetFileName().GetFullPath();
	}

	if (wd.IsEmpty()) {

		// use the project path
		wd = proj->GetFileName().GetPath();

	} else {

		// expand macros from the working directory
		wd = ExpandAllVariables(wd,
								WorkspaceST::Get(),
								proj->GetName(),
								bldConf->GetName(),
								filename);
	}

	::wxSetWorkingDirectory(wd);

	// Print message to the build tab
	AppendLine(wxString::Format(_("MESSAGE: Entering directory `%s'\n"), wd.c_str()));

	// Command handling:
	//////////////////////////////////////////////////////

	//expand the variables of the command
	cmd = ExpandAllVariables(cmd, w, m_info.GetProject(), m_info.GetConfiguration(), filename);

	// in case our configuration includes post/pre build commands
	// we generate a makefile to include them as well and we update
	// the build command
	DoUpdateCommand(manager, cmd, proj, bldConf, isClean);

#ifdef __WXMSW__
	// Windows CD command requires the paths to be backslashe
	if(cmd.Find(wxT("cd ")) != wxNOT_FOUND)
		cmd.Replace(wxT("/"), wxT("\\"));
#endif

	// Wrap the build command in the shell, so it will be able
	// to perform 'chain' commands like
	// cd SOMEWHERE && make && ...
	WrapInShell(cmd);

	//print the build command
	AppendLine(cmd + wxT("\n"));
	wxString configName(m_info.GetConfiguration());

	//also, send another message to the main frame, indicating which project is being built
	//and what configuration
	wxString text;
	if (isClean) {
		text << wxGetTranslation(CLEAN_PROJECT_PREFIX);
	} else {
		text << wxGetTranslation(BUILD_PROJECT_PREFIX);
	}
	text << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]----------\n");

	AppendLine(text);
	EnvSetter environment(env, &om);

	m_proc = CreateAsyncProcess(this, cmd);
	if ( !m_proc ) {
		wxString message;
		message << _("Failed to start build process, command: ") << cmd << _(", process terminated with exit code: 0");
		AppendLine(message);
		return;
	}
}

void CustomBuildRequest::DoUpdateCommand(IManager *manager, wxString& cmd, ProjectPtr proj, BuildConfigPtr bldConf, bool isClean)
{
	BuildCommandList preBuildCmds, postBuildCmds;
	wxArrayString pre, post;
	bldConf->GetPreBuildCommands(preBuildCmds);
	bldConf->GetPostBuildCommands(postBuildCmds);

	BuildManager *bm(manager ? manager->GetBuildManager() : BuildManagerST::Get());

	// collect all enabled commands
	BuildCommandList::iterator iter = preBuildCmds.begin();
	for (; iter != preBuildCmds.end(); iter ++) {
		BuildCommand command = *iter;
		if (command.GetEnabled()) {
			pre.Add(command.GetCommand());
		}
	}

	iter = postBuildCmds.begin();
	for (; iter != postBuildCmds.end(); iter ++) {
		BuildCommand command = *iter;
		if (command.GetEnabled()) {
			post.Add(command.GetCommand());
		}
	}

	if (pre.empty() && post.empty()) {
		return;
	}

	// we need to create a makefile which includes all the pre-build, the actual build command and the post-build commands
	// (in this exact order).

	wxString makefile;
	makefile << wxT(".PHONY: all\n");
	makefile << wxT("all:\n");

	if (pre.IsEmpty() == false && !isClean) {
		makefile << wxT("\t@echo Executing Pre Build commands ...\n");
		for (size_t i=0; i<pre.GetCount(); i++) {
			makefile << wxT("\t@") << pre.Item(i) << wxT("\n");
		}
		makefile << wxT("\t@echo Done\n");
	}

	// add the command
	makefile << wxT("\t@") << cmd << wxT("\n");

	if (post.IsEmpty() == false && !isClean) {
		makefile << wxT("\t@echo Executing Post Build commands ...\n");
		for (size_t i=0; i<post.GetCount(); i++) {
			makefile << wxT("\t@") << post.Item(i) << wxT("\n");
		}
		makefile << wxT("\t@echo Done\n");
	}

	// write the makefile
	wxFFile output;
	wxString fn;

	fn << proj->GetName() << wxT(".mk");

	output.Open(fn, wxT("w+"));
	if (output.IsOpened()) {
		output.Write( makefile );
		output.Close();
	}

	wxString buildTool = bm->GetSelectedBuilder()->GetBuildToolCommand(true);
	buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);

	cmd.Clear();
	cmd << buildTool << wxT(" \"") << fn << wxT("\"");
}
