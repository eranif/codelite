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

void PSBuildEventsPage::Load(BuildConfigPtr buildConf)
{
	Clear();
	
	BuildCommandList buildCmds;
	wxString text;
	if(m_isPreEvents) {
		buildConf->GetPreBuildCommands(buildCmds);
		text = _("Set the commands to run in the pre build stage");
		
	} else {
		buildConf->GetPostBuildCommands(buildCmds);
		text = _("Set the commands to run in the post build stage");
	}
	text << _("\nCommands starting with the hash sign ('#'), will not be executed");
	m_staticText11->SetLabel(text);
	BuildCommandList::const_iterator iter = buildCmds.begin();
	m_textCtrlBuildEvents->Clear();
	for (; iter != buildCmds.end(); iter ++) {
		wxString cmdText = iter->GetCommand();
		cmdText.Trim().Trim(false);
		if(iter->GetEnabled() == false && !cmdText.StartsWith(wxT("#"))) {
			cmdText.Prepend(wxT("#"));
		}
		cmdText.Append(wxT("\n"));
		m_textCtrlBuildEvents->AppendText(cmdText);
	}
	
	m_textCtrlBuildEvents->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSBuildEventsPage::OnCmdEvtVModified ), NULL, this );
}

void PSBuildEventsPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
	BuildCommandList cmds;
	wxArrayString commands = wxStringTokenize(m_textCtrlBuildEvents->GetValue(), wxT("\n\r"), wxTOKEN_STRTOK);
	for (size_t i=0; i<commands.GetCount(); i++) {
		wxString command = commands.Item(i).Trim().Trim(false);
		bool enabled = !command.StartsWith(wxT("#"));
		BuildCommand cmd(command, enabled);
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
	m_textCtrlBuildEvents->Clear();
}
