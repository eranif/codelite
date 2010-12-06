#include "ps_build_events_page.h"
#include "free_text_dialog.h"
#include <wx/tokenzr.h>
#include "macros.h"

PSBuildEventsPage::PSBuildEventsPage( wxWindow* parent, bool preEvents, ProjectSettingsDlg* dlg )
	: PSBuildEventsBasePage( parent )
	, m_isPreEvents(preEvents)
	, m_dlg(dlg)
{

}

void PSBuildEventsPage::OnCmdEvtVModified( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_dlg->SetIsDirty(true);
}

void PSBuildEventsPage::OnNewBuildCommand( wxCommandEvent& event )
{
	FreeTextDialog dlg(this);
	if (dlg.ShowModal() == wxID_OK) {
		wxString value = dlg.GetValue();
		TrimString(value);
		//incase several commands were entered, split them
		wxStringTokenizer tkz(value, wxT("\n"), wxTOKEN_STRTOK);
		while (tkz.HasMoreTokens()) {
			wxString command = tkz.NextToken();
			if (command.IsEmpty() == false) {
				m_checkListBuildCommands->Append(command);
				m_checkListBuildCommands->Check(m_checkListBuildCommands->GetCount()-1);
			}
		}
		m_dlg->SetIsDirty(true);
	}
}

void PSBuildEventsPage::OnDeleteBuildCommand( wxCommandEvent& event )
{
	int sel = m_checkListBuildCommands->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}
	m_checkListBuildCommands->Delete(sel);
	if (sel < (int)m_checkListBuildCommands->GetCount()) {
		m_checkListBuildCommands->Select(sel);
	} else if (sel - 1 < (int)m_checkListBuildCommands->GetCount()) {
		m_checkListBuildCommands->Select(sel -1);
	}
	m_dlg->SetIsDirty(true);
}

void PSBuildEventsPage::OnBuildSelectedUI( wxUpdateUIEvent& event )
{
	event.Enable(m_checkListBuildCommands->GetSelection() != wxNOT_FOUND);
}

void PSBuildEventsPage::OnEditBuildCommand( wxCommandEvent& event )
{
	wxString selectedString  = m_checkListBuildCommands->GetStringSelection();
	int sel = m_checkListBuildCommands->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}

	// on GTK it looks like that the state of the item in the list
	// is changed after calling 'SetString'
	bool selectIt = m_checkListBuildCommands->IsChecked((unsigned int) sel);

	FreeTextDialog dlg(this, selectedString);
	if (dlg.ShowModal() == wxID_OK) {
		wxString value = dlg.GetValue();
		TrimString(value);
		if (value.IsEmpty() == false) {
			m_checkListBuildCommands->SetString((unsigned int)sel, value);
			m_checkListBuildCommands->Check((unsigned int)sel, selectIt);
		}
		m_dlg->SetIsDirty(true);
	}
}

void PSBuildEventsPage::OnUpBuildCommand( wxCommandEvent& event )
{
	wxString selectedString  = m_checkListBuildCommands->GetStringSelection();

	int sel = m_checkListBuildCommands->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}

	bool isSelected = m_checkListBuildCommands->IsChecked(sel);
	sel --;
	if (sel < 0) {
		return;
	}

	// sel contains the new position we want to place the selection string
	m_checkListBuildCommands->Delete(sel + 1);
	m_checkListBuildCommands->Insert(selectedString, sel);
	m_checkListBuildCommands->Select(sel);
	m_checkListBuildCommands->Check(sel, isSelected);

	m_dlg->SetIsDirty(true);
}

void PSBuildEventsPage::OnDownBuildCommand( wxCommandEvent& event )
{
	int sel = m_checkListBuildCommands->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}

	sel ++;
	if (sel >= (int)m_checkListBuildCommands->GetCount()) {
		return;
	}

	// sel contains the new position we want to place the selection string
	wxString oldStr = m_checkListBuildCommands->GetString(sel);
	bool oldStringIsSelected = m_checkListBuildCommands->IsChecked(sel);

	m_checkListBuildCommands->Delete(sel);
	m_checkListBuildCommands->Insert(oldStr, sel - 1);
	m_checkListBuildCommands->Select(sel);
	m_checkListBuildCommands->Check(sel - 1, oldStringIsSelected);

	m_dlg->SetIsDirty(true);
}

void PSBuildEventsPage::Load(BuildConfigPtr buildConf)
{
	Clear();
	
	BuildCommandList buildCmds;
	if(m_isPreEvents) {
		buildConf->GetPreBuildCommands(buildCmds);
		m_staticText11->SetLabel(_("Set the commands to run in the pre build stage:"));
	} else {
		buildConf->GetPostBuildCommands(buildCmds);
		m_staticText11->SetLabel(_("Set the commands to run in the post build stage:"));
	}
	
	BuildCommandList::const_iterator iter = buildCmds.begin();
	m_checkListBuildCommands->Clear();
	for (; iter != buildCmds.end(); iter ++) {
		int index = m_checkListBuildCommands->Append(iter->GetCommand());
		m_checkListBuildCommands->Check(index, iter->GetEnabled());
	}
}

void PSBuildEventsPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
	BuildCommandList cmds;
	for (size_t i=0; i<m_checkListBuildCommands->GetCount(); i++) {
		wxString cmdLine = m_checkListBuildCommands->GetString((unsigned int)i);
		bool enabled = m_checkListBuildCommands->IsChecked((unsigned int)i);
		BuildCommand cmd(cmdLine, enabled);
		cmds.push_back(cmd);
	}
	
	if(m_isPreEvents) {
		buildConf->SetPreBuildCommands(cmds);
		
	} else {
		buildConf->SetPostBuildCommands(cmds);
	}
}

void PSBuildEventsPage::Clear()
{
	m_checkListBuildCommands->Clear();
}
