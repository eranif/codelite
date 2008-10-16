#include "buildmanager.h"
#include <wx/ffile.h>
#include "environmentconfig.h"
#include "globals.h"
#include "dirsaver.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "custombuildrequest.h"
#include "workspace.h"
CustomBuildRequest::CustomBuildRequest(wxEvtHandler *owner, const QueueCommand &buildInfo, const wxString &fileName)
		: ShellCommand(owner, buildInfo)
		, m_fileName(fileName)
{
}

CustomBuildRequest::~CustomBuildRequest()
{
}

void CustomBuildRequest::Process()
{
	wxString cmd;
	wxString errMsg;
	SetBusy(true);
	StringMap om;

	SendStartMsg();

	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(m_info.GetProject(), errMsg);
	if (!proj) {
		AppendLine(wxT("Cant find project: ") + m_info.GetProject());
		SetBusy(false);
		return;
	}

	//TODO:: make the builder name configurable
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
	if ( !bldConf ) {
		wxLogMessage(wxString::Format(wxT("Failed to find build configuration for project '%s' and configuration '%s'"), m_info.GetProject().c_str(), m_info.GetConfiguration().c_str()));
		SetBusy(false);
		return;
	}

	wxString cmpType = bldConf->GetCompilerType();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
	if (cmp) {
		wxString value( cmp->GetPathVariable() );
		if (value.Trim().Trim(false).IsEmpty() == false) {
			wxLogMessage(wxString::Format(wxT("Setting PATH to '%s'"), value.c_str()));
			om[wxT("PATH")] = value.Trim().Trim(false);
		}
	}

	// try the special targets first:
	if (m_info.GetCustomBuildTarget() == wxT("Build")) {
		cmd = bldConf->GetCustomBuildCmd();
	} else if (m_info.GetCustomBuildTarget() == wxT("Clean")) {
		cmd = bldConf->GetCustomCleanCmd();
	} else if (m_info.GetCustomBuildTarget() == wxT("Compile Single File")) {
		cmd = bldConf->GetSingleFileBuildCommand();
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
			AppendLine(wxT("Command line is empty. Build aborted."));
		}
		SetBusy(false);
		return;
	}

	m_proc = new clProcess(wxNewId(), cmd);
	if (m_proc) {
		DirSaver ds;

		DoSetWorkingDirectory(proj, true, false);

		//expand the variables of the command
		cmd = ExpandAllVariables(cmd, WorkspaceST::Get(), m_info.GetProject(), m_info.GetConfiguration(), m_fileName);

		// in case our configuration includes post/pre build commands
		// we generate a makefile to include them as well and we update
		// the build command
		DoUpdateCommand(cmd, proj, bldConf);
		
		//replace the command line
		m_proc->SetCommand(cmd);

		//print the build command
		AppendLine(cmd + wxT("\n"));
		wxString configName(m_info.GetConfiguration());

		//also, send another message to the main frame, indicating which project is being built
		//and what configuration
		wxString text;
		text << wxT("----------Building project:[ ") << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]----------\n");

		AppendLine(text);
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
		Connect(wxEVT_TIMER, wxTimerEventHandler(CustomBuildRequest::OnTimer), NULL, this);
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(CustomBuildRequest::OnProcessEnd), NULL, this);
	}
}

void CustomBuildRequest::DoUpdateCommand(wxString& cmd, ProjectPtr proj, BuildConfigPtr bldConf)
{
	BuildCommandList preBuildCmds, postBuildCmds;
	wxArrayString pre, post;
	bldConf->GetPreBuildCommands(preBuildCmds);
	bldConf->GetPostBuildCommands(postBuildCmds);
	
	// collect all enabled commands
	BuildCommandList::iterator iter = preBuildCmds.begin();
	for(; iter != preBuildCmds.end(); iter ++){
		BuildCommand command = *iter;
		if(command.GetEnabled()){
			pre.Add(command.GetCommand());
		}
	}
	
	iter = postBuildCmds.begin();
	for(; iter != postBuildCmds.end(); iter ++){
		BuildCommand command = *iter;
		if(command.GetEnabled()){
			post.Add(command.GetCommand());
		}
	}
	
	if(pre.empty() && post.empty()){
		return;
	}
	
	// we need to create a makefile which includes all the pre-build, the actual build command and the post-build commands
	// (in this exact order).
	
	wxString makefile;
	makefile << wxT(".PHONY: all\n");
	makefile << wxT("all:\n");
	
	if(pre.IsEmpty() == false){
		makefile << wxT("\t@echo Executing Pre Build commands ...\n");
		for(size_t i=0; i<pre.GetCount(); i++){
			makefile << wxT("\t@") << pre.Item(i) << wxT("\n");
		}
		makefile << wxT("\t@echo Done\n");
	}
	
	// add the command
	makefile << wxT("\t@") << cmd << wxT("\n");
	
	if(post.IsEmpty() == false){
		makefile << wxT("\t@echo Executing Post Build commands ...\n");
		for(size_t i=0; i<post.GetCount(); i++){
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
	
	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(true);
	buildTool = WorkspaceST::Get()->ExpandVariables(buildTool);
	
	cmd.Clear();
	cmd << buildTool << wxT(" \"") << fn << wxT("\"");
}
