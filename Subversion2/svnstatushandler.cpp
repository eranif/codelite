#include "svnstatushandler.h"
#include "svn_console.h"
#include "subversion_view.h"
#include "subversion2.h"
#include "svnxml.h"

SvnStatusHandler::SvnStatusHandler(Subversion2 *plugin, int commandId, wxEvtHandler *owner)
		: SvnCommandHandler(plugin, commandId, owner)
{
}

SvnStatusHandler::~SvnStatusHandler()
{
}

void SvnStatusHandler::Process(const wxString& output)
{
//	GetPlugin()->GetShell()->AppendText(output);
	wxArrayString modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles;
	SvnXML::GetFiles(output, modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles);
	
// new code
   modFiles.Sort();
   conflictedFiles.Sort();
   unversionedFiles.Sort();
   newFiles.Sort();
   deletedFiles.Sort();
   lockedFiles.Sort();
// end of new code

	GetPlugin()->GetSvnView()->UpdateTree(modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles);
}
