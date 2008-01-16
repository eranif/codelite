#include "fileexplorer.h"
#include "fileexplorertree.h"
#include "wx/sizer.h"
#include "wx/tokenzr.h"

#include "macros.h" 
#include "globals.h"
#include "plugin.h"

FileExplorer::FileExplorer(wxWindow *parent, const wxString &caption)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(250, 300))
, m_caption(caption)
#ifdef __WXMSW__
, m_thread(this)
#endif
{
	CreateGUIControls();
}

FileExplorer::~FileExplorer()
{
}

void FileExplorer::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);
	
#ifdef __WXMSW__
	wxArrayString volumes;
	Connect(wxEVT_THREAD_VOLUME_COMPLETED, wxCommandEventHandler(FileExplorer::OnVolumes), NULL, this);
	
	m_thread.Create();
	m_thread.Run();
	
	m_volumes = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, volumes, 0 );
	mainSizer->Add(m_volumes, 0, wxEXPAND|wxALL, 1);
#endif

	m_fileTree = new FileExplorerTree(this, wxID_ANY);
	mainSizer->Add(m_fileTree, 1, wxEXPAND|wxALL, 1);
	mainSizer->Layout();

#ifdef __WXMSW__
	m_fileTree->Connect(wxVDTC_ROOT_CHANGED, wxCommandEventHandler(FileExplorer::OnRootChanged), NULL, this);
	ConnectChoice(m_volumes, FileExplorer::OnVolumeChanged);
#endif
}

void FileExplorer::Scan()
{
	wxString cwd = wxGetCwd();
	cwd << wxT("/");
	wxFileName fn(cwd);
	if(fn.HasVolume()){
		wxString root;
		root << fn.GetVolume() << wxT(":\\");
		m_fileTree->SetRootPath(root, false, wxVDTC_DEFAULT);
#ifdef __WXMSW__
		if(m_volumes->FindString(fn.GetVolume() + wxT(":\\")) == wxNOT_FOUND) {
			m_volumes->AppendString(fn.GetVolume() + wxT(":\\"));
		}
		m_volumes->SetStringSelection(fn.GetVolume() + wxT(":\\"));
#endif
	}else{
		m_fileTree->SetRootPath(wxT("/"), false, wxVDTC_DEFAULT);
	}
	
	m_fileTree->ExpandToPath(fn);
	SendCmdEvent(wxEVT_FILE_EXP_INIT_DONE);
}

#ifdef __WXMSW__
void FileExplorer::OnVolumeChanged(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//Get the selection
	wxString newRoot = m_volumes->GetStringSelection();
	m_fileTree->SetRootPath(newRoot);
}

void FileExplorer::OnRootChanged(wxCommandEvent &e)
{
	wxTreeItemId root = m_fileTree->GetRootItem();
	if(root.IsOk()){
		wxString vol = m_fileTree->GetItemText(root);
		this->m_volumes->SetStringSelection(vol);
	}
	e.Skip();
}

void FileExplorer::OnVolumes(wxCommandEvent &e)
{
	wxString curvol = m_volumes->GetStringSelection();
	wxArrayString volumes = wxStringTokenize(e.GetString(), wxT(";"), wxTOKEN_STRTOK);
	int where = volumes.Index(curvol);
	if(where != wxNOT_FOUND){
		volumes.RemoveAt((size_t)where);
	}
	m_volumes->Append(volumes);
}

#endif

