#include "svnstatushandler.h"
#include "svn_console.h"
#include "subversion_view.h"
#include "subversion2.h"
#include "svnxml.h"

SvnStatusHandler::SvnStatusHandler(Subversion2 *plugin)
		: SvnCommandHandler(plugin)
{
}

SvnStatusHandler::~SvnStatusHandler()
{
}

void SvnStatusHandler::Process(const wxString& output)
{
//	GetPlugin()->GetShell()->AppendText(output);
	wxArrayString modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles;
	SvnXML::GetFiles(output, modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles);
	GetPlugin()->GetSvnPage()->UpdateTree(modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles);
}
