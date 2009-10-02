#include "quickfinder.h"
#include "quickfinder_selectsymbol.h"
#include <wx/msgdlg.h>
#include "ctags_manager.h"
#include "windowattrmanager.h"
#include <wx/choicdlg.h>
#include "workspace.h"
#include "imanager.h"
#include <set>

IManager *QuickFinder::m_manager = NULL;

static wxString format_pattern(const wxString &s)
{
	wxString match_pattern ( s );
	match_pattern.Trim().Trim(false);

	if ( match_pattern.EndsWith(wxT("*")) == false ) {
		match_pattern.Append(wxT("*"));
	}

	if ( match_pattern.StartsWith(wxT("*")) == false ) {
		match_pattern.Prepend(wxT("*"));
	}

	match_pattern.Trim().Trim(false);
	match_pattern.MakeLower();
	return match_pattern;
}

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

	wxString match_pattern  = format_pattern( s );
	// loop over the set and search for match

	std::set<wxString>::iterator iter = uniqueFileList.begin();
	for (; iter != uniqueFileList.end(); iter++) {
		wxString name = wxFileName((*iter)).GetFullName();
		name.MakeLower();
		if ( wxMatchWild( match_pattern, name ) ) {
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

void QuickFinder::FocusActiveEditor()
{
	IEditor *editor = m_manager->GetActiveEditor();
	if ( m_manager && editor ) {
		m_manager->OpenFile(editor->GetFileName().GetFullPath(), editor->GetProjectName(), editor->GetCurrentLine());
	}
}

bool QuickFinder::FilterAndDisplayTags(const std::vector<TagEntryPtr>& tags, const wxString& matchPattern)
{
	std::vector<TagEntryPtr> matches;

	// filter non interesting tags
	for (size_t i=0; i<tags.size(); i++) {
		TagEntryPtr t = tags.at(i);
		wxString name ( t->GetName() );
		name.MakeLower();
		if ( wxMatchWild(matchPattern, name) ) {
			matches.push_back(t);
		}
	}

	if ( matches.empty() ) {
		return false;
	}

	if ( matches.size() > 150 ) {
		wxMessageBox( wxT("Too many matches were found, please narrow down your search string"), wxT("QuickFinder"));
		return true;
	}

	// display to user
	QuickfinderSelect dlg(m_manager->GetTheApp()->GetTopWindow(), matches);
	WindowAttrManager::Load(&dlg, wxT("QuickfinderSelect"), m_manager->GetConfigTool());

	int rc = dlg.ShowModal();
	WindowAttrManager::Save(&dlg, wxT("QuickfinderSelect"), m_manager->GetConfigTool());
	if ( rc == wxID_OK ) {
		if ( m_manager->OpenFile(dlg.selection.GetFile(), wxEmptyString, dlg.selection.GetLine()) ) {
			IEditor *editor = m_manager->GetActiveEditor();
			if ( editor ) {
				editor->FindAndSelect(dlg.selection.GetPattern(), dlg.selection.GetName(), m_manager->GetNavigationMgr());
			}
		}
	}
	return true;
}

bool QuickFinder::OpenType(const wxString& s, const wxArrayString& kind)
{
	if (!m_manager) {
		wxMessageBox(wxT("You need an open workspace"), wxT("QuickFinder"));
		return false;
	}
	std::vector<TagEntryPtr> tags;
	tags.clear();
	m_manager->GetTagsManager()->GetTagsByKind(tags, kind);
	if ( tags.empty() ) {
		return false;
	}

	wxString match_pattern  = format_pattern( s );
	return FilterAndDisplayTags (tags, match_pattern);
}
