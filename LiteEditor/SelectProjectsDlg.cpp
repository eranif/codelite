#include "SelectProjectsDlg.h"
#include "windowattrmanager.h"
#include "manager.h"

SelectProjectsDlg::SelectProjectsDlg(wxWindow* parent)
    : SelectProjectsDlgBase(parent)
{
    // Populate the project list and check them all
    m_checkListBoxProjects->Clear();
    wxArrayString projects;
    ManagerST::Get()->GetProjectList( projects );
    
    for(size_t i=0; i<projects.GetCount(); ++i) {
        unsigned int pos = m_checkListBoxProjects->Append(projects.Item(i));
        m_checkListBoxProjects->Check(pos, true);
    }
    
    WindowAttrManager::Load(this, "SelectProjectsDlg", NULL);
}

SelectProjectsDlg::~SelectProjectsDlg()
{
    WindowAttrManager::Save(this, "SelectProjectsDlg", NULL);
}

void SelectProjectsDlg::OnSelectAll(wxCommandEvent& event)
{
    for(unsigned int i=0; i<m_checkListBoxProjects->GetCount(); ++i) {
        m_checkListBoxProjects->Check(i);
    }
}

void SelectProjectsDlg::OnUnSelectAll(wxCommandEvent& event)
{
    for(unsigned int i=0; i<m_checkListBoxProjects->GetCount(); ++i) {
        m_checkListBoxProjects->Check(i, false);
    }
}

wxArrayString SelectProjectsDlg::GetProjects() const
{
    wxArrayString projects;
    for(unsigned int i=0; i<m_checkListBoxProjects->GetCount(); ++i) {
        if ( m_checkListBoxProjects->IsChecked(i) ) {
            projects.Add( m_checkListBoxProjects->GetString(i) );
        }
    }
    return projects;
}
