#include "gitCommitDlg.h"
#include "gitCommitEditor.h"
#include "windowattrmanager.h"
#include <wx/tokenzr.h>


GitCommitDlg::GitCommitDlg(wxWindow* parent, const wxString& repoDir)
    : GitCommitDlgBase(parent)
    ,m_workingDir(repoDir)
{
    WindowAttrManager::Load(this, wxT("GitCommitDlg"), NULL);
}

/*******************************************************************************/
GitCommitDlg::~GitCommitDlg()
{
    WindowAttrManager::Save(this, wxT("GitCommitDlg"), NULL);
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

    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
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

void GitCommitDlg::OnCommitOK(wxCommandEvent& event)
{
    if ( m_commitMessage->GetValue().IsEmpty() ) {
        ::wxMessageBox(_("Git requires a commit message"), "codelite", wxICON_WARNING|wxOK|wxCENTER);
        return;
    }
    EndModal(wxID_OK);
}

/*******************************************************************************/
