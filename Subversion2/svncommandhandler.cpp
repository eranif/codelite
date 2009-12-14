#include "svncommandhandler.h"
#include "asyncprocess.h"

void SvnCommandHandler::OnProcessOutput(IProcess *process, const wxString& output)
{
	wxUnusedVar(process);
	wxUnusedVar(output);
}
