#include "svncommithandler.h"
#include "subversion_page.h"

SvnCommitHandler::SvnCommitHandler(IManager *manager, SubversionPage *ui)
		: SvnCommandHandler(manager)
		, m_ui(ui)
{
}

SvnCommitHandler::~SvnCommitHandler()
{
}

void SvnCommitHandler::Process(const wxString &output)
{
	// TODO:: do something with the output
	wxUnusedVar(output);

	// Refresh the SVN output page
	m_ui->BuildTree();
}
