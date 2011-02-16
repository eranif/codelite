#include "gitCommitListDlg.h"

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
	:wxDialog(parent, wxID_ANY, wxT("Commit list"),wxDefaultPosition, wxDefaultSize,
	          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	m_workingDir(workingDir)
{
	SetIcon(wxICON(icon_git));
	m_commitListBox = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	m_commitListBox->InsertColumn(0,wxT("Commit"));
	m_commitListBox->InsertColumn(1,wxT("Subject"));
	m_commitListBox->InsertColumn(2,wxT("Author"));
	m_commitListBox->InsertColumn(3,wxT("Date"));
	m_commitListBox->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(GitCommitListDlg::OnChangeCommit), NULL, this);

	m_fileListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
	m_fileListBox->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(GitCommitListDlg::OnChangeFile), NULL, this);
	m_fileListBox->SetMinSize(wxSize(350,50));
	m_editor = new GitCommitEditor(this);
	m_commitMessage = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2);

	wxFont font(10, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	wxTextAttr atr = m_commitMessage->GetDefaultStyle();
	atr.SetFont(font);
	m_commitMessage->SetDefaultStyle(atr);

	wxStdDialogButtonSizer* sizer_3 = CreateStdDialogButtonSizer(wxOK);

	wxStaticBoxSizer* sizer_listCommit = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Commit list"));
	sizer_listCommit->Add(m_commitListBox, 1, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizer_listFile = new wxStaticBoxSizer(wxVERTICAL, this, wxT("File list"));
	sizer_listFile->Add(m_fileListBox, 1, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizer_edit = new wxStaticBoxSizer(wxVERTICAL,this,wxT("File diff"));
	sizer_edit->Add(m_editor, 1, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizer_msg = new wxStaticBoxSizer(wxHORIZONTAL,this,wxT("Commit message"));
	sizer_msg->Add(m_commitMessage, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
	sizer_2->Add(sizer_listFile, 0, wxALL|wxEXPAND, 5);
	sizer_2->Add(sizer_edit, 1, wxALL|wxEXPAND, 5);
	sizer_1->Add(sizer_listCommit, 1, wxEXPAND, 0);
	sizer_1->Add(sizer_2, 1, wxEXPAND, 0);
	sizer_1->Add(sizer_msg, 1, wxALL|wxEXPAND, 5);
	sizer_1->Add(sizer_3, 0, wxALL|wxEXPAND, 5);
	SetSizer(sizer_1);
	sizer_1->Fit(this);
	Layout();

	wxSize s = GetSize();
	SetSize(s.GetWidth()+200,1000);

}
/*******************************************************************************/
GitCommitListDlg::~GitCommitListDlg()
{
	m_commitListBox->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(GitCommitListDlg::OnChangeCommit), NULL, this);
	m_fileListBox->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(GitCommitListDlg::OnChangeFile), NULL, this);
}
/*******************************************************************************/
void GitCommitListDlg::SetCommitList(const wxString& commits)
{
	wxArrayString gitList = wxStringTokenize(commits, wxT("\n"));
	for(unsigned i=0; i < gitList.GetCount(); ++i) {
		wxArrayString gitCommit = wxStringTokenize(gitList[i], wxT("|"));

		m_commitListBox->InsertItem(i,gitCommit[0]);
		m_commitListBox->SetItem(i,1,gitCommit[1]);
		m_commitListBox->SetItem(i,2,gitCommit[2]);
		m_commitListBox->SetItem(i,3,gitCommit[3]);
	}
	m_commitListBox->SetColumnWidth(0,150);
	m_commitListBox->SetColumnWidth(1,500);
	m_commitListBox->SetColumnWidth(2,250);
	m_commitListBox->SetColumnWidth(3,150);
}
/*******************************************************************************/
void GitCommitListDlg::OnChangeCommit(wxListEvent& e)
{
	wxString commitID = e.GetItem().GetText();

	wxString command = wxT("git --no-pager show ")+commitID;
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
