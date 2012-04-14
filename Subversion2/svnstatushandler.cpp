#include "svnstatushandler.h"
#include "svn_console.h"
#include "subversion_view.h"
#include "subversion2.h"
#include "svnxml.h"

SvnStatusHandler::SvnStatusHandler(Subversion2 *plugin, int commandId, wxEvtHandler *owner, bool fileExplorerOnly, const wxString &rootDir)
	: SvnCommandHandler(plugin, commandId, owner)
	, m_fileExplorerOnly(fileExplorerOnly)
	, m_rootDir(rootDir)
{
}

SvnStatusHandler::~SvnStatusHandler()
{
}

void SvnStatusHandler::Process(const wxString& output)
{
	wxArrayString modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles, ignoredFiles;
	SvnXML::GetFiles(output, modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles, ignoredFiles);

	modFiles.Sort();
	conflictedFiles.Sort();
	unversionedFiles.Sort();
	newFiles.Sort();
	deletedFiles.Sort();
	lockedFiles.Sort();
	ignoredFiles.Sort();
	GetPlugin()->GetSvnView()->UpdateTree(modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles, ignoredFiles, m_fileExplorerOnly, m_rootDir);
}
