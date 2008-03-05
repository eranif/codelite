#include "clean_request.h"
#include "buildmanager.h"
#include "wx/process.h"
#include "dirsaver.h"
#include "workspace.h"


static wxString ExpandAllVariables(const wxString &expression, const wxString &projectName, const wxString &fileName)
{
	wxString errMsg;
	wxString output(expression);
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(projectName, errMsg);
	if (proj) {
		wxString project_name(proj->GetName());

		//make sure that the project name does not contain any spaces
		project_name.Replace(wxT(" "), wxT("_"));

		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjSelBuildConf(proj->GetName());
		output.Replace(wxT("$(ProjectName)"), project_name);
		output.Replace(wxT("$(IntermediateDirectory)"), bldConf->GetIntermediateDirectory());
		output.Replace(wxT("$(ConfigurationName)"), bldConf->GetName());
		output.Replace(wxT("$(OutDir)"), bldConf->GetIntermediateDirectory());

		if (fileName.IsEmpty() == false) {
			wxFileName fn(fileName);

			output.Replace(wxT("$(CurrentFileName)"), fn.GetName());
			output.Replace(wxT("$(CurrentFilePath)"), fn.GetPath());
			output.Replace(wxT("$(CurrentFileExt)"), fn.GetExt());
			output.Replace(wxT("$(CurrentFileFullPath)"), fn.GetFullPath());
		}
	}

	//call the environment & workspace variables expand function
	output = WorkspaceST::Get()->ExpandVariables(output);
	return output;
}


CleanRequest::CleanRequest(wxEvtHandler *owner, const wxString &projectName, bool projectOnly)
		: CompilerAction(owner)
		, m_project(projectName)
		, m_projectOnly(projectOnly)
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
	SetBusy(true);

	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(m_project, errMsg);
	if (!proj) {
		AppendLine(wxT("Cant find project: ") + m_project);
		SetBusy(false);
		return;
	}

	bool isCustom(false);
	//TODO:: make the builder name configurable
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder(wxT("GNU makefile for g++/gcc"));
	if (m_projectOnly) {
		cmd = builder->GetPOCleanCommand(m_project, isCustom);
	} else {
		cmd = builder->GetCleanCommand(m_project, isCustom);
	}

	SendStartMsg();

	//expand the variables of the command
	cmd = ExpandAllVariables(cmd, m_project, wxEmptyString);
	m_proc = new clProcess(wxNewId(), cmd);

	if (m_proc) {

		DirSaver ds;

		if (isCustom) {
			//first set the path to the project working directory
			::wxSetWorkingDirectory(proj->GetFileName().GetPath());
			BuildConfigPtr buildConf = WorkspaceST::Get()->GetProjSelBuildConf(m_project);
			if (buildConf) {
				wxString wd = buildConf->GetCustomBuildWorkingDir();
				if (wd.IsEmpty()) {
					wd = proj->GetFileName().GetPath();
				}

				::wxSetWorkingDirectory(wd);
			}
		}

		if (m_projectOnly ) {
			//need to change directory to project dir
			wxSetWorkingDirectory(proj->GetFileName().GetPath());
		}

		if (m_proc->Start() == 0) {
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
