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
	event.Skip();
}

void BatchBuildDlg::OnItemToggled( wxCommandEvent& event )
{
	wxUnusedVar(event);
}

void BatchBuildDlg::OnBuild( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_cmd = BuildInfo::Build;
	EndModal(wxID_OK);
}

void BatchBuildDlg::OnBuildUI( wxUpdateUIEvent& event )
{
	bool enable(false);
	for(unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++){
		if(m_checkListConfigurations->IsChecked(i)) {
			enable = true;
			break;
		}
	}
	
	event.Enable(enable);
}

void BatchBuildDlg::OnClean( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_cmd = BuildInfo::Clean;
	EndModal(wxID_OK);
}

void BatchBuildDlg::OnCleanUI( wxUpdateUIEvent& event )
{
	bool enable(false);
	for(unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++){
		if(m_checkListConfigurations->IsChecked(i)) {
			enable = true;
			break;
		}
	}
	
	event.Enable(enable);
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
	wxString selectedString  = m_checkListConfigurations->GetStringSelection();
	
	int sel = m_checkListConfigurations->GetSelection();
	if(sel == wxNOT_FOUND){
		return;
	}
	bool checked = m_checkListConfigurations->IsChecked(sel);
	sel --;
	if(sel < 0){
		return;
	}

	// sel contains the new position we want to place the selection string
	m_checkListConfigurations->Delete(sel + 1);
	m_checkListConfigurations->Insert(selectedString, sel);
	m_checkListConfigurations->Check(sel, checked);
	m_checkListConfigurations->Select(sel);
}

void BatchBuildDlg::OnMoveUpUI( wxUpdateUIEvent& event )
{
	
}

void BatchBuildDlg::OnMoveDown( wxCommandEvent& event )
{
	int sel = m_checkListConfigurations->GetSelection();
	if(sel == wxNOT_FOUND){
		return;
	}
	bool checked = m_checkListConfigurations->IsChecked(sel);
	sel ++;
	if(sel >= (int)m_checkListConfigurations->GetCount()){
		return;
	}

	// sel contains the new position we want to place the selection string
	wxString oldStr = m_checkListConfigurations->GetString(sel);

	m_checkListConfigurations->Delete(sel);
	m_checkListConfigurations->Insert(oldStr, sel - 1);
	m_checkListConfigurations->Check(sel -1, checked);
	m_checkListConfigurations->Select(sel);
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

void BatchBuildDlg::GetBuildInfoList(std::list<BuildInfo>& buildInfoList)
{
	bool clean_log(true);
	for(unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++){
		if(m_checkListConfigurations->IsChecked(i)) {
			wxString text = m_checkListConfigurations->GetString(i);
			wxString project = text.BeforeFirst(wxT('|'));
			wxString config  = text.AfterFirst(wxT('|'));
			
			project = project.Trim().Trim(false);
			config = config.Trim().Trim(false);
			
			BuildInfo buildInfo(project, config, true, m_cmd);
			buildInfo.SetCleanLog(clean_log);
			buildInfoList.push_back(buildInfo);
			clean_log = false;
		}
	}
}
