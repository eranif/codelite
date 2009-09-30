#include "quickfinder.h"
#include "windowattrmanager.h"
#include <wx/choicdlg.h>
#include "workspace.h"
#include "imanager.h"
#include <set>

IManager *QuickFinder::m_manager = NULL;

void QuickFinder::Initialize(IManager* manager)
{
	m_manager = manager;
}

bool QuickFinder::OpenWorkspaceFile(const wxString& s)
{
	// sanity
	if ( !m_manager                   ) {
		return false;
	}
	if ( !m_manager->IsWorkspaceOpen()) {
		return false;
	}

	// get list of files from the workspace
	wxArrayString files, projects;
	wxString      errMsg;
	std::vector<wxFileName> workspaceFileNames;
	m_manager->GetWorkspace()->GetProjectList(projects);

	for (size_t i=0; i<projects.GetCount(); i++) {
		ProjectPtr p = m_manager->GetWorkspace()->FindProjectByName(projects.Item(i), errMsg);
		if ( p ) {
			p->GetFiles(workspaceFileNames, true);
		}
	}

	// remove duplicate files from teh list
	std::set<wxString> uniqueFileList;
	for ( size_t i=0; i<workspaceFileNames.size(); i++) {
		uniqueFileList.insert(workspaceFileNames.at(i).GetFullPath());
	}

	wxString match_pattern ( s );
	match_pattern.Trim().Trim(false);

	if ( match_pattern.EndsWith(wxT("*")) == false ) {
		match_pattern.Append(wxT("*"));
	}

	if ( match_pattern.StartsWith(wxT("*")) == false ) {
		match_pattern.Prepend(wxT("*"));
	}
	// loop over the set and search for match

	std::set<wxString>::iterator iter = uniqueFileList.begin();
	for (; iter != uniqueFileList.end(); iter++) {
		if ( wxMatchWild( match_pattern, wxFileName((*iter)).GetFullName() ) ) {
			files.Add( (*iter) );
		}
	}

	wxString file_name;
	if ( files.GetCount() > 1 ) {
		wxSingleChoiceDialog dlg(m_manager->GetTheApp()->GetTopWindow(), wxT("Select a file"), wxT("Choose a file"), files);
		WindowAttrManager::Load(&dlg, wxT("OpenFileSingleChoiceDlg"), m_manager->GetConfigTool());
		if (dlg.ShowModal() == wxID_OK) {
			file_name = dlg.GetStringSelection();
			if ( file_name.IsEmpty() ) {
				// user clicked 'cancel'
				WindowAttrManager::Save(&dlg, wxT("OpenFileSingleChoiceDlg"), m_manager->GetConfigTool());
				return false;
			}
		}
		WindowAttrManager::Save(&dlg, wxT("OpenFileSingleChoiceDlg"), m_manager->GetConfigTool());

	} else if ( files.GetCount() == 1 ) {
		file_name = files.Item(0);
	}

	if ( file_name.IsEmpty() == false ) {
		return m_manager->OpenFile( file_name );
	}
	return false;
}
