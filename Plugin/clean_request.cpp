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
#include "build_settings_config.h"
#include "compiler.h"
 #include "clean_request.h"
#include "environmentconfig.h"
#include "globals.h"
#include "buildmanager.h"
#include "wx/process.h"
#include "dirsaver.h"
#include "workspace.h"

CleanRequest::CleanRequest(wxEvtHandler *owner, const QueueCommand &info)
		: ShellCommand(owner, info)
{
}

CleanRequest::~CleanRequest()
{
	//no need to delete the process, it will be deleted by the wx library
}


//do the actual cleanup
void CleanRequest::Process()
{
	wxString cmd;
	wxString errMsg;
	StringMap om;
	
	SetBusy(true);
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(m_info.GetProject(), errMsg);
	if (!proj) {
		AppendLine(wxT("Cant find project: ") + m_info.GetProject());
		SetBusy(false);
		return;
	}
	
	bool isCustom(false);
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder(wxT("GNU makefile for g++/gcc"));
	if (m_info.GetProjectOnly()) {
		cmd = builder->GetPOCleanCommand(m_info.GetProject(), m_info.GetConfiguration(), isCustom);
	} else {
		cmd = builder->GetCleanCommand(m_info.GetProject(), m_info.GetConfiguration(), isCustom);
	}

	if ( cmd.IsEmpty() ) {
		AppendLine(wxT("Sorry, there is no 'Clean' command available\n"));
		SetBusy(false);
		return;
	}

	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
	if(bldConf) {
		wxString cmpType = bldConf->GetCompilerType();
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
		if(cmp) {
			wxString value( cmp->GetPathVariable() );
			if(value.Trim().Trim(false).IsEmpty() == false) {
				wxLogMessage(wxString::Format(wxT("Setting PATH to '%s'"), value.c_str()));
				om[wxT("PATH")] = value.Trim().Trim(false);
			}
		}
	} else {
		AppendLine(wxT("Sorry, couldn't find the Build configuration\n"));
		SetBusy(false);
		return;
	}

	SendStartMsg();

	//expand the variables of the command
	cmd = ExpandAllVariables(cmd, WorkspaceST::Get(), m_info.GetProject(), m_info.GetConfiguration(), wxEmptyString);
	m_proc = new clProcess(wxNewId(), cmd);

	if (m_proc) {

		DirSaver ds;
		DoSetWorkingDirectory(proj, isCustom, false);
		
		if (m_info.GetProjectOnly() ) {
			//need to change directory to project dir
			wxSetWorkingDirectory(proj->GetFileName().GetPath());
		}
		//print the build command
		AppendLine(cmd + wxT("\n"));
		
		// print the prefix message of the build start. This is important since the parser relies 
		// on this message 
		if(isCustom || m_info.GetProjectOnly()){
			wxString configName(m_info.GetConfiguration());
			
			//also, send another message to the main frame, indicating which project is being built
			//and what configuration
			wxString text;
			text << wxT("----------Building project:[ ") << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]");
			text << wxT("----------\n");
			AppendLine(text);
		}
		
		//apply environment settings
		EnvironmentConfig::Instance()->ApplyEnv( &om );
		
		if (m_proc->Start() == 0) {
			
			//remove environment settings applied
			EnvironmentConfig::Instance()->UnApplyEnv();
			
			wxString message;
			message << wxT("Failed to start clean process, command: ") << cmd << wxT(", process terminated with exit code: 0");
			AppendLine(message);
			SetBusy(false);
			delete m_proc;
			return;
		}

		Connect(wxEVT_TIMER, wxTimerEventHandler(CleanRequest::OnTimer), NULL, this);
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(CleanRequest::OnProcessEnd), NULL, this);
		m_timer->Start(10);
	}
}
