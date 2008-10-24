#include "applypatchpostcmdaction.h"
ApplyPatchPostCmdAction::ApplyPatchPostCmdAction(IManager *mgr, SubversionPlugin *plugin)
: SvnIconRefreshHandler(mgr, plugin)
{
}

ApplyPatchPostCmdAction::~ApplyPatchPostCmdAction()
{
}

void ApplyPatchPostCmdAction::DoCommand()
{
	// update icons
	SvnIconRefreshHandler::DoCommand();
	
	// remove temporary files
	wxRemoveFile(m_file);
}
