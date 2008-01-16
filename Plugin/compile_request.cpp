#include "compile_request.h"
#include "buildmanager.h"
#include "wx/process.h"
#include "workspace.h"
#include "dirsaver.h" 

CompileRequest::CompileRequest(wxEvtHandler *owner, const wxString &projectName, bool projectOnly, const wxString &fileName)
: CompilerAction(owner)
, m_project(projectName)
, m_projectOnly(projectOnly)
, m_fileName(fileName)
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
	SetBusy(true);

	//TODO:: make the builder name configurable
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder(wxT("GNU makefile for g++/gcc"));
	if(m_fileName.IsEmpty() == false){
		//we got a complie request of a single file
		cmd = builder->GetSingleFileCmd(m_project, m_fileName);
	}else if(m_projectOnly){
		cmd = builder->GetPOBuildCommand(m_project);
	}else{
		cmd = builder->GetBuildCommand(m_project);
	}

	SendStartMsg();
	if(cmd.IsEmpty()){
		AppendLine(wxT("Command line is empty. Build aborted."));
		SetBusy(false);
		return;
	}
	
	m_proc = new clProcess(wxNewId(), cmd);
	if(m_proc){
		DirSaver ds;
		if(m_projectOnly || m_fileName.IsEmpty() == false){
			//need to change directory to project dir
			wxString errMsg;
			ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(m_project, errMsg);
			if(proj){
				::wxSetWorkingDirectory(proj->GetFileName().GetPath());
			}
			
			wxString configName;
			int where = cmd.Find(wxT("type="));
			if(where != wxNOT_FOUND){
				configName = cmd.Mid((size_t)where);
				//skip type=
				configName = configName.AfterFirst(wxT('='));
				configName = configName.BeforeFirst(wxT(' '));
			}
			
			//also, send another message to the main frame, indicating which project is being built
			//and what configuration
			wxString text;
			text << wxT("----------Building project:[ ") << m_project << wxT(" - ") << configName << wxT(" ]");
			if(m_fileName.IsEmpty() == false){
				text << wxT(" (Single File Build)----------\n");
			}else{
				text << wxT("----------\n");
			}
			AppendLine(text);
		}
		if(m_proc->Start() == 0){
			wxString message;
			message << wxT("Failed to start build process, command: ") << cmd << wxT(", process terminated with exit code: 0");
			AppendLine(message);
			delete m_proc;
			SetBusy(false);
			return;
		}

		Connect(wxEVT_TIMER, wxTimerEventHandler(CompileRequest::OnTimer), NULL, this);
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(CompileRequest::OnProcessEnd), NULL, this);
		m_timer->Start(10);
	}
}
