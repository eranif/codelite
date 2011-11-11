#include "perspectivemanager.h"
#include "frame.h"
#include <wx/stdpaths.h>
#include <wx/aui/framemanager.h>
#include "globals.h"

wxString DEBUG_LAYOUT  = wxT("debug.layout");
wxString NORMAL_LAYOUT = wxT("default.layout");

PerspectiveManager::PerspectiveManager()
{
	ClearIds();
}

PerspectiveManager::~PerspectiveManager()
{
}

void PerspectiveManager::DeleteAllPerspectives()
{
	wxArrayString files;
	wxDir::GetAllFiles(wxStandardPaths::Get().GetUserDataDir() + wxT("/config/"), &files, wxT("*.layout"));

	wxLogNull noLog;
	for(size_t i=0; i<files.GetCount(); i++) {
		wxRemoveFile(files.Item(i));
	}
}

void PerspectiveManager::LoadPerspective(const wxString& name)
{
	wxString file = DoGetPathFromName(name);
	
	wxString content;
	if(ReadFileWithConversion(file, content)) {
		clMainFrame::Get()->GetDockingManager().LoadPerspective(content, true);

	} else {
		if(name == DEBUG_LAYOUT) {
			// First time, make sure that the debugger pane is visible
			wxAuiPaneInfo &info = clMainFrame::Get()->GetDockingManager().GetPane(wxT("Debugger"));
			if(info.IsOk() && !info.IsShown()) {
				info.Show();
				clMainFrame::Get()->GetDockingManager().Update();
			}
		}
	}
}

void PerspectiveManager::SavePerspective(const wxString& name, bool notify)
{
	WriteFileWithBackup(DoGetPathFromName(name), 
						clMainFrame::Get()->GetDockingManager().SavePerspective(),
						false);

	if(notify) {
		wxCommandEvent evt(wxEVT_REFRESH_PERSPECTIVE_MENU);
		clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evt);
	}
}

wxArrayString PerspectiveManager::GetAllPerspectives()
{
	wxArrayString files, perspectives;
	wxDir::GetAllFiles(wxStandardPaths::Get().GetUserDataDir() + wxT("/config/"), &files, wxT("*.layout"));

	for(size_t i=0; i<files.GetCount(); i++) {
		wxFileName fn(files.Item(i));
		wxString name = fn.GetName();
#if wxVERSION_NUMBER >= 2900
		name = name.Capitalize();
#else
		name.MakeLower();
		wxString start  = name.Mid(0, 1);
		start.MakeUpper();
		name[0] = start[0];
#endif
		perspectives.Add(name);
	}
	return perspectives;
}

void PerspectiveManager::ClearIds()
{
	m_menuIdToName.clear();
	m_nextId = FirstMenuId();
}

int PerspectiveManager::MenuIdFromName(const wxString& name)
{
	std::map<wxString, int>::iterator iter = m_menuIdToName.find(name);
	if(iter == m_menuIdToName.end()) {
		m_menuIdToName[name] = ++m_nextId;
		return m_menuIdToName[name];
	}
	return iter->second;
}

wxString PerspectiveManager::NameFromMenuId(int id)
{
	std::map<wxString, int>::iterator iter = m_menuIdToName.begin();
	for(; iter != m_menuIdToName.end(); iter++) {
		if(iter->second == id) {
			return iter->first;
		}
	}
	return wxT("");
}

void PerspectiveManager::LoadPerspectiveByMenuId(int id)
{
	wxString name = NameFromMenuId(id);
	if(name.IsEmpty())
		return;

	LoadPerspective(name);
}

void PerspectiveManager::Delete(const wxString& name)
{
	wxLogNull noLog;
	wxString path = DoGetPathFromName(name);
	wxRemoveFile(path);
}

void PerspectiveManager::Rename(const wxString& old, const wxString& new_name)
{
	wxString oldPath = DoGetPathFromName(old);
	wxString newPath = DoGetPathFromName(new_name);

	wxLogNull noLog;
	wxRename(oldPath, newPath);
}

wxString PerspectiveManager::DoGetPathFromName(const wxString& name)
{
	wxString file;
	wxString filename = name;
	
	filename.MakeLower();
	if(!filename.EndsWith(wxT(".layout"))) {
		filename << wxT(".layout");
	}

	file << wxStandardPaths::Get().GetUserDataDir() << wxT("/config/") << filename;
	return file;
}

