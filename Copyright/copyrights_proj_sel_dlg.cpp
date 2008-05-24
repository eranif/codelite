#include "copyrights_proj_sel_dlg.h"
#include "workspace.h"

CopyrightsProjectSelDlg::CopyrightsProjectSelDlg( wxWindow* parent, Workspace *wsp )
: CopyrightsProjectSelBaseDlg( parent )
{
	wxArrayString projects;
	wsp->GetProjectList( projects );
	
	for(size_t i=0; i<projects.GetCount(); i++) {
		int pos = m_checkListProjects->Append(projects.Item(i));
		m_checkListProjects->Check((unsigned int)pos, true);
	}
	m_checkListProjects->SetFocus();
}

void CopyrightsProjectSelDlg::OnCheckAll(wxCommandEvent& e)
{
	for(size_t i=0; i<m_checkListProjects->GetCount(); i++){
		m_checkListProjects->Check((unsigned int)i, true);
	}
}

void CopyrightsProjectSelDlg::OnUnCheckAll(wxCommandEvent& e)
{
	for(size_t i=0; i<m_checkListProjects->GetCount(); i++){
		m_checkListProjects->Check((unsigned int)i, false);
	}
}
void CopyrightsProjectSelDlg::GetProjects(wxArrayString& projects)
{
	for(size_t i=0; i<m_checkListProjects->GetCount(); i++){
		if(m_checkListProjects->IsChecked((unsigned int)i)) {
			projects.Add(m_checkListProjects->GetString((unsigned int)i));
		}
	}
}
