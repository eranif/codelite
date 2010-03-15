#include "svn_default_command_handler.h"
#include "subversion2.h"
#include "subversion_view.h"

SvnDefaultCommandHandler::SvnDefaultCommandHandler(Subversion2 *plugin, int commandId, wxEvtHandler *owner)
		: SvnCommandHandler(plugin, commandId, owner)
{
}

SvnDefaultCommandHandler::~SvnDefaultCommandHandler()
{
}

void SvnDefaultCommandHandler::Process(const wxString &output)
{
	wxUnusedVar(output);

	// Reload any modified files
	wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT);
	GetPlugin()->GetManager()->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(e);

	// Refresh the SVN output page
	GetPlugin()->GetSvnView()->BuildTree();
}
