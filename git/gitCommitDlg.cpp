#include "gitCommitDlg.h"
#include "gitCommitEditor.h"
#include <wx/tokenzr.h>

#include "icons/icon_git.xpm"

GitCommitDlg::GitCommitDlg(wxWindow* parent, const wxString& repoDir)
	:wxDialog(parent, wxID_ANY, wxT("Commit"),wxDefaultPosition, wxDefaultSize,
	          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
	,m_workingDir(repoDir)
{
	SetIcon(wxICON(icon_git));
	m_listBox = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
	m_listBox->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(GitCommitDlg::OnChangeFile), NULL, this);
	m_editor = new GitCommitEditor(this);
	m_commitMessage = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2);

	wxStdDialogButtonSizer* sizer_3 = CreateStdDialogButtonSizer(wxOK | wxCANCEL);

	wxStaticBoxSizer* sizer_list = new wxStaticBoxSizer(wxVERTICAL, this, wxT("File list"));
	sizer_list->Add(m_listBox, 1, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizer_edit = new wxStaticBoxSizer(wxVERTICAL,this,wxT("File diff"));
	sizer_edit->Add(m_editor, 1, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizer_msg = new wxStaticBoxSizer(wxHORIZONTAL,this,wxT("Commit message"));
	sizer_msg->Add(m_commitMessage, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
	sizer_2->Add(sizer_list, 0, wxALL|wxEXPAND, 5);
	sizer_2->Add(sizer_edit, 1, wxALL|wxEXPAND, 5);
	sizer_1->Add(sizer_2, 2, wxEXPAND, 0);
	sizer_1->Add(sizer_msg, 1, wxALL|wxEXPAND, 5);
	sizer_1->Add(sizer_3, 0, wxALL|wxEXPAND, 5);
	SetSizer(sizer_1);
	sizer_1->Fit(this);
	Layout();

	SetSize(1250,-1);
}
/*******************************************************************************/
GitCommitDlg::~GitCommitDlg()
{
	m_listBox->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(GitCommitDlg::OnChangeFile), NULL, this);
}
/*******************************************************************************/
void GitCommitDlg::AppendDiff(const wxString& diff)
{
	wxArrayString diffList = wxStringTokenize(diff, wxT("\n"), wxTOKEN_STRTOK);
	unsigned index = 0;
	wxString currentFile;
	while(index < diffList.GetCount()) {
		wxString line = diffList[index];
		if(line.StartsWith(wxT("diff"))) {
			line.Replace(wxT("diff --git a/"), wxT(""));
			currentFile = line.Left(line.Find(wxT(" ")));
		} else if(line.StartsWith(wxT("Binary"))) {
			m_diffMap[currentFile] = wxT("Binary diff");
		} else {
			m_diffMap[currentFile].Append(line+wxT("\n"));
		}
		++index;
	}
	index = 0;
	for (std::map<wxString,wxString>::iterator it=m_diffMap.begin() ; it != m_diffMap.end(); ++it) {
		m_listBox->Append((*it).first);
		m_listBox->Check(index++, true);
	}

	if(m_diffMap.size() != 0) {
		std::map<wxString,wxString>::iterator it=m_diffMap.begin();
		m_editor->SetText((*it).second);
		m_listBox->Select(0);
		m_editor->SetReadOnly(true);
	}

	wxFont font(10, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	wxTextAttr atr = m_commitMessage->GetDefaultStyle();
	atr.SetFont(font);
	m_commitMessage->SetDefaultStyle(atr);
	m_commitMessage->SetFont(font);

}
/*******************************************************************************/
wxArrayString GitCommitDlg::GetSelectedFiles()
{
	wxArrayString ret;
	for(unsigned i=0; i < m_listBox->GetCount(); ++i) {
		if(m_listBox->IsChecked(i))
			ret.Add(m_listBox->GetString(i));
	}
	return ret;
}
/*******************************************************************************/
wxString GitCommitDlg::GetCommitMessage()
{
	wxString msg = m_commitMessage->GetValue();
	msg.Replace(wxT("\""),wxT("\\\""));
	return msg;
}
/*******************************************************************************/
void GitCommitDlg::OnChangeFile(wxCommandEvent& e)
{
	int sel = m_listBox->GetSelection();
	wxString file = m_listBox->GetString(sel);
	m_editor->SetReadOnly(false);
	m_editor->SetText(m_diffMap[file]);
	m_editor->SetReadOnly(true);
}
/*******************************************************************************/
