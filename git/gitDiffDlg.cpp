#include "gitDiffDlg.h"
#include "gitentry.h"
#include "editor_config.h"
#include "windowattrmanager.h"

#include <wx/tokenzr.h>
#include "gitCommitEditor.h"
#include "asyncprocess.h"
#include "processreaderthread.h"


BEGIN_EVENT_TABLE(GitDiffDlg, wxDialog)

END_EVENT_TABLE()


GitDiffDlg::GitDiffDlg(wxWindow* parent, const wxString& workingDir)
	: GitDiffDlgBase(parent)
	, m_workingDir(workingDir)
{
	GitEntry data;
	EditorConfigST::Get()->ReadObject(wxT("GitData"), &data);
	m_gitPath = data.GetGITExecutablePath();
		
	WindowAttrManager::Load(this, wxT("GitDiffDlg"), NULL);
	
}

/*******************************************************************************/
GitDiffDlg::~GitDiffDlg()
{
	WindowAttrManager::Save(this, wxT("GitDiffDlg"), NULL);
}

/*******************************************************************************/

void GitDiffDlg::SetDiff(const wxString& diff)
{
	wxString m_commandOutput = diff;
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
		} else if(foundFirstDiff) {
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

}

/*******************************************************************************/
void GitDiffDlg::OnChangeFile(wxCommandEvent& e)
{
	int sel = m_fileListBox->GetSelection();
	wxString file = m_fileListBox->GetString(sel);
	m_editor->SetReadOnly(false);
	m_editor->SetText(m_diffMap[file]);
	m_editor->SetReadOnly(true);
}

