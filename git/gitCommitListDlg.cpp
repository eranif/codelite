#include "gitCommitListDlg.h"
#include "gitentry.h"
#include "editor_config.h"
#include "windowattrmanager.h"

#include <wx/tokenzr.h>
#include "gitCommitEditor.h"
#include "asyncprocess.h"
#include "processreaderthread.h"

#include "icons/icon_git.xpm"

BEGIN_EVENT_TABLE(GitCommitListDlg, wxDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  GitCommitListDlg::OnProcessOutput)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, GitCommitListDlg::OnProcessTerminated)
END_EVENT_TABLE()

GitCommitListDlg::GitCommitListDlg(wxWindow* parent, const wxString& workingDir)
	: GitCommitListDlgBase(parent)
	, m_workingDir(workingDir)
{
	GitEntry data;
	EditorConfigST::Get()->ReadObject(wxT("GitData"), &data);
	m_gitPath = data.GetGITExecutablePath();
	
	m_commitListBox->InsertColumn(0, wxT("Commit"));
	m_commitListBox->InsertColumn(1, wxT("Subject"));
	m_commitListBox->InsertColumn(2, wxT("Author"));
	m_commitListBox->InsertColumn(3, wxT("Date"));
	
	WindowAttrManager::Load(this, wxT("GitCommitListDlg"), NULL);
}

/*******************************************************************************/
GitCommitListDlg::~GitCommitListDlg()
{
	WindowAttrManager::Save(this, wxT("GitCommitListDlg"), NULL);
}

/*******************************************************************************/
void GitCommitListDlg::SetCommitList(const wxString& commits)
{
	wxArrayString gitList = wxStringTokenize(commits, wxT("\n"), wxTOKEN_STRTOK);
	
	for(unsigned i=0; i < gitList.GetCount(); ++i) {
		wxArrayString gitCommit = wxStringTokenize(gitList[i], wxT("|"));
		if(gitCommit.GetCount() >= 4) {
			m_commitListBox->InsertItem(i, gitCommit[0]);
			m_commitListBox->SetItem   (i, 1, gitCommit[1]);
			m_commitListBox->SetItem   (i, 2, gitCommit[2]);
			m_commitListBox->SetItem   (i, 3, gitCommit[3]);
		}
	}
	
	m_commitListBox->SetColumnWidth(0, 150);
	m_commitListBox->SetColumnWidth(1, 500);
	m_commitListBox->SetColumnWidth(2, 250);
	m_commitListBox->SetColumnWidth(3, 150);
}

/*******************************************************************************/
void GitCommitListDlg::OnChangeCommit(wxListEvent& e)
{
	wxString commitID = e.GetItem().GetText();

	wxString command = wxString::Format(wxT("%s --no-pager show %s"), m_gitPath.c_str(), commitID.c_str());
	m_process = CreateAsyncProcess(this, command, IProcessCreateDefault, m_workingDir);
}

/*******************************************************************************/
void GitCommitListDlg::OnChangeFile(wxCommandEvent& e)
{
	int sel = m_fileListBox->GetSelection();
	wxString file = m_fileListBox->GetString(sel);
	m_editor->SetReadOnly(false);
	m_editor->SetText(m_diffMap[file]);
	m_editor->SetReadOnly(true);
}
/*******************************************************************************/
void GitCommitListDlg::OnProcessTerminated(wxCommandEvent &event)
{
	m_commitMessage->Clear();
	m_fileListBox->Clear();
	m_diffMap.clear();
	m_commandOutput.Replace(wxT("\r"), wxT(""));
	wxArrayString diffList = wxStringTokenize(m_commandOutput, wxT("\n"));

	bool foundFirstDiff = false;
	unsigned index = 0;
	wxString currentFile;
	while(index < diffList.GetCount()) {
		wxString line = diffList[index];
		if(line.StartsWith(wxT("diff"))) {
			line.Replace(wxT("diff --git a/"), wxT(""));
			currentFile = line.Left(line.Find(wxT(" ")));
			foundFirstDiff = true;
		} else if(line.StartsWith(wxT("Binary"))) {
			m_diffMap[currentFile] = wxT("Binary diff");
		} else if(!foundFirstDiff) {
			m_commitMessage->AppendText(line+wxT("\n"));
		} else {
			m_diffMap[currentFile].Append(line+wxT("\n"));
		}
		++index;
	}
	for (std::map<wxString,wxString>::iterator it=m_diffMap.begin() ; it != m_diffMap.end(); ++it) {
		m_fileListBox->Append((*it).first);
	}
	m_editor->SetReadOnly(false);
	m_editor->SetText(wxT(""));

	if(m_diffMap.size() != 0) {
		std::map<wxString,wxString>::iterator it=m_diffMap.begin();
		m_editor->SetText((*it).second);
		m_fileListBox->Select(0);
		m_editor->SetReadOnly(true);
	}

	m_commandOutput.Clear();
}
/*******************************************************************************/
void GitCommitListDlg::OnProcessOutput(wxCommandEvent &event)
{
	ProcessEventData *ped = (ProcessEventData*)event.GetClientData();
	if( ped ) {
		m_commandOutput.Append(ped->GetData());
		delete ped;
	}
}
