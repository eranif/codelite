#include "workspace.h"
#include "subversion.h"
#include "imanager.h"
#include "updatepostcmdaction.h"

UpdatePostCmdAction::UpdatePostCmdAction(IManager *mgr, SubversionPlugin *plugin)
		: SvnIconRefreshHandler(mgr, plugin)
{
}

UpdatePostCmdAction::~UpdatePostCmdAction()
{
}

void UpdatePostCmdAction::DoCommand()
{
	// call the parent command
	SvnIconRefreshHandler::DoCommand();

	if (m_mgr->IsWorkspaceOpen()) {
		// test to see if any of the modified files is a project / workspace file
		wxArrayString projects;
		wxString err_msg;
		wxArrayString projectsFiles;

		m_mgr->GetWorkspace()->GetProjectList(projects);
		for (size_t i=0; i<projects.GetCount(); i++) {
			ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), err_msg);
			if (p) {
				wxString proj = p->GetFileName().GetFullPath();
				projectsFiles.Add(proj);
			}
		}
		// add the workspace file name as well
		wxString wsp = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetFullPath();
		projectsFiles.Add(wsp);
		
		// parse the lines receieved from the svn output
		bool reload_workspace(false);
		for (size_t i=0; i<m_text.GetCount(); i++) {
			wxString file_name;
			if (	m_text.Item(i).StartsWith(wxT("U "), &file_name) ||
			        m_text.Item(i).StartsWith(wxT("G "), &file_name) ||
			        m_text.Item(i).StartsWith(wxT("_ "), &file_name) ||
			        m_text.Item(i).StartsWith(wxT("M "), &file_name) ||
			        m_text.Item(i).StartsWith(wxT("A "), &file_name)
			   ) {
				file_name = file_name.Trim().Trim(false);
				if (projectsFiles.Index(file_name) != wxNOT_FOUND) {
					reload_workspace = true;
					break;
				}
			}
		}

		if (reload_workspace) {
			if(wxMessageBox(wxT("Workspace settings have been modified, would you like to reload the workspace?"), wxT("CodeLite"), wxICON_QUESTION|wxYES_NO) == wxYES){
				m_mgr->ReloadWorkspace();
			}
		}
	}
}
