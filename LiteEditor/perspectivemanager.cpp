#include "perspectivemanager.h"
#include "frame.h"
#include <wx/stdpaths.h>
#include <wx/aui/framemanager.h>
#include "globals.h"

wxString DEBUG_LAYOUT  = wxT("debug.layout");
wxString NORMAL_LAYOUT = wxT("normal.layout");

PerspectiveManager::PerspectiveManager()
{
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
	wxString file;
	file << wxStandardPaths::Get().GetUserDataDir() << wxT("/config/") << name;

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

void PerspectiveManager::SavePerspective(const wxString& name)
{
	wxString file;
	file << wxStandardPaths::Get().GetUserDataDir() << wxT("/config/") << name;
	WriteFileWithBackup(file, clMainFrame::Get()->GetDockingManager().SavePerspective(), false);
}

wxArrayString PerspectiveManager::GetAllPerspectives()
{
	wxArrayString files, perspectives;
	wxDir::GetAllFiles(wxStandardPaths::Get().GetUserDataDir() + wxT("/config/"), &files, wxT("*.layout"));

	for(size_t i=0; i<files.GetCount(); i++) {
		wxFileName fn(files.Item(i));
		perspectives.Add(fn.GetFullName());
	}
	return perspectives;
}
