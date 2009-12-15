#include "svn_default_command_handler.h"
#include "subversion2.h"
#include "subversion_view.h"

SvnDefaultCommandHandler::SvnDefaultCommandHandler(Subversion2 *plugin)
		: SvnCommandHandler(plugin)
{
}

SvnDefaultCommandHandler::~SvnDefaultCommandHandler()
{
}

void SvnDefaultCommandHandler::Process(const wxString &output)
{
	// TODO:: do something with the output
	wxUnusedVar(output);

	// Refresh the SVN output page
	GetPlugin()->GetSvnView()->BuildTree();
}
