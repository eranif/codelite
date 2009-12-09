#include "svnstatushandler.h"
#include "subversion_page.h"
#include "svnxml.h"

SvnStatusHandler::SvnStatusHandler(IManager *manager, SubversionPage *ui)
		: SvnCommandHandler(manager)
		, m_ui(ui)
{
}

SvnStatusHandler::~SvnStatusHandler()
{
}

void SvnStatusHandler::Process(const wxString& output)
{
	wxArrayString modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles;
	SvnXML::GetFiles(output, modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles);
	m_ui->UpdateTree(modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles);
}
