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
#include "compiler.h"
#include "build_settings_config.h"
#include "globals.h"
#include "build_config.h"
#include "environmentconfig.h"
#include "buildmanager.h"
#include "wx/process.h"
#include "workspace.h"
#include "dirsaver.h"

CompileRequest::CompileRequest(wxEvtHandler *owner, const BuildInfo &buildInfo, const wxString &fileName, bool runPremakeOnly)
		: CompilerAction(owner, buildInfo)
		, m_fileName(fileName)
		, m_premakeOnly(runPremakeOnly)
{
}

CompileRequest::~CompileRequest()
{
	//no need to delete the process, it will be deleted by the wx library
}

//do the actual cleanup
void CompileRequest::Process()
{
	wxString cmd;
	wxString errMsg;
	SetBusy(true);
	StringMap om;

	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(m_info.GetProject(), errMsg);
	if (!proj) {
		AppendLine(wxT("Cant find project: ") + m_info.GetProject());
		SetBusy(false);
		return;
	}

	//TODO:: make the builder name configurable
	bool isCustom(false);
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder(wxT("GNU makefile for g++/gcc"));
	if (m_fileName.IsEmpty() == false) {
		//we got a complie request of a single file
		cmd = builder->GetSingleFileCmd(m_info.GetProject(), m_info.GetConfiguration(), m_fileName, isCustom, errMsg);
	} else if (m_info.GetProjectOnly()) {
		cmd = builder->GetPOBuildCommand(m_info.GetProject(), m_info.GetConfiguration(), isCustom);
	} else {
		cmd = builder->GetBuildCommand(m_info.GetProject(), m_info.GetConfiguration(), isCustom);
	}

	SendStartMsg();
	if (!isCustom && m_premakeOnly) {
		AppendLine(wxT("Empty makefile generation command (see Project Settings -> Custom Build)"));
		return;
	}

	//if we require to run the makefile generation command only, replace the 'cmd' with the
	//generation command line
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
	if (m_premakeOnly && bldConf) {
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
		if (bldConf) {
			cmd = bldConf->GetMakeGenerationCommand();
		}
		
	}
	
	if(bldConf) {
		wxString cmpType = bldConf->GetCompilerType();
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
		if(cmp) {
			wxString value( cmp->GetPathVariable() );
			if(value.Trim().Trim(false).IsEmpty() == false) {
				om[wxT("PATH")] = value.Trim().Trim(false);
			}
		}
	}
	
	if (cmd.IsEmpty()) {
		//if we got an error string, use it
		if (errMsg.IsEmpty() == false) {
			AppendLine(errMsg);
		} else {
			AppendLine(wxT("Command line is empty. Build aborted."));
		}
		SetBusy(false);
		return;
	}

	m_proc = new clProcess(wxNewId(), cmd);
	if (m_proc) {
		DirSaver ds;

		//when using custom build, user can select different working directory
		if (isCustom) {
			//first set the path to the project working directory
			::wxSetWorkingDirectory(proj->GetFileName().GetPath());
			BuildConfigPtr buildConf = WorkspaceST::Get()->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
			if (buildConf) {
				wxString wd = buildConf->GetCustomBuildWorkingDir();
				if (wd.IsEmpty()) {
					wd = proj->GetFileName().GetPath();
				}

				::wxSetWorkingDirectory(wd);
			}
		}

		//expand the variables of the command
		cmd = ExpandAllVariables(cmd, WorkspaceST::Get(), m_info.GetProject(), m_info.GetConfiguration(), m_fileName);
		
		//replace the command line
		m_proc->SetCommand(cmd);
		
		//print the build command
		AppendLine(cmd + wxT("\n"));

		if (m_info.GetProjectOnly() || m_fileName.IsEmpty() == false) {
			//need to change directory to project dir
			::wxSetWorkingDirectory(proj->GetFileName().GetPath());

			wxString configName;
			int where = cmd.Find(wxT("type="));
			if (where != wxNOT_FOUND) {
				configName = cmd.Mid((size_t)where);
				//skip type=
				configName = configName.AfterFirst(wxT('='));
				configName = configName.BeforeFirst(wxT(' '));
			}

			//also, send another message to the main frame, indicating which project is being built
			//and what configuration
			wxString text;
			text << wxT("----------Building project:[ ") << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]");
			if (m_fileName.IsEmpty() == false) {
				text << wxT(" (Single File Build)----------\n");
			} else {
				text << wxT("----------\n");
			}
			AppendLine(text);
		}
	
		EnvironmentConfig::Instance()->ApplyEnv( &om );
		if (m_proc->Start() == 0) {
			wxString message;
			message << wxT("Failed to start build process, command: ") << cmd << wxT(", process terminated with exit code: 0");
			EnvironmentConfig::Instance()->UnApplyEnv();
			AppendLine(message);
			delete m_proc;
			SetBusy(false);
			return;
		}

		m_timer->Start(10);
		Connect(wxEVT_TIMER, wxTimerEventHandler(CompileRequest::OnTimer), NULL, this);
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(CompileRequest::OnProcessEnd), NULL, this);
	}
}
