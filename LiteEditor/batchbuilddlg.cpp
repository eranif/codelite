#include "manager.h"
#include "workspace.h"
#include "batchbuilddlg.h"

BatchBuildDlg::BatchBuildDlg( wxWindow* parent )
		: BatchBuildBaseDlg( parent )
{
	m_checkListConfigurations->SetFocus();

	// loop over all projects, for each project collect all available
	// build configurations and add them to the check list control
	wxArrayString projects;
	WorkspaceST::Get()->GetProjectList(projects);
	for (size_t i=0; i<projects.GetCount(); i++) {
		ProjectPtr p = ManagerST::Get()->GetProject(projects.Item(i));
		if (p) {
			ProjectSettingsPtr settings = p->GetSettings();
			if (settings) {
				ProjectSettingsCookie cookie;
				BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
				while (bldConf) {

					int idx = m_checkListConfigurations->Append( p->GetName() + wxT(" | ") + bldConf->GetName());
					m_checkListConfigurations->Check((unsigned int)idx, true);

					bldConf = settings->GetNextBuildConfiguration(cookie);
				}
			}
		}
	}
	
	if(m_checkListConfigurations->GetCount()>0){
		m_checkListConfigurations->Select(0);
	}
}

void BatchBuildDlg::OnItemSelected( wxCommandEvent& event )
{
//	m_checkListConfigurations->Select( event.GetSelection() );
//	m_checkListConfigurations->EnsureVisible( event.GetSelection() );
	event.Skip();
}

void BatchBuildDlg::OnItemToggled( wxCommandEvent& event )
{
	wxUnusedVar(event);
}

void BatchBuildDlg::OnBuild( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_cmd = BatchBuild_Build;
	EndModal(wxID_OK);
}

void BatchBuildDlg::OnBuildUI( wxUpdateUIEvent& event )
{
	event.Enable(m_checkListConfigurations->GetSelection() != wxNOT_FOUND);
}

void BatchBuildDlg::OnClean( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_cmd = BatchBuild_Clean;
	EndModal(wxID_OK);
}

void BatchBuildDlg::OnCleanUI( wxUpdateUIEvent& event )
{
	event.Enable(m_checkListConfigurations->GetSelection() != wxNOT_FOUND);
}

void BatchBuildDlg::OnCheckAll( wxCommandEvent& event )
{
	wxUnusedVar(event);
	for(unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++){
		m_checkListConfigurations->Check(i, true);
	}
}

void BatchBuildDlg::OnUnCheckAll( wxCommandEvent& event )
{
	wxUnusedVar(event);
	for(unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++){
		m_checkListConfigurations->Check(i, false);
	}
}

void BatchBuildDlg::OnMoveUp( wxCommandEvent& event )
{
	// TODO: Implement OnMoveUp
}

void BatchBuildDlg::OnMoveUpUI( wxUpdateUIEvent& event )
{
	// TODO: Implement OnMoveUpUI
}

void BatchBuildDlg::OnMoveDown( wxCommandEvent& event )
{
	// TODO: Implement OnMoveDown
}

void BatchBuildDlg::OnMoveDownUI( wxUpdateUIEvent& event )
{
	// TODO: Implement OnMoveDownUI
}

void BatchBuildDlg::OnClose( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}
