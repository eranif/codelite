#include "manager.h"
#include "project.h"
#include "depend_dlg_page.h"

DependenciesPage::DependenciesPage( wxWindow* parent, const wxString &projectName )
		: DependenciesPageBase( parent )
		, m_projectName(projectName)
		, m_dirty(false)
{
	Init();
}

void DependenciesPage::OnConfigChanged( wxCommandEvent& event )
{
	if(m_dirty) {
		// save old configuration
		if(wxMessageBox(wxString::Format(wxT("Build order for configuration '%s' has been modified, would you like to save it?"), m_currentSelection.GetData()), wxT("CodeLite"), wxYES_NO|wxICON_QUESTION) == wxYES){
			Save();
		}
		m_dirty = false;
	}
	
	m_currentSelection = event.GetString();
	// switch to new configuration
	DoPopulateControl(m_currentSelection);
}

void DependenciesPage::OnMoveUp( wxCommandEvent& event )
{
	wxUnusedVar(event);
	OnUpCommand(m_listBoxBuildOrder);
}

void DependenciesPage::OnMoveDown( wxCommandEvent& event )
{
	wxUnusedVar(event);
	OnDownCommand(m_listBoxBuildOrder);
}

void DependenciesPage::OnUpCommand(wxListBox *list)
{
	wxString selectedString  = list->GetStringSelection();

	int sel = list->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}

	sel --;
	if (sel < 0) {
		return;
	}

	// sel contains the new position we want to place the selection string
	list->Delete(sel + 1);
	list->Insert(selectedString, sel);
	list->Select(sel);
	m_dirty = true;
}

void DependenciesPage::OnDownCommand(wxListBox *list)
{
	int sel = list->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}

	sel ++;
	if (sel >= (int)list->GetCount()) {
		return;
	}

	// sel contains the new position we want to place the selection string
	wxString oldStr = list->GetString(sel);

	list->Delete(sel);
	list->Insert(oldStr, sel - 1);
	list->Select(sel);
	m_dirty = true;
}

void DependenciesPage::Save()
{
	//create project dependencie list
	ProjectPtr proj = ManagerST::Get()->GetProject(m_projectName);

	wxArrayString depsArr;
	for (size_t i=0; i<m_listBoxBuildOrder->GetCount(); i++) {
		depsArr.Add(m_listBoxBuildOrder->GetString((unsigned int)i));
	}
	
	if(m_currentSelection.IsEmpty()) {
		return;
	}
	
	proj->SetDependencies(depsArr, m_currentSelection);
	m_dirty = false;
}

void DependenciesPage::OnCheckListItemToggled(wxCommandEvent &event)
{
	int item = event.GetSelection();
	wxString name = m_checkListProjectList->GetString((unsigned int)item);
	if (!m_checkListProjectList->IsChecked((unsigned int)item)) {
		unsigned int buildOrderId = m_listBoxBuildOrder->FindString(name);
		if (buildOrderId != (unsigned int)wxNOT_FOUND) {
			m_listBoxBuildOrder->Delete(buildOrderId);
		}
	} else {
		m_listBoxBuildOrder->Append(name);
	}
	m_dirty = true;
}

void DependenciesPage::Init()
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(m_projectName, errMsg);
	if (proj) {

		// populate the choice control with the list of available configurations for this project
		ProjectSettingsPtr settings = proj->GetSettings();
		if ( settings ) {
			ProjectSettingsCookie cookie;
			BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
			while (bldConf) {
				m_choiceProjectConfig->Append(bldConf->GetName());
				bldConf = settings->GetNextBuildConfiguration(cookie);
			}
		}

		// by default select the first configuration
		if (m_choiceProjectConfig->GetCount()>0) {
			m_choiceProjectConfig->SetSelection(0);
		}

		// select the active configuration
		BuildConfigPtr selBuildConf = WorkspaceST::Get()->GetProjBuildConf(m_projectName, wxEmptyString);
		if (selBuildConf) {
			int where = m_choiceProjectConfig->FindString(selBuildConf->GetName());
			if (where != wxNOT_FOUND) {
				m_choiceProjectConfig->SetSelection(where);
			}
		}
		
		m_currentSelection = m_choiceProjectConfig->GetStringSelection();
		DoPopulateControl( m_choiceProjectConfig->GetStringSelection() );

	} else {
		wxMessageBox(errMsg, wxT("CodeLite"));
		return;
	}
}

void DependenciesPage::DoPopulateControl(const wxString& configuration)
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(m_projectName, errMsg);
	if(!proj) {
		return;
	}
	
	m_listBoxBuildOrder->Clear();
	m_checkListProjectList->Clear();
	
	//initialize the build order listbox
	wxArrayString depArr = proj->GetDependencies(configuration);
	size_t i=0;
	for (i=0; i<depArr.GetCount(); i++) {
		wxString item = depArr.Item(i);
		m_listBoxBuildOrder->Append(item);
	}
	
	//initialize the project dependencies check list
	wxArrayString projArr;
	ManagerST::Get()->GetProjectList(projArr);

	for (i=0; i<projArr.GetCount(); i++) {

		if (projArr.Item(i) != m_projectName) {
			int idx = m_checkListProjectList->Append(projArr.Item(i));
			m_checkListProjectList->Check(idx, depArr.Index(projArr.Item(i)) != wxNOT_FOUND);
		}
	}

}
