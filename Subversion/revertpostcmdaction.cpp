#include "revertpostcmdaction.h"
#include "plugin.h"
#include "imanager.h"
#include "svnoptions.h"
#include <wx/xrc/xmlres.h>
#include <wx/event.h>
#include <wx/app.h>

RevertPostCmdAction::RevertPostCmdAction(IManager *mgr)
		: SvnPostCmdAction()
		, m_mgr(mgr)
{
}

RevertPostCmdAction::~RevertPostCmdAction()
{
}

void RevertPostCmdAction::DoCommand()
{
	wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT);
	m_mgr->GetTheApp()->GetTopWindow()->AddPendingEvent(e);

	// Notify the mainframe to re-tag workspace
	// if the update provided any new / updated files, re-tag the workspace
	SvnOptions options;
	m_mgr->GetConfigTool()->ReadObject(wxT("SubversionOptions"), &options);
	if (options.GetKeepTagUpToDate()) {
		// send an event to the main frame indicating that a re-tag is required
		wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
		m_mgr->GetTheApp()->GetTopWindow()->AddPendingEvent(e);
	}
}
