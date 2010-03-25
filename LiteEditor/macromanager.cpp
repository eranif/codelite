#include "macromanager.h"
#include "environmentconfig.h"
#include "build_config.h"
#include "project.h"
#include "workspace.h"
#include "imanager.h"

MacroManager* MacroManager::ms_instance = 0;

MacroManager::MacroManager()
{
}

MacroManager::~MacroManager()
{
}

MacroManager* MacroManager::Instance()
{
	if(ms_instance == 0){
		ms_instance = new MacroManager();
	}
	return ms_instance;
}

void MacroManager::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

wxString MacroManager::Expand(const wxString& expression, IManager* manager, const wxString& project, const wxString &confToBuild)
{
	wxString   errMsg;
	wxString   expandedString(expression);
	Workspace *workspace = manager->GetWorkspace();

	if ( workspace ) {
		expandedString.Replace(wxT("$(WorkspaceName)"), workspace->GetName());
		ProjectPtr proj = workspace->FindProjectByName(project, errMsg);
		if (proj) {
			wxString project_name(proj->GetName());

			//make sure that the project name does not contain any spaces
			project_name.Replace(wxT(" "), wxT("_"));

			BuildConfigPtr bldConf = workspace->GetProjBuildConf(proj->GetName(), confToBuild);
			if (bldConf) {
				expandedString.Replace(wxT("$(ProjectOutputFile)"), bldConf->GetOutputFileName());
			}

			expandedString.Replace(wxT("$(ProjectPath)"), proj->GetFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
			expandedString.Replace(wxT("$(WorkspacePath)"), workspace->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
			expandedString.Replace(wxT("$(ProjectName)"), project_name);

			if (bldConf) {
				expandedString.Replace(wxT("$(IntermediateDirectory)"), bldConf->GetIntermediateDirectory());
				expandedString.Replace(wxT("$(ConfigurationName)"), bldConf->GetName());
				expandedString.Replace(wxT("$(OutDir)"), bldConf->GetIntermediateDirectory());
			}

			if(expandedString.Find(wxT("$(ProjectFiles)")) != wxNOT_FOUND)
				expandedString.Replace(wxT("$(ProjectFiles)"),   proj->GetFiles());

			if(expandedString.Find(wxT("$(ProjectFilesAbs)")) != wxNOT_FOUND)
				expandedString.Replace(wxT("$(ProjectFilesAbs)"),proj->GetFiles(true));


		}
	}

	IEditor *editor = manager->GetActiveEditor();

	if (editor) {
		wxFileName fn(editor->GetFileName());

		expandedString.Replace(wxT("$(CurrentFileName)"), fn.GetName());

		wxString fpath(fn.GetPath());
		fpath.Replace(wxT("\\"), wxT("/"));
		expandedString.Replace(wxT("$(CurrentFilePath)"), fpath);
		expandedString.Replace(wxT("$(CurrentFileExt)"), fn.GetExt());

		wxString ffullpath(fn.GetFullPath());
		ffullpath.Replace(wxT("\\"), wxT("/"));
		expandedString.Replace(wxT("$(CurrentFileFullPath)"), ffullpath);
		expandedString.Replace(wxT("$(CurrentSelection)"), editor->GetSelection());
		if(expandedString.Find(wxT("$(CurrentSelectionRange)")) != wxNOT_FOUND)
		{
			int start=editor->GetSelectionStart(),
			    end  =editor->GetSelectionEnd();

			wxString output=wxString::Format(wxT("%i:%i"),start,end);
			expandedString.Replace(wxT("$(CurrentSelectionRange)"), output);
		}
	}

	//exapand common macros
	wxDateTime now = wxDateTime::Now();
	expandedString.Replace(wxT("$(User)"), wxGetUserName());
	expandedString.Replace(wxT("$(Date)"), now.FormatDate());

	if (manager) {
		expandedString.Replace(wxT("$(CodeLitePath)"), manager->GetInstallDirectory());
	}

	//call the environment & workspace variables expand function
	expandedString = manager->GetEnv()->ExpandVariables(expandedString, true);
	return expandedString;
}
